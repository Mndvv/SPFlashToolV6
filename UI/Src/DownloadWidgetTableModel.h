#ifndef DOWNLOADWIDGETTABLEMODEL_H
#define DOWNLOADWIDGETTABLEMODEL_H

#include <QAbstractTableModel>
#include "../../Utility/PartitionInfo.h"
#include "../../Inc/CFlashToolTypes.h"

class DownloadWidgetTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    DownloadWidgetTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void resetData(const DownloadPartitionInfoList &download_part_list);
    void setHeaderLabels(const QStringList &headerLabels);

signals:
    void signal_all_item_selected(bool selected);

private:
    DownloadPartitionInfo getPartitionInfo(int index) const;
    bool isCheckableColumn(DOWNLOAD_COLUMN column_type) const;
    bool checkStateEnabledByConfig() const;
    bool isAllItemSelected() const;
    bool isValidIndex(int index) const;
    void checkImageFile();

private:
    DownloadPartitionInfoList m_partition_list;
    QStringList m_horizontal_header_labels;
    bool m_select_enabled;
};

#endif // DOWNLOADWIDGETTABLEMODEL_H
