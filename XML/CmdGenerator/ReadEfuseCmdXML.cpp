#include "ReadEfuseCmdXML.h"

ReadEfuseCmdXML::ReadEfuseCmdXML():
    ICmdXMLGenerator(READ_EFUSE_CMD_VER_VAL, READ_EFUSE_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void ReadEfuseCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_target_file_assist->loadXML(xml_node);
}

void ReadEfuseCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

void ReadEfuseCmdXML::setFileName(const QString &file_name)
{
    m_target_file_assist->setFileName(file_name);
}

QString ReadEfuseCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:READ-EFUSE</command>
    <arg>
        <target_file>ms-appdata:///local/efuse.xml</target_file>
    </arg>
</da>
*/
void ReadEfuseCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);

    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}
