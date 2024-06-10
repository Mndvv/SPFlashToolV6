#include "RebootCmdXML.h"
#include "../XMLConst.h"
#include "../../Utility/version.h"

QMap<QString, REBOOT_TYPE> RebootCmdXML::m_reboot_map = {
    {"IMMEDIATE", REBOOT_TYPE::IMMEDIATE},
    {"COLD-RESET", REBOOT_TYPE::PMIC_COLD_RESET},
    {"DISCONNECT", REBOOT_TYPE::DISCONNECT}
};


RebootCmdXML::RebootCmdXML():
    ICmdXMLGenerator(REBOOT_CMD_VER_VAL, REBOOT_CMD_NAME_VAL, DA_NODE_NAME),
    m_reboot_type(REBOOT_TYPE::IMMEDIATE)
{

}

void RebootCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    QString value = xml_node.attribute(QStringLiteral("action"));
    m_reboot_type = m_reboot_map.value(value, ToolInfo::IsCustomerVer() ? REBOOT_TYPE::DISCONNECT : REBOOT_TYPE::IMMEDIATE);
}

void RebootCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement reboot_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(REBOOT_CMD_NAME_VAL));
    reboot_node.setAttribute(VERSION_NODE_NAME, REBOOT_CMD_VER_VAL);
    xml_node.appendChild(reboot_node);

    reboot_node.setAttribute(QStringLiteral("action"), m_reboot_map.key(m_reboot_type,
        ToolInfo::IsCustomerVer() ? QStringLiteral("DISCONNECT") : QStringLiteral("IMMEDIATE")));
}

void RebootCmdXML::setRebootType(REBOOT_TYPE reboot_type)
{
    m_reboot_type = reboot_type;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:REBOOT</command>
    <arg>
        <action>IMMEDIATE</action>
    </arg>
</da>
*/
void RebootCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    if(ToolInfo::IsCustomerVer() && m_reboot_type != REBOOT_TYPE::DISCONNECT) {
        m_reboot_type = REBOOT_TYPE::IMMEDIATE;
    }
    this->createXMLSubNode(arg_node, ACTION_NODE_NAME, m_reboot_map.key(m_reboot_type,
        ToolInfo::IsCustomerVer() ? QStringLiteral("DISCONNECT") : QStringLiteral("IMMEDIATE")));
}
