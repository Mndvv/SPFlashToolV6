#include "FlashAllCmdXML.h"
#include <QDir>

FlashAllCmdXML::FlashAllCmdXML():
    ICmdXMLGenerator(FLASH_ALL_CMD_VER_VAL, FLASH_ALL_CMD_NAME_VAL, DA_NODE_NAME),
    m_path_separator(QDir::separator()),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void FlashAllCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_path_separator = xml_node.attribute(QStringLiteral("path_separator"), QDir::separator());
    m_source_file_assist->loadXML(xml_node);
}

void FlashAllCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement flash_all_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(FLASH_ALL_CMD_NAME_VAL));
    flash_all_node.setAttribute(VERSION_NODE_NAME, FLASH_ALL_CMD_VER_VAL);
    xml_node.appendChild(flash_all_node);
    flash_all_node.setAttribute(QStringLiteral("path_separator"), m_path_separator);
    m_source_file_assist->saveXML(xml_doc, flash_all_node);
}

void FlashAllCmdXML::setPathSeparator(const QString &path_separator)
{
    m_path_separator = path_separator;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:FLASH-ALL</command>
    <arg>
        <path_separator>/</path_separator>
        <source_file>D:/scatter.xml</source_file>
    </arg>
</da>
*/
void FlashAllCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PATH_SEPARATOR_NODE_NAME, QDir::toNativeSeparators(m_path_separator));
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, QDir::toNativeSeparators(m_source_file_assist->getFileDesc()));
}

void FlashAllCmdXML::setFileType(FILE_TYPE file_type)
{
    m_source_file_assist->setFileType(file_type);
}

void FlashAllCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void FlashAllCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
