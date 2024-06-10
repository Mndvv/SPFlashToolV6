#include "BootToCmdXML.h"

BootToCmdXML::BootToCmdXML():
    ICmdXMLGenerator(BOOT_TO_CMD_VER_VAL, BOOT_TO_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void BootToCmdXML::setAtAddress(const QString &at_address)
{
    m_at_address = at_address;
}

void BootToCmdXML::setJmpAddress(const QString &jmp_address)
{
    m_jmp_address = jmp_address;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:BOOT-TO</command>
    <arg>
        <at_address>0x2000000</at_address>
        <jmp_address>0x2000000</jmp_address>
        <source_file>MEM://0x8000000:0x100000</source_file>
    </arg>
</da>
*/
void BootToCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, AT_ADDRESS_NODE_NAME, m_at_address);
    this->createXMLSubNode(arg_node, JMP_ADDRESS_NODE_NAME, m_jmp_address);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void BootToCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void BootToCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void BootToCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
