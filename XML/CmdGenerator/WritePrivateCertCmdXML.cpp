#include "WritePrivateCertCmdXML.h"

WritePrivateCertCmdXML::WritePrivateCertCmdXML():
    ICmdXMLGenerator(WRITE_PRIVATE_CERT_CMD_VER_VAL, WRITE_PRIVATE_CERT_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:WRITE-PRIVATE-CERT</command>
    <arg>
        <source_file>ms-appdata:///local/cert.bin</source_file>
    </arg>
</da>
*/
void WritePrivateCertCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void WritePrivateCertCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void WritePrivateCertCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void WritePrivateCertCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
