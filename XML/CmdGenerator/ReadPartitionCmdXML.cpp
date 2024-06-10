#include "ReadPartitionCmdXML.h"
#include "../XMLConst.h"

ReadPartitionCmdXML::ReadPartitionCmdXML():
    ICmdXMLGenerator(READPARTITION_CMD_VER_VAL, READ_PARTITION_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void ReadPartitionCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_partition = xml_node.attribute(QStringLiteral("partition"));
    m_target_file_assist->loadXML(xml_node);
}

void ReadPartitionCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement read_flash_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(READ_PARTITION_CMD_NAME_VAL));
    read_flash_node.setAttribute(VERSION_NODE_NAME, READPARTITION_CMD_VER_VAL);
    xml_node.appendChild(read_flash_node);
    read_flash_node.setAttribute(QStringLiteral("partition"), m_partition);
    m_target_file_assist->saveXML(xml_doc, read_flash_node);
}

void ReadPartitionCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:READ-PARTITION</command>
    <arg>
        <partition>system</partition>
        <target_file>C:/file.bin</target_file>
    </arg>
</da>
*/
void ReadPartitionCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void ReadPartitionCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void ReadPartitionCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString ReadPartitionCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}
