#include "RunProgramCmdXML.h"

RunProgramCmdXML::RunProgramCmdXML():
    ICmdXMLGenerator(RUN_PROGRAM_CMD_VER_VAL, RUN_PROGRAM_CMD_NAME_VAL, BROM_NODE_NAME),
    m_signature_offset("0x0"),
    m_signature_length("0x0"),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void RunProgramCmdXML::setAtAddress(const QString &at_address)
{
    m_at_address = at_address;
}

void RunProgramCmdXML::setJmpAddress(const QString &jmp_address)
{
    m_jmp_address = jmp_address;
}

void RunProgramCmdXML::setSignatureOffset(const QString &signature_offset)
{
    m_signature_offset = signature_offset;
}

void RunProgramCmdXML::setSignatureLength(const QString &signature_length)
{
    m_signature_length = signature_length;
}

/*
<?xml version="1.0" encoding="utf-8"?
<brom>
    <version>1.0</version>
    <command>CMD:RUN-PROGRAM</command>
    <arg>
        <at_address>0x100000</at_address>
        <jmp_address>0x100000</jmp_address>
        <signature_offset>0x20000</signature_offset>
        <signature_length>0x100</signature_length>
        <source_file>MEM://0x8000000:0x100000</source_file>
    </arg>
</brom>
*/
void RunProgramCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, AT_ADDRESS_NODE_NAME, m_at_address);
    this->createXMLSubNode(arg_node, JMP_ADDRESS_NODE_NAME, m_jmp_address);
    this->createXMLSubNode(arg_node, SIGNATURE_OFFSET_NODE_NAME, m_signature_offset);
    this->createXMLSubNode(arg_node, SIGNATURE_LENGTH_NODE_NAME, m_signature_length);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void RunProgramCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void RunProgramCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void RunProgramCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
