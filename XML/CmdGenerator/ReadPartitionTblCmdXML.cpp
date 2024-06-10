#include "ReadPartitionTblCmdXML.h"

ReadPartitionTblCmdXML::ReadPartitionTblCmdXML():
    ICmdXMLGenerator(READ_PARTITION_TABLE_CMD_VER_VAL, READ_PARTITION_TABLE_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

QString ReadPartitionTblCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:READ-PARTITION-TABLE</command>
    <arg>
        <target_file>MEM://0x2000000:0x20000</target_file>
    </arg>
</da>
*/
void ReadPartitionTblCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);

    m_target_file_assist->setFileType(FILE_TYPE::MEMORY_FILE);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}
