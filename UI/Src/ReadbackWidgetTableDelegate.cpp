#include "ReadbackWidgetTableDelegate.h"
#include <QLineEdit>
#include <QComboBox>
#include <QRegExpValidator>
#include "../../Inc/CFlashToolTypes.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Utility/PartitionInfo.h"
#include "ReadbackWidgetTableModel.h"

ReadbackWidgetTableDelegate::ReadbackWidgetTableDelegate(): QStyledItemDelegate()
{

}

QWidget *ReadbackWidgetTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
    if (isLineEditColumn(column2Type(index.column()))) {
        QLineEdit *editor = new QLineEdit(parent);
        connect(editor, &QLineEdit::editingFinished, this, &ReadbackWidgetTableDelegate::commitAndCloseEditor);
        editor->setValidator(new QRegExpValidator(QRegExp("0x[\\da-fA-F]{1,16}"), parent));
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        editor->setText(text);
        return editor;
    } else if (isComboBoxColumn(column2Type(index.column()))) {
        QComboBox *comboBox = new QComboBox(parent);
        if (column2Type(index.column()) == READBACK_COLUMN::rbc_Region) {
            const ReadbackWidgetTableModel *model = dynamic_cast<const ReadbackWidgetTableModel *>(index.model());
            comboBox->addItems(model->getRegions());
            QString current_region = index.model()->data(index, Qt::DisplayRole).toString();
            comboBox->setCurrentText(current_region);
        }
        connect(comboBox, &QComboBox::currentTextChanged, this, &ReadbackWidgetTableDelegate::commitAndCloseEditor);
        return comboBox;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ReadbackWidgetTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return ;
    }
    if (isLineEditColumn(column2Type(index.column()))) {
        QString text = index.model()->data(index, Qt::DisplayRole).toString().trimmed();
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
    } else if (isComboBoxColumn(column2Type(index.column()))) {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if (column2Type(index.column()) == READBACK_COLUMN::rbc_Region) {
            QString region_text = index.model()->data(index, Qt::DisplayRole).toString().trimmed();
            comboBox->setCurrentText(region_text);
        }
        comboBox->setStyleSheet(comboBox_style_sheet);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ReadbackWidgetTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return ;
    }
    if (isLineEditColumn(column2Type(index.column()))) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        QString text = lineEdit->text().trimmed();
        model->setData(index, text, Qt::EditRole);
    } else if (isComboBoxColumn(column2Type(index.column()))) {
        if (column2Type(index.column()) == READBACK_COLUMN::rbc_Region) {
            QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
            QString region_text = comboBox->currentText();
            model->setData(index, region_text, Qt::EditRole);
        }
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ReadbackWidgetTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void ReadbackWidgetTableDelegate::commitAndCloseEditor()
{
    QWidget *editor = qobject_cast<QWidget*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

bool ReadbackWidgetTableDelegate::isLineEditColumn(READBACK_COLUMN column_type) const
{
    return column_type == READBACK_COLUMN::rbc_Addr || column_type == READBACK_COLUMN::rbc_Length;
}

bool ReadbackWidgetTableDelegate::isComboBoxColumn(READBACK_COLUMN column_type) const
{
    return column_type == READBACK_COLUMN::rbc_Region;
}

READBACK_COLUMN ReadbackWidgetTableDelegate::column2Type(int column) const
{
    return static_cast<READBACK_COLUMN>(column);
}
