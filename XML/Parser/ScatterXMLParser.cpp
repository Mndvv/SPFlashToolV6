#include "ScatterXMLParser.h"
#include "../XMLConst.h"
#include "../../Utility/Utils.h"
#include <QObject>
#include <QDir>

ScatterXMLParser::ScatterXMLParser():
    IXMLParser(),
    m_storage_type(HW_StorageType_E::HW_STORAGE_NONE),
    m_skip_pt_operation(false)
{

}

QString ScatterXMLParser::get_chip_name() const
{
    return m_chip_name;
}

DownloadPartitionInfoList ScatterXMLParser::get_scatter_partitions() const
{
    return m_scatter_list;
}

HW_StorageType_E ScatterXMLParser::get_storage_type() const
{
    return m_storage_type;
}

bool ScatterXMLParser::isSkipPTOperation() const
{
    return m_skip_pt_operation;
}

bool ScatterXMLParser::parseDocument(const QDomElement &root_node)
{
    bool parse_success = true;
    bool storage_type_has_parsed = false;
    QDomElement element = root_node.firstChildElement();
    while (!element.isNull()) {
        std::string tag_name = element.tagName().trimmed().toStdString();
        if (tag_name == GENERAL_NODE_NAME) {
            parse_success = this->parseGeneralNode(element);
        } else if (tag_name == STORAGE_TYPE_NODE_NAME && !storage_type_has_parsed) {
            parse_success = this->parseStorageTypeNode(element);
            storage_type_has_parsed = true;
        }
        if (!parse_success) {
            break;
        }
        element = element.nextSiblingElement();
    }
    return parse_success;
}

bool ScatterXMLParser::parseGeneralNode(const QDomElement &general_node)
{
    QDomNodeList node_list = general_node.elementsByTagName(PLATFORM_NODE_NAME);
    if (node_list.isEmpty()) {
        m_error_msg = QObject::tr("no tag named %1 under general node.").arg(PLATFORM_NODE_NAME);
        return false;
    }
    if (!node_list.at(0).isElement()) {
        m_error_msg = QObject::tr("%1 tag is NOT a element node.").arg(PLATFORM_NODE_NAME);
        return false;
    }
    QDomElement platformt_element = node_list.at(0).toElement();
    m_chip_name = platformt_element.text().trimmed();

    node_list = general_node.elementsByTagName(SKIP_PT_OPERATE_NODE_NAME);
    if (!node_list.isEmpty() && !node_list.at(0).isElement()) {
        m_skip_pt_operation = node_list.at(0).toElement().text().trimmed().toUpper() == "TRUE";
    }

    return true;
}

bool ScatterXMLParser::parseStorageTypeNode(const QDomElement &storage_type_node)
{
    bool parse_success = false;
    std::string storage_name = storage_type_node.attribute(NAME_NODE_NAME).trimmed().toStdString();
    m_storage_type = Utils::getHWStorageTypeByName(storage_name);

    DownloadPartitionInfoList partition_list;
    QDomElement partition_element = storage_type_node.firstChildElement();
    while (!partition_element.isNull()) {
        std::string tag_name = partition_element.tagName().trimmed().toStdString();
        if (tag_name != PARTITION_INDEX_NODE_NAME) {
            partition_element = partition_element.nextSiblingElement();
            continue;
        }
        parse_success = parsePartitionIndexNode(partition_list, partition_element);
        if (!parse_success) {
            break;
        }
        partition_element = partition_element.nextSiblingElement();
    }
    if (parse_success) {
        m_scatter_list = partition_list;
    }
    return parse_success;
}

bool ScatterXMLParser::parsePartitionIndexNode(DownloadPartitionInfoList &partition_list,
                                               const QDomElement &partition_index_node)
{
    QDomElement is_download_node = partition_index_node.firstChildElement(IS_DOWNLOAD_NODE_NAME);
    if (is_download_node.isNull()) {
        m_error_msg = this->genNoChilElementError(partition_index_node, IS_DOWNLOAD_NODE_NAME);
        return false;
    }

    QDomElement part_name_node = partition_index_node.firstChildElement(PARTITION_NAME_NODE_NAME);
    if (part_name_node.isNull()) {
        m_error_msg = this->genNoChilElementError(partition_index_node, PARTITION_NAME_NODE_NAME);
        return false;
    }

    QDomElement start_address_node = partition_index_node.firstChildElement(PHYSICAL_START_ADDR_NODE_NAME);
    if (start_address_node.isNull()) {
        m_error_msg = this->genNoChilElementError(partition_index_node, PHYSICAL_START_ADDR_NODE_NAME);
        return false;
    }

    QDomElement part_file_node = partition_index_node.firstChildElement(FILE_NAME_NODE_NAME);
    if (part_name_node.isNull()) {
        m_error_msg = this->genNoChilElementError(partition_index_node, FILE_NAME_NODE_NAME);
        return false;
    }

    bool is_download = is_download_node.text().trimmed().toLower() == "true";
    if (is_download)
    {
        DownloadPartitionInfo partition_info;
        partition_info.setEnabled(true);
        partition_info.setPartition_name(part_name_node.text().trimmed());
        ulong start_addres = start_address_node.text().trimmed().toULong(nullptr, 16);
        QString start_address_str = QString("0x%1").arg(start_addres, 16, 16, QChar('0'));
        partition_info.setStartAddress(start_address_str);
        QString part_file_name = m_xml_file_path + QDir::separator() + part_file_node.text().trimmed();
        partition_info.setFile_name(QDir::toNativeSeparators(part_file_name));
        partition_list.push_back(partition_info);
    }
    return true;
}

QString ScatterXMLParser::genNoChilElementError(const QDomElement &partition_index_node,
                                             const std::string &sub_node_name) const
{
    QString partition_index = partition_index_node.attribute(NAME_ATTR_NAME, "").trimmed();
    QDomElement storage_type_element = partition_index_node.parentNode().toElement();
    assert(!storage_type_element.isNull());
    QString storage_name = storage_type_element.attribute(NAME_ATTR_NAME, "").trimmed();
    QString error_msg = QString("\"%1\" node not exist under %2 storage type, %3 index.")
            .arg(sub_node_name.c_str()).arg(storage_name).arg(partition_index);
    return error_msg;
}
