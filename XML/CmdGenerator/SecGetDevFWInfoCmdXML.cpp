#include "SecGetDevFWInfoCmdXML.h"

SecGetDevFWInfoCmdXML::SecGetDevFWInfoCmdXML():
    ICmdXMLGenerator(SECURITY_GET_DEV_FW_INFO_CMD_VER_VAL, SECURITY_GET_DEV_FW_INFO_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SECURITY-GET-DEV-FW-INFO</command>
    <arg>
        <target_file>MEM://0x8000000:0x100000</target_file>
    </arg>
</da>
*/
void SecGetDevFWInfoCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void SecGetDevFWInfoCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void SecGetDevFWInfoCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString SecGetDevFWInfoCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}
