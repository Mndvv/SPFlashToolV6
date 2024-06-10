#include "FFUWidget.h"
#include "ui_FFUWidget.h"
#include <QFileDialog>
#include <QDir>
#include "../../Inc/CFlashToolConst.h"
#include "../../Inc/CFlashToolTypes.h"
#include "MainWindow.h"
#include "../../Utility/FileUtils.h"
#include "../../Utility/Utils.h"
#include "../../XML/CmdGenerator/DebugUFSCmdXML.h"
#include "MultiLanguageMapping.h"

FFUWidget::FFUWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(FFU_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_FFU_PAGE_NAME), parent),
    ui(new Ui::FFUWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    QRegExp regExpHex("0x[\\da-fA-F]{16}");
    ui->lineEdit_TWSize->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_TWSize));
    ui->lineEdit_HPBSize->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_HPBSize));
    ui->lineEdit_HPB_Pinned_Region_Start_Index->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_TWSize));
    ui->lineEdit_Pinned_Region_Count->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_HPBSize));
    ui->lineEdit_lu3_size->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_TWSize));
}

FFUWidget::~FFUWidget()
{
    delete ui;
}

CONN_END_STAGE FFUWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

void FFUWidget::on_toolButton_Firmwate_Update_clicked()
{
    if (ui->lineEdit_fwfile->text().isEmpty())
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, "Please select Firmware Update file firstly", STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(this->createFirmwareUpdateCmdWrapper());
    m_mainwindow->startCmd(tr("UFS Firmware Update Ok"));
}

void FFUWidget::on_toolButton_UFS_Config_clicked()
{
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(this->createUFSConfigCmdWrapper());
    m_mainwindow->startCmd(tr("UFS Config Ok"));
}

void FFUWidget::on_toolButton_Stop_clicked()
{
    m_mainwindow->stopCmd();
}

void FFUWidget::on_toolButton_openFwFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open UFS Firmware Upgrade File"),
                FileUtils::GetAppDirectory().c_str(),
                tr("All Files (*.*)"));

    ui->lineEdit_fwfile->setText(QDir::toNativeSeparators(file_name));
}

std::shared_ptr<CmdWrapper> FFUWidget::createFirmwareUpdateCmdWrapper()
{
    std::shared_ptr<UFSUpdateFirmwareCmdXML> updateFirmwareCmdXML = std::make_shared<UFSUpdateFirmwareCmdXML>();
    updateFirmwareCmdXML->setFileType(FILE_TYPE::LOCAL_FILE);
    updateFirmwareCmdXML->setFileName(ui->lineEdit_fwfile->text().trimmed());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(updateFirmwareCmdXML);
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> FFUWidget::createUFSConfigCmdWrapper()
{
    std::shared_ptr<UFSConfigSettingCmdXML> configSettingCmdXML = std::make_shared<UFSConfigSettingCmdXML>();
    configSettingCmdXML->setTWSize(ui->lineEdit_TWSize->text().toUInt(nullptr, 16));
    configSettingCmdXML->setTWReduction(!ui->checkBox_tw_no_reduction->isChecked());
    configSettingCmdXML->setHPBSize(ui->lineEdit_HPBSize->text().toUInt(nullptr, 16));
    configSettingCmdXML->setHPBPinnedIndex(ui->lineEdit_HPB_Pinned_Region_Start_Index->text().toUShort(nullptr, 16));
    configSettingCmdXML->setHPBPinnedCnt(ui->lineEdit_Pinned_Region_Count->text().toUShort(nullptr, 16));
    configSettingCmdXML->setLu3Size(ui->lineEdit_lu3_size->text().toUInt(nullptr, 16));
    configSettingCmdXML->setLu3TypeIndex(static_cast<uint16_t>(ui->comboBox_lu3_type->currentIndex()));
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(configSettingCmdXML);
    return  cmd_wrapper;
}

void FFUWidget::LockOnUI()
{
    ui->toolButton_Firmwate_Update->setEnabled(false);
    ui->toolButton_UFS_Config->setEnabled(false);
    ui->toolButton_Stop->setEnabled(true);
    ui->groupBox_ffu_parameter->setEnabled(false);
    ui->groupBox_ufs_setting_parameter->setEnabled(false);
}

void FFUWidget::DoFinished()
{
    ui->toolButton_Firmwate_Update->setEnabled(true);
    ui->toolButton_UFS_Config->setEnabled(true);
    ui->toolButton_Stop->setEnabled(false);
    ui->groupBox_ffu_parameter->setEnabled(true);
    ui->groupBox_ufs_setting_parameter->setEnabled(true);
}

void FFUWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_FFU_PAGE_NAME));
}
