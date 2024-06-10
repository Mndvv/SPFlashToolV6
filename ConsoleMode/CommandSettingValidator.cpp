#include "CommandSettingValidator.h"
#include <QFileInfo>
#include <QDir>
#include "../Utility/version.h"
#include "../Utility/Utils.h"
#include "../Logger/Log.h"
#include <cassert>

ConsoleMode::CommandSettingValidator::CommandSettingValidator(const ConsoleMode::Config &config,
                                                              const std::shared_ptr<CommandLineArguments> &args):
    m_config(config), m_args(args)
{

}

ConsoleMode::CommandSettingValidator::~CommandSettingValidator()
{

}

bool ConsoleMode::CommandSettingValidator::validate() const
{
    return this->validateRSCSetting();
}

bool ConsoleMode::CommandSettingValidator::hasRSCXMLFile() const
{
    QString rsc_file = m_config.getCommandSetting()->getRSCFileName();

    //check rsc.xml exist or not
    return QFileInfo(rsc_file).exists();
}

bool ConsoleMode::CommandSettingValidator::validateRSCSetting() const
{
    if (ToolInfo::IsCustomerVer())
    {
        //not support rsc mechanism in customer version
        //so quit if there has rsc cmd in config.xml(download/format/readback) or cmd arguments, no matter rsc.xml has or not.
        if (m_config.getCommandSetting()->hasRSCCmdSetting())
        {
            LOGE("NOT support rsc mechanism in customer tool!");
            return false;
        } else {
            return true;
        }
    }

    // if not download command, ignore rsc command if it is exists.
    if (!m_config.getCommandSetting()->hasDownloadCmd()) {
        if (m_config.getCommandSetting()->hasRSCCmdSetting()) {
            LOGI("rsc commnad exist, but ignore it for it's not a download command!");
        }
        return true;
    }

    //RSC only for internal flashtool
    if (this->hasRSCXMLFile()) {
        if (m_config.getCommandSetting()->hasRSCCmdSetting()) {
            LOGI("rsc file and rsc commnad both exist!");
            return true;
        } else {
            LOGE("rsc file exist, but has no rsc command!");
            return false;
        }
    } else {
        if (m_config.getCommandSetting()->hasRSCCmdSetting())
        {
            LOGW("rsc file NOT exist, and has rsc command, but ignore it!");
        }
        else
        {
            LOGI("rsc file NOT exist, and has no rsc command!!");
        }
        return true;
    }
}
