#include "BromReadRegCmdXML.h"

BromReadRegCmdXML::BromReadRegCmdXML():
    ICmdXMLGenerator(BROM_READ_REGISTER_CMD_VER_VAL, BROM_READ_REGISTER_CMD_NAME_VAL, DA_NODE_NAME),
    m_register_bit_width(REGISTER_BIT_WIDTH_TYPE::REGISTER_32_BIT),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void BromReadRegCmdXML::setRegisterBitWidth(REGISTER_BIT_WIDTH_TYPE register_bit_width)
{
    m_register_bit_width = register_bit_width;
}

void BromReadRegCmdXML::setBaseAddress(const QString &base_address)
{
    m_base_address = base_address;
}

/*
<?xml version="1.0" encoding="utf-8"?
<brom>
    <version>1.0</version>
    <command>CMD:READ-REGISTER</command>
    <arg>
        <bit_width>32</bit_width>
        <base_address>0x1000</base_address>
        <target_file>MEM://0x8000000:0x4</target_file>
    </arg>
</brom>
*/
void BromReadRegCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, BIT_WIDTH_NODE_NAME, this->getRegisterBitWidth());
    this->createXMLSubNode(arg_node, BASE_ADDRESS_NODE_NAME, m_base_address);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}

void BromReadRegCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void BromReadRegCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString BromReadRegCmdXML::getCmdXMLValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

QString BromReadRegCmdXML::getRegisterBitWidth() const
{
    QString register_bit_width = "32";
    if (m_register_bit_width == REGISTER_BIT_WIDTH_TYPE::REGISTER_64_BIT)
        register_bit_width = "64";
    return register_bit_width;
}
