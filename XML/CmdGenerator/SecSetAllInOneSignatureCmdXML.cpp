#include "SecSetAllInOneSignatureCmdXML.h"

SecSetAllInOneSignatureCmdXML::SecSetAllInOneSignatureCmdXML():
    ICmdXMLGenerator(SECURITY_SET_ALLINONE_SIGNATURE_CMD_VER_VAL, SECURITY_SET_ALLINONE_SIGNATURE_CMD_NAME_VAL, DA_NODE_NAME),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:SECURITY-SET-ALLINONE-SIGNATURE</command>
    <arg>
        <source_file>ms-appdata:///local/sig.bin</source_file>
    </arg>
</da>
*/
void SecSetAllInOneSignatureCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
}

void SecSetAllInOneSignatureCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void SecSetAllInOneSignatureCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void SecSetAllInOneSignatureCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
