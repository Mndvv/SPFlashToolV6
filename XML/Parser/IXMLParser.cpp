#include "IXMLParser.h"
#include <QFileInfo>
#include <QFile>
#include "../../Utility/FileUtils.h"
#include "../../Logger/Log.h"

static const QString xml_err_msg = QStringLiteral("%1 is invalid.\nPlease select the correct %1.");

IXMLParser::IXMLParser()
{

}

bool IXMLParser::parseXMLString(const QString &xml_string)
{
    bool parse_success = false;
    m_error_msg.clear();
    m_xml_doc.clear();
    try {
        if (xml_string.trimmed().isEmpty()) {
            m_error_msg = QObject::tr("xml string is empty.");
            return parse_success;
        }
        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!m_xml_doc.setContent(xml_string, &errorStr, &errorLine, &errorColumn)) {
            m_error_msg = QObject::tr("%1,\nerror line: %2,\nerror column: %3")
                           .arg(errorStr).arg(errorLine).arg(errorColumn);
            return parse_success;
        }
        QDomElement root = m_xml_doc.documentElement();
        parse_success = this->parseDocument(root);
    } catch (const std::exception &e) {
        m_error_msg = e.what();
        parse_success = false;
    }
    return parse_success;
}

bool IXMLParser::parseXMLFile(const QString &xml_file_name)
{
    bool parse_success = false;
    m_error_msg.clear();
    m_xml_doc.clear();
    try {
        parse_success = false;
        if (!FileUtils::validFile(xml_file_name, m_error_msg)) {
            return parse_success;
        }
        if (!isValidXMLSchema(xml_file_name)) {
            if (m_error_msg.isEmpty()) {
                m_error_msg = xml_err_msg.arg(getFileNameOnly(xml_file_name));
            }
            return false;
        }
        QFile file(xml_file_name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LOGE("open %s failed.", xml_file_name.toStdString().c_str());
            m_error_msg = xml_err_msg.arg(getFileNameOnly(xml_file_name));
            return parse_success;
        }
        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!m_xml_doc.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
            LOGE("%s, error line: %d, error column: %d", qPrintable(errorStr), errorLine, errorColumn);
            m_error_msg = xml_err_msg.arg(getFileNameOnly(xml_file_name));
            return parse_success;
        }
        m_xml_file_path = QFileInfo(xml_file_name).absolutePath();
        QDomElement root = m_xml_doc.documentElement();
        parse_success = this->parseDocument(root);
        if (!parse_success) {
            LOGE(qPrintable(m_error_msg));
            m_error_msg = xml_err_msg.arg(getFileNameOnly(xml_file_name));
        }
    } catch (const std::exception &e) {
        LOGE(e.what());
        m_error_msg = xml_err_msg.arg(getFileNameOnly(xml_file_name));
        parse_success = false;
    }
    return parse_success;
}

QString IXMLParser::get_error_msg() const
{
    return m_error_msg;
}

QString IXMLParser::getXMLString()
{
    return m_xml_doc.toString(4);
}

bool IXMLParser::isValidXMLSchema(const QString &file_name)
{
    Q_UNUSED(file_name)
    return true;
}

QString IXMLParser::getFileNameOnly(const QString &file_name) const
{
    return QFileInfo(file_name).fileName();
}
