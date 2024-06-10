#include "XMLFileAssistant.h"
#include "../../Err/Exception.h"
#include "../../Inc/error_code.h"
#include <cassert>
#include <QDir>

QMap<QString, FILE_TYPE> XMLFileAssistant::m_file_type_map = {
    {"MEMORY_FILE", FILE_TYPE::MEMORY_FILE},
    {"LOCAL_FILE", FILE_TYPE::LOCAL_FILE},
    {"REMOTE_FILE", FILE_TYPE::REMOTE_FILE}
};

XMLFileAssistant::XMLFileAssistant():
    m_file_type(FILE_TYPE::LOCAL_FILE)
{

}

void XMLFileAssistant::setFileType(FILE_TYPE file_type)
{
    m_file_type = file_type;
}

void XMLFileAssistant::setFileName(const QString &file_name)
{
    m_file_name = file_name;
}

QString XMLFileAssistant::getFileDesc()
{
    QString source_desc;
    if (m_file_type == FILE_TYPE::MEMORY_FILE) {
        source_desc = QString("MEM://0x%1:0x%2").arg(getStartAddress()).arg(getLength());
    }
    else {
        source_desc = QDir::toNativeSeparators(m_file_name);
    }
    return source_desc;
}

void XMLFileAssistant::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    QDomElement xml_file_assistant_node = xml_node.firstChildElement(getXMLNodeName());
    if (xml_file_assistant_node.isNull()) {
        QString error_msg = QString("%1 node is NULL").arg(getXMLNodeName());
        THROW_APP_EXCEPTION(E_ERROR, qPrintable(error_msg))
    }
    m_file_name = xml_file_assistant_node.attribute(QStringLiteral("file_name"));
    QString file_type = xml_file_assistant_node.attribute(QStringLiteral("file_type"));
    m_file_type = m_file_type_map.value(file_type, FILE_TYPE::LOCAL_FILE);
}

void XMLFileAssistant::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement xml_file_assistant_node = xml_doc.createElement(getXMLNodeName());
    xml_file_assistant_node.setAttribute(VERSION_NODE_NAME, QStringLiteral("1.0"));
    xml_node.appendChild(xml_file_assistant_node);

    xml_file_assistant_node.setAttribute(QStringLiteral("file_type"), m_file_type_map.key(m_file_type));
    xml_file_assistant_node.setAttribute(QStringLiteral("file_name"), m_file_name);
}

void XMLSourceFileAssistant::setStartAddress(const QString &start_address)
{
    m_start_address = start_address;
}

void XMLSourceFileAssistant::setLength(const QString &length)
{
    m_length = length;
}

QString XMLSourceFileAssistant::getStartAddress()
{
    return m_start_address;
}

QString XMLSourceFileAssistant::getLength() const
{
    return m_length;
}

QString XMLSourceFileAssistant::getXMLNodeName() const
{
    return QStringLiteral("source-file");
}

QString XMLTargetFileAssistant::getCmdXMLValue() const
{
    return QString::fromStdString(m_xml_data.get());
}

QString XMLTargetFileAssistant::getStartAddress()
{
    if (!m_xml_data) {
        m_xml_data = std::shared_ptr<char>(new char[XML_DATA_MAX_LENGTH](), std::default_delete<char []>());
    }
    quint64 address = reinterpret_cast<quint64>(m_xml_data.get());
    QString start_address = QString("%1").arg(address, 0, 16);
    return start_address;
}

QString XMLTargetFileAssistant::getLength() const
{
    QString length = QString("%1").arg(static_cast<quint64>(XML_DATA_MAX_LENGTH), 0, 16);
    return length;
}

QString XMLTargetFileAssistant::getXMLNodeName() const
{
    return QStringLiteral("target-file");
}
