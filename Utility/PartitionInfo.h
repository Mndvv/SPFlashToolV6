#ifndef PARTITIONINFO_H
#define PARTITIONINFO_H

#include <QString>
#include <QVector>
#include <QDomElement>
#include <QMap>
#include "../Inc/CFlashToolTypes.h"

class PartitionInfo
{
public:
    PartitionInfo();

    QString file_name() const;
    void setFile_name(const QString &file_name);

    QString partition_name() const;
    void setPartition_name(const QString &partition_name);

    QString startAddress() const;
    void setStartAddress(const QString &startAddress);

    bool enabled() const;
    void setEnabled(bool enabled);

protected:
    bool m_enabled;
    QString m_partition_name;
    QString m_startAddress;
    QString m_file_name;
};

using DownloadPartitionInfo = PartitionInfo;

using DownloadPartitionInfoList = QVector<DownloadPartitionInfo>;

class ReadbackPartitionInfo: public PartitionInfo
{
public:
    ReadbackPartitionInfo();

    void load(const QDomElement &element_node);
    void save(QDomDocument &xml_doc, QDomElement &element_node) const;

    QString length() const;
    void setLength(const QString &length);

    QString region_region() const;
    void setRegion_text(const QString &region_text);

private:
    QString m_length;
    QString m_region_text;
};

using ReadbackPartitionInfoList = QVector<ReadbackPartitionInfo>;

#endif // PARTITIONINFO_H
