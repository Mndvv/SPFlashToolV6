#include "ReadFlashCmdXML.h"
#include "../XMLConst.h"

ReadFlashCmdXML::ReadFlashCmdXML():
    ICmdXMLGenerator(READFLASH_CMD_VER_VAL, READ_FLASH_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void ReadFlashCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_partition = xml_node.attribute(QStringLiteral("partition"));
    m_offset = xml_node.attribute(QStringLiteral("offset"));
    m_length = xml_node.attribute(QStringLiteral("length"));
    m_target_file_assist->loadXML(xml_node);
}

void ReadFlashCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement read_flash_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(READ_FLASH_CMD_NAME_VAL));
    read_flash_node.setAttribute(VERSION_NODE_NAME, READFLASH_CMD_VER_VAL);
    xml_node.appendChild(read_flash_node);
    read_flash_node.setAttribute(QStringLiteral("partition"), m_partition);
    read_flash_node.setAttribute(QStringLiteral("offset"), m_offset);
    read_flash_node.setAttribute(QStringLiteral("length"), m_length);
    m_target_file_assist->saveXML(xml_doc, read_flash_node);
}

void ReadFlashCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:READ-FLASH</command>
    <arg>
        <partition>EMMC-USER</partition>
        <offset>0x0</offset>
        <length>0x100000</length>
        <target_file>MEM://0x8000000:0x100000</target_file>
    </arg>
</da>
*/
void ReadFlashCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
    this->createXMLSubNode(arg_node, OFFSET_NODE_NAME, m_offset);
    this->createXMLSubNode(arg_node, LENGTH_NODE_NAME, m_length);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void ReadFlashCmdXML::setOffSet(const QString &offset)
{
    m_offset = offset;
}

void ReadFlashCmdXML::setLength(const QString &length)
{
    m_length = length;
}

void ReadFlashCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void ReadFlashCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString ReadFlashCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}
