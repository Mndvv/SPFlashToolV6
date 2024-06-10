#include "GetHWInfoCmdXML.h"


BromGetHWInfoCmdXML::BromGetHWInfoCmdXML():
    ICmdXMLGenerator(BROM_GET_HW_CMD_VER_VAL, BROM_GET_HW_CMD_NAME_VAL, BROM_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

QString BromGetHWInfoCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?
<brom>
    <version>1.0</version>
    <command>CMD:GET-HW-INFO</command>
    <arg>
        <target_file>MEM://0x8000000:0x100000</target_file>
    </arg>
</brom>
*/
void BromGetHWInfoCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);

    m_target_file_assist->setFileType(FILE_TYPE::MEMORY_FILE);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

DAGetHWInfoCmdXML::DAGetHWInfoCmdXML():
    ICmdXMLGenerator(DA_GET_HW_CMD_VER_VAL, DA_GET_HW_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

QString DAGetHWInfoCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:GET-HW-INFO</command>
    <arg>
        <target_file>MEM://0x8000000:0x100000</target_file>
    </arg>
</da>
*/
void DAGetHWInfoCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);

    m_target_file_assist->setFileType(FILE_TYPE::MEMORY_FILE);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}
