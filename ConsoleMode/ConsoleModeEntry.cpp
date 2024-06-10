#include "ConsoleModeEntry.h"
#include "CommandLineArguments.h"
#include "../Logger/Log.h"
#include "../Utility/FileUtils.h"
#include "../Utility/version.h"
#include "Config.h"
#include "CommandSettingValidator.h"
#include "ConsoleModeRunner.h"
#include "../Err/Exception.h"
#include "../Network/NetworkThread.h"
#include "../Cmd/ConsoleModeCallBack.h"
#include <cassert>

namespace ConsoleMode {

#ifdef _WIN32
#include <Windows.h>
class ConsoleSentry
{
public:
    ConsoleSentry(bool do_redirect)
    {
        attached_ = AttachConsole(ATTACH_PARENT_PROCESS);

        if (attached_ && (!do_redirect))
        {
            freopen("conin$", "r+t", stdin);
            freopen("conout$", "w+t", stdout);
            freopen("conout$", "w+t", stderr);
            /*
            BOOL WINAPI SetConsoleCtrlHandler(
              _In_opt_  PHANDLER_ROUTINE HandlerRoutine,
              _In_      BOOL Add
            );
            */
        }

    }

    ~ConsoleSentry()
    {
        if (attached_)
        {
            FreeConsole();
        }
    }

private:
    bool attached_;
};
#endif

ConsoleModeEntry::ConsoleModeEntry():
    m_network_thread(new NetworkThread(QString()))
{
    ConsoleModeCallBack::setConsoleModeEntry(this);
}

ConsoleModeEntry::~ConsoleModeEntry()
{
#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if (m_network_thread && m_network_thread->isRunning()) {
        m_network_thread->terminate();
        m_network_thread->wait();
    }
#endif
}

int ConsoleModeEntry::run(const QStringList &arguments)
{
    try {
        std::shared_ptr<CommandLineArguments> cmdArg(new CommandLineArguments());
        CommandLineArguments::CommandLineParseResult parser_result = cmdArg->parseCmdLine(arguments);

#ifdef _WIN32
        ConsoleSentry consoleSentry(cmdArg->needRedirect());
#endif

        FileUtils::DebugLogsOn();

        switch (parser_result) {
        case CommandLineArguments::CommandLineParseResult::CommandLineHelpRequested:
            LOGI("%s", qPrintable(cmdArg->getHelpInfo()));
            return 0;
        case CommandLineArguments::CommandLineParseResult::CommandLineVersionRequested:
            LOGI("%s", qPrintable(cmdArg->getVersionInfo()));
            return 0;
        case CommandLineArguments::CommandLineParseResult::CommandLineError:
            LOGE("Invalid parameter:\n%s", qPrintable(cmdArg->getParseErrorMsg()));
            LOGI("\\\n%s\n", qPrintable(cmdArg->getHelpInfo()));
            return -1;
        }
        assert(parser_result == CommandLineArguments::CommandLineParseResult::CommandLineOk);

        if(!cmdArg->validArgumentValues())
        {
            return -1;
        }

        LOGI("Begin");
        LOGI("%s", ToolInfo::ToolName().c_str());
        LOGI("Build Time: " __DATE__ " " __TIME__);

#ifdef _INTERNAL_PHONE_TRACKING_MODE
        // should after log on to open log.
        if(!ToolInfo::IsCustomerVer()) {
            m_network_thread->start();
        }
#endif

#if defined(_LINUX64)
        FileUtils::copy_99ttyacms_file();
        FileUtils::runModemManagerCmd();
#endif

        Config config(cmdArg);

        //check validation of command setting
        CommandSettingValidator cmdSettingValidator(config, cmdArg);
        if (!cmdSettingValidator.validate())
        {
            return -1;
        }
        if (!cmdSettingValidator.hasRSCXMLFile())
        {
            config.getCommandSetting()->removeRSCSetting();
        }

        ConsoleModeRunner runner;
        if (config.getCommandSetting()->hasDownloadCmd()) {
            LOGI("Begin verify load images checksum");
            runner.imageChksumVerify(config.getGeneralSetting()->getFlashXmlFile());
            LOGI("verify load images checksum done!");
        }

        runner.execute(config);
        LOGI("All command exec done!");
        return 0;
    } catch (const BaseException &e) {
        QString error_msg = e.err_msg() + QStringLiteral("\n") + e.context();
        LOGE("BaseException: err_msg[%s]", qPrintable(error_msg));
    } catch (const std::exception &e) {
        LOGE("std::exception: err_msg[%s]", e.what());
    } catch (...) {
        LOGE("Unknown error ocurred.");
    }
    return 1;
}

bool ConsoleModeEntry::network_online() const
{
    return m_network_thread->network_online();
}

}
