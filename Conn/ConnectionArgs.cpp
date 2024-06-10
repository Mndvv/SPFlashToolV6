#include "ConnectionArgs.h"
#include <cassert>
#include <memory>
#include "../Err/Exception.h"
#include "../Inc/error_code.h"
#include "../XML/Parser/FlashXMLParser.h"

QMap<QString, COM_PORT_TYPE> ConnectionArgs::m_com_port_type_map = {
    {"USB-PCIE", COM_PORT_TYPE::COM_PORT_USB_PCIE},
    {"UART", COM_PORT_TYPE::COM_PORT_UART},
    {"unknown", COM_PORT_TYPE::COM_PORT_UNKNOWN}
};

QMap<QString, CONN_END_STAGE> ConnectionArgs::m_conn_stage_map = {
    {"brom", CONN_END_STAGE::BROM_STAGE},
    {"1st DA", CONN_END_STAGE::FIRST_DA_STAGE},
    {"2nd DA", CONN_END_STAGE::SECOND_DA_STAGE},
    {"unknown", CONN_END_STAGE::CONN_UNKNOWN}
};

ConnectionArgs::ConnectionArgs():
    m_com_port_type(COM_PORT_TYPE::COM_PORT_UNKNOWN),
    m_baud_rate("115200"),
    m_conn_end_stage(CONN_END_STAGE::CONN_UNKNOWN),
    m_timeout_ms(60 * 60 * 1000)
{

}

void ConnectionArgs::loadXML(const QDomElement &xml_node)
{
    QDomElement conn_args_node = xml_node.firstChildElement("connect-args");
    if (conn_args_node.isNull()) {
        THROW_APP_EXCEPTION(E_ERROR, QStringLiteral("connect-args node not exists."))
    }
    QString com_port_type = conn_args_node.attribute("com_port_type", "USB-PCIE");
    m_com_port_type = m_com_port_type_map.value(com_port_type);
    m_com_port_name = conn_args_node.attribute("com_port_name");
    m_baud_rate = conn_args_node.attribute("baud_rate", "115200");
    QString conn_stage = conn_args_node.attribute("conn_stage", "2nd DA");
    m_conn_end_stage = m_conn_stage_map.value(conn_stage);
    m_timeout_ms = conn_args_node.attribute("timeout_ms", "3600000").toInt();

    parseNodeValue(m_flash_xml_file, conn_args_node, "flash-xml");
    parseNodeValue(m_auth_file, conn_args_node, "auth-file");
    parseNodeValue(m_cert_file, conn_args_node, "cert-file");
    parseNodeValue(m_da_file, conn_args_node, "da-file", false);
}

void ConnectionArgs::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement conn_args_node = xml_doc.createElement("connect-args");
    conn_args_node.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    xml_node.appendChild(conn_args_node);

    conn_args_node.setAttribute("com_port_type", m_com_port_type_map.key(m_com_port_type));
    conn_args_node.setAttribute("com_port_name", m_com_port_name);
    conn_args_node.setAttribute("baud_rate", m_baud_rate);
    conn_args_node.setAttribute("conn_stage", m_conn_stage_map.key(m_conn_end_stage));
    conn_args_node.setAttribute("timeout_ms", m_timeout_ms);

    this->appendChildTextNode(xml_doc, conn_args_node, "flash-xml", m_flash_xml_file);
    this->appendChildTextNode(xml_doc, conn_args_node, "auth-file", m_auth_file);
    this->appendChildTextNode(xml_doc, conn_args_node, "cert-file", m_cert_file);
    this->appendChildTextNode(xml_doc, conn_args_node, "da-file", m_da_file);
}

void ConnectionArgs::parseNodeValue(QString &tag_value, const QDomElement &root_node,
                                    const QString &tag_name, bool b_must_exist) const
{
    QDomElement node = root_node.firstChildElement(tag_name);
    if (b_must_exist && node.isNull()) {
        THROW_APP_EXCEPTION(E_ERROR, QStringLiteral("%1 tag name not exist.").arg(tag_name))
    }
    if (!node.isNull()) {
        tag_value = node.text().trimmed();
    }
}

void ConnectionArgs::appendChildTextNode(QDomDocument &xml_doc, QDomElement &parent_node,
                                         const QString &tag_name, const QString &tag_value) const
{
    QDomElement sub_name = xml_doc.createElement(tag_name);
    QDomText text_node = xml_doc.createTextNode(tag_value);
    sub_name.appendChild(text_node);
    parent_node.appendChild(sub_name);
}

const QString &ConnectionArgs::get_flash_xml_file() const
{
    assert(!m_flash_xml_file.isEmpty());
    return m_flash_xml_file;
}

QString ConnectionArgs::get_conn_flash_xml_file() const
{
    assert(!m_flash_xml_file.isEmpty());
    if (m_da_file.isEmpty()) {
        return m_flash_xml_file;
    } else {
        FlashXMLParser flash_xml_parser;
        QString mem_flash_xml;
#define MAX_FLASH_XML_LEN 64 * 1024
        static std::unique_ptr<char> flash_xml(new char[MAX_FLASH_XML_LEN]);
        if (flash_xml_parser.parseXMLFile(m_flash_xml_file)) {
            flash_xml_parser.setDAFile(m_da_file);
            qstrcpy(flash_xml.get(), flash_xml_parser.getXMLString().toStdString().c_str());
            quint64 addr = reinterpret_cast<quint64>(flash_xml.get());
            mem_flash_xml = QStringLiteral("MEM://0x%1:0x%2").arg(addr, 0, 16).arg(MAX_FLASH_XML_LEN, 0, 16);
        }
        return mem_flash_xml;
    }
}

const QString &ConnectionArgs::get_auth_file() const
{
    return m_auth_file;
}

const QString &ConnectionArgs::get_cert_file() const
{
    return m_cert_file;
}

QString ConnectionArgs::get_brom_conn_options() const
{
    if (m_com_port_type == COM_PORT_TYPE::COM_PORT_UART) {
        return m_baud_rate + QStringLiteral("@");
    }
    return m_baud_rate;
}

QString ConnectionArgs::get_flash_mode() const
{
    QString flash_mode;
    if (m_conn_end_stage == CONN_END_STAGE::FIRST_DA_STAGE) {
        flash_mode = QStringLiteral("FLASH-MODE-DA-SRAM");
    } else if (m_conn_end_stage == CONN_END_STAGE::SECOND_DA_STAGE) {
        flash_mode = QStringLiteral("FLASH-MODE-DA");
    } else {
        assert(false);
    }
    return flash_mode;
}

QString ConnectionArgs::get_com_port_clue() const
{
    QString com_port_clue QStringLiteral("COM://");
    if (!m_com_port_name.isEmpty()) {
        com_port_clue += m_com_port_name;
    }
    return com_port_clue;
}

COM_PORT_TYPE ConnectionArgs::get_com_port_type() const
{
    return m_com_port_type;
}

CONN_END_STAGE ConnectionArgs::get_conn_end_strage() const
{
    return m_conn_end_stage;
}

void ConnectionArgs::set_flash_xml_file(const QString &flash_xml_file)
{
    this->m_flash_xml_file = flash_xml_file;
}

void ConnectionArgs::set_auth_file(const QString &auth_file)
{
    this->m_auth_file = auth_file;
}

void ConnectionArgs::set_cert_file(const QString &cert_file)
{
    this->m_cert_file = cert_file;
}

void ConnectionArgs::set_da_file(const QString &da_file)
{
    m_da_file = da_file;
}

void ConnectionArgs::set_com_port_type(COM_PORT_TYPE com_port_type)
{
    m_com_port_type = com_port_type;
}

void ConnectionArgs::set_baud_rate(const QString &baud_rate)
{
    m_baud_rate = baud_rate;
}

void ConnectionArgs::set_com_port_name(const QString &com_port_name)
{
    m_com_port_name = com_port_name;
}

void ConnectionArgs::set_conn_end_stage(CONN_END_STAGE conn_end_strage)
{
    m_conn_end_stage = conn_end_strage;
}

int ConnectionArgs::getTimeout_ms() const
{
    return m_timeout_ms;
}
