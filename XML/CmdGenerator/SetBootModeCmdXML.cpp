#include "SetBootModeCmdXML.h"
#include "../XMLConst.h"

QMap<QString, BOOT_MODE_CONNECT_TYPE> MetaBootModeCmdXML::m_boot_mode_conn_map = {
    {"USB", BOOT_MODE_CONNECT_TYPE::CONN_TYPE_USB},
    {"UART", BOOT_MODE_CONNECT_TYPE::CONN_TYPE_UART},
    {"WIFI", BOOT_MODE_CONNECT_TYPE::CONN_TYPE_WIFI}
};

SetBootModeCmdXML::SetBootModeCmdXML():
    ICmdXMLGenerator(SET_BOOT_MODE_CMD_VER_VAL, SET_BOOT_MODE_CMD_NAME_VAL, DA_NODE_NAME)
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-BOOT-MODE</command>
    <arg>
        <mode>META</mode>
        <connect_type>WIFI</connect_type>
        <mobile_log>ON</mobile_log>
        <adb>ON</adb>
    </arg>
</da>

<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-BOOT-MODE</command>
    <arg>
        <mode>FASTBOOT</mode>
    </arg>
</da>

<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-BOOT-MODE</command>
    <arg>
        <mode>ANDROID-TEST-MODE</mode>
    </arg>
</da>
*/
void SetBootModeCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, MODE_NODE_NAME, this->getModeFieldVal());
}

MetaBootModeCmdXML::MetaBootModeCmdXML():
    SetBootModeCmdXML(),
    m_boot_mode_conn_type(BOOT_MODE_CONNECT_TYPE::CONN_TYPE_USB),
    m_mobile_log_on(true),
    m_adb_on(false)
{

}

void MetaBootModeCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    QString value = xml_node.attribute(QStringLiteral("connect_type"));
    m_boot_mode_conn_type = m_boot_mode_conn_map.value(value, BOOT_MODE_CONNECT_TYPE::CONN_TYPE_USB);
    value = xml_node.attribute(QStringLiteral("mobile_log"), QStringLiteral("on"));
    m_mobile_log_on = this->convertOnOffToBool(value);
    value = xml_node.attribute(QStringLiteral("adb"), QStringLiteral("off"));
    m_adb_on = this->convertOnOffToBool(value);
}

void MetaBootModeCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement set_boot_mode_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(SET_BOOT_MODE_CMD_NAME_VAL));
    set_boot_mode_node.setAttribute(VERSION_NODE_NAME, SET_BOOT_MODE_CMD_VER_VAL);
    xml_node.appendChild(set_boot_mode_node);

    set_boot_mode_node.setAttribute(QStringLiteral("connect_type"), m_boot_mode_conn_map.key(m_boot_mode_conn_type, QStringLiteral("USB")));
    set_boot_mode_node.setAttribute(QStringLiteral("mobile_log"), this->convertBoolToOnOff(m_mobile_log_on));
    set_boot_mode_node.setAttribute(QStringLiteral("adb"), this->convertBoolToOnOff(m_adb_on));
}

QString MetaBootModeCmdXML::convertBoolToOnOff(bool value) const
{
    return value ? "ON" : "OFF";
}

bool MetaBootModeCmdXML::convertOnOffToBool(const QString &value) const
{
    return value.trimmed().toLower() == "on";
}

void MetaBootModeCmdXML::setCom_type(const BOOT_MODE_CONNECT_TYPE &com_type)
{
    m_boot_mode_conn_type = com_type;
}

QString MetaBootModeCmdXML::getModeFieldVal() const
{
    return QStringLiteral("META");
}

void MetaBootModeCmdXML::setAdb_on(bool adb_on)
{
    m_adb_on = adb_on;
}

void MetaBootModeCmdXML::setMobile_log_on(bool mobile_log_on)
{
    m_mobile_log_on = mobile_log_on;
}

void MetaBootModeCmdXML::fillArgNode(QDomElement * const arg_node)
{
    SetBootModeCmdXML::fillArgNode(arg_node);
    this->createXMLSubNode(arg_node, CONNECT_TYPE_NODE_NAME, m_boot_mode_conn_map.key(m_boot_mode_conn_type, "USB"));
    this->createXMLSubNode(arg_node, MOBILE_LOG_NODE_NAME, this->convertBoolToOnOff(m_mobile_log_on));
    this->createXMLSubNode(arg_node, ADB_NODE_NAME, this->convertBoolToOnOff(m_adb_on));
}

QString FastBootModeCmdXML::getModeFieldVal() const
{
    return QStringLiteral("FASTBOOT");
}

QString AndroidTestModeCmdXMl::getModeFieldVal() const
{
    return QStringLiteral("ANDROID-TEST-MODE");
}
