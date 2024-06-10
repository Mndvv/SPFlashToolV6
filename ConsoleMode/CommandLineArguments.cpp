#include <QMap>
#include "CommandLineArguments.h"
#include "../Utility/FileUtils.h"
#include "../Logger/Log.h"
#include "SchemaValidator.h"
#include "../Utility/version.h"
#include "CommandSetting.h"
#include <cassert>


namespace ConsoleMode
{

struct CommandLineOption {
    QStringList option_name;
    QString option_description;
    QString option_value_name;
};

static const QMap<QString, CommandLineOption> g_cmd_option_map = {
    {"config_xml_file_option", {QStringList() << "i" << "config",
                                "console mode configuration file, mandatory with config.xml.\n\t\tSPFlashToolV6.exe -i config.xml\n",
                                "config xml file"}},
    {"flash_xml_file_option", {QStringList() << "f" << "flash_xml",
                               "flash xml file path, mandatory without config.xml.\n\t\tSPFlashToolV6.exe -c download -f flash.xml\n",
                               "flash xml file"}},
    {"operation_option", {QStringList() << "c" << "operation",
                          "flash tool features, mandatory without config.xml, just support:\n"
                          "            format\n"
                          "            download\n"
                          "            format-download\n"
                          "            firmware-upgrade\n"
                          "            dram-repair\n"
                          "            write-efuse\n"
                          "            read-efuse\n"
                          "          SPFlashToolV6.exe -c download -f flash.xml\n",
                          "operation"}},
    {"cert_file_option", {QStringList() << "e" << "cert_file",
                          "certification file path.\n\t\tSPFlashToolV6.exe -e cert_file.cert -f flash.xml -c download\n",
                          "cert file"}},
    {"auth_file_option", {QStringList() << "a" << "auth_file",
                          "authentication file path.\n\t\tSPFlashToolV6.exe -a auth_file.auth -f flash.xml -c download\n",
                          "auth file"}},
    {"com_port_option", {QStringList() << "p" << "com_port",
                         "com port, format is as COM4+COM6 (BROM download), COM5 (preloader download) on Windows "
                         "and format is as 5-2+1-6(BROM download), 5-1(preloader download) on Linux.\n"
                         "          SPFlashToolV6.exe -p COM4+COM6 -f flash.xml -c download\n"
                         "          SPFlashToolV6 -p 5-1+1-6 -i config.xml\n",
                         "com port"}},
    {"redirect_option", {QStringList() << "r" << "redirect",
                         "redirect debug log to parent process.\n\t\tSPFlashToolV6.exe -r -i config.xml\n",
                         QString()}},
    {"battery_mode_option", {QStringList() << "t" << "battery_mode",
                             "Specify battery mode. with, without and auto are available. if no specify the -t argument, the auto mode used."
                             "        Only valid without config.xml.\n"
                             "          SPFlashToolV6.exe -f flash.xml  -c download -t without\n",
                             "battery mode"}},
    {"reboot_option", {QStringList() << "b" << "reboot",
                       "reboot the device.\n\t\tSPFlashToolV6.exe -b -i config.xml\n",
                       QString()}},
    {"disable_storage_life_cycle_check_option", {QStringList() << "disable_storage_life_cycle_check",
                                                 "disable storage life cycle check feature.\n"
                                                 "          SPFlashToolV6.exe -f flash.xml -c download --disable_storage_life_cycle_check\n",
                                                 QString()}},
    {"rsc_option", {QStringList() << "rsc",
                    "Specify the project name in rsc.xml, ONLY support download/format-download/firmware-upgrade for internal tool.\n"
                    "          SPFlashToolV6.exe -f flash.xml -c download --rsc k75v1_64_rsc_cn[op02]\n",
                    "rsc project name"}},
    {"file_option", {QStringList() << "file",
                    "Specify the file which can be used for dram-repair, read-efuse and write-efuse command.\n"
                    "          SPFlashToolV6.exe -f flash.xml -c write-efuse --file efuse.xml\n",
                    "dram repair file"}},
    {"da_log_channel_option", {QStringList() << "l" << "da_log_channel",
                    "Specify the DA log channel. USB and UART are available. if no specify the -l argument, the UART will be default used.\n"
                    "          SPFlashToolV6.exe -f flash.xml -c write-efuse --file efuse.xml -l USB\n",
                    "DA log channel"}},
    {"android_test_mode_option", {QStringList() << "atm",
                    "Set to reboot to ATM mode after download, ONLY support download/format-download/firmware-upgrade for internal tool.\n"
                    "          SPFlashToolV6.exe -f flash.xml -c download --atm\n",
                    QString()}}
};

CommandLineArguments::CommandLineArguments():
    m_redirect(false),
    m_reboot(false),
    m_disable_storage_life_cycle_check(true),
    m_reboot_to_atm(false),
    m_da_log_channel("UART"),
    m_console_mode_current_mode(ConsoleModeCurrentMode::UnknownMode)
{

}

const QString &CommandLineArguments::getConfigXMLFileName() const
{
    return m_config_xml_file_name;
}

const QString &CommandLineArguments::getFlashXMLFilename() const
{
    return m_flash_xml_file_name;
}

const QString &CommandLineArguments::getCommandName() const
{
    return m_cmd_name;
}

const QString &CommandLineArguments::getAuthFilename() const
{
    return m_auth_file_name;
}

const QString &CommandLineArguments::getCertFilename() const
{
    return m_cert_file_name;
}

const QString &CommandLineArguments::getComPortName() const
{
    return m_com_port_name;
}

const QString &CommandLineArguments::getRSCProjName() const
{
    return m_rsc_proj_name;
}

bool CommandLineArguments::needRedirect() const
{
    return m_redirect;
}

bool CommandLineArguments::valid_com_port(const QString &usb_com_port) const
{
#define NOT_FOUND -1
#ifdef _WIN32
    int port_num = 0;
    QRegExp regex("COM(\\d+)");
    if(regex.indexIn(usb_com_port) != NOT_FOUND){
        QString port = regex.cap(1);
        port_num = port.toInt();
    }
    return port_num > 0;
#else
    QRegExp regex("^\\d+-\\d+(?:\\.\\d+)*$");
    int pos = regex.indexIn(usb_com_port);
    return pos != NOT_FOUND;
#endif
}

bool CommandLineArguments::validConfigXMLFile() const
{
    QString error_msg;
    if (!FileUtils::validFile(m_config_xml_file_name, error_msg)) {
        LOGE(qPrintable(error_msg));
        return false;
    }
    try
    {
        SchemaValidator validator(ABS_PATH_C("console_mode.xsd"));
        validator.Validate(m_config_xml_file_name);
    }
    catch(...)
    {
        LOGE("Invalid xml file, please check the config xml file and try again.");
        return false;
    }
    return true;
}

const QString &CommandLineArguments::getFile() const
{
    return m_file;
}

CommandLineArguments::CommandLineParseResult CommandLineArguments::parseCmdLine(const QStringList &cmd_args)
{
    //parse options
    CommandLineOption option = g_cmd_option_map.value("config_xml_file_option");
    const QCommandLineOption config_file_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("flash_xml_file_option");
    const QCommandLineOption flash_xml_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("operation_option");
    const QCommandLineOption operation_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("cert_file_option");
    const QCommandLineOption cert_file_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("com_port_option");
    const QCommandLineOption com_port_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("redirect_option");
    const QCommandLineOption redirect_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("battery_mode_option");
    const QCommandLineOption battery_mode_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("reboot_option");
    const QCommandLineOption reboot_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("disable_storage_life_cycle_check_option");
    const QCommandLineOption disable_storage_life_check_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("auth_file_option");
    const QCommandLineOption auth_file_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("rsc_option");
    const QCommandLineOption rsc_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("file_option");
    const QCommandLineOption file_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("da_log_channel_option");
    const QCommandLineOption da_log_channel_option(option.option_name, option.option_description, option.option_value_name);
    option = g_cmd_option_map.value("android_test_mode_option");
    const QCommandLineOption android_test_mode_option(option.option_name, option.option_description, option.option_value_name);


    QCommandLineParser cmd_parser;
    cmd_parser.addOption(config_file_option);
    cmd_parser.addOption(flash_xml_option);
    cmd_parser.addOption(operation_option);
    cmd_parser.addOption(cert_file_option);
    cmd_parser.addOption(com_port_option);
    cmd_parser.addOption(battery_mode_option);
    cmd_parser.addOption(reboot_option);
    cmd_parser.addOption(disable_storage_life_check_option);
    cmd_parser.addOption(auth_file_option);
    if (!ToolInfo::IsCustomerVer()) {
        cmd_parser.addOption(rsc_option);
        cmd_parser.addOption(android_test_mode_option);
    }
#ifdef _WIN32
    cmd_parser.addOption(redirect_option);
#endif
    cmd_parser.addOption(file_option);
    cmd_parser.addOption(da_log_channel_option);

    const QCommandLineOption helpOption = cmd_parser.addHelpOption();
    const QCommandLineOption versionOption = cmd_parser.addVersionOption();

    if (!cmd_parser.parse(cmd_args)) {
        m_error_text = cmd_parser.errorText();
        return CommandLineParseResult::CommandLineError;
    }

    if (cmd_parser.isSet(helpOption)) {
        m_help_info = cmd_parser.helpText();
        return CommandLineParseResult::CommandLineHelpRequested;
    }
    if (cmd_parser.isSet(versionOption)) {
        m_version_info = QString("%1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion());
        return CommandLineParseResult::CommandLineVersionRequested;
    }

    if (cmd_parser.isSet(config_file_option)) {
        m_config_xml_file_name = cmd_parser.value(config_file_option);
        m_console_mode_current_mode = ConsoleModeCurrentMode::ConfigXMLMode;
    } else {
        if (cmd_parser.isSet(flash_xml_option)) {
            m_flash_xml_file_name = cmd_parser.value(flash_xml_option);
        }
        if (cmd_parser.isSet(operation_option)) {
            m_cmd_name = cmd_parser.value(operation_option).toLower();
        }
        if (cmd_parser.isSet(auth_file_option)) {
            m_auth_file_name = cmd_parser.value(auth_file_option);
        }
        if (cmd_parser.isSet(cert_file_option)) {
            m_cert_file_name = cmd_parser.value(cert_file_option);
        }
        if (cmd_parser.isSet(battery_mode_option)) {
            m_battery_option = cmd_parser.value(battery_mode_option);
        }
        if (cmd_parser.isSet(rsc_option)) {
            m_rsc_proj_name = cmd_parser.value(rsc_option);
        }
        if (cmd_parser.isSet(file_option)) {
            m_file = cmd_parser.value(file_option);
        }
        if (cmd_parser.isSet(da_log_channel_option)) {
            m_da_log_channel = cmd_parser.value(da_log_channel_option);
        }
        m_reboot = cmd_parser.isSet(reboot_option);
        m_disable_storage_life_cycle_check = !cmd_parser.isSet(disable_storage_life_check_option);
        m_console_mode_current_mode = ConsoleModeCurrentMode::CmdArgumentMode;
    }
    if (cmd_parser.isSet(com_port_option)) {
        m_com_port_name = cmd_parser.value(com_port_option);
    }
#ifdef _WIN32
    m_redirect = cmd_parser.isSet(redirect_option);
#endif
    m_reboot_to_atm = cmd_parser.isSet(android_test_mode_option);

    return CommandLineParseResult::CommandLineOk;
}

bool CommandLineArguments::validArgumentValues() const
{
    assert(m_console_mode_current_mode != ConsoleModeCurrentMode::UnknownMode);
    //to valid argument option values
    if (m_console_mode_current_mode == ConsoleModeCurrentMode::ConfigXMLMode) {
        return validConfigXMLFile();
    } else {
        if (m_flash_xml_file_name.isEmpty() || m_cmd_name.isEmpty()) {
            LOGE("flash xml file and operation MUST be set together.");
            return false;
        }

        QStringList file_cmd_list;
        file_cmd_list << QStringLiteral("write-efuse") << QStringLiteral("read-efuse") << QStringLiteral("dram-repair");
        if (file_cmd_list.contains(m_cmd_name)) {
            if (m_file.isEmpty()) {
                LOGE("MUST specify --file argument when using %s command.", file_cmd_list.join(",").toUtf8().data());
                return false;
            }
        }

        QString error_msg;
        if (!FileUtils::validFile(m_flash_xml_file_name, error_msg)) {
            LOGE(qPrintable(error_msg));
            return false;
        }
        if (!CommandSetting::getSupportedInputCmds().contains(m_cmd_name)) {
            LOGE("Command \"%s\" is not support!", qPrintable(m_cmd_name));
            return false;
        }
        if(!m_com_port_name.isEmpty() && !valid_com_port(m_com_port_name))
        {
            LOGE("invalid com port name[%s].", qPrintable(m_com_port_name));
            return false;
        }
        if (!m_auth_file_name.isEmpty() && !FileUtils::validFile(m_auth_file_name, error_msg)) {
            LOGE(qPrintable(error_msg));
            return false;
        }
        if (!m_cert_file_name.isEmpty() && !FileUtils::validFile(m_cert_file_name, error_msg)) {
            LOGE(qPrintable(error_msg));
            return false;
        }
    }
    return true;
}

CommandLineArguments::ConsoleModeCurrentMode CommandLineArguments::getCurrentMode() const
{
    return m_console_mode_current_mode;
}

}
