#ifndef READBACKBACKUP_H
#define READBACKBACKUP_H

#include "../../Utility/PartitionInfo.h"

class ReadbackBackup
{
public:
    ReadbackBackup();

    void setChipName(const QString &chip_name);
    void setStorageType(HW_StorageType_E storage_type);
    QString getChipName() const;
    HW_StorageType_E getStorageType() const;
    void setRBManualList(const ReadbackPartitionInfoList &rb_list);
    void setRBAutoList(const ReadbackPartitionInfoList &rb_list);
    ReadbackPartitionInfoList getRBManualList() const;
    ReadbackPartitionInfoList getRBAutoList() const;

    bool loadFile(const QString &file_name);
    void saveFile(const QString &file_name) const;

private:
    bool loadGeneral(const QDomElement &root_elememnt);
    bool loadRBInfo(const QDomElement &root_element);
    void saveGeneral(QDomDocument &xml_doc, QDomElement &root_element) const;
    void saveRBInfo(QDomDocument &xml_doc, QDomElement &root_elemenet) const;

private:
    QString m_chip_name;
    HW_StorageType_E m_storage_type;
    ReadbackPartitionInfoList m_rb_manual_list;
    ReadbackPartitionInfoList m_rb_auto_list;
};

#endif // READBACKBACKUP_H
