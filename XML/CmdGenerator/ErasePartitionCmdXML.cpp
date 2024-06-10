#include "ErasePartitionCmdXML.h"
#include "../XMLConst.h"

ErasePartitionCmdXML::ErasePartitionCmdXML():
    ICmdXMLGenerator(ERASEPARTITION_CMD_VER_VAL, ERASEPARTITION_CMD_NAME_VAL, DA_NODE_NAME)
{

}

void ErasePartitionCmdXML::setPartition(const QString &partition)
{
    m_partition = partition;
}
/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:ERASE-PARTITION</command>
    <arg>
        <partition>system</partition>
    </arg>
</da>
*/
void ErasePartitionCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARTITION_NODE_NAME, m_partition);
}
