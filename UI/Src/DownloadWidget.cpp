#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "../../Inc/CFlashToolConst.h"
#include "MainWindow.h"
#include "../../Utility/PartitionInfo.h"
#include "DownloadWidgetTableModel.h"
#include "CheckHeader.h"
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <functional>
#include <string>
#include "../../Utility/OptionIniFileMgr.h"
#include "../../Utility/version.h"
#include "../../Utility/Utils.h"
#include "../../Utility/FileUtils.h"
#include "../../Logger/Log.h"
#include "../../XML/Parser/RSCParser.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../XML/CmdGenerator/EMMCControlCmdXML.h"
#include "../../XML/CmdGenerator/SetBootModeCmdXML.h"
#include "../../XML/CmdGenerator/SetRSCCmdXML.h"
#include "../../XML/CmdGenerator/FlashAllExCmdXML.h"
#include "../../XML/CmdGenerator/FlashUpdateExCmdXML.h"
#include "../../XML/CmdGenerator/WritePartitionsCmdXML.h"
#include "../../XML/CmdGenerator/GetDLImageFeedbackCmdXML.h"
#include "../../Conn/Connection.h"
#include "../../Cmd/CmdWrapper.h"
#include "FormatWidget.h"
#include "../../Err/CFlashToolErrorCode.h"
#include "../../Cmd/ICmdCallBack.h"
#include "MultiLanguageMapping.h"
#include <cassert>

using namespace std::placeholders;

#define RSC_FILE_NAME "rsc.xml"
#define CHKSUM_FILE_NAME QStringLiteral("scatter_checksum.xml")

static const QString PDownloadBLColorDarkMagenta (" QProgressBar::chunk{background-color: darkMagenta} QProgressBar {text-align: center; border-radius: 0px}");
static const QString PDownloadFlashColorYellow (" QProgressBar::chunk{background-color: yellow} QProgressBar {text-align: center; border-radius: 0px}");

DownloadWidget::DownloadWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(DOWNLOAD_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_DOWNLOAD_PAGE_NAME), parent),
    ui(new Ui::DownloadWidget),
    m_mainwindow(mainwindow),
    m_download_scene(UNKOWN_DOWNLOAD_SCENE)
{
    ui->setupUi(this);
    ui->tableView->setAcceptDrops(true);
    ui->tableView->installEventFilter(this);

    setUIState();
    connectSignalsAndSlots();
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::cbDownloadProgress(void *_this, unsigned int progress, unsigned long long data_xferd,
                                        const QString &info)
{
    QString color = PDownloadFlashColorYellow;
    QString image_name = info;
    QString progress_text = tr("[%1] Download Flash %2%").arg(image_name).arg(progress);
    if (image_name.isEmpty()) {
        progress_text = tr("Download Flash %1%").arg(progress);
    }
    // for preoload information.
    if (image_name.contains("preloader", Qt::CaseInsensitive)) {
        progress_text = tr("Download Boot Loader %1%").arg(progress);
        color = PDownloadBLColorDarkMagenta;
    }
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, color, progress_text, true);
}

void DownloadWidget::reStartDownload()
{
    this->setupDownloadCmdList();
    m_mainwindow->startCmd(STRING_DOWNLOAD_OK);
}

CONN_END_STAGE DownloadWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

CmdWrapperSPList DownloadWidget::createCmdWrapperList()
{
    CmdWrapperSPList cmd_wrapper_list;

    if(m_mainwindow->checkRPMBEnable())
        cmd_wrapper_list.push_back(this->createRPMBCmdWrapper());

    foreach(auto cmd_wrapper, this->createDownloadCmdWrapperList()) {
        cmd_wrapper_list.push_back(cmd_wrapper);
    }

    if (m_mainwindow->isUSBChksumEnabled() && QFileInfo::exists(this->getLoadChksumFile()))
        cmd_wrapper_list.push_back(this->createDLImageFeedbackCmdWrapper());

    bool enable_set_boot_mode = m_mainwindow->advancedModeOn()
         && ui->checkbox_set_boot_mode_to_meta->checkState() == Qt::Checked;
    if(enable_set_boot_mode)
        cmd_wrapper_list.push_back(this->createMetaBootModeCmdWrapper());

    if(isRSCEnabled())
        cmd_wrapper_list.push_back(this->createRSCCmdWrapper());

    cmd_wrapper_list.push_back(m_mainwindow->createWDTCmdWrapper());

    return cmd_wrapper_list;
}

void DownloadWidget::slot_after_rpmb_cmd(int *p_result, const std::shared_ptr<Connection> conn,
                                        const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ICmdCallBack::cb_after_rpmb_cmd(conn, iCmdXMLGenerator);
}

void DownloadWidget::slot_start_addr_action_toggled(bool checked)
{
    ui->tableView->setColumnHidden(DOWNLOAD_COLUMN::dct_Addr, !checked);
}

void DownloadWidget::slot_show_context_menu(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (nullptr != m_context_menu) {
        QString text = m_start_length_act->isChecked() ? GET_TRANSLATE_STRING(IDS_STRING_HIDE_START_LENGTH_COLUMN) : GET_TRANSLATE_STRING(IDS_STRING_SHOW_START_LENGTH_COLUMN);
        m_start_length_act->setText(text);
        m_context_menu->exec(cursor().pos());
    }
}

void DownloadWidget::slot_flash_xml_changed(const DownloadPartitionInfoList &download_partition_list)
{
    this->resetTableWidget(download_partition_list);
}

void DownloadWidget::slot_flash_xml_changed(bool is_nand_operation)
{
    Q_UNUSED(is_nand_operation)
    this->resetDownloadSceneInfo();
    this->resetRSCInfo();
}

void DownloadWidget::slot_after_get_DL_img_feedback_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ICmdCallBack::cb_after_get_dl_img_feedback_cmd(conn, iCmdXMLGenerator, getLoadChksumFile());
}

void DownloadWidget::setUIState()
{
    showRSCItem(false);
    Enable_groupBox_boot_mode_flag(false);
    showBootModeInfo(false);
    initDownloadSceneByConfig();
    initTableWidget();
    ui->comboBox_Scene->setStyleSheet(comboBox_style_sheet);
    ui->comboBox_rsc->setStyleSheet(comboBox_style_sheet);
}

void DownloadWidget::initTableWidget()
{
    DownloadWidgetTableModel *table_mode = new DownloadWidgetTableModel();
    table_mode->setHeaderLabels(getTableHeaderLabels());
    QObject::connect(table_mode, &DownloadWidgetTableModel::signal_all_item_selected,
                this, &DownloadWidget::slot_all_items_selected);
    ui->tableView->setModel(table_mode);

    CheckHeader *header_view = new CheckHeader(Qt::Horizontal, ui->tableView);
    QObject::connect(header_view, SIGNAL(signal_enable_checked(bool)),
                     this, SLOT(slot_enable_checked(bool)));
    ui->tableView->setHorizontalHeader(header_view);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // MUST set column width after model to make column width setting affect.
    ui->tableView->setColumnWidth(DOWNLOAD_COLUMN::dct_ENABLE, 20);
    ui->tableView->setColumnWidth(DOWNLOAD_COLUMN::dct_NAME, 100);
    ui->tableView->setColumnWidth(DOWNLOAD_COLUMN::dct_Addr, 140);

    QObject::connect(ui->tableView, &QTableView::doubleClicked,
                     this, &DownloadWidget::slot_table_doubleClicked);
    QObject::connect(ui->tableView, &QTableView::clicked,
                     this, &DownloadWidget::slot_table_clicked);

    ui->tableView->hideColumn(DOWNLOAD_COLUMN::dct_Addr);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested,
            this, &DownloadWidget::slot_show_context_menu);
    createContextMenu();
}

void DownloadWidget::connectSignalsAndSlots()
{
    QObject::connect(ui->comboBox_Scene, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(slot_comboBox_Scene_currentIndexChanged(int)));
    QObject::connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(const DownloadPartitionInfoList &)),
                     this, SLOT(slot_flash_xml_changed(const DownloadPartitionInfoList &)));
    QObject::connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(bool)),
                     this, SLOT(slot_flash_xml_changed(bool)));
}

QStringList DownloadWidget::getTableHeaderLabels() const
{
    return QStringList() << QStringLiteral("") << tr("Name")
                         << tr("Start Address") << tr("Location");
}

void DownloadWidget::replaceImageFile(const QModelIndex & index)
{
    QString file = index.model()->data(index, Qt::DisplayRole).toString();
    QString new_file = QFileDialog::getOpenFileName(this, tr("Open File"),
                                        file, "All File (*.*)");
    if(!new_file.isEmpty())
    {
        new_file = QDir::toNativeSeparators(new_file);
        const DownloadWidgetTableModel *model = dynamic_cast<const DownloadWidgetTableModel *>(index.model());
        assert(model != nullptr);
        const_cast<DownloadWidgetTableModel *>(model)->setData(index, new_file, Qt::EditRole);
        QModelIndex enable_index = index.model()->index(index.row(), static_cast<int>(DOWNLOAD_COLUMN::dct_ENABLE));
        const_cast<DownloadWidgetTableModel *>(model)->setData(enable_index, Qt::Checked, Qt::CheckStateRole);
    }
}

void DownloadWidget::romEnabledChanged()
{
    if (m_download_scene != DOWNLOAD_ONLY) {
        updateScene();
    }
}

void DownloadWidget::initDownloadSceneByConfig()
{
    if (OptionIniFileMgr::getInstance()->getFmt_all_dl()) {
        ui->comboBox_Scene->addItem(GET_TRANSLATE_STRING(IDS_STRING_FMT_ALL_DL), IDS_STRING_FMT_ALL_DL);
    }
    if (OptionIniFileMgr::getInstance()->getFirmware_upgrade()) {
        ui->comboBox_Scene->addItem(GET_TRANSLATE_STRING(IDS_STRING_FMT_ALL_DL), IDS_STRING_FIRMWARE_UPGRADE);
    }
    if (OptionIniFileMgr::getInstance()->getDl_only()) {
        ui->comboBox_Scene->addItem(GET_TRANSLATE_STRING(IDS_STRING_FMT_ALL_DL), IDS_STRING_DL_ONLY);
    }

    changeDownloadSceneToDefault();

    if (ui->comboBox_Scene->count() > 0) { // unlock downloadwidget page
        ui->comboBox_Scene->setVisible(true);
        this->DoFinished();
    } else { // lock downloadwidget page
        ui->comboBox_Scene->setVisible(false);
        this->LockOnUI();
        ui->pushButton_stop->setEnabled(false);
    }
}

void DownloadWidget::showRSCItem(bool show)
{
    ui->groupBox_rsc->setVisible(show);
}

void DownloadWidget::showBootModeInfo(bool show)
{
    ui->groupBox_com_type->setVisible(show);
    ui->groupBox_com_id->setVisible(show);
}

void DownloadWidget::clear_RSC_Info()
{
    ui->comboBox_rsc->clear();
}

void DownloadWidget::init_RSC_list(const QStringList &rsc_proj_name_list)
{
    ui->comboBox_rsc->clear();
    ui->comboBox_rsc->addItems(rsc_proj_name_list);
    ui->comboBox_rsc->setCurrentIndex(0);
}

void DownloadWidget::changeDownloadSceneToDefault()
{
#define NOT_FOUND -1
    // linear switch to DOWNLOAD_ONLY, FIRMWARE_UPGEADE and FOEMAT_ALL_DOWNLOAD if they exists
    // when do scatter change, item checked change or location change.
    if(ui->comboBox_Scene->findData(DOWNLOAD_ONLY) != NOT_FOUND)
    {
        if (m_download_scene != DOWNLOAD_ONLY) {
            ui->comboBox_Scene->setCurrentIndex(ui->comboBox_Scene->findData(DOWNLOAD_ONLY));
            m_download_scene = DOWNLOAD_ONLY;
        }
    }
    else if(ui->comboBox_Scene->findData(FIRMWARE_UPGRADE) != NOT_FOUND)
    {
        if (m_download_scene != FIRMWARE_UPGRADE) {
            ui->comboBox_Scene->setCurrentIndex(ui->comboBox_Scene->findData(FIRMWARE_UPGRADE));
            m_download_scene = FIRMWARE_UPGRADE;
        }
    }
    else if(ui->comboBox_Scene->findData(FORMAT_ALL_DOWNLOAD) != NOT_FOUND)
    {
        if (m_download_scene != FORMAT_ALL_DOWNLOAD) {
            ui->comboBox_Scene->setCurrentIndex(ui->comboBox_Scene->findData(FORMAT_ALL_DOWNLOAD));
            m_download_scene = FORMAT_ALL_DOWNLOAD;
        }
    }
}

void DownloadWidget::updateScene()
{
    if (ui->comboBox_Scene->count() == 0)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX,
                                     STRING_TOOL_NAME,
                                     tr("Operation failed!\nPlease select at least one download scene or contact tool owner."),
                                     STRING_OK);
        return ;
    }
    changeDownloadSceneToDefault();
}

void DownloadWidget::updateImageList(bool checked_all)
{
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        QModelIndex index = model->index(row, dct_FILE_NAME);
        bool image_exist = !model->data(index, Qt::DisplayRole).toString().isEmpty();
        if((m_download_scene == FORMAT_ALL_DOWNLOAD || m_download_scene == FIRMWARE_UPGRADE) && !image_exist)
        {
            index = model->index(row, dct_NAME);
            QString part_name = model->data(index, Qt::DisplayRole).toString();
            QString err_msg = QStringLiteral("Partition [") + part_name + QStringLiteral("] No image file Exist!");
            Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX,
                                         STRING_TOOL_NAME, err_msg, STRING_OK);
            updateScene();
            return;
        }

        index = model->index(row, dct_ENABLE);
        Qt::CheckState old_state = static_cast<Qt::CheckState>(model->data(index, Qt::CheckStateRole).toInt());
        Qt::CheckState new_state = (checked_all && image_exist) ? Qt::Checked : Qt::Unchecked;
        if (new_state != old_state)
        {
            model->setData(index, new_state, Qt::CheckStateRole);
        }
    }
}

void DownloadWidget::process_rsc(const QString &rsc_file)
{
    if(ToolInfo::IsCustomerVer())
    {
        return ;
    }
    //internal flashtool, check rsc.xml exist or not
    if(QFileInfo(rsc_file).exists())
    {
        LOG("rsc exist");
        showRSCItem(true);
        std::string rsc = rsc_file.toLocal8Bit().constData();
        LOG("rsc file: %s", rsc.c_str());

        RSCParser rsc_parser;
        if (rsc_parser.parseXMLFile(rsc_file)) {
            init_RSC_list(rsc_parser.getRSCProjNames());
        } else {
            Utils::flashtool_message_box(m_mainwindow,
                                  m_mainwindow->getConnectButton(),
                                  CRITICAL_MSGBOX,
                                  STRING_TOOL_NAME,
                                  rsc_parser.get_error_msg(),
                                  STRING_OK);
            clear_RSC_Info();
        }
    } else {
        showRSCItem(false);
    }
}

bool DownloadWidget::validateBeforeDownload()
{
    if(OptionIniFileMgr::getInstance()->getRisk_reminder())
    {
        int ristRet = Utils::flashtool_message_box(this,
                                            nullptr,
                                            QUESTION_MSGBOX,
                                            tr("Warning"),
                                            tr("Flash Phone have risk.\nContinue?"),
                                            tr("Continue"),
                                            tr("&Cancel"));
        if(ristRet == 1) {//cancel
            return false;
        }
    }

    if (!m_mainwindow->isFlashXMLLoaded()) {
        Utils::flashtool_message_box(m_mainwindow, nullptr,
                                     WARNING_MSGBOX, STRING_TOOL_NAME,
                                     STRING_SELECT_FLASH_XML,
                                     STRING_OK);
        return false;
    }

    if (!this->hasImageSelected()) {
        Utils::flashtool_message_box(m_mainwindow,
                                     m_mainwindow->getConnectButton(),
                                     CRITICAL_MSGBOX,
                                     STRING_TOOL_NAME,
                                     tr("Please select one ROM at least before execution."),
                                     STRING_OK);
        return false;
    }

    bool rsc_invalid = ui->comboBox_rsc->isVisible() && (ui->comboBox_rsc->currentText().isEmpty());
    if(rsc_invalid) {
        Utils::flashtool_message_box(m_mainwindow,
                                     m_mainwindow->getConnectButton(),
                                     CRITICAL_MSGBOX,
                                     STRING_TOOL_NAME,
                                     tr("rsc xml file invalid, please check"),
                                     STRING_OK);
        return false;
    }

    if (m_mainwindow->isSkipPTOperate()) { //std::pait<bool, bool>, first is whether has pgpt partition, second is whether pgpt parition selected.
        std::pair<bool, bool> pgpt_pair = this->checkPGPTImage();
        if (!pgpt_pair.first) {
            Utils::flashtool_message_box(m_mainwindow,
                                         nullptr,
                                         WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("Please provide pgpt partition before download when skip_pmt_operate is true in scatter file."),
                                         STRING_OK);
            return false;
        } else if (pgpt_pair.second) {
            Utils::flashtool_message_box(m_mainwindow,
                                         nullptr,
                                         WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("Please pgpt partition MUST be selected before download when skip_pmt_operate is true in scatter file."),
                                         STRING_OK);
            return false;
        }
    }
    return true;
}

bool DownloadWidget::hasImageSelected() const
{
    bool has_image_selected = false;
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, dct_ENABLE);
        Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
        if (check_state == Qt::Checked) {
            has_image_selected = true;
            break;
        }
    }
    return has_image_selected;
}

bool DownloadWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->tableView) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *drag_enter_event = dynamic_cast<QDragEnterEvent *>(event);
            Q_ASSERT(drag_enter_event);
            this->dragEnterEventHandler(drag_enter_event);
            return true;
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *drop_event = dynamic_cast<QDropEvent *>(event);
            Q_ASSERT(drop_event);
            this->dropEventHandler(drop_event);
            return true;
        }
    }
    QObject::eventFilter(watched, event);
    return false;
}

std::pair<bool, bool> DownloadWidget::checkPGPTImage() const
{
    //std::pait<bool, bool>, first is whether has pgpt partition, second is whether pgpt parition selected.
    std::pair<bool, bool> pgpt_pair = std::make_pair<bool, bool>(false, false);
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, dct_NAME);
        QString image_name = index.data().toString().toUpper();
        if (image_name == "PGPT") {
            pgpt_pair.first = true;
            index = model->index(row, dct_ENABLE);
            Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
            if (check_state == Qt::Checked) {
                pgpt_pair.second = true;
            }
            break;
        }
    }
    return pgpt_pair;
}

void DownloadWidget::setupDownloadCmdList()
{
    m_mainwindow->addCmdWrapper(this->createCmdWrapperList());
}

CmdWrapperSPList DownloadWidget::createDownloadCmdWrapperList()
{
    std::list<std::shared_ptr<CmdWrapper>> dlCmdWrapperList;
    switch (m_download_scene)
    {
    case DOWNLOAD_ONLY:
    {
        dlCmdWrapperList = this->createDLOnlyCmdWrapperList();
        break;
    }
    case FORMAT_ALL_DOWNLOAD:
    {
        dlCmdWrapperList = this->createFmtDLCmdWrapperList();
        break;
    }
    case FIRMWARE_UPGRADE:
    {
        dlCmdWrapperList = this->createFWCmdWrapperList();
        break;
    }
    case UNKOWN_DOWNLOAD_SCENE:
        assert(false);
    }

    return dlCmdWrapperList;
}

std::list<std::shared_ptr<CmdWrapper> > DownloadWidget::createDLOnlyCmdWrapperList()
{
    std::list<std::shared_ptr<CmdWrapper>> cmd_wrapper_list;
    cmd_wrapper_list.push_back(this->createDLOnlyCmdWrapper(this->filterNeedDLPartitionList()));
    return cmd_wrapper_list;
}

std::list<std::shared_ptr<CmdWrapper> > DownloadWidget::createFmtDLCmdWrapperList()
{
    std::list<std::shared_ptr<CmdWrapper>> cmd_wrapper_list;
    cmd_wrapper_list.push_back(m_mainwindow->getFormatWidget()->createAutoFormatCmdWrapper());
    cmd_wrapper_list.push_back(this->createFmtDLCmdWrapper());
    return cmd_wrapper_list;
}

std::list<std::shared_ptr<CmdWrapper> > DownloadWidget::createFWCmdWrapperList()
{
    std::list<std::shared_ptr<CmdWrapper>> cmd_wrapper_list;
    cmd_wrapper_list.push_back(this->createFWCmdWrapper());
    return cmd_wrapper_list;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createDLOnlyCmdWrapper(const DownloadPartitionInfoList &dl_partition_list)
{
    std::shared_ptr<WritePartitionsCmdXML> writePartitionsExCmdXML = std::make_shared<WritePartitionsCmdXML>();
    writePartitionsExCmdXML->setFileName(m_mainwindow->getScatterFileName());
    writePartitionsExCmdXML->setDLPartitionList(dl_partition_list);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(writePartitionsExCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&DownloadWidget::cbDownloadProgress,
                                                   this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createFmtDLCmdWrapper()
{
    std::shared_ptr<FlashAllExCmdXML> flashAllExCmdXML = std::make_shared<FlashAllExCmdXML>();
    flashAllExCmdXML->setFileName(m_mainwindow->getScatterFileName());
    flashAllExCmdXML->setDLPartitionList(this->filterNeedDLPartitionList());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(flashAllExCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&DownloadWidget::cbDownloadProgress,
                                                   this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createFWCmdWrapper()
{
    std::shared_ptr<FlashUpdateExCmdXML> flashUpdateExCmdXML = std::make_shared<FlashUpdateExCmdXML>();
    flashUpdateExCmdXML->setBackupFolder(FileUtils::FWBackupFolder());
    flashUpdateExCmdXML->setFileName(m_mainwindow->getScatterFileName());
    flashUpdateExCmdXML->setDLPartitionList(this->filterNeedDLPartitionList());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(flashUpdateExCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&DownloadWidget::cbDownloadProgress,
                                                   this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createRPMBCmdWrapper()
{
    std::shared_ptr<GetRPMBKeyStatusCmdXML> getRPMBStatusCmdXml = std::make_shared<GetRPMBKeyStatusCmdXML>();
    getRPMBStatusCmdXml->setTargetFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(getRPMBStatusCmdXml);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &DownloadWidget::slot_after_rpmb_cmd, Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createMetaBootModeCmdWrapper() const
{
    std::shared_ptr<MetaBootModeCmdXML> metaBootModeCmdXml = std::make_shared<MetaBootModeCmdXML>();
    metaBootModeCmdXml->setCom_type(getBootModeConnType());
    metaBootModeCmdXml->setMobile_log_on(ui->checkbox_mobile_log_on->isChecked());
    metaBootModeCmdXml->setAdb_on(ui->checkbox_adb_on->isChecked());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(metaBootModeCmdXml);
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createRSCCmdWrapper() const
{
    std::shared_ptr<SetRSCCmdXML> setRSCCmdXmlGenerator = std::make_shared<SetRSCCmdXML>();
    setRSCCmdXmlGenerator->setKey(ui->comboBox_rsc->currentText().trimmed());
    QString rsc_dir = (QFileInfo(m_mainwindow->getScatterFileName())).path();
    QString rsc_file_name = rsc_dir + QDir::separator().toLatin1() + RSC_FILE_NAME;
    rsc_file_name = QDir::toNativeSeparators(rsc_file_name);
    setRSCCmdXmlGenerator->setFileName(rsc_file_name);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(setRSCCmdXmlGenerator);
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> DownloadWidget::createDLImageFeedbackCmdWrapper() const
{
    std::shared_ptr<GetDLImageFeedbackCmdXML> dl_feedback_cmd_xml = std::make_shared<GetDLImageFeedbackCmdXML>();
    dl_feedback_cmd_xml->setFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(dl_feedback_cmd_xml);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &DownloadWidget::slot_after_get_DL_img_feedback_cmd, Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

bool DownloadWidget::isRSCEnabled() const
{
    return ui->comboBox_rsc->isVisible() && (!ui->comboBox_rsc->currentText().isEmpty());
}

BOOT_MODE_CONNECT_TYPE DownloadWidget::getBootModeConnType() const
{
    BOOT_MODE_CONNECT_TYPE com_type;
    if (ui->com_type_usb->isChecked()) {
        com_type = BOOT_MODE_CONNECT_TYPE::CONN_TYPE_USB;
    } else if (ui->com_type_uart->isChecked()) {
        com_type = BOOT_MODE_CONNECT_TYPE::CONN_TYPE_UART;
    } else {
        com_type = BOOT_MODE_CONNECT_TYPE::CONN_TYPE_WIFI;
    }
    return com_type;
}

void DownloadWidget::createContextMenu()
{
    m_context_menu = new QMenu(this);
    m_start_length_act = new QAction(GET_TRANSLATE_STRING(IDS_STRING_SHOW_START_LENGTH_COLUMN), m_context_menu);
    m_start_length_act->setCheckable(true);
    connect(m_start_length_act, &QAction::toggled, this, &DownloadWidget::slot_start_addr_action_toggled);
    m_context_menu->addAction(m_start_length_act);
    m_context_menu->setStyleSheet(context_menu_style_sheet);
}

void DownloadWidget::dragEnterEventHandler(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> file_name_urls = event->mimeData()->urls();
        QString file_name = file_name_urls.first().toLocalFile();
        QFileInfo file_info(file_name);
        if (file_info.suffix().toLower() == QStringLiteral("xml")) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void DownloadWidget::dropEventHandler(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls()) {
        event->ignore();
        return ;
    }

    QList<QUrl> file_name_urls = event->mimeData()->urls();
    QString file_name = file_name_urls.first().toLocalFile();

    if(!file_name.isEmpty()) {
        this->m_mainwindow->updateFlashXMLFile(file_name);
    }

    event->accept();
}

QString DownloadWidget::getLoadChksumFile() const
{
    QFileInfo file_info(m_mainwindow->getScatterFileName());
    return QFileInfo(file_info.absoluteDir(), CHKSUM_FILE_NAME).absoluteFilePath();
}

DownloadPartitionInfoList DownloadWidget::filterNeedDLPartitionList() const
{
    DownloadPartitionInfoList need_dl_partition_list;
    QAbstractItemModel *model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, dct_ENABLE);
        Qt::CheckState check_state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
        if (check_state == Qt::Checked) {
            QString part_name = model->index(row, dct_NAME).data(Qt::DisplayRole).toString();
            assert(!part_name.isEmpty());
            QString image_file_name = model->index(row, dct_FILE_NAME).data(Qt::DisplayRole).toString();
            assert(!image_file_name.isEmpty());
            QString start_address = model->index(row, dct_Addr).data(Qt::DisplayRole).toString();
            DownloadPartitionInfo dl_info;
            dl_info.setEnabled(true); // set enable property to export UI completely.
            dl_info.setStartAddress(start_address); // set enable property to export UI completely.
            dl_info.setPartition_name(part_name);
            dl_info.setFile_name(image_file_name);
            need_dl_partition_list.append(dl_info);
        }
    }
    return need_dl_partition_list;
}

void DownloadWidget::Enable_groupBox_boot_mode_flag(bool enable)
{
    ui->groupBox_boot_mode_flag->setVisible(enable);
    ui->groupBox_boot_mode_flag->setEnabled(enable);
}

void DownloadWidget::on_pushButton_download_clicked()
{
    if(validateBeforeDownload())
    {
        m_mainwindow->setupBeforeCmdOperation(getConnStage());
        this->setupDownloadCmdList();
        m_mainwindow->startCmd(STRING_DOWNLOAD_OK);
    }
}

void DownloadWidget::on_pushButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void DownloadWidget::resetTableWidget(const DownloadPartitionInfoList &download_partition_list)
{
    DownloadWidgetTableModel *download_model = dynamic_cast<DownloadWidgetTableModel *>(ui->tableView->model());
    assert(nullptr != download_model);
    download_model->resetData(download_partition_list);
}

void DownloadWidget::resetDownloadSceneInfo()
{
    changeDownloadSceneToDefault();
}

void DownloadWidget::resetRSCInfo()
{
    QString rsc_dir = (QFileInfo(m_mainwindow->getScatterFileName())).path();
    QString rsc_file_path = rsc_dir + QDir::separator().toLatin1() + RSC_FILE_NAME;
    rsc_file_path = QDir::toNativeSeparators(rsc_file_path);
    process_rsc(rsc_file_path);
}

void DownloadWidget::LockOnUI()
{
    ui->pushButton_download->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
    ui->FileLoadFrame->setEnabled(false);
    ui->tableView->setEnabled(false);
}

void DownloadWidget::DoFinished()
{
    ui->pushButton_download->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
    ui->FileLoadFrame->setEnabled(true);
    ui->tableView->setEnabled(true);
}

void DownloadWidget::on_checkbox_set_boot_mode_to_meta_clicked()
{
    bool set_mode_to_meta_clicked = ui->checkbox_set_boot_mode_to_meta->isChecked();
    showBootModeInfo(set_mode_to_meta_clicked);
}

void DownloadWidget::slot_comboBox_Scene_currentIndexChanged(int index)
{
    m_download_scene = static_cast<DOWNLOAD_SCENE>(ui->comboBox_Scene->itemData(index).toInt());
    if (m_download_scene != DOWNLOAD_ONLY) {
        updateImageList(true);
    }
}

void DownloadWidget::slot_enable_checked(bool checked)
{
    updateScene();
    updateImageList(checked);
}

void DownloadWidget::slot_table_doubleClicked(const QModelIndex &index)
{
    assert(index.isValid());
    DOWNLOAD_COLUMN column_type = static_cast<DOWNLOAD_COLUMN>(index.column());
    if (column_type == DOWNLOAD_COLUMN::dct_FILE_NAME)
    {
        replaceImageFile(index);
    }
}

void DownloadWidget::slot_table_clicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    if ((index.flags() & Qt::ItemIsEnabled) == 0) return;
    DOWNLOAD_COLUMN column_type = static_cast<DOWNLOAD_COLUMN>(index.column());
    if (column_type == DOWNLOAD_COLUMN::dct_ENABLE)
    {
        romEnabledChanged();
    }
}

void DownloadWidget::slot_all_items_selected(bool all_selected)
{
    CheckHeader *header_view = dynamic_cast<CheckHeader *>(ui->tableView->horizontalHeader());
    assert(nullptr != header_view);
    header_view->SetChecked(all_selected);
}

void DownloadWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_DOWNLOAD_PAGE_NAME));

    for(int i = 0; i < ui->comboBox_Scene->count(); i++)
    {
        QVariant item_data = ui->comboBox_Scene->itemData(i);
        STRING_TAG scene = static_cast<STRING_TAG>(item_data.toInt());
        ui->comboBox_Scene->setItemText(i, GET_TRANSLATE_STRING(scene));
    }
    DownloadWidgetTableModel *table_mode = dynamic_cast<DownloadWidgetTableModel *>(ui->tableView->model());
    table_mode->setHeaderLabels(getTableHeaderLabels());
}
