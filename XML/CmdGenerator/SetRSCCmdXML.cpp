#include "SetRSCCmdXML.h"

SetRSCCmdXML::SetRSCCmdXML():
    ICmdXMLGenerator(SET_RSC_CMD_VER_VAL, SET_RSC_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void SetRSCCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_key = xml_node.attribute(QStringLiteral("key"));
    m_source_file_assist->loadXML(xml_node);
}

void SetRSCCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement set_rsc_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(SET_RSC_CMD_NAME_VAL));
    set_rsc_node.setAttribute(VERSION_NODE_NAME, SET_RSC_CMD_VER_VAL);
    xml_node.appendChild(set_rsc_node);
    set_rsc_node.setAttribute(QStringLiteral("key"), m_key);
    m_source_file_assist->saveXML(xml_doc, set_rsc_node);
}

void SetRSCCmdXML::setKey(const QString &key)
{
    m_key = key;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SET-RSC</command>
    <arg>
        <key>k6885v1_64[op01]</key>
        <source_file>ms-appdata:///local/RSC.bin</source_file>    
    </arg>
</da>
*/
void SetRSCCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, KEY_NODE_NAME, m_key);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void SetRSCCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void SetRSCCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void SetRSCCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
