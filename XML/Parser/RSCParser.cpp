#include "RSCParser.h"
#include <QObject>
#include "../../XML/XMLConst.h"

#define RSC_MAGIC "0x5253434D"

RSCParser::RSCParser(): IXMLParser()
{

}

QStringList RSCParser::getRSCProjNames() const
{
    return m_rsc_proj_name_list;
}

bool RSCParser::parseDocument(const QDomElement &root_node)
{
    bool parse_success = true;
    QStringList proj_name_list;
    QDomElement element = root_node.firstChildElement();
    while (!element.isNull()) {
        std::string tag_name = element.tagName().trimmed().toStdString();
        if (tag_name == MAGIC_NODE_NAME) {
            parse_success = this->parseMagicNode(element);
        } else if (tag_name == PROJ_INFO_NODE_NAME) {
            parse_success = this->parseProjInfoNode(proj_name_list, element);
        }
        if (!parse_success) {
            break;
        }
        element = element.nextSiblingElement();
    }
    if (parse_success) {
        m_rsc_proj_name_list = proj_name_list;
    }
    return parse_success;
}

bool RSCParser::parseMagicNode(const QDomElement &magic_node)
{
    QString magic_value = magic_node.text().trimmed();
    if (magic_value != RSC_MAGIC) {
        m_error_msg = QObject::tr("Invalid magic number, %1 != %2(expected).").
                arg(magic_value.toStdString().c_str()).arg(RSC_MAGIC);
        return false;
    }
    return true;
}

bool RSCParser::parseProjInfoNode(QStringList &proj_name_list, const QDomElement &proj_info_node)
{
    bool parse_success = true;
    QDomElement proj_item_element = proj_info_node.firstChildElement();
    while (!proj_item_element.isNull()) {
        std::string tag_name = proj_item_element.tagName().trimmed().toStdString();
        assert(tag_name == PROJ_ITEM_NODE_NAME);
        parse_success = this->parseProjItemNode(proj_name_list, proj_item_element);
        if (!parse_success) {
            break;
        }
        proj_item_element = proj_item_element.nextSiblingElement();
    }
    return parse_success;
}

bool RSCParser::parseProjItemNode(QStringList &proj_name_list, const QDomElement &proj_item_node)
{
    QDomElement name_node = proj_item_node.firstChildElement(NAME_NODE_NAME);
    if (name_node.isNull()) {
        QString index = proj_item_node.attribute(INDEX_NODE_NAME);
        m_error_msg = QObject::tr("project item(index=%1) has NOT name node.").arg(index.toStdString().c_str());
        return false;
    }
    proj_name_list.append(name_node.text().trimmed());
    return true;
}
