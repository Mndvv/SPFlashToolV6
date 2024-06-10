#include "DownloadWidgetTableModel.h"
#include "../../Inc/CFlashToolTypes.h"
#include "../../Utility/OptionIniFileMgr.h"
#include <cassert>
#include <QFileInfo>

DownloadWidgetTableModel::DownloadWidgetTableModel()
{
    m_select_enabled = this->checkStateEnabledByConfig();
}

int DownloadWidgetTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_partition_list.size());
}

int DownloadWidgetTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_horizontal_header_labels.count();
}

QVariant DownloadWidgetTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !isValidIndex(index.row())) {
        return QVariant();
    }

    DOWNLOAD_COLUMN column_type = static_cast<DOWNLOAD_COLUMN>(index.column());
    if (role == Qt::DisplayRole && !isCheckableColumn(column_type)) {
        DownloadPartitionInfo row_item = getPartitionInfo(index.row());
        switch (column_type) {
        case DOWNLOAD_COLUMN::dct_NAME:
            return row_item.partition_name();
        case DOWNLOAD_COLUMN::dct_Addr:
            return row_item.startAddress();
        case DOWNLOAD_COLUMN::dct_FILE_NAME:
            return row_item.file_name();
        default:
            assert(false);
        }
    } else if (role == Qt::CheckStateRole && isCheckableColumn(column_type)) {
        DownloadPartitionInfo row_item = getPartitionInfo(index.row());
        switch (column_type) {
        case DOWNLOAD_COLUMN::dct_ENABLE:
            return row_item.enabled() ? Qt::Checked : Qt::Unchecked;
        default:
            assert(false);
        }
    } else if (DOWNLOAD_COLUMN::dct_FILE_NAME == column_type && role == Qt::ToolTipRole) {
        DownloadPartitionInfo row_item = getPartitionInfo(index.row());
        return row_item.file_name();
    }
    return QVariant();
}

bool DownloadWidgetTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return QAbstractTableModel::setData(index, value, role);
    }
    if (!isValidIndex(index.row())) {
        return false;
    }

    DOWNLOAD_COLUMN column_type = static_cast<DOWNLOAD_COLUMN>(index.column());
    if (role == Qt::EditRole && !isCheckableColumn(column_type)) {
        switch (column_type) {
        case DOWNLOAD_COLUMN::dct_NAME:
            m_partition_list[index.row()].setPartition_name(value.toString());
            break;
        case DOWNLOAD_COLUMN::dct_Addr:
            m_partition_list[index.row()].setStartAddress(value.toString());
            break;
        case DOWNLOAD_COLUMN::dct_FILE_NAME:
            m_partition_list[index.row()].setFile_name(value.toString());
            break;
        default:
            assert(false);
        }
        emit dataChanged(index, index);
        return true;
    } else if (role == Qt::CheckStateRole && isCheckableColumn(column_type)) {
        switch (column_type) {
        case DOWNLOAD_COLUMN::dct_ENABLE:
        {
            Qt::CheckState checked = static_cast<Qt::CheckState>(value.toInt());
            m_partition_list[index.row()].setEnabled(checked == Qt::Checked);
            break;
        }
        default:
            assert(false);
        }
        emit dataChanged(index, index);
        emit signal_all_item_selected(this->isAllItemSelected());
        return true;
    }
    return QAbstractTableModel::setData(index, value, role);
}

QVariant DownloadWidgetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_horizontal_header_labels.at(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags DownloadWidgetTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags item_flags = QAbstractTableModel::flags(index);
    if (index.isValid() && static_cast<DOWNLOAD_COLUMN>(index.column()) == DOWNLOAD_COLUMN::dct_ENABLE) {
        item_flags |= Qt::ItemIsUserCheckable;
        if (m_select_enabled && !m_partition_list[index.row()].file_name().isEmpty()) {
            item_flags |= Qt::ItemIsSelectable;
        } else {
            item_flags &= ~Qt::ItemIsSelectable;
            item_flags &= ~Qt::ItemIsEnabled;
        }
    }
    return item_flags;
}

void DownloadWidgetTableModel::resetData(const DownloadPartitionInfoList &download_part_list)
{
    beginResetModel();
    m_partition_list = download_part_list;
    checkImageFile();
    emit signal_all_item_selected(this->isAllItemSelected());
    endResetModel();
}

void DownloadWidgetTableModel::setHeaderLabels(const QStringList &headerLabels)
{
    m_horizontal_header_labels = headerLabels;
}

DownloadPartitionInfo DownloadWidgetTableModel::getPartitionInfo(int index) const
{
    if (isValidIndex(index)) {
        return m_partition_list.at(index);
    } else {
        return DownloadPartitionInfo();
    }
}

bool DownloadWidgetTableModel::isCheckableColumn(DOWNLOAD_COLUMN column_type) const
{
    return column_type == DOWNLOAD_COLUMN::dct_ENABLE;
}

bool DownloadWidgetTableModel::checkStateEnabledByConfig() const
{
    return OptionIniFileMgr::getInstance()->getDl_only(); // table widget disable when Download_Only set false.
}

bool DownloadWidgetTableModel::isAllItemSelected() const
{
    bool all_selected = !m_partition_list.empty();
    foreach (auto item, m_partition_list) {
        if (!item.enabled() && !item.file_name().isEmpty()) {
            all_selected = false;
            break;
        }
    }
    return all_selected;
}

bool DownloadWidgetTableModel::isValidIndex(int index) const
{
    return index >= 0 && index < m_partition_list.size();
}

void DownloadWidgetTableModel::checkImageFile()
{
    for (int i = 0; i < m_partition_list.size(); ++i) {
        DownloadPartitionInfo & item = m_partition_list[i];
        if (!QFileInfo::exists(item.file_name())) {
            item.setFile_name(QString());
            item.setEnabled(false);
        }
    }
}
