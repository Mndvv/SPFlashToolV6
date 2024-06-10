#include "FlashUpdateExCmdXML.h"
#include <QDir>

FlashUpdateExCmdXML::FlashUpdateExCmdXML():
    ICmdXMLGenerator(FLASH_UPDATE_EX_CMD_VER_VAL, FLASH_UPDATE_EX_CMD_NAME_VAL, DA_NODE_NAME),
    m_path_separator(QDir::separator()),
    m_source_file_assist(new XMLSourceFileAssistant())
{

}

void FlashUpdateExCmdXML::loadXML(const QDomElement &xml_node)
{
    assert(!xml_node.isNull());
    m_path_separator = xml_node.attribute(QStringLiteral("path_separator"), QDir::separator());
    m_source_file_assist->loadXML(xml_node);

    QDomElement element = xml_node.firstChildElement();
    while (!element.isNull()) {
        QString tag_name = element.tagName().trimmed();
        if (tag_name == FLASH_LIST_NODE_NAME) {
            QDomElement sub_node = element.firstChildElement();
            while (!sub_node.isNull()) {
                QString sub_tag_name = sub_node.tagName().trimmed();
                if (sub_tag_name == PT_NODE_NAME) {
                    QString partition_name = sub_node.attribute(NAME_ATTR_NAME);
                    QString file_path = sub_node.text().trimmed();
                    DownloadPartitionInfo dl_info;
                    dl_info.setEnabled(true);
                    dl_info.setPartition_name(partition_name);
                    dl_info.setFile_name(file_path);
                    m_dl_partition_list.append(dl_info);
                }
                sub_node = sub_node.nextSiblingElement();
            }
        } else if (tag_name == QStringLiteral("backup_folder")) {
            m_backupFolder = element.text().trimmed();
        }
        element = element.nextSiblingElement();
    }
}

void FlashUpdateExCmdXML::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomElement flash_update_ex_node = xml_doc.createElement(EXTRACT_CMD_NAME_ONLY(FLASH_UPDATE_EX_CMD_NAME_VAL));
    flash_update_ex_node.setAttribute(VERSION_NODE_NAME, FLASH_UPDATE_EX_CMD_VER_VAL);
    xml_node.appendChild(flash_update_ex_node);
    flash_update_ex_node.setAttribute(QStringLiteral("path_separator"), m_path_separator);
    m_source_file_assist->saveXML(xml_doc, flash_update_ex_node);

    QDomElement back_folder_node = xml_doc.createElement(QStringLiteral("backup_folder"));
    QDomText value_node = xml_doc.createTextNode(m_backupFolder);
    back_folder_node.appendChild(value_node);

    flash_update_ex_node.appendChild(back_folder_node);

    QDomElement flash_list_node = xml_doc.createElement(FLASH_LIST_NODE_NAME);
    flash_update_ex_node.appendChild(flash_list_node);

    foreach(auto item, m_dl_partition_list) {
        QString partition_name = item.partition_name();
        QString file_path = QDir::toNativeSeparators(item.file_name());
        QDomText value_node = xml_doc.createTextNode(file_path);

        QDomElement pt_node = xml_doc.createElement(PT_NODE_NAME);
        pt_node.setAttribute(NAME_ATTR_NAME, partition_name);
        pt_node.appendChild(value_node);

        flash_list_node.appendChild(pt_node);
    }
}

void FlashUpdateExCmdXML::setPathSeparator(const QString &path_separator)
{
    m_path_separator = path_separator;
}

void FlashUpdateExCmdXML::setBackupFolder(const QString &backup_folder)
{
    m_backupFolder = backup_folder;
}

void FlashUpdateExCmdXML::setDLPartitionList(const DownloadPartitionInfoList &dl_partition_list)
{
    m_dl_partition_list = dl_partition_list;
}

/*
<?xml version="1.0" encoding="utf-8"?>
<da>
    <version>2.0</version>
    <command>CMD:FLASH-UPDATE</command>
    <arg>
        <path_separator>/</path_separator>
        <source_file>D:/scatter.xml</source_file>
        <backup_folder>D:/backup</backup_folder>
        <flash_list>
           <pt name=”boot”>D:/boot.img</pt>
           <pt name=”system”>D:/system.img</pt>
        </flash_list>
    </arg>
</da>

*/
void FlashUpdateExCmdXML::fillArgNode(QDomElement * const arg_node)
{
    assert(nullptr != arg_node);
    this->createXMLSubNode(arg_node, PATH_SEPARATOR_NODE_NAME, QDir::toNativeSeparators(m_path_separator));
    this->createXMLSubNode(arg_node, SOURCE_FILE_NODE_NAME, m_source_file_assist->getFileDesc());
    this->createXMLSubNode(arg_node, BACKUP_FOLDER_NODE_NAME, QDir::toNativeSeparators(m_backupFolder));

    QDomElement flash_list_node = this->createXMLSubNode(arg_node, FLASH_LIST_NODE_NAME, QStringLiteral(""));
    QMap<QString, QString> attrs_map;
    foreach(auto item, m_dl_partition_list) {
        QString partition_name = item.partition_name();
        QString file_path = QDir::toNativeSeparators(item.file_name());
        attrs_map.clear();
        attrs_map.insert(NAME_ATTR_NAME, partition_name);
        this->createNodeWithAttrs(&flash_list_node, PT_NODE_NAME, attrs_map, file_path);
    }
}

void FlashUpdateExCmdXML::setFileType(FILE_TYPE file_type)
{
    assert(file_type != FILE_TYPE::MEMORY_FILE);
    m_source_file_assist->setFileType(file_type);
}

void FlashUpdateExCmdXML::setFileName(const QString &file_name)
{
    m_source_file_assist->setFileName(file_name);
}

void FlashUpdateExCmdXML::setMemInfo(const QString &start_address, const QString &length)
{
    m_source_file_assist->setStartAddress(start_address);
    m_source_file_assist->setLength(length);
}
