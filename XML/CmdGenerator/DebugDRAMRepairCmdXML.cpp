#include "DebugDRAMRepairCmdXML.h"
#include "../../XML/XMLConst.h"

DebugDRAMRepairCmdXML::DebugDRAMRepairCmdXML():
    ICmdXMLGenerator(DEBUG_DRAM_REPAIR_CMD_VER_VAL, DEBUG_DRAM_REPAIR_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void DebugDRAMRepairCmdXML::setParam_file(const QString &param_file)
{
    m_param_file = param_file;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:DEBUG:DRAM-REPAIR</command>
    <arg>
        <param_file>D:/dram.info</param_file>
        <target_file>MEM://0x10000:0x1000</target_file>
    </arg>
</da>
target_file: the result string: "SUCCEEDED", "NO-NEED", "FAILED".
*/
void DebugDRAMRepairCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PARAM_FILE_NODE_NAME, m_param_file);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void DebugDRAMRepairCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void DebugDRAMRepairCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString DebugDRAMRepairCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}
