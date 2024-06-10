#include "PartitionTableParser.h"
#include "../XMLConst.h"
#include <QObject>
#include "../../Utility/FileUtils.h"

PartitionTableParser::PartitionTableParser():
    IXMLParser()
{

}

ReadbackPartitionInfoList PartitionTableParser::getRBList() const
{
    return m_rb_list;
}

bool PartitionTableParser::parseDocument(const QDomElement &root_node)
{
    bool parse_success = true;
    ReadbackPartitionInfoList rb_list;
    QDomElement element = root_node.firstChildElement();
    while (!element.isNull()) {
        QString tag_name = element.tagName().trimmed();
        if (tag_name == PT_NODE_NAME) {
            parse_success = this->parsePartitionNode(rb_list, element);
        }
        if (!parse_success) {
            break;
        }
        element = element.nextSiblingElement();
    }
    if (parse_success) {
        m_rb_list = rb_list;
    }
    return parse_success;
}

bool PartitionTableParser::parsePartitionNode(ReadbackPartitionInfoList &rb_list, const QDomElement &pt_node)
{
    QDomElement name_node = pt_node.firstChildElement(NAME_NODE_NAME);
    if (name_node.isNull()) {
        m_error_msg = QObject::tr("%1 node not exists.").arg(NAME_NODE_NAME);
        return false;
    }
    QDomElement start_node = pt_node.firstChildElement(START_NODE_NAME);
    if (name_node.isNull()) {
        m_error_msg = QObject::tr("%1 node not exists.").arg(START_NODE_NAME);
        return false;
    }
    QDomElement size_node = pt_node.firstChildElement(SIZE_NODE_NAME);
    if (size_node.isNull()) {
        m_error_msg = QObject::tr("%1 node not exists.").arg(SIZE_NODE_NAME);
        return false;
    }
    ReadbackPartitionInfo rb_info;
    QString partition_name = name_node.text().trimmed();
    rb_info.setPartition_name(partition_name);
    rb_info.setStartAddress(start_node.text().trimmed());
    rb_info.setLength(size_node.text().trimmed());
    rb_info.setFile_name(FileUtils::generateRBFileName(partition_name));
    rb_list.append(rb_info);
    return true;
}
