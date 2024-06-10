#include "ReadbackWidgetTableModel.h"
#include <QDir>
#include <QSet>
#include "../../Inc/CFlashToolTypes.h"
#include "../../Utility/FileUtils.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Utility/Utils.h"
#include <cassert>


ReadbackWidgetTableModel::ReadbackWidgetTableModel():
    m_rb_method(READBACK_METHOD::RB_MANUAL),
    m_is_nand_storage(false)
{

}

int ReadbackWidgetTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_rb_method == READBACK_METHOD::RB_MANUAL)
        return static_cast<int>(m_rb_manual_item_list.size());
    else
        return static_cast<int>(m_rb_auto_item_list.size());
}

int ReadbackWidgetTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_horizontal_header_labels.count();
}

QVariant ReadbackWidgetTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !isValidIndex(index.row())) {
        return QVariant();
    }

    READBACK_COLUMN column_type = static_cast<READBACK_COLUMN>(index.column());
    if (role == Qt::DisplayRole && !isCheckableColumn(column_type)) {
        ReadbackPartitionInfo cur_rb_item = getRBItem(index.row());
        switch (column_type) {
        case READBACK_COLUMN::rbc_Name:
            return cur_rb_item.partition_name();
        case READBACK_COLUMN::rbc_Addr:
            return cur_rb_item.startAddress();
        case READBACK_COLUMN::rbc_Length:
            return cur_rb_item.length();
        case READBACK_COLUMN::rbc_Region:
            return cur_rb_item.region_region();
        case READBACK_COLUMN::rbc_File:
            return cur_rb_item.file_name();
        default:
            assert(false);
        }
    } else if (role == Qt::CheckStateRole && isCheckableColumn(column_type)) {
        ReadbackPartitionInfo cur_rb_item = getRBItem(index.row());
        switch (column_type) {
        case READBACK_COLUMN::rbc_Enabled:
            return cur_rb_item.enabled() ? Qt::Checked : Qt::Unchecked;
        default:
            assert(false);
        }
    } else if (column_type == READBACK_COLUMN::rbc_File && role == Qt::ToolTipRole) {
        ReadbackPartitionInfo cur_rb_item = getRBItem(index.row());
        return cur_rb_item.file_name();
    }
    return QVariant();
}

bool ReadbackWidgetTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return QAbstractTableModel::setData(index, value, role);
    }
    if (!isValidIndex(index.row())) {
        return false;
    }

    READBACK_COLUMN column_type = static_cast<READBACK_COLUMN>(index.column());
    if (role == Qt::EditRole && !isCheckableColumn(column_type)) {
        ReadbackPartitionInfo cur_rb_item = this->getRBItem(index.row());
        switch (column_type) {
        case READBACK_COLUMN::rbc_Name:
        {
            cur_rb_item.setPartition_name(value.toString());
            break;
        }
        case READBACK_COLUMN::rbc_Addr:
        {
            cur_rb_item.setStartAddress(value.toString());
            break;
        }
        case READBACK_COLUMN::rbc_Length:
        {
            cur_rb_item.setLength(value.toString());
            break;
        }
        case READBACK_COLUMN::rbc_Region:
        {
            cur_rb_item.setRegion_text(value.toString());
            break;
        }
        case READBACK_COLUMN::rbc_File:
        {
            cur_rb_item.setFile_name(value.toString());
            break;
        }
        default:
            assert(false);
        }
        if (m_rb_method == READBACK_METHOD::RB_MANUAL)
            m_rb_manual_item_list.replace(index.row(), cur_rb_item);
        else
            m_rb_auto_item_list.replace(index.row(), cur_rb_item);
        emit dataChanged(index, index);
        return true;
    } else if (role == Qt::CheckStateRole && isCheckableColumn(column_type)) {
        ReadbackPartitionInfo cur_rb_item = this->getRBItem(index.row());
        switch (column_type) {
        case READBACK_COLUMN::rbc_Enabled:
        {
            Qt::CheckState check_state = static_cast<Qt::CheckState>(value.toInt());
            cur_rb_item.setEnabled(check_state == Qt::Checked);
            break;
        }
        default:
            assert(false);
        }
        if (m_rb_method == READBACK_METHOD::RB_MANUAL)
            m_rb_manual_item_list.replace(index.row(), cur_rb_item);
        else
            m_rb_auto_item_list.replace(index.row(), cur_rb_item);
        emit dataChanged(index, index);
        emit signal_all_item_selected(this->isAllItemSelected());
        return true;
    }
    return QAbstractTableModel::setData(index, value, role);
}

QVariant ReadbackWidgetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_horizontal_header_labels.at(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags ReadbackWidgetTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags item_flags = QAbstractTableModel::flags(index);
    if (!index.isValid()) {
        return item_flags;
    }
    READBACK_COLUMN rb_column =  static_cast<READBACK_COLUMN>(index.column());
    if (this->isCheckableColumn(rb_column)) {
        item_flags |= Qt::ItemIsUserCheckable;
    } else if (this->isEditableColumn(rb_column)) {
        item_flags |= Qt::ItemIsEditable;
    }
    return item_flags;
}

void ReadbackWidgetTableModel::resetRBManualData(const ReadbackPartitionInfoList &rb_manual_item_list)
{
    beginResetModel();
    m_rb_manual_item_list = rb_manual_item_list;
    emit signal_all_item_selected(this->isAllItemSelected());
    endResetModel();
}

void ReadbackWidgetTableModel::resetRBAutoData(const ReadbackPartitionInfoList &rb_auto_item_list)
{
    beginResetModel();
    m_rb_auto_item_list = rb_auto_item_list;
    emit signal_all_item_selected(this->isAllItemSelected());
    endResetModel();
}

ReadbackPartitionInfoList ReadbackWidgetTableModel::getRBManualList() const
{
    return m_rb_manual_item_list;
}

ReadbackPartitionInfoList ReadbackWidgetTableModel::getRBAutoList() const
{
    return m_rb_auto_item_list;
}

void ReadbackWidgetTableModel::setHeaderLabels(const QStringList &headerLabels)
{
    m_horizontal_header_labels = headerLabels;
}

bool ReadbackWidgetTableModel::isNandStorage() const
{
    return m_is_nand_storage;
}

void ReadbackWidgetTableModel::setIsNandStorage(bool is_nand_storage)
{
    beginResetModel();
    m_is_nand_storage = is_nand_storage;
    endResetModel();
}

QStringList ReadbackWidgetTableModel::getRegions() const
{
    return m_is_nand_storage ? Utils::getNANDRegionList() : Utils::getEMMCUFSRegionList();
}

bool ReadbackWidgetTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (m_rb_method != READBACK_METHOD::RB_MANUAL) {
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    for (int index = row; index < row + count; ++index) {
        ReadbackPartitionInfo rb_part_info;
        rb_part_info.setFile_name(FileUtils::generateRBFileName());
        QString default_region_text = m_is_nand_storage ? NAND_WHOLE_VAL : UFS_LUA2_VAL;
        rb_part_info.setRegion_text(default_region_text);
        rb_part_info.setEnabled(true);
        m_rb_manual_item_list.push_back(rb_part_info);
    }
    endInsertRows();
    emit signal_all_item_selected(this->isAllItemSelected());
    return true;
}

bool ReadbackWidgetTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (m_rb_method != READBACK_METHOD::RB_MANUAL) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    for (int index = row; index < row + count; ++index) {
        m_rb_manual_item_list.removeAt(index);
    }
    endRemoveRows();
    emit signal_all_item_selected(this->isAllItemSelected());
    return true;
}

void ReadbackWidgetTableModel::slot_reset_rb_method(READBACK_METHOD rb_method)
{
    beginResetModel();
    m_rb_method = rb_method;
    emit signal_all_item_selected(this->isAllItemSelected());
    endResetModel();
}

ReadbackPartitionInfo ReadbackWidgetTableModel::getRBItem(int index) const
{
    if (this->isValidIndex(index)) {
        return this->getCurrentUsedRBItems().at(index);
    }
    return ReadbackPartitionInfo();
}

const ReadbackPartitionInfoList &ReadbackWidgetTableModel::getCurrentUsedRBItems() const
{
    return m_rb_method == READBACK_METHOD::RB_MANUAL ? m_rb_manual_item_list : m_rb_auto_item_list;
}

bool ReadbackWidgetTableModel::isAllItemSelected() const
{
    const ReadbackPartitionInfoList & current_used_rb_items = this->getCurrentUsedRBItems();
    bool all_selected = !current_used_rb_items.empty();
    foreach (auto item, current_used_rb_items) {
        if (!item.enabled()) {
            all_selected = false;
            break;
        }
    }
    return all_selected;
}

bool ReadbackWidgetTableModel::isCheckableColumn(READBACK_COLUMN column_type) const
{
    return column_type == READBACK_COLUMN::rbc_Enabled;
}

bool ReadbackWidgetTableModel::isEditableColumn(READBACK_COLUMN column_type) const
{
    QSet<READBACK_COLUMN> edit_set;
    edit_set << READBACK_COLUMN::rbc_Addr << READBACK_COLUMN::rbc_Length << READBACK_COLUMN::rbc_Region;
    return edit_set.contains(column_type);
}

bool ReadbackWidgetTableModel::isValidIndex(int index) const
{
    const ReadbackPartitionInfoList & current_used_rb_items = this->getCurrentUsedRBItems();
    return index >= 0 && index < current_used_rb_items.count();
}
