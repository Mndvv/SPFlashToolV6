#include "GetDAInfoCmdXML.h"

GetDAInfoCmdXML::GetDAInfoCmdXML():
    ICmdXMLGenerator(GET_DA_INFO_CMD_VER_VAL, GET_DA_INFO_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

QString GetDAInfoCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:GET-DA-INFO</command>
    <arg>
        <target_file>MEM://0x2000000:0x20000</target_file>
    </arg>
</da>
*/
void GetDAInfoCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);

    m_target_file_assist->setFileType(FILE_TYPE::MEMORY_FILE);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}
