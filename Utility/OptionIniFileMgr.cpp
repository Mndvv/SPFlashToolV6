#include "OptionIniFileMgr.h"
#include <QSettings>
#include "IniItem.h"
#include "Utils.h"
#include "FileUtils.h"

QSharedPointer<OptionIniFileMgr> OptionIniFileMgr::m_instance = QSharedPointer<OptionIniFileMgr>();

OptionIniFileMgr::OptionIniFileMgr():
    m_enable_trace(true), m_clear_older(2), m_da_log_level("INFO"),
    m_da_log_channel("UART"), m_log_path(Utils::GetSpecifalFolderPath().c_str()),
    m_baud_rate(3), m_battery(0), m_print_driver_version(false),
    m_language(0), m_da_chksum_level(0), m_risk_reminder(false),
    m_check_rpmb(false), m_check_storage_life_cycle(true),
    m_fmt_all_dl(true), m_firmware_upgrade(true), m_dl_only(true),
    m_show_welcome(true), m_update(true), m_run_modemManager_cmd(false),
    m_check_kernel_version(true)
{
    loadOptionFile();
}

QString OptionIniFileMgr::getLog_path() const
{
    return m_log_path;
}

void OptionIniFileMgr::setLog_path(const QString &log_path)
{
    m_log_path = log_path;
}

bool OptionIniFileMgr::getRun_modemManager_cmd() const
{
    return m_run_modemManager_cmd;
}

bool OptionIniFileMgr::getCheck_kernel_version() const
{
    return m_check_kernel_version;
}

bool OptionIniFileMgr::getUpdate() const
{
    return m_update;
}

void OptionIniFileMgr::setUpdate(bool update)
{
    m_update = update;
}

bool OptionIniFileMgr::getShow_welcome() const
{
    return m_show_welcome;
}

void OptionIniFileMgr::setShow_welcome(bool show_welcome)
{
    m_show_welcome = show_welcome;
}

bool OptionIniFileMgr::getDl_only() const
{
    return m_dl_only;
}

bool OptionIniFileMgr::getFirmware_upgrade() const
{
    return m_firmware_upgrade;
}

bool OptionIniFileMgr::getFmt_all_dl() const
{
    return m_fmt_all_dl;
}

bool OptionIniFileMgr::getCheck_storage_life_cycle() const
{
    return m_check_storage_life_cycle;
}

void OptionIniFileMgr::setCheck_storage_life_cycle(bool check_storage_life_cycle)
{
    m_check_storage_life_cycle = check_storage_life_cycle;
}

bool OptionIniFileMgr::getCheck_rpmb() const
{
    return m_check_rpmb;
}

void OptionIniFileMgr::setCheck_rpmb(bool check_rpmb)
{
    m_check_rpmb = check_rpmb;
}

bool OptionIniFileMgr::getRisk_reminder() const
{
    return m_risk_reminder;
}

int OptionIniFileMgr::getDa_chksum_level() const
{
    return m_da_chksum_level;
}

void OptionIniFileMgr::setDa_chksum_level(int da_chksum_level)
{
    m_da_chksum_level = da_chksum_level;
}

int OptionIniFileMgr::getLanguage() const
{
    return m_language;
}

void OptionIniFileMgr::setLanguage(int language)
{
    m_language = language;
}

bool OptionIniFileMgr::getPrint_driver_version() const
{
    return m_print_driver_version;
}

int OptionIniFileMgr::getBattery() const
{
    return m_battery;
}

void OptionIniFileMgr::setBattery(int battery)
{
    m_battery = battery;
}

int OptionIniFileMgr::getBaud_rate() const
{
    return m_baud_rate;
}

void OptionIniFileMgr::setBaud_rate(int baud_rate)
{
    m_baud_rate = baud_rate;
}

QString OptionIniFileMgr::getDa_log_channel() const
{
    return m_da_log_channel;
}

void OptionIniFileMgr::setDa_log_channel(const QString &da_log_channel)
{
    m_da_log_channel = da_log_channel;
}

QString OptionIniFileMgr::getDa_log_level() const
{
    return m_da_log_level;
}

void OptionIniFileMgr::setDa_log_level(const QString &da_log_level)
{
    m_da_log_level = da_log_level;
}

int OptionIniFileMgr::getClear_older() const
{
    return m_clear_older;
}

void OptionIniFileMgr::setClear_older(int clear_older)
{
    m_clear_older = clear_older;
}

bool OptionIniFileMgr::getEnable_trace() const
{
    return m_enable_trace;
}

void OptionIniFileMgr::setEnable_trace(bool enable_trace)
{
    m_enable_trace = enable_trace;
}

OptionIniFileMgr::~OptionIniFileMgr()
{

}

QSharedPointer<OptionIniFileMgr> OptionIniFileMgr::getInstance()
{
    // ONLY used in main thread, so NO NEED to care multi-thread condition.
    if (!m_instance) {
        m_instance = QSharedPointer<OptionIniFileMgr>(new OptionIniFileMgr());
    }
    return m_instance;
}

void OptionIniFileMgr::loadOptionFile()
{
    //EnableTrace section
    IniItem item("option.ini", "Trace", "EnableTrace");
    m_enable_trace = item.GetBooleanValue();
    item.SetItemName("CleanOlder");
    m_clear_older = item.GetIntValue();
    item.SetItemName("DALogLevel");
    m_da_log_level = item.GetStringValue();
    item.SetItemName("DALogChannel");
    m_da_log_channel = item.GetStringValue();
    item.SetItemName("LogPath");
    if (item.hasKey()) {
        m_log_path = item.GetStringValue();
    } else {
        m_log_path = QString::fromStdString(Utils::GetSpecifalFolderPath());
    }

    //Conn section
    item.SetSectionName("Conn");
    item.SetItemName("BaudRate");
    m_baud_rate = item.GetIntValue();
    item.SetItemName("Battery");
    m_battery = item.GetIntValue();
    item.SetItemName("PrintDriverVersion");
    m_print_driver_version = item.GetBooleanValue();

    //Language section
    item.SetSectionName("Language");
    item.SetItemName("CurrentLang");
    m_language = item.GetIntValue();

    //Download section
    item.SetSectionName("Download");
    item.SetItemName("DAChksum");
    m_da_chksum_level = item.GetIntValue();
    item.SetItemName("RiskReminder");
    m_risk_reminder = item.GetBooleanValue();
    item.SetItemName("CheckRPMB");
    m_check_rpmb = item.GetBooleanValue();
    item.SetItemName("StorageLifeCycleCheck");
    m_check_storage_life_cycle = item.GetBooleanValue();

    //DownloadScene section
    item.SetSectionName("DownloadScene");
    item.SetItemName("Format_All_Download");
    m_fmt_all_dl = item.GetBooleanValue();
    item.SetItemName("Firmware_Upgrade");
    m_firmware_upgrade = item.GetBooleanValue();
    item.SetItemName("Download_Only");
    m_dl_only = item.GetBooleanValue();

    //WelcomePage section
    item.SetSectionName("WelcomePage");
    item.SetItemName("ShowWelcome");
    m_show_welcome = item.GetBooleanValue();

    //CheckUpdate section
    item.SetSectionName("CheckUpdate");
    item.SetItemName("Update");
    m_update = item.GetBooleanValue();

    //ModemManager section
    item.SetSectionName("ModemManager");
    item.SetItemName("RunModemManagerCmd");
    m_run_modemManager_cmd = item.GetBooleanValue();
    item.SetItemName("CheckKernelVersion");
    m_check_kernel_version = item.GetBooleanValue();
}

void OptionIniFileMgr::saveOptionFile()
{
    QSettings settings(ABS_PATH_C("option.ini"), QSettings::IniFormat);

    //EnableTrace section
    settings.beginGroup(QStringLiteral("Trace"));
    settings.setValue(QStringLiteral("EnableTrace"), m_enable_trace);
    settings.setValue(QStringLiteral("CleanOlder"), m_clear_older);
    settings.setValue(QStringLiteral("DALogLevel"), m_da_log_level);
    settings.setValue(QStringLiteral("DALogChannel"), m_da_log_channel);
    settings.setValue(QStringLiteral("LogPath"), m_log_path);
    settings.endGroup();

    //Conn section
    settings.beginGroup(QStringLiteral("Conn"));
    settings.setValue(QStringLiteral("BaudRate"), m_baud_rate);
    settings.setValue(QStringLiteral("Battery"), m_battery);
    settings.setValue(QStringLiteral("PrintDriverVersion"), m_print_driver_version);
    settings.endGroup();

    //Language section
    settings.beginGroup(QStringLiteral("Language"));
    settings.setValue(QStringLiteral("CurrentLang"), m_language);
    settings.endGroup();

    //Download section
    settings.beginGroup(QStringLiteral("Download"));
    settings.setValue(QStringLiteral("DAChksum"), m_da_chksum_level);
    settings.setValue(QStringLiteral("RiskReminder"), m_risk_reminder);
    settings.setValue(QStringLiteral("CheckRPMB"), m_check_rpmb);
    settings.setValue(QStringLiteral("StorageLifeCycleCheck"), m_check_storage_life_cycle);
    settings.endGroup();

    //DownloadScene section
    settings.beginGroup(QStringLiteral("DownloadScene"));
    settings.setValue(QStringLiteral("Format_All_Download"), m_fmt_all_dl);
    settings.setValue(QStringLiteral("Firmware_Upgrade"), m_firmware_upgrade);
    settings.setValue(QStringLiteral("Download_Only"), m_dl_only);
    settings.endGroup();

    //WelcomePage section
    settings.beginGroup(QStringLiteral("WelcomePage"));
    settings.setValue(QStringLiteral("ShowWelcome"), m_show_welcome);
    settings.endGroup();

    //CheckUpdate section
    settings.beginGroup(QStringLiteral("CheckUpdate"));
    settings.setValue(QStringLiteral("Update"), m_update);
    settings.endGroup();

    //ModemManager section
    settings.beginGroup(QStringLiteral("ModemManager"));
    settings.setValue(QStringLiteral("RunModemManagerCmd"), m_run_modemManager_cmd);
    settings.setValue(QStringLiteral("CheckKernelVersion"), m_check_kernel_version);
    settings.endGroup();

    settings.sync();
}
