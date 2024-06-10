#include "FlashXMLParser.h"
#include "../XMLConst.h"
#include <QObject>
#include <QDir>
#include <QFileInfo>
#include "../../Utility/FileUtils.h"
#include "../../Logger/Log.h"
#include "../../ConsoleMode/SchemaValidator.h"

const static QString FLASH_XML_XSD_FILE = QStringLiteral("flash.xsd");

FlashXMLParser::FlashXMLParser():
    IXMLParser()
{

}

QString FlashXMLParser::getScatterFileName() const
{
    return m_xml_scatter_file;
}

QString FlashXMLParser::getContact() const
{
    return m_contact;
}

void FlashXMLParser::setDAFile(const QString &da_file)
{
    m_da_file = da_file;
}

QString FlashXMLParser::getXMLString()
{
    this->replaceDAFile();
    return IXMLParser::getXMLString();
}

bool FlashXMLParser::parseDocument(const QDomElement &root_node)
{
    QString value;
    bool parse_result = parseNodeValue(value, root_node, SCATTER_NODE_NAME);
    if (!parse_result) return parse_result;
    m_xml_scatter_file = this->convertToAbsolutePath(value);
    parse_result = parseNodeValue(value, root_node, CONTACT_NODE_NAME);
    if (!parse_result) return parse_result;
    m_contact = value;
    return parse_result;
}

bool FlashXMLParser::isValidXMLSchema(const QString &file_name)
{
    QString file_path = QFileInfo(file_name).absolutePath();
    QString flash_xsd_file = file_path + QDir::separator() + FLASH_XML_XSD_FILE;
    flash_xsd_file = QDir::toNativeSeparators(flash_xsd_file);
    ConsoleMode::SchemaValidator schemaValidator(flash_xsd_file);
    try
    {
        schemaValidator.Validate(file_name);
    }
    catch (...)
    {
        QString error_msg;
        FileUtils::validFile(flash_xsd_file, error_msg);
        m_error_msg = QStringLiteral("%1 is invalid.\nPlease select the ./download_agent/flash.xml in the load.").arg(file_name);
        if (error_msg.isEmpty()) {
            LOGE(qPrintable(m_error_msg));
        } else {
            LOGE(qPrintable(error_msg));
        }
        return false;
    }
    return true;
}

bool FlashXMLParser::parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name)
{
    QDomElement node = root_node.firstChildElement(tag_name);
    if (node.isNull()) {
        m_error_msg = QObject::tr("%1 node is not exist.").arg(tag_name);
        return false;
    }
     tag_value = node.text().trimmed();
     return true;
}

QString FlashXMLParser::convertToAbsolutePath(const QString &file_path) const
{
    if (QFileInfo(file_path).isAbsolute()) {
        return file_path;
    }
    QString abs_file_path = m_xml_file_path + QDir::separator() + file_path;
    abs_file_path = QFileInfo(abs_file_path).absoluteFilePath();
    abs_file_path = QDir::toNativeSeparators(abs_file_path);
    return abs_file_path;
}

void FlashXMLParser::replaceDAFile()
{
    if (m_da_file.isEmpty()) {
        return;
    }
    m_da_file = this->convertToAbsolutePath(m_da_file);

    QDomElement root = m_xml_doc.documentElement();
    QDomElement old_da_node = root.firstChildElement(QStringLiteral("dagent"));
    QDomElement new_da_node = m_xml_doc.createElement(QStringLiteral("dagent"));
    QDomText newDAText = m_xml_doc.createTextNode(m_da_file);
    new_da_node.appendChild(newDAText);
    root.replaceChild(new_da_node, old_da_node);
}
