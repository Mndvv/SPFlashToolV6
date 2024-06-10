#include "SetRuntimeParaCmdXML.h"
#include "../XMLConst.h"
#include "../../Err/Exception.h"
#include "../../Utility/Utils.h"
#include "../../Inc/error_code.h"

SetRuntimeParaCmdXML::SetRuntimeParaCmdXML():
    ICmdXMLGenerator(SET_RUNTIME_PARAMETER_CMD_VER_VAL, SET_RUNTIME_PARAMETER_CMD_NAME_VAL, DA_NODE_NAME),
    m_checksum_level(ChkSumLevel::Chksum_NONE),
    m_battery_status(USB_POWER::AutoDetect),
    m_da_log_level("INFO"),
    m_log_channel("UART"),
    m_system_os(Utils::getSystemOS()),
    m_initialize_dram(true)
{

}

void SetRuntimeParaCmdXML::loadXML(const QDomElement &xml_node)
{
    QDomElement runtime_parameter_node = xml_node.firstChildElement(QStringLiteral("runtime-parameter"));
    if (runtime_parameter_node.isNull()) {
        THROW_APP_EXCEPTION(E_ERROR, QStringLiteral("runtime-parameter node not exists."))
    }
    int chksum_level = runtime_parameter_node.attribute(QStringLiteral("checksum_level"), QStringLiteral("0")).toInt();
    m_checksum_level = static_cast<ChkSumLevels>(chksum_level);
    int battery_status = runtime_parameter_node.attribute(QStringLiteral("battery_status"), QStringLiteral("0")).toInt();
    m_battery_status = static_cast<USB_POWER>(battery_status);
    m_da_log_level = runtime_parameter_node.attribute(QStringLiteral("da_log_level"), QStringLiteral("INFO"));
    m_log_channel = runtime_parameter_node.attribute(QStringLiteral("log_channel"), QStringLiteral("UART"));
    m_system_os = runtime_parameter_node.attribute(QStringLiteral("system_os"));
    QString initialize_dram = runtime_parameter_node.attribute(QStringLiteral("initialize_dram"), QStringLiteral("YES"));
    m_initialize_dram = initialize_dram == QStringLiteral("NO") ? false : true;
}

void SetRuntimeParaCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement runtime_parameter_node = xml_doc.createElement(QStringLiteral("runtime-parameter"));
    runtime_parameter_node.setAttribute(VERSION_NODE_NAME, SET_RUNTIME_PARAMETER_CMD_VER_VAL);
    xml_node.appendChild(runtime_parameter_node);

    runtime_parameter_node.setAttribute(QStringLiteral("checksum_level"), static_cast<int>(m_checksum_level));
    runtime_parameter_node.setAttribute(QStringLiteral("battery_status"), static_cast<int>(m_battery_status));
    runtime_parameter_node.setAttribute(QStringLiteral("da_log_level"), m_da_log_level);
    runtime_parameter_node.setAttribute(QStringLiteral("log_channel"), m_log_channel);
    runtime_parameter_node.setAttribute(QStringLiteral("system_os"), m_system_os);
    runtime_parameter_node.setAttribute(QStringLiteral("initialize_dram"), Utils::transBoolToYESNO(m_initialize_dram));
}

void SetRuntimeParaCmdXML::setChecksum_level(const ChkSumLevels &checksum_level)
{
    m_checksum_level = checksum_level;
}

void SetRuntimeParaCmdXML::setBattery_exist(const USB_POWER &usb_power)
{
    m_battery_status = usb_power;
}

void SetRuntimeParaCmdXML::setDa_log_level(const QString &da_log_level)
{
    m_da_log_level = da_log_level;
}

void SetRuntimeParaCmdXML::setDa_log_channel(const QString &da_log_channel)
{
    m_log_channel = da_log_channel;
}

void SetRuntimeParaCmdXML::setInitial_dram(bool initial_dram)
{
    m_initialize_dram = initial_dram;
}

QString SetRuntimeParaCmdXML::getBatteryStatus() const
{
    if (m_battery_status == USB_POWER::WithBattery) {
        return "YES";
    } else if (m_battery_status == USB_POWER::WithoutBattery) {
        return "NO";
    } else {
        return "AUTO-DETECT";
    }
}

QString SetRuntimeParaCmdXML::getChkSumLevel() const
{
    QString chksum_level;
    if (m_checksum_level.testFlag(Chksum_NONE)) {
        chksum_level = "NONE";
    } else {
        if (m_checksum_level.testFlag(Chksum_STORAGE)) {
            chksum_level = "STORAGE";
        }
        if (m_checksum_level.testFlag(Chksum_USB)) {
            chksum_level = chksum_level.isEmpty() ? "USB" : "USB-" + chksum_level;
        }
    }
    return chksum_level;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-RUNTIME-PARAMETER</command>
    <arg>
        <checksum_level>USB</checksum_level>
        <battery_exist>AUTO-DETECT</battery_exist>
        <da_log_level>INFO</da_log_level>
        <log_channel>UART</da_log_channel>
        <system_os>WINDOWS</system_os>
    </arg>
</da>
*/

/*
<da>
    <version>1.1</version>
    <command>CMD:SET-RUNTIME-PARAMETER</command>
    <arg>
        <checksum_level>USB</checksum_level>
        <battery_exist>AUTO-DETECT</battery_exist>
        <da_log_level>INFO</da_log_level>
        <log_channel>UART</log_channel>
        <system_os>WINDOWS</system_os>
    </arg>
    <adv>
        <initialize_dram>YES</initialize_dram>
    </adv>
</da>
*/
void SetRuntimeParaCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, CHECKSUM_LEVEL_NODE_NAME, getChkSumLevel());
    this->createXMLSubNode(arg_node, BATTERY_EXIST_NODE_NAME, this->getBatteryStatus());
    this->createXMLSubNode(arg_node, DA_LOG_LEVEL_NODE_NAME, m_da_log_level);
    this->createXMLSubNode(arg_node, LOG_CHANNEL_NODE_NAME, m_log_channel);
    this->createXMLSubNode(arg_node, SYSTEM_OS_NODE_NAME, m_system_os);
    QDomNode da_node = arg_node->parentNode();
    QDomElement adv_node = this->createXMLSubNode(&da_node, ADV_NODE_NAME, QStringLiteral(""));
    this->createXMLSubNode(&adv_node, INITIALIZE_DRAM_NODE_NAME, Utils::transBoolToYESNO(m_initialize_dram));
}
