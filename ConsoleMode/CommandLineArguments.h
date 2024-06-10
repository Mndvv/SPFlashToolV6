#ifndef COMMANDLINEARGUMENTS_H
#define COMMANDLINEARGUMENTS_H

#include <QString>
#include <QCommandLineParser>

namespace ConsoleMode
{

class CommandLineArguments
{
public:
    enum class CommandLineParseResult: unsigned char {
        CommandLineOk = 0,
        CommandLineError,
        CommandLineVersionRequested,
        CommandLineHelpRequested
    };

    enum class ConsoleModeCurrentMode: unsigned char {
        UnknownMode = 0,
        ConfigXMLMode,
        CmdArgumentMode
    };

public:
    CommandLineArguments();

    const QString &getConfigXMLFileName() const;
    const QString &getFlashXMLFilename() const;
    const QString &getCommandName() const;
    const QString &getAuthFilename() const;
    const QString &getCertFilename() const;
    const QString &getComPortName() const;
    bool needRedirect() const;
    const QString & getRSCProjName() const;
    bool validConfigXMLFile() const;
    const QString &getFile() const;

    bool needReboot() const{
        return m_reboot;
    }

    const QString & getBatteryOption() const{
        return m_battery_option;
    }

    bool needStorageLifeCycleCheck() const
    {
        return m_disable_storage_life_cycle_check;
    }

    bool reboot_to_atm() const {
        return m_reboot_to_atm;
    }

    const QString & getDALogChannel() const { return m_da_log_channel; }

    CommandLineParseResult parseCmdLine(const QStringList &cmd_args);
    QString getVersionInfo() const { return m_version_info; }
    QString getHelpInfo() const { return m_help_info; }
    QString getParseErrorMsg() const { return  m_error_text; }
    bool validArgumentValues() const;
    ConsoleModeCurrentMode getCurrentMode() const;

private:
    bool valid_com_port(const QString &usb_com_port) const;

private:
    QString m_config_xml_file_name;
    QString m_flash_xml_file_name;
    QString m_auth_file_name;
    QString m_cert_file_name;
    QString m_cmd_name;
    QString m_com_port_name;
    QString m_file;
    bool m_redirect;
    bool m_reboot;
    bool m_disable_storage_life_cycle_check;
    bool m_reboot_to_atm;
    QString m_rsc_proj_name;
    QString m_battery_option;
    QString m_da_log_channel;

    QString m_error_text;
    QString m_help_info;
    QString m_version_info;
    ConsoleModeCurrentMode m_console_mode_current_mode;
};

}

#endif // COMMANDLINEARGUMENTS_H
