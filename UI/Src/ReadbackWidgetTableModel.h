#ifndef READBACKWIDGETTABLEMODEL_H
#define READBACKWIDGETTABLEMODEL_H

#include <QAbstractTableModel>
#include "../../Utility/PartitionInfo.h"

class ReadbackWidgetTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    ReadbackWidgetTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void resetRBManualData(const ReadbackPartitionInfoList &rb_manual_item_list);
    void resetRBAutoData(const ReadbackPartitionInfoList &rb_auto_item_list);
    ReadbackPartitionInfoList getRBManualList() const;
    ReadbackPartitionInfoList getRBAutoList() const;
    void setHeaderLabels(const QStringList &headerLabels);
    bool isNandStorage() const;
    void setIsNandStorage(bool is_nand_storage);
    QStringList getRegions() const;

public slots:
    void slot_reset_rb_method(READBACK_METHOD rb_method);

signals:
    void signal_all_item_selected(bool selected);

private:
    ReadbackPartitionInfo getRBItem(int index) const;
    const ReadbackPartitionInfoList &getCurrentUsedRBItems() const;
    bool isAllItemSelected() const;
    bool isCheckableColumn(READBACK_COLUMN column_type) const;
    bool isEditableColumn(READBACK_COLUMN column_type) const;
    bool isValidIndex(int index) const;

private:
    ReadbackPartitionInfoList m_rb_manual_item_list;
    ReadbackPartitionInfoList m_rb_auto_item_list;
    QStringList m_horizontal_header_labels;
    READBACK_METHOD m_rb_method;
    bool m_is_nand_storage;
};

#endif // READBACKWIDGETTABLEMODEL_H
