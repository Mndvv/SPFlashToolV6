#ifndef SETBOOTMODECMDXMLGENERATOR_H
#define SETBOOTMODECMDXMLGENERATOR_H

#include "ICmdXMLGenerator.h"
#include "../../Inc/CFlashToolTypes.h"
#include <QMap>

class SetBootModeCmdXML : public ICmdXMLGenerator
{
public:
    SetBootModeCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

protected:
    virtual QString getModeFieldVal() const = 0;
};

class MetaBootModeCmdXML : public SetBootModeCmdXML
{
public:
    MetaBootModeCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setMobile_log_on(bool mobile_log_on);
    void setAdb_on(bool adb_on);
    void setCom_type(const BOOT_MODE_CONNECT_TYPE &com_type);

protected:
    virtual QString getModeFieldVal() const override;

private:
    QString convertBoolToOnOff(bool value) const;
    bool convertOnOffToBool(const QString &value) const;

private:
    BOOT_MODE_CONNECT_TYPE m_boot_mode_conn_type;
    bool m_mobile_log_on;
    bool m_adb_on;

    static QMap<QString, BOOT_MODE_CONNECT_TYPE> m_boot_mode_conn_map;
};

class FastBootModeCmdXML: public SetBootModeCmdXML
{
public:
    FastBootModeCmdXML() {}

protected:
    virtual QString getModeFieldVal() const override;
};

class AndroidTestModeCmdXMl: public SetBootModeCmdXML
{
public:
    AndroidTestModeCmdXMl() {}

protected:
    virtual QString getModeFieldVal() const override;
};

#endif // SETBOOTMODECMDXMLGENERATOR_H
