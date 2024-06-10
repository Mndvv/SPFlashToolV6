#ifndef OPTIONINIFILEMGR_H
#define OPTIONINIFILEMGR_H

#include <QSharedPointer>
#include <QString>

class OptionIniFileMgr
{
public:
    virtual ~OptionIniFileMgr();
    static QSharedPointer<OptionIniFileMgr> getInstance();
    void saveOptionFile();


    bool getEnable_trace() const;
    void setEnable_trace(bool enable_trace);

    int getClear_older() const;
    void setClear_older(int clear_older);

    QString getDa_log_level() const;
    void setDa_log_level(const QString &da_log_level);

    QString getDa_log_channel() const;
    void setDa_log_channel(const QString &da_log_channel);

    int getBaud_rate() const;
    void setBaud_rate(int baud_rate);

    int getBattery() const;
    void setBattery(int battery);

    bool getPrint_driver_version() const;

    int getLanguage() const;
    void setLanguage(int language);

    int getDa_chksum_level() const;
    void setDa_chksum_level(int da_chksum_level);

    bool getRisk_reminder() const;

    bool getCheck_rpmb() const;
    void setCheck_rpmb(bool check_rpmb);

    bool getCheck_storage_life_cycle() const;
    void setCheck_storage_life_cycle(bool check_storage_life_cycle);

    bool getFmt_all_dl() const;

    bool getFirmware_upgrade() const;

    bool getDl_only() const;

    bool getShow_welcome() const;
    void setShow_welcome(bool show_welcome);

    bool getUpdate() const;
    void setUpdate(bool update);

    bool getRun_modemManager_cmd() const;
    bool getCheck_kernel_version() const;

    QString getLog_path() const;
    void setLog_path(const QString &log_path);

private:
    void loadOptionFile();

private:
    OptionIniFileMgr();
    OptionIniFileMgr(const OptionIniFileMgr &) = delete;
    OptionIniFileMgr &operator=(const OptionIniFileMgr &) = delete;
    OptionIniFileMgr(const OptionIniFileMgr &&) = delete;
    OptionIniFileMgr &operator=(const OptionIniFileMgr &&) = delete;

private:
    static QSharedPointer<OptionIniFileMgr> m_instance;

    //Trace section
    bool m_enable_trace;
    int m_clear_older;
    QString m_da_log_level;
    QString m_da_log_channel;
    QString m_log_path;

    //Conn section
    int m_baud_rate;
    int m_battery;
    bool m_print_driver_version;

    //language section
    int m_language;

    //download section
    int m_da_chksum_level;
    bool m_risk_reminder;
    bool m_check_rpmb;
    bool m_check_storage_life_cycle;

    //download scene section
    bool m_fmt_all_dl;
    bool m_firmware_upgrade;
    bool m_dl_only;

    //welcome section
    bool m_show_welcome;

    //Checkupdate section
    bool m_update;

    //modemManager section
    bool m_run_modemManager_cmd;
    bool m_check_kernel_version;
};

#endif // OPTIONINIFILEMGR_H
