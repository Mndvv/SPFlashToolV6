#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <functional>
#include <QString>
#include <QFileDialog>
#include <QShortcut>
#include <QSettings>
#include <QCloseEvent>
#include <QFileInfo>
#include <QProcess>
#include <QTranslator>
#include <QMessageBox>

#include "FormatWidget.h"
#include "DownloadWidget.h"
#include "ReadBackWidget.h"
#include "MemoryTestWidget.h"
#include "WriteMemoryWidget.h"
#include "BromAdapterWidget.h"
#include "FFUWidget.h"
#include "WelcomeWidget.h"
#include "DLCertWidget.h"
#include "BackgroundWorker.h"
#include "OptionDialog.h"
#include "AboutDialog.h"
#include "../../Conn/ConnectionArgs.h"
#include "../../Cmd/MainWindowCallBack.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../XML/CmdGenerator/SetRuntimeParaCmdXML.h"
#include "../../XML/CmdGenerator/RebootCmdXML.h"
#include "../../XML/Parser/FlashXMLParser.h"
#include "../../XML/Parser/ScatterXMLParser.h"
#include "../../XML/CmdGenerator/EMMCControlCmdXML.h"
#include "../../Utility/FileUtils.h"
#include "../../Utility/OptionIniFileMgr.h"
#include "../../Utility/IniItem.h"
#include "../../Utility/version.h"
#include "../../Utility/Utils.h"
#include "../../Logger/Log.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Com/AsyncComPortInfo.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../ConsoleMode/Config.h"
#include "ToolUpdate.h"
#include "../../Err/CFlashToolErrorCode.h"
#include "HWInfoToHTML.h"
#include "MultiLanguageMapping.h"
#include "ProcessingDialog.h"
#include <cassert>
#include "Network/NetworkIntf.h"

#ifdef _WIN32
#include <Windows.h>
#include <WinUser.h>
#include <Dbt.h>
#endif

static const QString PDAColorRed (" QProgressBar::chunk{background-color: red} QProgressBar {text-align: center; border-radius: 0px}");
static const QString PColorMagenta (" QProgressBar::chunk{background-color: magenta} QProgressBar {text-align: center; border-radius: 0px}");

using namespace std::placeholders;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_option_dialog(new OptionDialog(this, this)),
      m_ok_dialog(new OkDialog(this)),
      m_processing_dialog(new ProcessingDialog(this)),
#ifdef _WIN32
      m_tool_update(new ToolUpdateMgr(this)),
#endif
      m_stop_flag(false),
      m_lockui_on(false),
      m_advance_on(false),
      m_Log_on(false),
      m_security_on(false),
      m_bromAdapter_on(false),
      m_show_welcome(true),
      m_dl_cert_on(false),
      m_about_dialog(nullptr),
      m_current_auto_polling_count(0),
      m_storage_type(HW_StorageType_E::HW_STORAGE_NAND),
      m_time_label_timer(new QTimer(this)),
      m_skip_pt_operate(false)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    QSplitterHandle *pHandle = ui->splitter->handle(1);
    pHandle->setDisabled(true);

    initMembers();

    CreateTabWidgets();

    connectSignals();

    setUIInitState();

    initShortcuts();

#if defined(_LINUX64)
    FileUtils::copy_99ttyacms_file(true);
    FileUtils::runModemManagerCmd();
#endif
}

MainWindow::~MainWindow()
{
    IniItem item("history.ini", "RecentOpenFile", "flashxmlHistory");
    item.SaveStringListValue(m_flash_xml_file_history_list);

    item.SetItemName("authHistory");
    // move current selected auth file to first item.
    if (m_auth_file_history_list.count() > 0) {
        QString current_text = ui->comboBox_authFilePath->currentText();
        int index = m_auth_file_history_list.indexOf(current_text);
        Q_ASSERT(index != -1);
        m_auth_file_history_list.move(index, 0);
    }
    item.SaveStringListValue(m_auth_file_history_list);

    item.SetItemName("certHistory");
    item.SaveStringListValue(m_cert_file_history_list);

    item.SetItemName("daHistory");
    item.SaveStringListValue(m_da_file_history_list);

    delete ui;
}

void MainWindow::setupBeforeCmdOperation(CONN_END_STAGE conn_end_strage, bool initial_dram/* = true*/)
{
    m_backgroundworker->setConnArgs(this->createConnSetting(conn_end_strage));
    m_backgroundworker->setRuntimeParameter(this->createRuntimeParamterSetting(initial_dram));
    // MUST add the Check Storage Life Cycle Cmd as the first one if needed. To avoid forget it, so write here.
    if (conn_end_strage == CONN_END_STAGE::SECOND_DA_STAGE
            && OptionIniFileMgr::getInstance()->getCheck_storage_life_cycle()) {
        m_backgroundworker->AddCmdJob(this->createCheckStorageLifeCycleCmdWrapper());
    }
}

void MainWindow::slot_current_operation_ok()
{
    this->ui->progressBar->setValue(0);
    this->ui->progressBar->setFormat(tr(""));

    if(!operation_support_auto_polling()
            || !m_option_dialog->isAutoPollingEnabled()
            || m_current_auto_polling_count >= m_option_dialog->getAutoPollingUpperLimit())
    {
        this->GetOkDialog()->exec();
        this->unLockUI();
        this->ResetStatus();
        ui->label_stress_count->setText(QStringLiteral(""));
        m_current_auto_polling_count = 0;
        emit signal_reset_common_jobs();
    } else {
        if(!this->cb_notify_stop(nullptr))
        {
            LOGD("Auto polling upper limit(%d), polling count(%d).",
                 this->m_option_dialog->getAutoPollingUpperLimit(),
                 this->m_current_auto_polling_count);
            QThread::currentThread()->sleep(1);

            QString tab_page_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex()).trimmed();
            if (tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_DOWNLOAD_PAGE_NAME))
            {
                m_download_widget->reStartDownload();
            }
            else if (tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_READ_BACK_PAGE_NAME))
            {
                m_readback_widget->reStartReadback();
            }
            else if(tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_FORMAT_PAGE_NAME))
            {
                m_format_widget->reStartFormat();
            }
        }
    }
}

void MainWindow::slot_current_operation_finished()
{
    m_time_label_timer->stop();
    if (operation_support_auto_polling() && m_option_dialog->isAutoPollingEnabled()) {
        m_current_auto_polling_count++;
    }
}

void MainWindow::slot_operation_failed(int error_code, QString error_msg)
{
    LOG("current operation failed: error code[%d], error msg[%s].", error_code, qPrintable(error_msg));
    if (needShowErrDlgByErrCode(error_code)) {
        QString msg = QString("error_msg:\n%2").arg(error_msg);
        Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                     STRING_TOOL_NAME, msg, STRING_OK);
    }
    this->unLockUI();
    this->ResetStatus();
    m_current_auto_polling_count = 0;
    emit signal_reset_common_jobs();
}

void MainWindow::LockOnUI()
{
    m_lockui_on = true;
    refreshMenuItemsEnabled();
    refreshkMainUIEnabled();
    for (int index = 0; index < ui->tabWidget->count(); ++index) {
        TabWidgetBase *page = qobject_cast<TabWidgetBase *>(ui->tabWidget->widget(index));
        assert(nullptr != page);
        if (page->IsSelectedTab()) {
            page->LockOnUI();
        } else {
            page->SetTabEnabled(false);
        }
    }
    ui->bromConnSettingWidget->lockUI();
}

void MainWindow::unLockUI()
{
    m_lockui_on = false;
    refreshMenuItemsEnabled();
    refreshkMainUIEnabled();
    for (int index = 0; index < ui->tabWidget->count(); ++index) {
        TabWidgetBase *page = qobject_cast<TabWidgetBase *>(ui->tabWidget->widget(index));
        assert(nullptr != page);
        page->DoFinished();
        page->SetTabEnabled(true);
    }
    ui->bromConnSettingWidget->unLockUI();
}

void MainWindow::ResetStatus()
{
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat(tr(""));
    ui->label_SpeedInfo->setText(tr("0 B/s"));
    ui->label_total->setText(tr("0 Bytes"));
    ui->label_time->setText("0:00");
    ui->label_status_port->setText(ui->bromConnSettingWidget->getDescription());
}

void MainWindow::LoadLastFlashXMLFile()
{
    IniItem item("history.ini", "RecentOpenFile", "flashxmlHistory");

    m_flash_xml_file_history_list = item.GetStringListValue();

    item.SetItemName("lastFlashXMLDir");

    QString last_dir = item.GetStringValue();

    LOG("Init read the flash xml history list size is %d.\n", m_flash_xml_file_history_list.size());

    if(!m_flash_xml_file_history_list.empty())
    {
        int index = 0;

        if(last_dir.length() > 0)
        {
            index = m_flash_xml_file_history_list.indexOf(last_dir);

            if(index < 0)
            {
                m_flash_xml_file_history_list.insert(0, QDir::toNativeSeparators(last_dir));
                index = 0;
            }
        }
        QString path = QDir::toNativeSeparators((m_flash_xml_file_history_list.at(index)));

        QString error_msg;
        if(FileUtils::validFile(path, error_msg)) {
            loadFlashXMLFile(path);
            ui->comboBox_flash_xml_file->clear();

            ui->comboBox_flash_xml_file->insertItems(0, m_flash_xml_file_history_list);

            ui->comboBox_flash_xml_file->setCurrentIndex(index);

            ui->comboBox_flash_xml_file->setEditText(path);
            emit signal_flash_xml_changed(this->isNANDStorage());
        }
        else {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         tr("Open Flash XML File..."),
                                         error_msg, STRING_OK);

            m_flash_xml_file_history_list.removeAt(index);
            ui->comboBox_flash_xml_file->clear();
            ui->comboBox_flash_xml_file->insertItems(0, m_flash_xml_file_history_list);
            ui->comboBox_flash_xml_file->setCurrentIndex(-1);
            ui->comboBox_flash_xml_file->setEditText("");

            item.SetItemName("lastFlashXMLDir");
            item.SaveStringValue("");

            index = ui->comboBox_flash_xml_file->findText(path);
            if(index >= 0)
                ui->comboBox_flash_xml_file->removeItem(index);
        }
    }
}

bool MainWindow::LoadAuthFile(const QString &file_name)
{
    LOG("file_name: %s", file_name.toLocal8Bit().constData());

    QString error_msg;
    int current_index = 1;
    if (file_name.isEmpty()) {
        current_index = 0;
    } else {
        QString auth_file_name = QDir::toNativeSeparators(file_name);
        if(FileUtils::validFile(auth_file_name, error_msg))
        {
            int index = m_auth_file_history_list.indexOf(file_name);

            if(index < 0) {
                m_auth_file_history_list.push_front(auth_file_name);
            } else {
                m_auth_file_history_list.move(index, 0);
            }
        }
        else
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         tr("Open AUTH File..."),
                                         error_msg, STRING_OK);
            m_auth_file_history_list.removeAll(file_name);
            current_index = -1;
        }
    }

    m_auth_file_history_list.removeAll(QStringLiteral(""));
    m_auth_file_history_list.removeDuplicates();

    ui->comboBox_authFilePath->clear();
    if (m_auth_file_history_list.count() > 0) {
        m_auth_file_history_list.push_front(QStringLiteral(""));
        ui->comboBox_authFilePath->addItems(m_auth_file_history_list);
        ui->comboBox_authFilePath->setCurrentIndex(current_index);
    }

    LOG("The authFile history list size is %d.\n", m_auth_file_history_list.size());
    return (current_index == 1);
}

void MainWindow::LoadLastAuthFile()
{
    IniItem item("history.ini", "RecentOpenFile", "authHistory");
    m_auth_file_history_list = item.GetStringListValue();
    if(!m_auth_file_history_list.isEmpty())
    {
        QString lastAuth = m_auth_file_history_list[0];
        LoadAuthFile(lastAuth);
    }
}

void MainWindow::LoadLastCertFile()
{
    IniItem item("history.ini", "RecentOpenFile", "certHistory");
    m_cert_file_history_list = item.GetStringListValue();
    if(!m_cert_file_history_list.isEmpty())
    {
        QString lastCert = m_cert_file_history_list[0];
        LoadCertFile(lastCert);
    }
}

void MainWindow::LoadLastDAFile()
{
    IniItem item("history.ini", "RecentOpenFile", "daHistory");
    m_da_file_history_list = item.GetStringListValue();
    if(!m_da_file_history_list.isEmpty())
    {
        QString lastCert = m_da_file_history_list[0];
        LoadDAFile(lastCert);
    }
}

bool MainWindow::LoadCertFile(const QString &file_name)
{
    LOG("file_name: %s", file_name.toLocal8Bit().constData());

    QString error_msg;
    int current_index = 1;
    if (file_name.isEmpty()) {
        current_index = 0;
    } else {
        QString cert_file_name = QDir::toNativeSeparators(file_name);
        if(FileUtils::validFile(cert_file_name, error_msg))
        {
            int index = m_cert_file_history_list.indexOf(file_name);

            if(index < 0) {
                m_cert_file_history_list.push_front(cert_file_name);
            } else {
                m_cert_file_history_list.move(index, 0);
            }
        }
        else
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         tr("Open CERT File..."),
                                         error_msg, STRING_OK);
            m_cert_file_history_list.removeAll(file_name);
            current_index = -1;
        }
    }

    m_cert_file_history_list.removeAll(QStringLiteral(""));
    m_cert_file_history_list.removeDuplicates();

    ui->comboBox_certFilePath->clear();
    if (m_cert_file_history_list.count() > 0) {
        m_cert_file_history_list.push_front(QStringLiteral(""));
        ui->comboBox_certFilePath->addItems(m_cert_file_history_list);
        ui->comboBox_certFilePath->setCurrentIndex(current_index);
    }

    LOG("The certFile history list size is %d.\n", m_cert_file_history_list.size());
    return (current_index == 1);
}

bool MainWindow::LoadDAFile(const QString &file_name)
{
    LOG("file_name: %s", file_name.toLocal8Bit().constData());

    QString error_msg;
    int current_index = 1;
    if (file_name.isEmpty()) {
        current_index = 0;
    } else {
        QString da_file_name = QDir::toNativeSeparators(file_name);
        if(FileUtils::validFile(da_file_name, error_msg))
        {
            int index = m_da_file_history_list.indexOf(file_name);

            if(index < 0) {
                m_da_file_history_list.push_front(da_file_name);
            } else {
                m_da_file_history_list.move(index, 0);
            }
        }
        else
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         tr("Open DA File..."),
                                         error_msg, STRING_OK);
            m_da_file_history_list.removeAll(file_name);
            current_index = -1;
        }
    }

    m_da_file_history_list.removeAll(QStringLiteral(""));
    m_da_file_history_list.removeDuplicates();

    ui->comboBox_daFilePath->clear();
    if (m_da_file_history_list.count() > 0) {
        m_da_file_history_list.push_front(QStringLiteral(""));
        ui->comboBox_daFilePath->addItems(m_da_file_history_list);
        ui->comboBox_daFilePath->setCurrentIndex(current_index);
    }

    LOG("The daFile history list size is %d.\n", m_da_file_history_list.size());
    return (current_index == 1);
}

std::shared_ptr<ConnectionArgs> MainWindow::createConnSetting(CONN_END_STAGE conn_end_strage) const
{
    std::shared_ptr<ConnectionArgs> connArgs = std::make_shared<ConnectionArgs>();
    connArgs->set_flash_xml_file(ui->comboBox_flash_xml_file->currentText().trimmed());
    connArgs->set_auth_file(ui->comboBox_authFilePath->currentText());
    connArgs->set_cert_file(ui->comboBox_certFilePath->currentText());
    if (ui->comboBox_daFilePath->isVisible() && !ui->comboBox_daFilePath->currentText().isEmpty()) {
        connArgs->set_da_file(ui->comboBox_daFilePath->currentText());
    }
    connArgs->set_com_port_type(ui->bromConnSettingWidget->getComPortType());
    connArgs->set_com_port_name(ui->bromConnSettingWidget->getComPortName());
    connArgs->set_baud_rate(ui->bromConnSettingWidget->getBaudrate());
    connArgs->set_conn_end_stage(conn_end_strage);
    return connArgs;
}

void MainWindow::addCmdWrapper(const std::shared_ptr<CmdWrapper> cmd_wrapper)
{
    assert(nullptr != m_backgroundworker);
    m_backgroundworker->AddCmdJob(cmd_wrapper);
}

void MainWindow::addCmdWrapper(const CmdWrapperSPList cmd_wrapper_list)
{
    assert(nullptr != m_backgroundworker);
    m_backgroundworker->AddCmdJob(cmd_wrapper_list);
}

void MainWindow::addInteractionCmdWrapperList(const CmdWrapperSPList cmd_wrapper_list)
{
    assert(nullptr != m_backgroundworker);
    m_backgroundworker->addInteractionCmdJob(cmd_wrapper_list);
}

void MainWindow::setNeedInteraction(bool need_interaction)
{
    assert(nullptr != m_backgroundworker);
    m_backgroundworker->setNeedInteraction(need_interaction);
}

cmd_wrapper_ptr MainWindow::createWDTCmdWrapper() const
{
    REBOOT_TYPE reboot_type = ui->bromConnSettingWidget->getRebootType();
    std::shared_ptr<RebootCmdXML> reboot_setting = std::make_shared<RebootCmdXML>();
    reboot_setting->setRebootType(reboot_type);
    return std::make_shared<CmdWrapper>(reboot_setting);
}

std::shared_ptr<SetRuntimeParaCmdXML> MainWindow::createRuntimeParamterSetting(bool initial_dram) const
{
    std::shared_ptr<SetRuntimeParaCmdXML> runtime_parameter_setting = std::make_shared<SetRuntimeParaCmdXML>();
    m_option_dialog->fillRuntimeParameterSetting(runtime_parameter_setting);
    ui->bromConnSettingWidget->fillRuntimeParameterSetting(runtime_parameter_setting);
    runtime_parameter_setting->setInitial_dram(initial_dram);
    return runtime_parameter_setting;
}

std::shared_ptr<CmdWrapper> MainWindow::createCheckStorageLifeCycleCmdWrapper() const
{
    std::shared_ptr<LifeCycleStatusCmdXML> checkStorageLifeCycleCmd = std::make_shared<LifeCycleStatusCmdXML>();
    checkStorageLifeCycleCmd->setTargetFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(checkStorageLifeCycleCmd);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    qRegisterMetaType<std::shared_ptr<ICmdXMLGenerator>>("std::shared_ptr<ICmdXMLGenerator>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd,
                     this, &MainWindow::slot_after_check_storage_life_cycle_cmd, Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

void MainWindow::startCmd()
{
    assert(nullptr != m_backgroundworker);
    m_stop_flag = false;
    m_backgroundworker->start();
}

void MainWindow::startCmd(const QString &success_title)
{
    this->startCmd();
    this->LockOnUI();
    this->GetOkDialog()->setWindowTitle(success_title);
}

void MainWindow::stopCmd(bool is_stopped_by_user/* = true*/)
{
    assert(nullptr != m_backgroundworker);
    m_stop_flag = true;
    m_backgroundworker->wait();
    if (is_stopped_by_user) {
        this->ResetStatus();
    }
}

bool MainWindow::isUSBChksumEnabled() const
{
    return m_option_dialog->isUSBChksumEnabled();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_lockui_on)
    {
        const QString info = tr("Warning: The program is running, still continue?");
        const QString title = tr("Exit");
        int promptbutton = Utils::flashtool_message_box(this, nullptr, QUESTION_MSGBOX,
                                                        title, info, STRING_YES, STRING_NO);
        if(promptbutton == 0)
        {
            //wait for real stop;
            this->stopCmd(false);
            saveReadBackSetting();
        }
        else if(promptbutton == 1)
        {
            event->ignore();
            return;
        }
    } else {
        LOGD("UI has unlocked!");
        saveReadBackSetting();
        event->accept();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_pconnectButton && event->type() == QEvent::MouseButtonPress)
    {
        LOGD("grab window");
        Utils::grabWindow();
        return true;
    }

    return false;
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
#ifdef _WIN32
    if (!ui->bromConnSettingWidget->isUARTInfoVisible()) {
        return false;
    }
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = reinterpret_cast<MSG *>(message);
        if (msg->message == WM_DEVICECHANGE)
        {
            switch(msg->wParam)
            {
                case DBT_DEVICEARRIVAL:
                {
                    LOGI("USB Device plug in.");
                    ui->bromConnSettingWidget->refreshCOMPortInfo();
                    *result = true;
                    return true;
                }
                case DBT_DEVICEREMOVECOMPLETE:
                {
                    LOGI("USB Device Removed.");
                    ui->bromConnSettingWidget->refreshCOMPortInfo();
                    *result = true;
                    return true;
                }
            }
        }
    }
#endif
    return false;
}

void MainWindow::showEvent(QShowEvent *event)
{
#ifdef _WIN32
    if (!event->spontaneous()) {
        if(!ToolInfo::IsCustomerVer())
        {
            if(OptionIniFileMgr::getInstance()->getUpdate()) {
                if (m_tool_update) {
                    m_tool_update->startUpdate(false);
                }
            }
        }
    }
#endif
   QMainWindow::showEvent(event);
}

void MainWindow::CreateTabWidgets()
{
    m_download_widget = new DownloadWidget(this, ui->tabWidget);
    m_memorytest_widget = new MemoryTestWidget(this, ui->tabWidget);
    m_format_widget = new FormatWidget(this, ui->tabWidget);
    m_writememory_widget = new WriteMemoryWidget(this, ui->tabWidget);
    m_readback_widget = new ReadBackWidget(this, ui->tabWidget);
    m_bromadapter_widget = new BromAdapterWidget(this, ui->tabWidget);
    m_welcome_widget = new WelcomeWidget(this, ui->tabWidget);
    m_dl_cert_widget = new DLCertWidget(this, ui->tabWidget);
    m_ffu_widget = new FFUWidget(this, ui->tabWidget);

    ui->tabWidget->setUpdatesEnabled(false);
    m_welcome_widget->Attach();
    m_format_widget->Attach();
    m_download_widget->Attach();
    m_readback_widget->Attach();
    m_memorytest_widget->Attach();

    ui->tabWidget->setCurrentWidget(m_download_widget);
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::connectSignals()
{
    connect(ui->menuExport_Console_Mode_XML, SIGNAL(aboutToShow()), this,
            SLOT(slot_setMenuItemStatus()));
    connect(ui->bromConnSettingWidget, &BromConnSettingWidget::signal_update_status_port_info,
            this, &MainWindow::slot_update_status_port_info);
    connect(this, SIGNAL(signal_language_change(int)), this, SLOT(slot_language_changed(int)));
    connect(m_time_label_timer, SIGNAL(timeout()), this, SLOT(slot_UpdateTimeLabel()));
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slot_current_tab_page_changed);
    connect(m_option_dialog, &OptionDialog::signal_auto_polling_clicked, this, &MainWindow::slot_enable_auto_polling_clicked);
    connect(m_processing_dialog, SIGNAL(user_cancel_processing()),this, SLOT(slot_UserCancelLoadFlashXml()));
    connect(NetworkInft::instance().get(), &NetworkInft::signal_network_connection_changed, this, &MainWindow::slot_show_network_connected);
}

void MainWindow::initMembers()
{
    m_mainwindow_callback = std::make_shared<MainWindowCallBack>(this);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_conn_brom_operation_process,
            this, &MainWindow::slot_conn_brom_operation_progress);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_conn_da_operation_process,
            this, &MainWindow::slot_conn_da_operation_progress);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_cb_com_port_status,
            this, &MainWindow::slot_com_port_status);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_cb_da_connected,
            this, &MainWindow::slot_da_connected);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_get_device_tracking,
            this, &MainWindow::slot_get_device_tracking, Qt::BlockingQueuedConnection);
    connect(m_mainwindow_callback.get(), &MainWindowCallBack::signal_scatter_chksum_operation_process,
            this, &MainWindow::slot_scatter_chksum_operation_process);

    m_backgroundworker = std::make_shared<BackgroundWorker>(m_mainwindow_callback);

    qRegisterMetaType<std::string>("std::string");
    connect(m_backgroundworker.get(), &BackgroundWorker::signal_current_operation_finished,
            this, &MainWindow::slot_current_operation_finished);
    connect(m_backgroundworker.get(), &BackgroundWorker::signal_current_operation_ok,
            this, &MainWindow::slot_current_operation_ok);
    connect(m_backgroundworker.get(), &BackgroundWorker::signal_current_operation_failed,
            this, &MainWindow::slot_operation_failed);
    connect(m_backgroundworker.get(), &BackgroundWorker::signal_user_interaction_mode,
            this, &MainWindow::slot_user_interaction_mode);
    connect(this, &MainWindow::signal_reset_common_jobs, m_backgroundworker.get(), &BackgroundWorker::slot_reset_common_jobs);
}

void MainWindow::initShortcuts()
{
    QShortcut *madvance_shortcut = new QShortcut(this);
    madvance_shortcut->setKey(Qt::CTRL+Qt::ALT+Qt::Key_V);
    connect(madvance_shortcut, SIGNAL(activated()), this, SLOT(toggleEnableAdvanceMode()));

    QShortcut *mbromAdapter_shortcut = new QShortcut(this);
    mbromAdapter_shortcut->setKey(Qt::CTRL + Qt::ALT + Qt::Key_A);
    connect(mbromAdapter_shortcut, SIGNAL(activated()), this, SLOT(toggleBromAdapterWidget()));

    QShortcut *mlogger_shortcut = new QShortcut(this);
    mlogger_shortcut->setKey(Qt::CTRL + Qt::ALT + Qt::Key_M);
    connect(mlogger_shortcut,SIGNAL(activated()), this, SLOT(toggleLogging()));

    QShortcut *mdlCert_shortcut = new QShortcut(this);
    mdlCert_shortcut->setKey(Qt::CTRL + Qt::ALT + Qt::Key_E);
    connect(mdlCert_shortcut,SIGNAL(activated()), this, SLOT(toggleDLCertWidget()));
}

bool MainWindow::operation_support_auto_polling() const
{
    QString tab_page_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex()).trimmed();
    if (tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_DOWNLOAD_PAGE_NAME)
            || tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_FORMAT_PAGE_NAME)
            || tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_READ_BACK_PAGE_NAME))
    {
        return true;
    }
    else
    {
        LOGW("current operation does NOT support stress test!");
        return false;
    }
}

void MainWindow::setUIInitState()
{
    ui->progressBar->setTextVisible(true);

    this->setCertUIVisible(false);

    if(ToolInfo::IsCustomerVer())
    {
        ui->actionExportFormat->setVisible(false);
        ui->actionExportReadback->setVisible(false);
        ui->label_owner->setVisible(false);
        ui->label_owner_name->setVisible(false);
    }
    ui->actionExportWriteMemory->setVisible(false);

    ui->label_status_port->setText(ui->bromConnSettingWidget->getDescription());
    ui->actionWrite_Memory->setVisible(false);
    ui->actionWrite_Memory->setChecked(false);
    ui->actionField_Firmware_Update->setVisible(false);
    ui->actionField_Firmware_Update->setChecked(false);
    ui->actionSecurity_Mode->setVisible(true);
    ui->actionSecurity_Mode->setChecked(false);
    ui->actionNetwork_Test->setVisible(false);
    m_writememory_widget->setVisible(false);
    m_bromadapter_widget->setVisible(false);
    m_dl_cert_widget->setVisible(false);
    m_ffu_widget->setVisible(false);
    m_download_widget->Enable_groupBox_boot_mode_flag(false);

    m_show_welcome = OptionIniFileMgr::getInstance()->getShow_welcome();
    if(m_show_welcome)
    {
        ui->actionShow_Welcome->setText(GET_TRANSLATE_STRING(IDS_STRING_HIDE_WELCOME_PAGE));
    }
    else
    {
        ui->tabWidget->setUpdatesEnabled(false);
        m_welcome_widget->Detach();
        //ui->tabWidget->removeTab(0);
        ui->tabWidget->setUpdatesEnabled(true);
        ui->actionShow_Welcome->setText(GET_TRANSLATE_STRING(IDS_STRING_SHOW_WELCOME_PAGE));
    }

    m_Log_on = OptionIniFileMgr::getInstance()->getEnable_trace();
    UpdateWindowTitle(m_Log_on);
    this->EnableLog(m_Log_on);

    this->slot_update_status_port_info();

    ui->label_daFile->setVisible(false);
    ui->comboBox_daFilePath->setVisible(false);
    ui->pushButton_daFile->setVisible(false);

#ifdef _WIN32
    ui->action_SoftwareUpdate->setVisible(!ToolInfo::IsCustomerVer());
#endif
}

void MainWindow::UpdateWindowTitle(bool enableTrace)
{
    QString title = GET_TRANSLATE_STRING(IDS_STRING_TOOL_NAME);

    if(enableTrace) {
        title += GET_TRANSLATE_STRING(IDS_STRING_TRACE_MODE);
    }

    if(m_advance_on) {
        title += GET_TRANSLATE_STRING(IDS_STRING_ADVANCE_MODE);
    }

    if(m_security_on) {
        title += GET_TRANSLATE_STRING(IDS_STRING_SECURITY_MODE);
    }

    this->setWindowTitle(title);
}

void MainWindow::ChangeLanguage(int index)
{
    QTranslator qt_translator;
    QTranslator app_translator;
    bool result;
    switch(index)
    {
    case 0:
        qApp->removeTranslator(&app_translator);
        SET_TRANSLATE_TAG(LANGUAGE_TAG::TT_ENG);
        break;

    case 1:
        result = qt_translator.load(QStringLiteral("qt_zh_CN.qm"), QStringLiteral(":/translations"));
        if(!result)
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("load qt_zh_CN.qm failed."),
                                         STRING_OK);
            return;
        }
        qApp->installTranslator(&qt_translator);
        result = app_translator.load(QStringLiteral("CFlashtool_zh_CN.qm"), QStringLiteral(":/translations"));
        if(!result)
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("load CFlashtool_zh_CN.qm failed."),
                                         STRING_OK);
            return;
        }
        qApp->installTranslator(&app_translator);
        SET_TRANSLATE_TAG(LANGUAGE_TAG::TT_ZH_CH);
        break;

    case 2:
        result = qt_translator.load(QStringLiteral("qt_zh_TW.qm"), QStringLiteral(":/translations"));
        if(!result)
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("load qt_zh_TW.qm failed."),
                                         STRING_OK);
            return;
        }
        qApp->installTranslator(&qt_translator);
        result = app_translator.load(QStringLiteral("CFlashtool_zh_TW.qm"), QStringLiteral(":/translations"));
        if(!result)
        {
            Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX,
                                         STRING_TOOL_NAME,
                                         tr("load CFlashtool_zh_TW.qm failed."),
                                         STRING_OK);
            return;
        }
        qApp->installTranslator(&app_translator);
        SET_TRANSLATE_TAG(LANGUAGE_TAG::TT_ZH_TW);
        break;
    }

    emit signal_language_change(index);
}

HW_StorageType_E MainWindow::getStorageType() const
{
    return m_storage_type;
}

QString MainWindow::getChipName() const
{
    return ui->label_chip_value->text().trimmed();
}

bool MainWindow::isFlashXMLLoaded() const
{
    return !ui->comboBox_flash_xml_file->currentText().isEmpty();
}

long long MainWindow::get_ext_ram_size() const
{
    return m_ext_ram_size;
}

QString MainWindow::getFlashXMLFile() const
{
    return ui->comboBox_flash_xml_file->currentText().trimmed();
}

QAbstractButton **MainWindow::getConnectButton()
{
    return &(this->m_pconnectButton);
}

bool MainWindow::checkRPMBEnable() const
{
    return this->advancedModeOn() && OptionIniFileMgr::getInstance()->getCheck_rpmb();
}

bool MainWindow::advancedModeOn() const
{
    return m_advance_on;
}

FormatWidget *MainWindow::getFormatWidget() const
{
    return m_format_widget;
}

QString MainWindow::getScatterFileName() const
{
    return m_scatter_file_name;
}

bool MainWindow::isSkipPTOperate() const
{
    return m_skip_pt_operate;
}

void MainWindow::logSwitch()
{
    m_Log_on = !m_Log_on;
    this->EnableLog(m_Log_on);
    UpdateWindowTitle(m_Log_on);
}

void MainWindow::refreshTabText(const QWidget * const tab_widget, const QString &tab_text)
{
    if (nullptr == tab_widget) {
        return ;
    }
    int index = ui->tabWidget->indexOf(const_cast<QWidget *>(tab_widget));
    ui->tabWidget->setTabText(index, tab_text);
}

void MainWindow::refreshMenuItemsEnabled()
{
    bool enable = !m_lockui_on;
    ui->actionUSB_UART_options->setEnabled(enable);
    ui->actionWrite_Memory->setEnabled(enable);
    ui->actionField_Firmware_Update->setEnabled(enable);
    ui->menuExport_Console_Mode_XML->setEnabled(enable);
    ui->actionExit->setEnabled(enable);
    ui->actionSecurity_Mode->setEnabled(enable);
}

void MainWindow::refreshkMainUIEnabled()
{
    bool enable = !m_lockui_on;
    ui->comboBox_flash_xml_file->setEnabled(enable);
    ui->comboBox_authFilePath->setEnabled(enable);
    ui->comboBox_certFilePath->setEnabled(enable);
    ui->comboBox_daFilePath->setEnabled(enable);
    ui->pushButton_flash_xml->setEnabled(enable);
    ui->pushButton_authFile->setEnabled(enable);
    ui->pushButton_CertFile->setEnabled(enable);
    ui->pushButton_daFile->setEnabled(enable);
}

void MainWindow::loadFlashXMLFile(const QString &flash_xml_file_name)
{
    processing_dialog()->showCancelButton(true);
    processing_dialog()->show();

    m_stop_flag = false; // used for load flash xml file and execute command.

    AsyncFlashXMLLoader *flash_xml_loader = new AsyncFlashXMLLoader(m_mainwindow_callback, flash_xml_file_name);
    qRegisterMetaType<FlashXMLDataLoader>("FlashXMLDataLoader");
    connect(flash_xml_loader, &AsyncFlashXMLLoader::signal_parse_flash_xml_successed,
            this, &MainWindow::slot_parse_flash_xml_successed);
    connect(flash_xml_loader, &AsyncFlashXMLLoader::signal_parse_flash_xml_failed,
            this, &MainWindow::slot_parse_flash_xml_failed);
    connect(flash_xml_loader, &AsyncFlashXMLLoader::signal_parse_flash_xml_cancelled,
            this, &MainWindow::slot_parse_flash_xml_cancelled);
    connect(flash_xml_loader, &AsyncFlashXMLLoader::signal_start_timer,
            this, &MainWindow::slot_start_timer);
    connect(flash_xml_loader, SIGNAL(finished()), flash_xml_loader, SLOT(deleteLater()));
    flash_xml_loader->start();
}

bool MainWindow::isNANDStorage() const
{
    return m_storage_type == HW_StorageType_E::HW_STORAGE_NAND;
}

void MainWindow::updateSpeedInfo(unsigned long long bytes)
{
    std::string speedInfo;
    std::string totalByte;

    unsigned long timeUseMsec = m_speed_clock.elapsed();

    if(timeUseMsec > 0)
    {
        speedInfo = Utils::PrettyByteString(bytes*1000/timeUseMsec);
        speedInfo += "/s";
    }

    totalByte = Utils::PrettyByteString(bytes);

    ui->label_SpeedInfo->setText(speedInfo.c_str());
    ui->label_total->setText(totalByte.c_str());
}

void MainWindow::setCertUIVisible(bool visible)
{
    ui->label_certFile->setVisible(visible);
    ui->comboBox_certFilePath->setVisible(visible);
    ui->pushButton_CertFile->setVisible(visible);
}

void MainWindow::clearFlashXMLInfo()
{
    ui->label_owner_name->setText("");
    ui->label_chip_value->setText("");
    m_scatter_file_name.clear();
    m_skip_pt_operate = false;
    ui->comboBox_flash_xml_file->setCurrentText("");
    ui->comboBox_flash_xml_file->setCurrentIndex(-1);
    m_storage_type = HW_StorageType_E::HW_STORAGE_NONE;
    ui->label_storage->setText(Utils::getStorageNameByType(m_storage_type).c_str());
    emit signal_flash_xml_changed(DownloadPartitionInfoList());
}

void MainWindow::exportConfigXMLFile(const QString &default_file, const GetCmdWrapperListFunc get_cmd_wrapper_list,
                                     const GetConnStageFunc get_conn_stage_func, bool initial_dram/* = true*/) const
{
    QString file_name = QFileDialog::getSaveFileName(
                const_cast<MainWindow *>(this), tr("Save File"), default_file, "*.xml");
    if(!file_name.isEmpty())
    {
        ConsoleMode::Config config;
        CONN_END_STAGE conn_stage = get_conn_stage_func();
        buildConsoleModeGeneral(&config, conn_stage, initial_dram);
        // MUST add the Check Storage Life Cycle Cmd as the first one if needed.
        if (conn_stage == CONN_END_STAGE::SECOND_DA_STAGE
                && OptionIniFileMgr::getInstance()->getCheck_storage_life_cycle()) {
            config.addCmdWrapper(this->createCheckStorageLifeCycleCmdWrapper());
        }
        config.addCmdWrapper(get_cmd_wrapper_list());
        config.saveFile(QDir::toNativeSeparators(file_name));
    }
}

void MainWindow::buildConsoleModeGeneral(ConsoleMode::Config *p_config,
                                         CONN_END_STAGE conn_stage, bool initial_dram) const
{
    assert(nullptr != p_config);
    p_config->setConnSetting(this->createConnSetting(conn_stage));
    p_config->setRuntimeParameterSetting(this->createRuntimeParamterSetting(initial_dram));
    p_config->setLogInfoSetting(m_option_dialog->createLogInfoSetting());
}

bool MainWindow::needShowErrDlgByErrCode(int error_code) const
{
    QSet<int> no_show_dialog_err_code_set;
    no_show_dialog_err_code_set << E_CANCEL << CFT_CHECK_STORAG_LIFE_CYCLE_WARN_CANCLE << CMD_DEVICE_TRACKING_CANCEL;
    return !(no_show_dialog_err_code_set.contains(error_code));
}

void MainWindow::parseInfoFromDAHwInfo(const QString &da_conn_xml)
{
    QString device_id;
    std::shared_ptr<GetHWInfoXMLParser> sp_hw_xml_parser = std::make_shared<GetHWInfoXMLParser>();
    if (sp_hw_xml_parser->parseXMLString(da_conn_xml)) {
        HWInfoTree::HWInfoTree_ptr hw_info_tree = sp_hw_xml_parser->get_hw_info();
        for (int i = 0; i < hw_info_tree->count(); ++i) {
            HWInfoTree::HWInfoTree_ptr tree_node = hw_info_tree->getChildren(i);
            assert(tree_node);
            if (tree_node->getName() == QStringLiteral("ram_size")) {
                m_ext_ram_size = tree_node->getValue().toLongLong(nullptr, 16);
            } else if (tree_node->getName() == QStringLiteral("storage")) {
                ui->label_storage->setText(tree_node->getValue());
            } else if (tree_node->hasChildren()) {
                for (int sub_index = 0; sub_index < tree_node->count(); ++sub_index) {
                    HWInfoTree::HWInfoTree_ptr sub_tree = tree_node->getChildren(sub_index);
                    assert(sub_tree);
                    if (sub_tree->hasChildren()) {
                        continue;
                    }
                    if (sub_tree->getName() == QStringLiteral("id"))
                        device_id = sub_tree->getValue();
                }
            }
        }
    } else {
        LOG(qPrintable(sp_hw_xml_parser->get_error_msg()));
    }

#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if (!device_id.isEmpty() && !ToolInfo::IsCustomerVer()) {
        NetworkInft::instance()->postData(device_id);
    }
#endif
}

void MainWindow::updateWriteMemoryTab()
{
    ui->tabWidget->setUpdatesEnabled(false);
    if(ui->actionWrite_Memory->isChecked()) {
        m_writememory_widget->Attach();
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(m_writememory_widget), GET_TRANSLATE_STRING(IDS_STRING_WRITE_MEMORY_PAGE_NAME));
        ui->tabWidget->setCurrentWidget(m_writememory_widget);
    } else {
        m_writememory_widget->Detach();
    }
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::updateFFUTab()
{
    ui->tabWidget->setUpdatesEnabled(false);
    if(ui->actionField_Firmware_Update->isChecked()) {
        m_ffu_widget->Attach();
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(m_ffu_widget), GET_TRANSLATE_STRING(IDS_STRING_FFU_PAGE_NAME));
        ui->tabWidget->setCurrentWidget(m_ffu_widget);
    } else {
        m_ffu_widget->Detach();
    }
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::saveReadBackSetting()
{
    if (this->isFlashXMLLoaded())
    {
        m_readback_widget->autoSaveRBSetting(ui->label_chip_value->text(), m_storage_type);
    }
}

void MainWindow::on_pushButton_flash_xml_clicked()
{
    IniItem item("history.ini", "RecentOpenFile", "lastFlashXMLDir");

    QString last_dir = item.GetStringValue();

    QString file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open flash xml File"),
                last_dir,
                tr("xml File (*.xml);;All Files(*.*)"));


    if(!file_name.isEmpty())
    {
        updateFlashXMLFile(file_name);
    }
}

void MainWindow::updateFlashXMLFile(const QString &file_name)
{
    QString flash_xml_file = QDir::toNativeSeparators(file_name);
    loadFlashXMLFile(flash_xml_file);
    IniItem item("history.ini", "RecentOpenFile", "lastFlashXMLDir");
    item.SaveStringValue(flash_xml_file);
}

void MainWindow::EnableLog(bool enable) const
{
    if(enable)
    {
        FileUtils::DebugLogsOn();
    }
    else
    {
        FileUtils::DebugLogsOff();
    }
}

void MainWindow::detectNetworkConnected()
{
    NetworkInft::instance()->tryConnect();
}

void MainWindow::on_pushButton_CertFile_clicked()
{
    IniItem item("history.ini", "RecentOpenFile", "lastCertDir");

    QString last_cert_dir = item.GetStringValue();
    QString cert_file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open CERT File..."),
                last_cert_dir,
                tr("Cert File (*.cert);;All Files(*.*)"));

    if (cert_file_name.isEmpty()) {
        return ;
    }
    if (LoadCertFile(cert_file_name)) {
        item.SaveStringValue(QDir::toNativeSeparators(cert_file_name));
    }
}

void MainWindow::on_pushButton_authFile_clicked()
{
    IniItem item("history.ini", "RecentOpenFile", "lastAuthDir");

    QString last_auth_dir = item.GetStringValue();
    QString auth_file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open AUTH File..."),
                last_auth_dir,
                tr("Authentication File (*.auth);;All Files(*.*)"));

    if (auth_file_name.isEmpty()) {
        return ;
    }
    if (LoadAuthFile(auth_file_name)) {
        item.SaveStringValue(QDir::toNativeSeparators(auth_file_name));
    }
}

void MainWindow::on_pushButton_daFile_clicked()
{
    IniItem item("history.ini", "RecentOpenFile", "lastDADir");

    QString last_da_dir = item.GetStringValue();
    QString da_file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open DA File..."),
                last_da_dir,
                tr("DA File (*.bin);;All Files(*.*)"));

    if (da_file_name.isEmpty()) {
        return ;
    }
    if (LoadDAFile(da_file_name)) {
        item.SaveStringValue(QDir::toNativeSeparators(da_file_name));
    }
}

void MainWindow::on_comboBox_flash_xml_file_activated(const QString &arg1)
{
    QString file_name = QDir::toNativeSeparators(arg1);
    QFile file(file_name);
    if(file_name.isEmpty())
        return;

    updateFlashXMLFile(file_name);

    IniItem item("history.ini", "RecentOpenFile", "lastFlashXMLDir");
    item.SaveStringValue(file_name);
}

void MainWindow::on_comboBox_authFilePath_activated(int index)
{
    if(index >= 0 && index < ui->comboBox_authFilePath->count())
    {
        QString selected_auth_file = QDir::toNativeSeparators(ui->comboBox_authFilePath->itemText(index));
        if (LoadAuthFile(selected_auth_file)) {
            IniItem item("history.ini", "RecentOpenFile", "lastAuthDir");
            item.SaveStringValue(selected_auth_file);
        }
    }
}

void MainWindow::on_comboBox_certFilePath_activated(int index)
{
    if(index >= 0 && index < ui->comboBox_certFilePath->count())
    {
        QString selected_cert_file = QDir::toNativeSeparators(ui->comboBox_certFilePath->itemText(index));
        if (LoadCertFile(selected_cert_file)) {
            IniItem item("history.ini", "RecentOpenFile", "lastCertDir");
            item.SaveStringValue(selected_cert_file);
        }
    }
}

void MainWindow::on_comboBox_daFilePath_activated(int index)
{
    if(index >= 0 && index < ui->comboBox_daFilePath->count())
    {
        QString selected_da_file = QDir::toNativeSeparators(ui->comboBox_daFilePath->itemText(index));
        if (LoadDAFile(selected_da_file)) {
            IniItem item("history.ini", "RecentOpenFile", "lastDADir");
            item.SaveStringValue(selected_da_file);
        }
    }
}

bool MainWindow::cb_notify_stop(void *_this)
{
    Q_UNUSED(_this)
    return this->m_stop_flag;
}

void MainWindow::slot_conn_brom_operation_progress(void *_this, unsigned int progress,
                                                   unsigned long long data_xferd,
                                                   const QString &info)
{
    Q_UNUSED(_this)
    Q_UNUSED(progress)
    Q_UNUSED(data_xferd)
    Q_UNUSED(info)
    LOG("cb_conn_brom_operation_progress callback succeed");
}

void MainWindow::slot_conn_da_operation_progress(void *_this, unsigned int progress,
                                               unsigned long long data_xferd,
                                               const QString &info)
{
    if (!this->cb_notify_stop(this) && progress == 0) { // operation init
        slot_start_timer();
        if (m_option_dialog->isAutoPollingEnabled()) {
            QString stress_count_info = QString("stress count: %1").arg(m_current_auto_polling_count + 1);
            ui->label_stress_count->setText(stress_count_info);
        }
        else {
            ui->label_stress_count->setText(QStringLiteral(""));
        }
    }
    QString progress_text = tr("Download DA %1%").arg(progress);
    operation_progress(_this, progress, data_xferd, info, PDAColorRed, progress_text, true);
}

void MainWindow::slot_scatter_chksum_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const QString &info)
{
    QString progress_text = tr("CheckSum %1 %2%").arg(info).arg(progress);
    operation_progress(_this, progress, data_xferd, info, PColorMagenta, progress_text, true);
}

void MainWindow::slot_com_port_status(const std::string &symbolic_port_name)
{
    QString friendly_port_name = QString::fromStdString(symbolic_port_name);
    QStringList port_names = friendly_port_name.split(QStringLiteral("@"));
    if (port_names.count() > 1) {
        friendly_port_name = port_names[1];
    }
    ui->label_status_port->setText(friendly_port_name);
}

void MainWindow::slot_da_connected(const QString &da_conn_xml)
{
    ui->textEdit_device_info->clear();
    QString da_hw_info = da_conn_xml;
    HWInfoToHTML daHTML(QStringLiteral("DA HW info"));
    QString html_text = daHTML.toHTML(da_hw_info);
    LOG("DA HTML text:\n%s", qPrintable(html_text));
    ui->textEdit_device_info->insertHtml(html_text);
    ui->label_usbStatus->setText(tr("High Speed"));
    m_speed_clock.restart();
    parseInfoFromDAHwInfo(da_hw_info);
}

void MainWindow::slot_get_device_tracking(int *p_out)
{
    assert(nullptr != p_out);

/*
#define MTK_VID QStringLiteral("vid_0e8d")

    *p_out = 1;

    if (ToolInfo::IsCustomerVer() || !NetworkInft::instance()->network_online()) {
        return ;
    }

    QString vid;
    QString port_name = QString::fromStdString(friendly_port_name);
    QStringList port_name_list = port_name.split(QStringLiteral("@"));
    if (port_name_list.count() > 3) {
        vid = port_name_list.at(3).split(QStringLiteral("&")).at(0);
    }
    if (vid.toUpper() == MTK_VID.toUpper()) return;
    LOG("mismatch vendor id: %s", vid.toStdString().c_str());
*/

    *p_out = 1;
    int ret = Utils::flashtool_message_box(nullptr, nullptr, QUESTION_MSGBOX,
        tr("Warning"),
        tr("The download operation will collect your device id information.\nDo you want to continue?"),
        tr("Continue"),
        tr("&Cancel"));
    if (ret != QMessageBox::AcceptRole) {
        *p_out = 0;
    }
}

void MainWindow::slot_setMenuItemStatus()
{
    bool isLoad = this->isFlashXMLLoaded();

    QPair<bool, QString> valid = m_format_widget->validateBeforeFormat();
    ui->actionExportFormat->setEnabled(isLoad && valid.first);
    ui->actionExportDownload->setEnabled(isLoad && m_download_widget->hasImageSelected());
    ui->actionExportReadback->setEnabled(isLoad && m_readback_widget->hasRBItemSelected());
    valid = m_writememory_widget->validBeforeWriteMemeory();
    ui->actionExportWriteMemory->setEnabled(isLoad && valid.first);
}

void MainWindow::on_actionUSB_UART_options_triggered()
{
    m_option_dialog->exec();
}

void MainWindow::on_actionWrite_Memory_triggered()
{
    updateWriteMemoryTab();
}

void MainWindow::on_actionField_Firmware_Update_triggered()
{
    updateFFUTab();
}

void MainWindow::toggleEnableAdvanceMode()
{
    if(m_lockui_on)
        return;

    m_advance_on = !m_advance_on;

    ui->actionWrite_Memory->setVisible(m_advance_on);
    if (!m_advance_on) {
        ui->actionWrite_Memory->setChecked(false);
        updateWriteMemoryTab();
    }

    ui->actionField_Firmware_Update->setVisible(m_advance_on);
    if (!m_advance_on) {
        ui->actionField_Firmware_Update->setChecked(false);
        updateFFUTab();
    }

    ui->actionExportWriteMemory->setVisible(m_advance_on);
#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if (!ToolInfo::IsCustomerVer()) {
        ui->actionNetwork_Test->setVisible(m_advance_on);
    }
#endif
    m_memorytest_widget->ShowDRAMRepairInfo(m_advance_on);
    m_download_widget->Enable_groupBox_boot_mode_flag(m_advance_on && this->isFlashXMLLoaded());
    m_option_dialog->ShowCheckRPMBItem(m_advance_on && isFlashXMLLoaded());
    ui->bromConnSettingWidget->updateAdvancedMode(m_advance_on);
    m_format_widget->refreshRegion(m_advance_on, isNANDStorage());

    UpdateWindowTitle(OptionIniFileMgr::getInstance()->getEnable_trace());

    ui->label_daFile->setVisible(m_advance_on);
    ui->comboBox_daFilePath->setVisible(m_advance_on);
    ui->pushButton_daFile->setVisible(m_advance_on);
}

void MainWindow::toggleBromAdapterWidget()
{
    if(m_lockui_on)
        return;

    m_bromAdapter_on = !m_bromAdapter_on;

    ui->tabWidget->setUpdatesEnabled(false);
    if(m_bromAdapter_on) {
        m_bromadapter_widget->Attach();
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(m_bromadapter_widget), GET_TRANSLATE_STRING(IDS_STRING_BROM_ADAPTER_PAGE_NAME));
        ui->tabWidget->setCurrentWidget(m_bromadapter_widget);
    } else {
        m_bromadapter_widget->Detach();
    }
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::toggleLogging()
{
    if(m_lockui_on)
        return;

    this->logSwitch();
    m_option_dialog->updateToolLogStatus(m_Log_on);
}

void MainWindow::toggleDLCertWidget()
{
    if(m_lockui_on)
        return;

    m_dl_cert_on = !m_dl_cert_on;

    ui->tabWidget->setUpdatesEnabled(false);
    if(m_dl_cert_on) {
        m_dl_cert_widget->Attach();
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(m_dl_cert_widget), GET_TRANSLATE_STRING(IDS_STRING_DL_CERT_PAGE_NAME));
        ui->tabWidget->setCurrentWidget(m_dl_cert_widget);
    } else {
        m_dl_cert_widget->Detach();
    }
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::on_actionSecurity_Mode_triggered()
{
    if(m_lockui_on)
        return;

    m_security_on = ui->actionSecurity_Mode->isChecked();
    ui->tabWidget->setCurrentWidget(m_download_widget);
    this->setCertUIVisible(m_security_on);

    UpdateWindowTitle(m_Log_on);
}

void MainWindow::on_actionAbout_triggered()
{
    if(m_about_dialog == nullptr)
    {
        m_about_dialog = new AboutDialog(this);
    }
    m_about_dialog->UpdateUI();
    m_about_dialog->show();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionExportFormat_triggered()
{
    exportConfigXMLFile("format.xml", std::bind(&FormatWidget::createCmdWrapperList, m_format_widget),
                        std::bind(&FormatWidget::getConnStage, m_format_widget));
}

void MainWindow::on_actionExportDownload_triggered()
{
    exportConfigXMLFile("download.xml", std::bind(&DownloadWidget::createCmdWrapperList, m_download_widget),
                        std::bind(&DownloadWidget::getConnStage, m_download_widget));
}

void MainWindow::on_actionExportReadback_triggered()
{
    exportConfigXMLFile("readback.xml", std::bind(&ReadBackWidget::createRBCmdWrapperList, m_readback_widget),
                        std::bind(&ReadBackWidget::getConnStage, m_readback_widget));
}

void MainWindow::on_actionExportWriteMemory_triggered()
{
    exportConfigXMLFile("write_memory.xml", std::bind(&WriteMemoryWidget::createCmdWrapperList, m_writememory_widget),
                        std::bind(&WriteMemoryWidget::getConnStage, m_writememory_widget));
}

void MainWindow::on_actionOpen_Logs_Folder_triggered()
{
    Logger::SetSPFlashToolLogFolder();
    QString log_path = QString::fromLocal8Bit(Logger::GetSPFlashToolLogFolder().c_str());
#ifdef _WIN32
    QProcess::startDetached("explorer "+ log_path);
#else
    QProcess::startDetached("nautilus "+ log_path);
#endif
}

void MainWindow::on_actionShow_Welcome_triggered()
{
    if(m_lockui_on)
        return;

    m_show_welcome = !m_show_welcome;

    ui->tabWidget->setUpdatesEnabled(false);
    if(m_show_welcome) {
        m_welcome_widget->Attach();
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(m_welcome_widget), GET_TRANSLATE_STRING(IDS_STRING_WELCOME_PAGE_NAME));
        ui->tabWidget->setCurrentWidget(m_welcome_widget);
        ui->actionShow_Welcome->setText(GET_TRANSLATE_STRING(IDS_STRING_HIDE_WELCOME_PAGE));
    } else {
        m_welcome_widget->Detach();
        ui->actionShow_Welcome->setText(GET_TRANSLATE_STRING(IDS_STRING_SHOW_WELCOME_PAGE));
    }
    ui->tabWidget->setUpdatesEnabled(true);
}

void MainWindow::on_action_SoftwareUpdate_triggered()
{
#ifdef Q_OS_WIN
    if (m_tool_update) {
        m_tool_update->startUpdate(true);
    }
#endif
}

void MainWindow::on_actionNetwork_Test_triggered()
{
    NetworkInft::instance()->tryConnectShow();
}

void MainWindow::slot_update_status_port_info()
{
    ui->label_status_port->setText(ui->bromConnSettingWidget->getDescription());
}

void MainWindow::slot_language_changed(int index)
{
    QString chip = ui->label_chip_value->text();
    QString owner = ui->label_owner_name->text();
    ui->retranslateUi(this);
    ui->label_chip_value->setText(chip);
    ui->label_owner_name->setText(owner);

    UpdateWindowTitle(OptionIniFileMgr::getInstance()->getEnable_trace());

    ui->actionShow_Welcome->setText(m_show_welcome ? GET_TRANSLATE_STRING(IDS_STRING_HIDE_WELCOME_PAGE) : GET_TRANSLATE_STRING(IDS_STRING_SHOW_WELCOME_PAGE));

    for (int index = 0; index < ui->tabWidget->count(); ++index) {
        TabWidgetBase *page = qobject_cast<TabWidgetBase *>(ui->tabWidget->widget(index));
        assert(nullptr != page);
        page->UpdateUI();
    }

    m_writememory_widget->UpdateUI();
    m_bromadapter_widget->UpdateUI();
    m_dl_cert_widget->UpdateUI();
    m_ffu_widget->UpdateUI();
    ui->bromConnSettingWidget->UpdateUI();

    m_option_dialog->UpdateUI();
    m_option_dialog->setLanguageIndex(index);

    if (nullptr != m_about_dialog) {
        m_about_dialog->UpdateUI();
    }

#ifdef _WIN32
    m_tool_update->updateUI();
#endif
}

void MainWindow::slot_UpdateTimeLabel()
{
    int seconds = m_start_clock.elapsed()/1000;
    QString zero;
    zero = (10 > (seconds % 60)) ? "0" : "";

    ui->label_time->setText(QString::number(seconds / 60) + ":" + zero + QString::number(seconds % 60));
}

void MainWindow::slot_after_check_storage_life_cycle_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ICmdCallBack::cb_after_check_storage_life_cycle_cmd(conn, iCmdXMLGenerator);
}

void MainWindow::slot_current_tab_page_changed(int index)
{
    QString tab_page_name = ui->tabWidget->tabText(index).trimmed();
    if (tab_page_name == GET_TRANSLATE_STRING(IDS_STRING_READ_BACK_PAGE_NAME))
    {
        if (isFlashXMLLoaded()) {
            m_readback_widget->autoLoadRBSetting();
        }
        disconnect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slot_current_tab_page_changed);
    }
}

void MainWindow::slot_user_interaction_mode()
{
    this->ui->progressBar->setValue(0);
    this->ui->progressBar->setFormat(tr(""));
}

void MainWindow::slot_enable_auto_polling_clicked(bool checked)
{
    if (!checked) {
        this->ui->label_stress_count->setText(QStringLiteral(""));
    }
}

void MainWindow::slot_parse_flash_xml_successed(FlashXMLDataLoader flash_xml_data_loader)
{
    processing_dialog()->hide();
    m_time_label_timer->stop();

    // flash xml file info
    if(!ToolInfo::IsCustomerVer()) //not show owner name in customer version tool
    {
        ui->label_owner_name->setText(flash_xml_data_loader.contact_name());
    }

    m_scatter_file_name = flash_xml_data_loader.getScatter_file_name();

    // scatter file info
    QString chip_name = flash_xml_data_loader.getChip_name();
    ui->label_chip_value->setText(chip_name + QStringLiteral(" "));
    m_storage_type = flash_xml_data_loader.getStorage_type();
    ui->label_storage->setText(Utils::getStorageNameByType(m_storage_type).c_str());
    m_skip_pt_operate = flash_xml_data_loader.getSkip_pt_operate();

    emit signal_flash_xml_changed(flash_xml_data_loader.getDl_partition_list());

    QString flash_xml_file = flash_xml_data_loader.getFlash_xml_file();
    int index = ui->comboBox_flash_xml_file->findText(flash_xml_file);
    if(index < 0) {
        m_flash_xml_file_history_list.insert(0, flash_xml_file);
        LOG("The flash xml history list size is %d.\n", m_flash_xml_file_history_list.size());

        ui->comboBox_flash_xml_file->insertItem(0, flash_xml_file);
        ui->comboBox_flash_xml_file->setCurrentIndex(0);
    } else {
        ui->comboBox_flash_xml_file->setCurrentIndex(index);
    }
    emit signal_flash_xml_changed(this->isNANDStorage());

    ResetStatus();
}

void MainWindow::slot_parse_flash_xml_failed(const QString &error_msg, const QString &flash_xml_file)
{
    Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, error_msg, STRING_OK);
    m_flash_xml_file_history_list.removeAll(flash_xml_file);
    ui->comboBox_flash_xml_file->clear();
    ui->comboBox_flash_xml_file->insertItems(0, m_flash_xml_file_history_list);
    slot_parse_flash_xml_cancelled();
}

void MainWindow::slot_parse_flash_xml_cancelled()
{
    processing_dialog()->hide();
    m_time_label_timer->stop();
    ResetStatus();
    clearFlashXMLInfo();
}

void MainWindow::slot_UserCancelLoadFlashXml()
{
    m_stop_flag = true;
}

void MainWindow::slot_start_timer()
{
    m_start_clock.restart();
    m_time_label_timer->start(1000);
}

void MainWindow::slot_show_network_connected()
{
    if (NetworkInft::instance()->network_online()) {
        Utils::flashtool_message_box(this, nullptr, INFORMATION_MSGBOX, STRING_TOOL_NAME, "Connection successed", STRING_OK);
    } else {
        Utils::flashtool_message_box(this, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, "Connection failed", STRING_OK);
    }
}

void MainWindow::operation_progress(void *_this, unsigned int progress,
                                    unsigned long long data_xferd, const QString &info,
                                    const QString &backColor, const QString &progress_text,
                                    bool update_speed_info)
{
    Q_UNUSED(info)
    if(!this->cb_notify_stop(_this))
    {
        ui->progressBar->setStyleSheet(backColor);
        ui->progressBar->setValue((int)progress);
        ui->progressBar->setFormat(progress_text);
        if (update_speed_info) {
            if (progress == 0) { // progress init
                m_speed_clock.restart();
            }
            this->updateSpeedInfo(data_xferd);
        }
    }
}

