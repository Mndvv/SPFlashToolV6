#include "ReadBackWidget.h"
#include "ui_ReadBackWidget.h"

#include "CheckHeader.h"
#include "../../Inc/CFlashToolConst.h"
#include "MainWindow.h"
#include "ReadbackWidgetTableModel.h"
#include "ReadbackWidgetTableDelegate.h"
#include "../../Utility/Utils.h"
#include "../../XML/CmdGenerator/ReadFlashCmdXML.h"
#include "../../XML/CmdGenerator/ReadPartitionCmdXML.h"
#include "../../XML/CmdGenerator/ReadPartitionTblCmdXML.h"
#include "../../Cmd/CmdWrapper.h"
#include <QFileDialog>
#include <functional>
#include "BackgroundWorker.h"
#include "MultiLanguageMapping.h"
#include "../../Utility/FileUtils.h"
#include "../../Logger/Log.h"
#include "../../ConsoleMode/SchemaValidator.h"
#include "../../Utility/Utils.h"
#include "ReadbackBackup.h"
#include "../../XML/Parser/PartitionTableParser.h"
#include "../../Err/CFlashToolErrorCode.h"
#include <cassert>

using namespace std::placeholders;

static const QString PReadbackColorBlue ("QProgressBar::chunk{background-color: blue} QProgressBar {text-align: center; border-radius: 0px}");

#define RB_UI_BACKUP_FILE ABS_PATH("readback_ui_bak.xml")

ReadBackWidget::ReadBackWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(READBACK_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_READ_BACK_PAGE_NAME), parent),
    ui(new Ui::ReadBackWidget),
    m_mainwindow(mainwindow),
    m_context_menu(nullptr),
    m_add_act(nullptr),
    m_remove_act(nullptr),
    m_show_start_addr(nullptr),
    m_show_length(nullptr),
    m_auto_RB_finished(true)
{
    ui->setupUi(this);

    setUIInitState();

    connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(bool)),
            this, SLOT(slot_flash_xml_changed(bool)));
    connect(ui->radioButton_auto, &QRadioButton::clicked, this, &ReadBackWidget::slot_RB_method_changed);
    connect(ui->radioButton_manual, &QRadioButton::clicked, this, &ReadBackWidget::slot_RB_method_changed);
}

ReadBackWidget::~ReadBackWidget()
{
    delete ui;
}

void ReadBackWidget::cbReadbackProgress(void *_this, unsigned int progress, unsigned long long data_xferd,
                                        const QString &info)
{
    QString progress_text;
    if (ui->radioButton_auto->isChecked() && !info.isEmpty()) {
        progress_text = tr("[%1] Read back %2%").arg(info).arg(progress);
    } else {
        progress_text = tr("Read back %1%").arg(progress);
    }
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, PReadbackColorBlue, progress_text, true);
}

void ReadBackWidget::reStartReadback()
{
    this->setupManualRBCmdList();
    m_mainwindow->startCmd();
}

CONN_END_STAGE ReadBackWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

CmdWrapperSPList ReadBackWidget::createRBCmdWrapperList()
{
    return ui->radioButton_manual->isChecked() ? this->createManualRBCmdWrapperList() : this->createAutoRBCmdWrapperList();
}

CmdWrapperSPList ReadBackWidget::createManualRBCmdWrapperList()
{
    CmdWrapperSPList cmd_wrapper_list;

    foreach(auto cmd_wrapper, this->createReadFlashCmdWrapperList()) {
        cmd_wrapper_list.push_back(cmd_wrapper);
    }

    cmd_wrapper_list.push_back(m_mainwindow->createWDTCmdWrapper());

    return cmd_wrapper_list;
}

CmdWrapperSPList ReadBackWidget::createAutoRBCmdWrapperList()
{
    CmdWrapperSPList cmd_wrapper_list;
    foreach(auto cmd_wrapper, this->createReadPartitionCmdWrapperList()) {
        cmd_wrapper_list.push_back(cmd_wrapper);
    }

    cmd_wrapper_list.push_back(m_mainwindow->createWDTCmdWrapper());

    return cmd_wrapper_list;
}

void ReadBackWidget::LockOnUI()
{
    ui->toolButton_add->setEnabled(false);
    ui->toolButton_remove->setEnabled(false);
    ui->toolButton_readBack->setEnabled(false);
    ui->toolButton_stop->setEnabled(true);
    ui->groupBox_read_method->setEnabled(false);
    ui->toolButton_read_pt->setEnabled(false);
    ui->tableView->setEnabled(false);
    m_context_menu->setEnabled(false);
}

void ReadBackWidget::DoFinished()
{
    ui->toolButton_add->setEnabled(true);
    updateBtnEnabledByRBData();
    ui->toolButton_stop->setEnabled(false);
    ui->groupBox_read_method->setEnabled(true);
    ui->toolButton_read_pt->setEnabled(true);
    ui->tableView->setEnabled(true);
    m_auto_RB_finished = true;
    m_context_menu->setEnabled(true);
}

void ReadBackWidget::setUIInitState()
{
    ui->toolButton_read_pt->setVisible(false);
    ui->label_prompt->setVisible(false);
    initTableWidget();
    updateColumnVisible();
}

void ReadBackWidget::initTableWidget()
{
    ReadbackWidgetTableModel *table_model = new ReadbackWidgetTableModel();
    table_model->setHeaderLabels(getTableHeaderLabels());
    QObject::connect(table_model, &ReadbackWidgetTableModel::signal_all_item_selected,
                this, &ReadBackWidget::slot_all_items_selected);
    connect(this, &ReadBackWidget::signal_reset_rb_method, table_model, &ReadbackWidgetTableModel::slot_reset_rb_method);
    ui->tableView->setModel(table_model);

    ReadbackWidgetTableDelegate *table_delegate = new ReadbackWidgetTableDelegate();
    ui->tableView->setItemDelegate(table_delegate);

    CheckHeader *header_view = new CheckHeader(Qt::Horizontal, ui->tableView);
    QObject::connect(header_view, SIGNAL(signal_enable_checked(bool)),
                     this, SLOT(slot_enable_checked(bool)));
    ui->tableView->setHorizontalHeader(header_view);

    QObject::connect(ui->tableView, &QTableView::doubleClicked,
                     this, &ReadBackWidget::slot_table_doubleClicked);
    adjustTableColWidth();

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested,
            this, &ReadBackWidget::slot_show_context_menu);
    createContextMenu();
}

void ReadBackWidget::adjustTableColWidth()
{
    // MUST set column width after model & delegate to make column width setting affect.
    ui->tableView->setColumnWidth(READBACK_COLUMN::rbc_Enabled, 20);
    ui->tableView->setColumnWidth(READBACK_COLUMN::rbc_Name, 100);
    ui->tableView->setColumnWidth(READBACK_COLUMN::rbc_Addr, 140);
    ui->tableView->setColumnWidth(READBACK_COLUMN::rbc_Length, 140);
    ui->tableView->setColumnWidth(READBACK_COLUMN::rbc_Region, 115);
}

void ReadBackWidget::createContextMenu()
{
    m_context_menu = new QMenu(this);

    m_add_act = new QAction(tr("&Add"), m_context_menu);
    connect(m_add_act, &QAction::triggered, this, &ReadBackWidget::on_toolButton_add_clicked);

    m_remove_act = new QAction(tr("&Remove"), m_context_menu);
    connect(m_remove_act, &QAction::triggered, this, &ReadBackWidget::on_toolButton_remove_clicked);

    m_show_start_addr = new QAction(GET_TRANSLATE_STRING(IDS_STRING_SHOW_START_LENGTH_COLUMN), m_context_menu);
    m_show_start_addr->setCheckable(true);
    connect(m_show_start_addr, &QAction::toggled, this, &ReadBackWidget::slot_show_start_addr_toggled);

    m_show_length = new QAction(GET_TRANSLATE_STRING(IDS_STRING_SHOW_LENGTH_COLUMN), m_context_menu);
    m_show_length->setCheckable(true);
    connect(m_show_length, &QAction::toggled, this, &ReadBackWidget::slot_show_length_toggled);

    m_context_menu->setStyleSheet(context_menu_style_sheet);

    connect(m_context_menu, &QMenu::aboutToShow, this, &ReadBackWidget::slot_context_menu_about_to_show);
}

QStringList ReadBackWidget::getTableHeaderLabels() const
{
    return QStringList() << QStringLiteral("") << tr("Name") << tr("Start Address")
                         << tr("Length") << tr("Region") << tr("File");
}

void ReadBackWidget::replaceRBFile(const QModelIndex &index)
{
    QString file = index.model()->data(index, Qt::DisplayRole).toString();
    QString new_file = QFileDialog::getSaveFileName(this,tr("Save File"), file);;
    if(!new_file.isEmpty())
    {
        new_file = QDir::toNativeSeparators(new_file);
        const ReadbackWidgetTableModel *model = dynamic_cast<const ReadbackWidgetTableModel *>(index.model());
        assert(model != nullptr);
        const_cast<ReadbackWidgetTableModel *>(model)->setData(index, new_file, Qt::EditRole);
    }
}

QPair<bool, QString> ReadBackWidget::validateBeforeReadback() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    if (!this->hasRBItemSelected()) {
        result.first = false;
        result.second = QStringLiteral("Please add a read back item before execution.");
    }
    return result;
}

bool ReadBackWidget::hasRBItemSelected() const
{
    bool has_rb_item_selected = false;
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, READBACK_COLUMN::rbc_Enabled);
        Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
        if (check_state == Qt::Checked) {
            has_rb_item_selected = true;
            break;
        }
    }
    return has_rb_item_selected;
}

void ReadBackWidget::setupManualRBCmdList()
{
    m_mainwindow->addCmdWrapper(this->createManualRBCmdWrapperList());
}

CmdWrapperSPList ReadBackWidget::createReadFlashCmdWrapperList()
{
    CmdWrapperSPList cmdWrapperList;

    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, READBACK_COLUMN::rbc_Enabled);
        Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
        if (check_state == Qt::Checked) {
            QString region_text = model->index(row, READBACK_COLUMN::rbc_Region).data(Qt::DisplayRole).toString();
            QString start_address = model->index(row, READBACK_COLUMN::rbc_Addr).data(Qt::DisplayRole).toString();
            QString length = model->index(row, READBACK_COLUMN::rbc_Length).data(Qt::DisplayRole).toString();
            QString rb_file_name = model->index(row, READBACK_COLUMN::rbc_File).data(Qt::DisplayRole).toString();
            cmdWrapperList.push_back(this->createReadFlashCmdWrapper(region_text, start_address, length, rb_file_name));
        }
    }

    return cmdWrapperList;
}

CmdWrapperSPList ReadBackWidget::createReadPartitionCmdWrapperList()
{
    CmdWrapperSPList cmdWrapperList;

    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, READBACK_COLUMN::rbc_Enabled);
        Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
        if (check_state == Qt::Checked) {
            QString partition_name = model->index(row, READBACK_COLUMN::rbc_Name).data(Qt::DisplayRole).toString();
            QString rb_file_name = model->index(row, READBACK_COLUMN::rbc_File).data(Qt::DisplayRole).toString();
            cmdWrapperList.push_back(this->createReadPartitionCmdWrapper(partition_name, rb_file_name));
        }
    }

    return cmdWrapperList;
}

std::shared_ptr<CmdWrapper> ReadBackWidget::createReadFlashCmdWrapper(const QString &region_text,
                                                                      const QString &start_addr,
                                                                      const QString &length,
                                                                      const QString &file_name)
{
    std::shared_ptr<ReadFlashCmdXML> readFlashCmdXML = std::make_shared<ReadFlashCmdXML>();
    readFlashCmdXML->setPartition(region_text);
    readFlashCmdXML->setOffSet(start_addr);
    readFlashCmdXML->setLength(length);
    readFlashCmdXML->setFileName(file_name);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(readFlashCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&ReadBackWidget::cbReadbackProgress, this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> ReadBackWidget::createReadPartitionCmdWrapper(const QString &partition_name, const QString &file_name)
{
    std::shared_ptr<ReadPartitionCmdXML> readPartitionCmdXML = std::make_shared<ReadPartitionCmdXML>();
    readPartitionCmdXML->setPartition(partition_name);
    readPartitionCmdXML->setFileName(file_name);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(readPartitionCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&ReadBackWidget::cbReadbackProgress, this, _1, _2, _3, _4));
    return cmd_wrapper;
}

cmd_wrapper_ptr ReadBackWidget::createReadPartitionTableCmdWrapper()
{
    //create cmd to read partition table from device.
    std::shared_ptr<ReadPartitionTblCmdXML> readPartitionTblCmdXML = std::make_shared<ReadPartitionTblCmdXML>();
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(readPartitionTblCmdXML);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &ReadBackWidget::slot_after_read_partition_table_cmd, Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

bool ReadBackWidget::validReadBackXMLFormat(const QString &file_name) const
{
    QString err_msg;
    if (!FileUtils::validFile(file_name, err_msg))
    {
        LOG(qPrintable(err_msg));
        return false;
    }
    ConsoleMode::SchemaValidator schemaValidator(ABS_PATH_C("readback_ui_bak.xsd"));
    try
    {
        schemaValidator.Validate(file_name);
        return true;
    }
    catch (...)
    {
        LOGI("Invalid readback setting xml file.");
        return false;
    }
}

bool ReadBackWidget::isRBXMLMatched(const ReadbackBackup &rb_backup) const
{
    QString current_chip_name = m_mainwindow->getChipName();
    HW_StorageType_E current_storage_type = m_mainwindow->getStorageType();
    return rb_backup.getChipName() == current_chip_name && rb_backup.getStorageType() == current_storage_type;
}

void ReadBackWidget::updateBtnEnabledByRBData() const
{
    bool isEnabled = ui->tableView->model()->rowCount() > 0;
    ui->toolButton_remove->setEnabled(isEnabled);
    ui->toolButton_readBack->setEnabled(isEnabled);
}

void ReadBackWidget::updateColumnVisible() const
{
    hideAllOptionalColumns();
    if (ui->radioButton_auto->isChecked()) {
        this->showAutoRBColumns();
    } else {
        showManualRBColumns();
    }
}

void ReadBackWidget::hideAllOptionalColumns() const
{
    ui->tableView->hideColumn(READBACK_COLUMN::rbc_Name);
    ui->tableView->hideColumn(READBACK_COLUMN::rbc_Addr);
    ui->tableView->hideColumn(READBACK_COLUMN::rbc_Length);
    ui->tableView->hideColumn(READBACK_COLUMN::rbc_Region);
}

void ReadBackWidget::showManualRBColumns() const
{
    ui->tableView->showColumn(READBACK_COLUMN::rbc_Addr);
    ui->tableView->showColumn(READBACK_COLUMN::rbc_Length);
    ui->tableView->showColumn(READBACK_COLUMN::rbc_Region);
}

void ReadBackWidget::showAutoRBColumns() const
{
    ui->tableView->showColumn(READBACK_COLUMN::rbc_Name);
}

void ReadBackWidget::manualReadback()
{
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    this->setupManualRBCmdList();
    m_mainwindow->startCmd(tr("Readback Ok"));
}

void ReadBackWidget::autoReadbackWithReadPt()
{
    ui->toolButton_readBack->setEnabled(false);
    ui->tableView->setEnabled(false);
    m_mainwindow->addInteractionCmdWrapperList(this->createAutoRBCmdWrapperList());
    BackgroundWorker::m_interaction_finished = true;
}

void ReadBackWidget::autoReadbackWithoutReadPt()
{
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(this->createAutoRBCmdWrapperList());
    m_mainwindow->startCmd(tr("Readback Ok"));
}

void ReadBackWidget::slot_flash_xml_changed(bool is_nand_storage)
{
    ReadbackWidgetTableModel *model = dynamic_cast<ReadbackWidgetTableModel *>(ui->tableView->model());
    assert(nullptr != model);
    model->setIsNandStorage(is_nand_storage);

    ReadbackPartitionInfoList manual_rb_list = model->getRBManualList();
    QString default_region_text = is_nand_storage ? NAND_WHOLE_VAL : UFS_LUA2_VAL;
    // no need to modify m_rb_auto_item_list data, because they are no use.
    for (int i = 0; i < manual_rb_list.count(); ++i) {
        manual_rb_list[i].setRegion_text(default_region_text);
    }
    model->resetRBManualData(manual_rb_list);
}

void ReadBackWidget::slot_RB_method_changed(bool checked)
{
    Q_UNUSED(checked)

    bool is_manual_rb = ui->radioButton_manual->isChecked();
    ui->toolButton_read_pt->setVisible(!is_manual_rb);
    ui->label_prompt->setVisible(!is_manual_rb);
    ui->toolButton_add->setVisible(is_manual_rb);
    ui->toolButton_remove->setVisible(is_manual_rb);

    updateColumnVisible();

    emit signal_reset_rb_method(is_manual_rb ? READBACK_METHOD::RB_MANUAL : READBACK_METHOD::RB_AUTO);

    updateBtnEnabledByRBData();
}

void ReadBackWidget::slot_show_start_addr_toggled(bool checked)
{
    ui->tableView->setColumnHidden(READBACK_COLUMN::rbc_Addr, !checked);
}

void ReadBackWidget::slot_show_length_toggled(bool checked)
{
    ui->tableView->setColumnHidden(READBACK_COLUMN::rbc_Length, !checked);
}

void ReadBackWidget::autoLoadRBSetting()
{
    QString rb_backup_file = QString::fromStdString(RB_UI_BACKUP_FILE);
    rb_backup_file = QDir::toNativeSeparators(rb_backup_file);
    if (validReadBackXMLFormat(rb_backup_file))
    {
        ReadbackBackup rb_backup;
        if (rb_backup.loadFile(rb_backup_file) && isRBXMLMatched(rb_backup)) {
            ReadbackWidgetTableModel *model = dynamic_cast<ReadbackWidgetTableModel *>(ui->tableView->model());
            assert(nullptr != model);
            model->resetRBManualData(rb_backup.getRBManualList());
            model->resetRBAutoData(rb_backup.getRBAutoList());
        }
    }
    updateBtnEnabledByRBData();
}

void ReadBackWidget::autoSaveRBSetting(const QString &chip_name, HW_StorageType_E storage_type) const
{
    QString file_name = QString::fromStdString(RB_UI_BACKUP_FILE);
    file_name = QDir::toNativeSeparators(file_name);
    if (QFile::exists(file_name)) {
        QFile::remove(file_name); //remove the old file always.
    }

    ReadbackWidgetTableModel *model = dynamic_cast<ReadbackWidgetTableModel *>(ui->tableView->model());
    assert(nullptr != model);
    ReadbackPartitionInfoList rb_manual_list = model->getRBManualList();
    ReadbackPartitionInfoList rb_auto_list = model->getRBAutoList();
    if (rb_manual_list.count() > 0 || rb_auto_list.count() > 0)
    {
        ReadbackBackup rb_backup;
        rb_backup.setChipName(chip_name);
        rb_backup.setStorageType(storage_type);
        rb_backup.setRBManualList(rb_manual_list);
        rb_backup.setRBAutoList(rb_auto_list);
        rb_backup.saveFile(file_name);
    }
}

void ReadBackWidget::on_toolButton_readBack_clicked()
{
    QPair<bool, QString> valid_result = this->validateBeforeReadback();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }

    if (ui->radioButton_manual->isChecked()) {
        // manual readback flow
        this->manualReadback();
    } else {
        // auto readback flow
        if (m_auto_RB_finished) {
            // readback by partition name separately.
            this->autoReadbackWithoutReadPt();
        } else {
            // readback by partition name in read PT flow.
            this->autoReadbackWithReadPt();
        }
    }
}

void ReadBackWidget::on_toolButton_read_pt_clicked()
{
    if (!m_mainwindow->isFlashXMLLoaded())
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX,
                                     STRING_TOOL_NAME, STRING_SELECT_FLASH_XML, STRING_OK);
        return ;
    }

    m_auto_RB_finished = false;
    m_mainwindow->setNeedInteraction(true);
    BackgroundWorker::m_interaction_finished = false;
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(createReadPartitionTableCmdWrapper());
    m_mainwindow->startCmd(tr("Readback Ok"));
}

void ReadBackWidget::on_toolButton_stop_clicked()
{
    if (!m_auto_RB_finished) {
        BackgroundWorker::m_interaction_finished = true;
    }
    m_mainwindow->stopCmd();
}

void ReadBackWidget::on_toolButton_add_clicked()
{
    if (!m_mainwindow->isFlashXMLLoaded())
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX,
                                     STRING_TOOL_NAME, STRING_SELECT_FLASH_XML, STRING_OK);
        return ;
    }
    QAbstractItemModel *model = ui->tableView->model();
    model->insertRow(model->rowCount());
    updateBtnEnabledByRBData();
}

void ReadBackWidget::on_toolButton_remove_clicked()
{
    QModelIndex cur_index = ui->tableView->currentIndex();
    if (cur_index.isValid()) {
        ui->tableView->model()->removeRow(cur_index.row());
        updateBtnEnabledByRBData();
    } else {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME,
                                     tr("Please select at least one row before remove."),
                                     STRING_OK);
    }
}

void ReadBackWidget::slot_enable_checked(bool checked)
{
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, READBACK_COLUMN::rbc_Enabled);
        Qt::CheckState old_state = static_cast<Qt::CheckState>(model->data(index, Qt::CheckStateRole).toInt());
        Qt::CheckState new_state = checked ? Qt::Checked : Qt::Unchecked;
        if (new_state != old_state)
        {
            model->setData(index, new_state, Qt::CheckStateRole);
        }
    }
}

void ReadBackWidget::slot_table_doubleClicked(const QModelIndex &index)
{
    assert(index.isValid());
    READBACK_COLUMN column_type = static_cast<READBACK_COLUMN>(index.column());
    if (column_type == READBACK_COLUMN::rbc_File)
    {
        replaceRBFile(index);
    }
}

void ReadBackWidget::slot_all_items_selected(bool all_selected)
{
    CheckHeader *header_view = dynamic_cast<CheckHeader *>(ui->tableView->horizontalHeader());
    assert(nullptr != header_view);
    header_view->SetChecked(all_selected);
}

void ReadBackWidget::slot_show_context_menu(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (nullptr != m_context_menu) {
        if (ui->radioButton_manual->isChecked()) {
            m_context_menu->removeAction(m_show_start_addr);
            m_context_menu->removeAction(m_show_length);
            m_context_menu->addAction(m_add_act);
            m_context_menu->addAction(m_remove_act);
        } else {
            m_context_menu->removeAction(m_add_act);
            m_context_menu->removeAction(m_remove_act);

            QString text = m_show_start_addr->isChecked() ? GET_TRANSLATE_STRING(IDS_STRING_HIDE_START_LENGTH_COLUMN) : GET_TRANSLATE_STRING(IDS_STRING_SHOW_START_LENGTH_COLUMN);
            m_show_start_addr->setText(text);
            text = m_show_length->isChecked() ? GET_TRANSLATE_STRING(IDS_STRING_HIDE_LENGTH_COLUMN) : GET_TRANSLATE_STRING(IDS_STRING_SHOW_LENGTH_COLUMN);
            m_show_length->setText(text);

            m_context_menu->addAction(m_show_start_addr);
            m_context_menu->addAction(m_show_length);
        }

        m_context_menu->exec(cursor().pos());
    }
}

void ReadBackWidget::slot_context_menu_about_to_show()
{
    bool has_rb_item = ui->tableView->model()->rowCount() > 0;
    m_remove_act->setEnabled(has_rb_item);
}

void ReadBackWidget::slot_after_read_partition_table_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                                         const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    Q_UNUSED(conn)
    *p_result = CFT_SUCCESS;

    ui->tableView->setEnabled(true);
    ui->toolButton_readBack->setEnabled(true);

    std::shared_ptr<ReadPartitionTblCmdXML> read_pt_table_xml =
            std::dynamic_pointer_cast<ReadPartitionTblCmdXML>(iCmdXMLGenerator);
    assert(read_pt_table_xml);
    PartitionTableParser pt_parser;
    if (!pt_parser.parseXMLString(read_pt_table_xml->getCmdValue())) {
        LOG(qPrintable(pt_parser.get_error_msg()));
        return ;
    }

    ReadbackWidgetTableModel *model = dynamic_cast<ReadbackWidgetTableModel *>(ui->tableView->model());
    assert(nullptr != model);
    model->resetRBAutoData(pt_parser.getRBList());
}

void ReadBackWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_READ_BACK_PAGE_NAME));

    ReadbackWidgetTableModel *table_mode = dynamic_cast<ReadbackWidgetTableModel *>(ui->tableView->model());
    table_mode->setHeaderLabels(getTableHeaderLabels());
    m_add_act->setText(tr("&Add"));
    m_remove_act->setText(tr("&Remove"));
}
