#include "SecSetFlashPolicyCmdXML.h"

SecSetFlashPolicyCmdXML::SecSetFlashPolicyCmdXML():
    ICmdXMLGenerator(SECURITY_SET_FLASH_POLICY_CMD_VER_VAL, SECURITY_SET_FLASH_POLICY_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SECURITY-SET-FLASH-POLICY</command>
    <arg>
        <source_file>MEM://0x8000000:0x100000</source_file>
    </arg>
</da>
*/
void SecSetFlashPolicyCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void SecSetFlashPolicyCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void SecSetFlashPolicyCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void SecSetFlashPolicyCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
