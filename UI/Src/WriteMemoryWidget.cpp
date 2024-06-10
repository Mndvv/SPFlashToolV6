#include "WriteMemoryWidget.h"
#include "ui_WriteMemoryWidget.h"
#include "../../Inc/CFlashToolConst.h"
#include "MainWindow.h"
#include <QFileDialog>
#include "../../Utility/FileUtils.h"
#include "../../Utility/Utils.h"
#include "../../XML/CmdGenerator/WriteFlashCmdXML.h"
#include "../../Cmd/CmdWrapper.h"
#include "MultiLanguageMapping.h"

static const QString PWriteMemoryColorDarkCyan (" QProgressBar::chunk{background-color: darkCyan} QProgressBar {text-align: center; border-radius: 0px}");

using namespace std::placeholders;

WriteMemoryWidget::WriteMemoryWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(WRITEMEMORY_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_WRITE_MEMORY_PAGE_NAME), parent),
    ui(new Ui::WriteMemoryWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    QRegExp regExpHex("0x[\\da-fA-F]{1,16}");
    ui->lineEdit_address->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_address));

    m_ufs_emmc_regions = Utils::getEMMCUFSRegionList();
    m_nand_dummy_region = Utils::getNANDRegionList();
    ui->comboBox_region->addItems(m_ufs_emmc_regions);
    ui->comboBox_region->setStyleSheet(comboBox_style_sheet);

    connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(bool)), this, SLOT(slot_flash_xml_changed(bool)));
}

WriteMemoryWidget::~WriteMemoryWidget()
{
    delete ui;
}

void WriteMemoryWidget::cbWriteMemoryProgress(void *_this, unsigned int progress,
                                              unsigned long long data_xferd,
                                              const QString &info)
{
    QString progress_text = tr("Write Memory %1%").arg(progress);
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, PWriteMemoryColorDarkCyan, progress_text, true);
}

void WriteMemoryWidget::slot_flash_xml_changed(bool is_nand_storage)
{
    ui->comboBox_region->clear();
    if(is_nand_storage) {
        ui->comboBox_region->addItems(m_nand_dummy_region);
    }
    else {
        ui->comboBox_region->addItems(m_ufs_emmc_regions);
        ui->comboBox_region->setCurrentIndex(UFS_LUA2_INDEX);
    }
}

CONN_END_STAGE WriteMemoryWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

CmdWrapperSPList WriteMemoryWidget::createCmdWrapperList()
{
    CmdWrapperSPList cmdWrapperList;
    cmdWrapperList.push_back(this->createCmdWrapper());
    return cmdWrapperList;
}

std::shared_ptr<CmdWrapper> WriteMemoryWidget::createCmdWrapper()
{
    std::shared_ptr<WriteFlashCmdXML> writeFlashCmdXML = std::make_shared<WriteFlashCmdXML>();
    writeFlashCmdXML->setPartition(ui->comboBox_region->currentText().trimmed());
    writeFlashCmdXML->setOffSet(ui->lineEdit_address->text().trimmed());
    writeFlashCmdXML->setFileName(ui->lineEdit_FilePath->text().trimmed());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(writeFlashCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&WriteMemoryWidget::cbWriteMemoryProgress,
                                                this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

QPair<bool, QString> WriteMemoryWidget::validBeforeWriteMemeory() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    QRegExp regexp(valid_hex_address_expr);
    if (ui->lineEdit_FilePath->text().isEmpty()) {
        result.first = false;
        result.second = QStringLiteral("Please select a file before operation.");
    } else if (regexp.indexIn(ui->lineEdit_address->text()) == -1) {
        result.first = false;
        result.second = QStringLiteral("Please fill the \"Begin Address\" field.");
    }
    return result;
}

void WriteMemoryWidget::on_toolButton_writeMemory_clicked()
{
    QPair<bool, QString> valid_result = this->validBeforeWriteMemeory();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(this->createCmdWrapper());
    m_mainwindow->addCmdWrapper(m_mainwindow->createWDTCmdWrapper());
    m_mainwindow->startCmd(tr("Write Memory Ok"));
}

void WriteMemoryWidget::on_toolButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void WriteMemoryWidget::on_toolButton_openFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open Raw Data File"),
                FileUtils::GetAppDirectory().c_str(),
                tr("All Files (*)"));

    if(!file_name.isEmpty()) {
        QFileInfo fileInfo(file_name);
        ui->lineEdit_FilePath->setText(file_name);
    }
    else {
        ui->lineEdit_FilePath->setText("");
    }
}

void WriteMemoryWidget::LockOnUI()
{
    ui->toolButton_writeMemory->setEnabled(false);
    ui->toolButton_stop->setEnabled(true);
    ui->groupBox_write_parameter->setEnabled(false);
}

void WriteMemoryWidget::DoFinished()
{
    ui->toolButton_writeMemory->setEnabled(true);
    ui->toolButton_stop->setEnabled(false);
    ui->groupBox_write_parameter->setEnabled(true);
}

void WriteMemoryWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_WRITE_MEMORY_PAGE_NAME));
}
