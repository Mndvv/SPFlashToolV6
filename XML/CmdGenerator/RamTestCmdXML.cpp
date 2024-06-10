#include "RamTestCmdXML.h"
#include "../XMLConst.h"

RAMTestCmdXML::RAMTestCmdXML():
    ICmdXMLGenerator(RAM_TEST_CMD_VER_VAL, RAM_TEST_CMD_NAME_VAL, DA_NODE_NAME)
{

}

FlipRAMTestCmdXML::FlipRAMTestCmdXML()
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:RAM-TEST</command>
    <arg>
        <function>FLIP</function>
        <start_address>0x4000000</start_address>
        <length>0x100000</length>
        <repeat>0xA</repeat>
    </arg>
</da>

*/
void FlipRAMTestCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, FUNCTION_NODE_NAME, this->getFunctionValue());
    this->createXMLSubNode(arg_node, START_ADDRESS_NODE_NAME, m_start_address);
    this->createXMLSubNode(arg_node, LENGTH_NODE_NAME, m_length);
    this->createXMLSubNode(arg_node, REPEAT_NODE_NAME, QString("0x%1").arg(m_repeat_count, 0, 16));
}

void FlipRAMTestCmdXML::setStartAddress(const QString &start_address)
{
    m_start_address = start_address;
}

void FlipRAMTestCmdXML::setLength(const QString &length)
{
    m_length = length;
}

void FlipRAMTestCmdXML::setRepeatCount(int repeat_count)
{
    m_repeat_count = repeat_count;
}

QString FlipRAMTestCmdXML::getFunctionValue() const
{
    return "FLIP";
}

CalibrationRAMTestCmdXML::CalibrationRAMTestCmdXML():
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:RAM-TEST</command>
    <arg>
         <function>CALIBRATION</function>
         <target_file>MEM://0x8000000:0x100000</target_file>
    </arg>
</da>
*/
void CalibrationRAMTestCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, FUNCTION_NODE_NAME, this->getFunctionValue());
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void CalibrationRAMTestCmdXML::setTargetFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void CalibrationRAMTestCmdXML::setTargetFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString CalibrationRAMTestCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

QString CalibrationRAMTestCmdXML::getFunctionValue() const
{
    return "CALIBRATION";
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:RAM-TEST</command>
    <arg>
         <function>SRAM</function>
    </arg>
</da>
*/
void SRAMTestCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, FUNCTION_NODE_NAME, this->getFunctionValue());
}

QString SRAMTestCmdXML::getFunctionValue() const
{
    return "SRAM";
}
