#include "WritePartitionCmdXML.h"
#include "../XMLConst.h"

WritePartitionCmdXML::WritePartitionCmdXML():
    ICmdXMLGenerator(WRITEPARTITION_CMD_VER_VAL, WRITE_PARTITION_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void WritePartitionCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_partition = xml_node.attribute(QStringLiteral("partition"));
    m_source_file_assist->loadXML(xml_node);
}

void WritePartitionCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement write_partition_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(WRITE_PARTITION_CMD_NAME_VAL));
    write_partition_node.setAttribute(VERSION_NODE_NAME, WRITEPARTITION_CMD_VER_VAL);
    xml_node.appendChild(write_partition_node);
    write_partition_node.setAttribute(QStringLiteral("partition"), m_partition);
    m_source_file_assist->saveXML(xml_doc, write_partition_node);
}

void WritePartitionCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:WRITE-PARTITION</command>
    <arg>
        <partition>system</partition>
        <source_file>MEM://0x8000000:0x100000</source_file>
    </arg>
</da>
*/
void WritePartitionCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void WritePartitionCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void WritePartitionCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void WritePartitionCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
