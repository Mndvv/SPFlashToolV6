#include "WriteEfuseCmdXML.h"

WriteEfuseCmdXML::WriteEfuseCmdXML():
    ICmdXMLGenerator(WRITE_EFUSE_CMD_VER_VAL, WRITE_EFUSE_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void WriteEfuseCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_source_file_assist->loadXML(xml_node);
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:WRITE-EFUSE</command>
    <arg>
        <source_file>ms-appdata:///local/efuse.xml</source_file>
    </arg>
</da>
*/
void WriteEfuseCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void WriteEfuseCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void WriteEfuseCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void WriteEfuseCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
