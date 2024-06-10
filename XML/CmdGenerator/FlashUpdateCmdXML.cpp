#include "FlashUpdateCmdXML.h"
#include <QDir>

FlashUpdateCmdXML::FlashUpdateCmdXML():
    ICmdXMLGenerator(FLASH_UPDATE_CMD_VER_VAL, FLASH_UPDATE_CMD_NAME_VAL, DA_NODE_NAME),
    m_path_separator(QDir::separator()),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void FlashUpdateCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_path_separator = xml_node.attribute(QStringLiteral("path_separator"), QDir::separator());
    m_source_file_assist->loadXML(xml_node);
    m_backupFolder = xml_node.attribute(QStringLiteral("backup_folder"));
}

void FlashUpdateCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement flash_update_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(FLASH_UPDATE_CMD_NAME_VAL));
    flash_update_node.setAttribute(VERSION_NODE_NAME, FLASH_UPDATE_CMD_VER_VAL);
    xml_node.appendChild(flash_update_node);
    flash_update_node.setAttribute(QStringLiteral("path_separator"), m_path_separator);
    m_source_file_assist->saveXML(xml_doc, flash_update_node);
    flash_update_node.setAttribute(QStringLiteral("backup_folder"), m_backupFolder);
}

void FlashUpdateCmdXML::setPathSeparator(const QString &path_separator)
{
    m_path_separator = path_separator;
}

void FlashUpdateCmdXML::setBackupFolder(const QString &backup_folder)
{
    m_backupFolder = backup_folder;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>1.0</version>
    <command>CMD:FLASH-UPDATE</command>
    <arg>
        <path_separator>/</path_separator>
        <source_file>D:/scatter.xml</source_file>
        <backup_folder>D:/backup</backup_folder>
    </arg>
</da>
*/
void FlashUpdateCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PATH_SEPARATOR_NODE_NAME, QDir::toNativeSeparators(m_path_separator));
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, QDir::toNativeSeparators(m_source_file_assist->getFileDesc()));
    this->createXMLSubNode(arg_node, BACKUP_FOLDER_NODE_NAME, QDir::toNativeSeparators(m_backupFolder));
}

void FlashUpdateCmdXML::setFileType(FILE_TYPE file_type)
{
    assert(file_type != FILE_TYPE::MEMORY_FILE);
    m_source_file_assist->setFileType(file_type);
}

void FlashUpdateCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void FlashUpdateCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
