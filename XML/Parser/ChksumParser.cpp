#include "ChksumParser.h"
#include <QFileInfo>

LoadChksumParser::LoadChksumParser()
{

}

QHash<QString, QString> LoadChksumParser::getChksumHash() const
{
    return m_chksum_hash;
}

bool LoadChksumParser::parseDocument(const QDomElement &root_node)
{
    bool parse_success = true;
    QStringList proj_name_list;
    QDomElement images_element = root_node.firstChildElement(QStringLiteral("images"));
    assert(!images_element.isNull());
    QDomElement element = images_element.firstChildElement(QStringLiteral("file"));
    int n_index = 0;
    while (!element.isNull()) {
        if (!element.hasAttribute(QStringLiteral("name"))) {
            parse_success = false;
            m_error_msg = QStringLiteral("%1(index %2) node has NO %3 attribute")
                    .arg(QStringLiteral("file")).arg(n_index).arg(QStringLiteral("name"));
            break;
        }
        QString hash_key = element.attribute(QStringLiteral("name"));
        QString hash_value = element.attribute(QStringLiteral("checksum"));
        m_chksum_hash.insert(hash_key, hash_value);
        element = element.nextSiblingElement();
    }
    return parse_success;
}

DeviceChksumParser::DeviceChksumParser()
{

}

QHash<QString, QPair<QString, QString> > DeviceChksumParser::getChksumHash() const
{
    return m_chksum_hash;
}

bool DeviceChksumParser::parseDocument(const QDomElement &root_node)
{
    bool parse_success = true;
    QStringList proj_name_list;
    QDomElement element = root_node.firstChildElement(QStringLiteral("item"));
    int n_index = 0;
    while (!element.isNull()) {
        if (!element.hasAttribute(QStringLiteral("file"))) {
            parse_success = false;
            m_error_msg = QStringLiteral("%1(index %2) node has NO %3 attribute")
                    .arg(QStringLiteral("item")).arg(n_index).arg(QStringLiteral("file"));
            break;
        }
        QString hash_key = element.attribute(QStringLiteral("file"));
        QString chksum_value = element.attribute(QStringLiteral("image_chksum"));
        QString pt_value = element.attribute(QStringLiteral("pt"));
        m_chksum_hash.insert(this->getFileNameOnly(hash_key), qMakePair(chksum_value, pt_value));
        element = element.nextSiblingElement();
    }
    return parse_success;
}

QString DeviceChksumParser::getFileNameOnly(const QString &file_name) const
{
    return QFileInfo(file_name).fileName();
}
