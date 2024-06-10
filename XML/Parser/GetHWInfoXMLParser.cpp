#include "GetHWInfoXMLParser.h"
#include "../XMLConst.h"

GetHWInfoXMLParser::GetHWInfoXMLParser():
    IXMLParser(),
    m_hw_info_tree(new HWInfoContainerNode())
{

}

HWInfoTree::HWInfoTree_ptr GetHWInfoXMLParser::get_hw_info() const
{
    return m_hw_info_tree;
}

bool GetHWInfoXMLParser::parseDocument(const QDomElement &root_node)
{
    QDomElement element = root_node.firstChildElement();
    while (!element.isNull()) {
        QString tag_name = element.tagName().trimmed();
        if (tag_name == VERSION_NODE_NAME) {
            element = element.nextSiblingElement();
            continue;
        }
        this->parseElement(element, m_hw_info_tree);
        element = element.nextSiblingElement();
    }
    return true;
}

bool GetHWInfoXMLParser::parseElement(const QDomElement &element, QSharedPointer<HWInfoContainerNode> &container_node)
{
    QString tag_name = element.tagName().trimmed();
    QDomElement sub_element = element.firstChildElement();
    if (sub_element.isNull()) {
        QString tag_value = element.text().trimmed();
        HWInfoTree::HWInfoTree_ptr leaf_node = QSharedPointer<HWInfoLeafNode>(new HWInfoLeafNode(tag_name, tag_value));
        container_node->add(leaf_node);
    } else {
        QSharedPointer<HWInfoContainerNode> sub_tree(new HWInfoContainerNode(tag_name, QStringLiteral("see %1 info").arg(tag_name)));
        container_node->add(sub_tree);
        do {
            parseElement(sub_element, sub_tree);
            sub_element = sub_element.nextSiblingElement();
        } while (!sub_element.isNull());
    }
    return true;
}
