#include "OptionDialog.h"
#include "ui_OptionDialog.h"

#include "MainWindow.h"
#include "../../Utility/OptionIniFileMgr.h"
#include "../../Utility/LogFilesClean.h"
#include "../../Utility/version.h"
#include "../../Logger/Logger.h"
#include "../../Logger/Log.h"
#include "../../Utility/FileUtils.h"
#include "../../XML/CmdGenerator/SetRuntimeParaCmdXML.h"
#include "../../Utility/LogInfoSetting.h"

#include <QFileDialog>

OptionDialog::OptionDialog(MainWindow *mainwindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ShowCheckRPMBItem(false);

    InitUpdateOptions();

    ui->groupBox_DA_log->setVisible(false);

    ui->treeWidget->expandAll();

    Logger::SetSPFlashToolLogFolder();

    ui->lineEdit_logPath->setText(QString::fromLocal8Bit(Logger::GetSPFlashToolLogFolder().c_str()));

    InitSettings();

    createConnections();

    clearLogFiles();
}

OptionDialog::~OptionDialog()
{    
    SaveSettings();

    delete ui;
}

void OptionDialog::InitSettings()
{
    ui->checkBox_ShowWelcome->setCheckState(OptionIniFileMgr::getInstance()->getShow_welcome() ? Qt::Checked : Qt::Unchecked);

    int da_chksum_level = OptionIniFileMgr::getInstance()->getDa_chksum_level();
    ChkSumLevels chksumlevel = QFlag(da_chksum_level);
    if(chksumlevel.testFlag(Chksum_USB))
        ui->checkBox_usbchksum->setCheckState(Qt::Checked);

    if(chksumlevel.testFlag(Chksum_STORAGE))
        ui->checkBox_emmcchksum->setCheckState(Qt::Checked);

    ui->checkBox_StorageLifeCycle->setCheckState(OptionIniFileMgr::getInstance()->getCheck_storage_life_cycle() ? Qt::Checked : Qt::Unchecked);

#ifdef _WIN32
    if (!ToolInfo::IsCustomerVer())
    {
        ui->checkBox_check_update->setCheckState(OptionIniFileMgr::getInstance()->getUpdate() ? Qt::Checked : Qt::Unchecked);
    }
#endif
    LoadGeneralSettings();
}

void OptionDialog::SaveSettings() const
{
    SaveGeneralSettings();
}

void OptionDialog::LoadGeneralSettings()
{
    bool check = OptionIniFileMgr::getInstance()->getEnable_trace();
    ui->groupBox_enable_tool_log->setChecked(check);

    int cleanLogIndex = OptionIniFileMgr::getInstance()->getClear_older();
    if(cleanLogIndex >= 0 && cleanLogIndex < ui->comboBox__LogDate->count())
        ui->comboBox__LogDate->setCurrentIndex(cleanLogIndex);
    else
        ui->comboBox__LogDate->setCurrentIndex(2); //default: one month

    QString daLogLevel = OptionIniFileMgr::getInstance()->getDa_log_level();
    if (ui->comboBox_da_log_level->findText(daLogLevel) != -1) {
        ui->comboBox_da_log_level->setCurrentText(daLogLevel);
    }

    QString daLogChannel = OptionIniFileMgr::getInstance()->getDa_log_channel();
    if (ui->comboBox_da_log_channel->findText(daLogChannel) != -1) {
        ui->comboBox_da_log_channel->setCurrentText(daLogChannel);
    }
}

void OptionDialog::SaveGeneralSettings() const
{
    OptionIniFileMgr::getInstance()->setEnable_trace(ui->groupBox_enable_tool_log->isChecked());
    OptionIniFileMgr::getInstance()->setClear_older(ui->comboBox__LogDate->currentIndex());
    OptionIniFileMgr::getInstance()->setDa_log_level(ui->comboBox_da_log_level->currentText());
    OptionIniFileMgr::getInstance()->setDa_log_channel(ui->comboBox_da_log_channel->currentText());
}

std::shared_ptr<LogInfoSetting> OptionDialog::createLogInfoSetting()
{
    std::shared_ptr<LogInfoSetting> log_setting(new LogInfoSetting());
    log_setting->setLogPath(ui->lineEdit_logPath->text());
    //convert the negative hour value to positive, for user convenience
    qint64 hours = LogCleaner::GetHoursFromTimeOption(ui->comboBox__LogDate->currentText());
    log_setting->setCleanHours(-hours);
    return log_setting;
}

void OptionDialog::ShowCheckRPMBItem(bool show)
{
    ui->groupBox_rpmb_setting->setVisible(show);
}

void OptionDialog::on_treeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem * /*previous*/)
{
    int index = ui->treeWidget->indexOfTopLevelItem(current);

    if(index < 0)
    {
       QTreeWidgetItem *parent = current->parent();
       index = parent->indexOfChild(current);

       index += 4;
    }

    switch(index)
    {
    case 0:
        ui->stackedWidget_view->setCurrentWidget(ui->page_General);
        break;
    case 1:
        ui->stackedWidget_view->setCurrentWidget(ui->page_Download);
        break;
    case 2:
        ui->stackedWidget_view->setCurrentWidget(ui->page_Pressue);
        break;
    case 3:
        ui->stackedWidget_view->setCurrentWidget(ui->page_Update);
        break;
    case 4:
        ui->stackedWidget_view->setCurrentWidget(ui->page_Language);
        break;
    case 5:
        ui->stackedWidget_view->setCurrentWidget(ui->page_Tracing);
        break;
    default:
        ui->stackedWidget_view->setCurrentWidget(ui->page_General);
        break;
    }
}

void OptionDialog::on_comboBox_Language_activated(int index)
{
    OptionIniFileMgr::getInstance()->setLanguage(index);
    m_mainwindow->ChangeLanguage(index);
}

void OptionDialog::UpdateUI()
{
    ui->retranslateUi(this);
}

void OptionDialog::setLanguageIndex(int index)
{
    ui->comboBox_Language->setCurrentIndex(index);
}

void OptionDialog::on_checkBox_ShowWelcome_clicked()
{
    OptionIniFileMgr::getInstance()->setShow_welcome(ui->checkBox_ShowWelcome->checkState() == Qt::Checked);
}

void OptionDialog::fillRuntimeParameterSetting(std::shared_ptr<SetRuntimeParaCmdXML> &runtime_paramter_setting) const
{
    runtime_paramter_setting->setChecksum_level(this->getChkSumLevel());
    runtime_paramter_setting->setDa_log_level(ui->comboBox_da_log_level->currentText().trimmed());
    runtime_paramter_setting->setDa_log_channel(ui->comboBox_da_log_channel->currentText().trimmed());
}

void OptionDialog::slot_flash_xml_changed(bool is_nand_storage)
{
    Q_UNUSED(is_nand_storage)
    bool is_advanced_mode = m_mainwindow->advancedModeOn();
    bool is_flash_xml_loaded = m_mainwindow->isFlashXMLLoaded();

    ui->groupBox_DA_log->setVisible(is_flash_xml_loaded);

    bool show_rpmb = is_flash_xml_loaded && is_advanced_mode;
    if(!show_rpmb) {
        OptionIniFileMgr::getInstance()->setCheck_rpmb(false);
    }
    ShowCheckRPMBItem(show_rpmb);
}

bool OptionDialog::isAutoPollingEnabled() const
{
    return ui->groupBox_auto_polling_enabled->isChecked();
}

int OptionDialog::getAutoPollingUpperLimit() const
{
    return ui->spinBox_auto_polling_limit->value();
}

void OptionDialog::updateToolLogStatus(bool checked)
{
    ui->groupBox_enable_tool_log->setChecked(checked);
}

bool OptionDialog::isUSBChksumEnabled() const
{
    return ui->checkBox_usbchksum->isChecked();
}

void OptionDialog::on_checkBox_check_update_clicked()
{
    OptionIniFileMgr::getInstance()->setUpdate(ui->checkBox_check_update->checkState() == Qt::Checked);
}

void OptionDialog::clearLogFiles()
{
    LogCleaner::cleanLogFiles(ui->lineEdit_logPath->text(), ui->comboBox__LogDate->currentText());
}

void OptionDialog::createConnections()
{
    connect(ui->checkBox_usbchksum, &QCheckBox::toggled, this, &OptionDialog::slot_checkbox_chksum_toggled);
    connect(ui->checkBox_emmcchksum, &QCheckBox::toggled, this, &OptionDialog::slot_checkbox_chksum_toggled);
    connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(bool)), this, SLOT(slot_flash_xml_changed(bool)));
    connect(ui->groupBox_auto_polling_enabled, &QGroupBox::clicked, this, &OptionDialog::signal_auto_polling_clicked);
}

ChkSumLevels OptionDialog::getChkSumLevel() const
{
    ChkSumLevels chksum_level = Chksum_NONE;
    if(ui->checkBox_usbchksum->checkState() == Qt::Checked) {
        chksum_level &= ~Chksum_NONE;
        chksum_level |= Chksum_USB;
    }
    if(ui->checkBox_emmcchksum->checkState() == Qt::Checked) {
        chksum_level &= ~Chksum_NONE;
        chksum_level |= Chksum_STORAGE;
    }
    return chksum_level;
}

void OptionDialog::InitUpdateOptions()
{
    bool bNeedRemove = true;
#ifdef _WIN32
    if (!ToolInfo::IsCustomerVer())
    {
        bNeedRemove = false;
    }
#endif
    if (bNeedRemove)
    {
        QList<QTreeWidgetItem *> updateItems = ui->treeWidget->findItems("Install/Update", Qt::MatchFixedString | Qt::MatchCaseSensitive);
        Q_ASSERT(updateItems.count() == 1);
        updateItems[0]->setHidden(true);
        for (int i = 0; i < ui->stackedWidget_view->count(); ++i)
        {
            QWidget * cur_widget = ui->stackedWidget_view->widget(i);
            Q_ASSERT(NULL != cur_widget);
            if (cur_widget->objectName() == "page_Update")
            {
                ui->stackedWidget_view->removeWidget(cur_widget);
                break;
            }
        }
    }
}

void OptionDialog::on_groupBox_enable_tool_log_clicked(bool checked)
{
    Q_UNUSED(checked)
    m_mainwindow->logSwitch();
}

void OptionDialog::on_checkBox_rpmb_clicked()
{
    OptionIniFileMgr::getInstance()->setCheck_rpmb(ui->checkBox_rpmb->checkState() == Qt::Checked);
}

void OptionDialog::on_btn_logPath_clicked()
{
    QString sCurrentLogPath = QString::fromLocal8Bit(Logger::GetSPFlashToolLogFolder().c_str());
    sCurrentLogPath = QDir::toNativeSeparators(sCurrentLogPath).trimmed();
    QString sSelectedLogPath = QFileDialog::getExistingDirectory(this, tr("Select Log Path"),
                                                                 sCurrentLogPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    sSelectedLogPath = QDir::toNativeSeparators(sSelectedLogPath).trimmed();
    if (!sSelectedLogPath.trimmed().isEmpty() && sCurrentLogPath != sSelectedLogPath)
    {
        ui->lineEdit_logPath->setText(sSelectedLogPath);
        OptionIniFileMgr::getInstance()->setLog_path(ui->lineEdit_logPath->text());
        // re-start log in backgroud.
        m_mainwindow->EnableLog(false);
        m_mainwindow->EnableLog(true);
    }
}

void OptionDialog::on_checkBox_StorageLifeCycle_clicked()
{
    OptionIniFileMgr::getInstance()->setCheck_storage_life_cycle(ui->checkBox_StorageLifeCycle->checkState() == Qt::Checked);
}

void OptionDialog::slot_checkbox_chksum_toggled()
{
    ChkSumLevels chksum_level = getChkSumLevel();
    OptionIniFileMgr::getInstance()->setDa_chksum_level(static_cast<int>(chksum_level));
}
