#include "FlashXMLDataLoader.h"

FlashXMLDataLoader::FlashXMLDataLoader()
{

}

QString FlashXMLDataLoader::contact_name() const
{
    return m_contact_name;
}

void FlashXMLDataLoader::setContact_name(const QString &contact_name)
{
    m_contact_name = contact_name;
}

QString FlashXMLDataLoader::getScatter_file_name() const
{
    return m_scatter_file_name;
}

void FlashXMLDataLoader::setScatter_file_name(const QString &scatter_file_name)
{
    m_scatter_file_name = scatter_file_name;
}

QString FlashXMLDataLoader::getChip_name() const
{
    return m_chip_name;
}

void FlashXMLDataLoader::setChip_name(const QString &chip_name)
{
    m_chip_name = chip_name;
}

HW_StorageType_E FlashXMLDataLoader::getStorage_type() const
{
    return m_storage_type;
}

void FlashXMLDataLoader::setStorage_type(const HW_StorageType_E &storage_type)
{
    m_storage_type = storage_type;
}

bool FlashXMLDataLoader::getSkip_pt_operate() const
{
    return m_skip_pt_operate;
}

void FlashXMLDataLoader::setSkip_pt_operate(bool skip_pt_operate)
{
    m_skip_pt_operate = skip_pt_operate;
}

DownloadPartitionInfoList FlashXMLDataLoader::getDl_partition_list() const
{
    return m_dl_partition_list;
}

void FlashXMLDataLoader::setDl_partition_list(const DownloadPartitionInfoList &dl_partition_list)
{
    m_dl_partition_list = dl_partition_list;
}

QString FlashXMLDataLoader::getFlash_xml_file() const
{
    return m_flash_xml_file;
}

void FlashXMLDataLoader::setFlash_xml_file(const QString &flash_xml_file)
{
    m_flash_xml_file = flash_xml_file;
}
