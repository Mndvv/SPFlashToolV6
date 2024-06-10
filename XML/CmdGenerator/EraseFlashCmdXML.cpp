#include "EraseFlashCmdXML.h"
#include "../XMLConst.h"

EraseFlashCmdXML::EraseFlashCmdXML():
    ICmdXMLGenerator(ERASEFLASH_CMD_VER_VAL, ERASE_FLASH_CMD_NAME_VAL, DA_NODE_NAME),
    m_partition(QStringLiteral("ALL")),
    m_offset(QStringLiteral("0x0")),
    m_length(QStringLiteral("MAX"))
{

}

void EraseFlashCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_partition = xml_node.attribute(QStringLiteral("partition"));
    m_offset = xml_node.attribute(QStringLiteral("offset"));
    m_length = xml_node.attribute(QStringLiteral("length"));
}

void EraseFlashCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement erase_flash_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(ERASE_FLASH_CMD_NAME_VAL));
    erase_flash_node.setAttribute(VERSION_NODE_NAME, ERASEFLASH_CMD_VER_VAL);
    xml_node.appendChild(erase_flash_node);

    erase_flash_node.setAttribute(QStringLiteral("partition"), m_partition);
    erase_flash_node.setAttribute(QStringLiteral("offset"), m_offset);
    erase_flash_node.setAttribute(QStringLiteral("length"), m_length);
}

void EraseFlashCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:ERASE-FLASH</command>
    <arg>
        <partition>EMMC-USER</partition>
        <offset>0x0</offset>
        <length>0x100000</length>
    </arg>
</da>
*/
void EraseFlashCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
    this->createXMLSubNode(arg_node, OFFSET_NODE_NAME, m_offset);
    this->createXMLSubNode(arg_node, LENGTH_NODE_NAME, m_length);
}

void EraseFlashCmdXML::setOffSet(const QString &offset)
{
    m_offset = offset;
}

void EraseFlashCmdXML::setLength(const QString &length)
{
    m_length = length;
}
