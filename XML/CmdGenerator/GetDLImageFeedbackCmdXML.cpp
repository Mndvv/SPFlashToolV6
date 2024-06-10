#include "GetDLImageFeedbackCmdXML.h"

GetDLImageFeedbackCmdXML::GetDLImageFeedbackCmdXML():
    ICmdXMLGenerator(GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_VER_VAL,
                     GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_NAME_VAL, DA_NODE_NAME),
    m_target_file_assist(new XMLTargetFileAssistant())
{

}

void GetDLImageFeedbackCmdXML::loadXML(const QDomElement &xml_node)
{
    m_target_file_assist->loadXML(xml_node);
}

void GetDLImageFeedbackCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement dl_feedback_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_NAME_VAL));
    dl_feedback_node.setAttribute(VERSION_NODE_NAME, GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_VER_VAL);
    xml_node.appendChild(dl_feedback_node);

    m_target_file_assist->saveXML(xml_doc, dl_feedback_node);
}

void GetDLImageFeedbackCmdXML::setFileType(FILE_TYPE file_type)
{
    m_target_file_assist->setFileType(file_type);
}

QString GetDLImageFeedbackCmdXML::getCmdValue() const
{
    return m_target_file_assist->getCmdXMLValue();
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:GET-DA-INFO</command>
    <arg>
        <target_file>MEM://0x2000000:0x20000</target_file>
    </arg>
</da>
*/
void GetDLImageFeedbackCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, TARGET_FILE_NODE_NAME, m_target_file_assist->getFileDesc());
}
