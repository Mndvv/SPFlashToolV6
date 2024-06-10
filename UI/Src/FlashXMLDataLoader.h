#ifndef FLASHXMLDATALOADER_H
#define FLASHXMLDATALOADER_H

#include <QString>
#include "../../Inc/CFlashToolTypes.h"
#include "../../Utility/PartitionInfo.h"


class FlashXMLDataLoader
{
public:
    FlashXMLDataLoader();

    QString contact_name() const;
    void setContact_name(const QString &contact_name);

    QString getScatter_file_name() const;
    void setScatter_file_name(const QString &scatter_file_name);

    QString getChip_name() const;
    void setChip_name(const QString &chip_name);

    HW_StorageType_E getStorage_type() const;
    void setStorage_type(const HW_StorageType_E &storage_type);

    bool getSkip_pt_operate() const;
    void setSkip_pt_operate(bool skip_pt_operate);

    DownloadPartitionInfoList getDl_partition_list() const;
    void setDl_partition_list(const DownloadPartitionInfoList &dl_partition_list);

    QString getFlash_xml_file() const;
    void setFlash_xml_file(const QString &flash_xml_file);

private:
    QString m_flash_xml_file;
    QString m_contact_name;
    QString m_scatter_file_name;
    QString m_chip_name;
    HW_StorageType_E m_storage_type;
    bool m_skip_pt_operate;
    DownloadPartitionInfoList m_dl_partition_list;
};

#endif // FLASHXMLDATALOADER_H
