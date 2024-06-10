#ifndef READBACKWIDGETTABLEDELEGATE_H
#define READBACKWIDGETTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "../../Inc/CFlashToolTypes.h"

class ReadbackWidgetTableDelegate: public QStyledItemDelegate
{
public:
    ReadbackWidgetTableDelegate();

    QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
    void setEditorData(QWidget * editor, const QModelIndex & index) const override;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

private slots:
    void commitAndCloseEditor();

private:
    void initRegionMap();
    void initNANDReadFlagMap();
    bool isLineEditColumn(READBACK_COLUMN column_type) const;
    bool isComboBoxColumn(READBACK_COLUMN column_type) const;
    READBACK_COLUMN column2Type(int column) const;
};

#endif // READBACKWIDGETTABLEDELEGATE_H
