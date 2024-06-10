#include "EMMCControlCmdXML.h"

EMMCControlCmdXML::EMMCControlCmdXML():
    ICmdXMLGenerator(EMMC_CONTROL_CMD_VER_VAL, EMMC_CONTROL_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void EMMCControlCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_target_file_assist->loadXML(xml_node);
}

void EMMCControlCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QString cmd_node_name = this->getFunctionFieldVal();
    QDomElement emmc_control_node = xml_doc.createElement(cmd_node_name);
    emmc_control_node.setAttribute(VERSION_NODE_NAME, EMMC_CONTROL_CMD_VER_VAL);
    xml_node.appendChild(emmc_control_node);

    m_target_file_assist->saveXML(xml_doc, emmc_control_node);
}

/*
<da>
    <version>1.0</version>
    <command>CMD:EMMC-CONTROL</command>
    <arg>
       <The fields in this place is different according to function, refer to next row.>
    </arg>
</da>

//GET-RPMB-KEY-STATUS field
<function>GET-RPMB-KEY-STATUS</function>
<target_file>MEM://0x8000000:0x100000</target_file>

//LIFE-CYCLE-STATUS field
<function>LIFE-CYCLE-STATUS</function>
<target_file>MEM://0x8000000:0x100000</target_file>
*/
void EMMCControlCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, FUNCTION_NODE_NAME, this->getFunctionFieldVal());
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void EMMCControlCmdXML::setTargetFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void EMMCControlCmdXML::setTargetFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString EMMCControlCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

GetRPMBKeyStatusCmdXML::GetRPMBKeyStatusCmdXML()
{

}

QString GetRPMBKeyStatusCmdXML::getFunctionFieldVal() const
{
    return GET_RPMB_KEY_STATUS;
}

LifeCycleStatusCmdXML::LifeCycleStatusCmdXML()
{

}

QString LifeCycleStatusCmdXML::getFunctionFieldVal() const
{
    return LIFE_CYCLE_CHECK;
}
