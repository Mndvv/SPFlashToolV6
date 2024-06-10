#ifndef SETRUNTIMEPARAMETERCMDXMLGENERATOR_H
#define SETRUNTIMEPARAMETERCMDXMLGENERATOR_H

#include "ICmdXMLGenerator.h"
#include "../../Inc/CFlashToolTypes.h"

class SetRuntimeParaCmdXML : public ICmdXMLGenerator
{
public:
    SetRuntimeParaCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setChecksum_level(const ChkSumLevels &checksum_level);
    void setBattery_exist(const USB_POWER &usb_power);
    void setDa_log_level(const QString &da_log_level);
    void setDa_log_channel(const QString &da_log_channel);
    void setInitial_dram(bool initial_dram);

private:
    QString getBatteryStatus() const;
    QString getChkSumLevel() const;

private:
    ChkSumLevels m_checksum_level;
    USB_POWER m_battery_status;
    QString m_da_log_level;
    QString m_log_channel;
    QString m_system_os;
    bool m_initialize_dram;
};

#endif // SETRUNTIMEPARAMETERCMDXMLGENERATOR_H
