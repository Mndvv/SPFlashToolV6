#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QSplashScreen>
#include <QDebug>

#include "../../Utility/FileUtils.h"
#include "../../Utility/OptionIniFileMgr.h"
#include "../../Utility/version.h"
#include "../../Inc/CFlashToolAPI.h"
#include "../../Logger/Log.h"
#include "../../ConsoleMode/ConsoleModeEntry.h"
#include "../../Err/Exception.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Utility/Utils.h"


static void init_app_path(char* argv[])
{
    QString app_path = QString::fromLocal8Bit(argv[0]);
    QDir dir = QFileInfo(app_path).absoluteDir();
    app_path = dir.absolutePath();
    FileUtils::UpdateAppPath(app_path.toStdString());
}

static void setApplicationInfo(QCoreApplication &app)
{
    app.setApplicationName(ToolInfo::ToolName().c_str());
    app.setApplicationVersion(ToolInfo::VersionNum().c_str());
}

static int runQtApplication(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setApplicationInfo(app);

#if defined(_LINUX64)
    bool check_kernel = OptionIniFileMgr::getInstance()->getCheck_kernel_version();
    if (check_kernel && !FileUtils::IsValidKernelVersion()) {
        Utils::flashtool_message_box(0, 0, CRITICAL_MSGBOX, STRING_TOOL_NAME,
                              "Invalid linux kervel version.\nPlease upgrade linux kernel up to at least 4.19.14!", "OK");
        return 5;
    }
#endif

    QPixmap pixmap(":/images/CFlashToolSplash.png");
    QSplashScreen splash(pixmap);
    splash.showMessage(STRING_TOOL_NAME + " is loading...\n\n\n\n\n\n\n\n\n\n\n\n\n"
                       "Copyright (c) 2019-2020, MediaTek Inc. All rights reserved."
                       , Qt::AlignBottom, Qt::gray);
    splash.show();
    app.processEvents();

    MainWindow w;
    int index = OptionIniFileMgr::getInstance()->getLanguage();
    w.ChangeLanguage(index);
#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if (!ToolInfo::IsCustomerVer()) {
        w.detectNetworkConnected();
    }
#endif
    w.LoadLastAuthFile();
    w.LoadLastCertFile();
    w.LoadLastDAFile();
    w.LoadLastFlashXMLFile();
    app.installNativeEventFilter(&w);
    w.show();

    splash.finish(&w);
    return app.exec();
}

static int runConsoleMode(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    setApplicationInfo(app);

#if defined(_LINUX64)
    bool check_kernel = OptionIniFileMgr::getInstance()->getCheck_kernel_version();
    if (check_kernel && !FileUtils::IsValidKernelVersion()) {
        std::cout << "Invalid linux kervel version, please upgrade linux kernel up to at least 4.19.14!" << std::endl;
        return 5;
    }
#endif
    ConsoleMode::ConsoleModeEntry cm_entry;
    return cm_entry.run(app.arguments());
}

int main(int argc, char *argv[])
{
    try {
        //EnableMemLeakCheck();
        Utils::setTextCodec();

        //init cflash tool lib, how to deal with error code
        ZHRESULT hresult = CFlashToolAPI::cflashtool_startup(nullptr);
        if (FAIL(hresult)) {
            qDebug() << "startup flash tool lib failed";
        }
        init_app_path(argv);

        int result_status = 0;
        if(argc > 1) {
            result_status = runConsoleMode(argc, argv);
        } else {
            result_status = runQtApplication(argc, argv);
        }
        // save manually, because call this function in destroy function by shared pointer will crash.
        OptionIniFileMgr::getInstance()->saveOptionFile();
        //destroy cflash tool lib
        CFlashToolAPI::cflashtool_cleanup();
        return result_status;
    }
    catch(const BaseException &base_exp)
    {
        QString error_msg = base_exp.err_msg();
        error_msg.append("\n");
        error_msg.append(base_exp.context());

        LOGE("Uncaught BaseException in main(): %s", qPrintable(error_msg));
        return 1;
    }
    catch(const std::exception &std_exp)
    {
        LOGE("Uncaught std::exception in main(): %s", std_exp.what());
        return 2;
    }
    catch(...)
    {
        LOGE("Uncaught std::exception #$$#$ in main()");
        return 3;
    }
}
