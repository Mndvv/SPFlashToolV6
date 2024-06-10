#include "WriteFlashCmdXML.h"
#include "../XMLConst.h"

WriteFlashCmdXML::WriteFlashCmdXML():
    ICmdXMLGenerator(WRITEFLASH_CMD_VER_VAL, WRITE_FLASH_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void WriteFlashCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_partition = xml_node.attribute(QStringLiteral("partition"));
    m_offset = xml_node.attribute(QStringLiteral("offset"));
    m_source_file_assist->loadXML(xml_node);
}

void WriteFlashCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement write_flash_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(WRITE_FLASH_CMD_NAME_VAL));
    write_flash_node.setAttribute(VERSION_NODE_NAME, WRITEFLASH_CMD_VER_VAL);
    xml_node.appendChild(write_flash_node);
    write_flash_node.setAttribute(QStringLiteral("partition"), m_partition);
    write_flash_node.setAttribute(QStringLiteral("offset"), m_offset);
    m_source_file_assist->saveXML(xml_doc, write_flash_node);
}

void WriteFlashCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:WRITE-FLASH</command>
    <arg>
        <partition>EMMC-USER</partition>
        <offset>0x0</offset>
        <source_file>MEM://0x8000000:0x100000</source_file>
    </arg>
</da>
*/
void WriteFlashCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
    this->createXMLSubNode(arg_node, OFFSET_NODE_NAME, m_offset);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void WriteFlashCmdXML::setOffSet(const QString &offset)
{
    m_offset = offset;
}

void WriteFlashCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void WriteFlashCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void WriteFlashCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
