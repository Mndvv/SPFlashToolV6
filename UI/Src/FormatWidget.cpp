#include "FormatWidget.h"
#include "ui_FormatWidget.h"
#include <memory>

#include "MainWindow.h"
#include "../../XML/CmdGenerator/EraseFlashCmdXML.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../Cmd/CmdWrapper.h"
#include "MultiLanguageMapping.h"
#include "../../Utility/Utils.h"

static const QString PFormatColorGreen (" QProgressBar::chunk{background-color: green} QProgressBar {text-align: center; border-radius: 0px}");

using namespace std::placeholders;

FormatWidget::FormatWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(FORMAT_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_FORMAT_PAGE_NAME), parent),
    ui(new Ui::FormatWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    setUIState();

    connect(m_mainwindow, SIGNAL(signal_flash_xml_changed(bool)),
            this, SLOT(slot_flash_xml_changed(bool)));
}

FormatWidget::~FormatWidget()
{
    delete ui;
}

void FormatWidget::LockOnUI()
{
    ui->toolButton_Start->setEnabled(false);
    ui->toolButton_stop->setEnabled(true);
    ui->frame_2->setEnabled(false);
    ui->frame_ManualFormatSettings->setEnabled(false);
}

void FormatWidget::DoFinished()
{
    ui->toolButton_Start->setEnabled(true);
    ui->toolButton_stop->setEnabled(false);
    ui->frame_2->setEnabled(true);
    ui->frame_ManualFormatSettings->setEnabled(true);
}

void FormatWidget::cbFormatProgress(void *_this, unsigned int progress,
                                    unsigned long long data_xferd,
                                    const QString &info)
{
    QString progress_text = tr("Format %1%").arg(progress);
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, PFormatColorGreen, progress_text, false);
}

void FormatWidget::setUIState()
{
    m_ufs_emmc_regions = Utils::getEMMCUFSRegionList();
    m_nand_dummy_region = Utils::getNANDRegionList();
    m_nand_dummy_region_advanced = Utils::getNANDBMTRegionList(); // NAND-BMT only for NAND format operation.
    ui->comboBox_region->addItems(m_ufs_emmc_regions);
    ui->comboBox_region->setStyleSheet(comboBox_style_sheet);

    QRegExp regExpHex("0x[\\da-fA-F]{1,16}");
    ui->lineEdit_BeginAddress->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_BeginAddress));
    ui->lineEdit_FormatLength->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_FormatLength));

    ui->frame_ManualFormatSettings->setHidden(true);
    ui->radioButton_AutoFormat->setChecked(true);
}

void FormatWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_FORMAT_PAGE_NAME));
}

void FormatWidget::slot_flash_xml_changed(bool is_nand_storage)
{
    ui->comboBox_region->clear();
    if(is_nand_storage) {
        ui->comboBox_region->addItems(m_mainwindow->advancedModeOn() ? m_nand_dummy_region_advanced : m_nand_dummy_region);
    }
    else {
        ui->comboBox_region->addItems(m_ufs_emmc_regions);
        ui->comboBox_region->setCurrentIndex(UFS_LUA2_INDEX);
    }
}

void FormatWidget::on_toolButton_Start_clicked()
{
    QPair<bool, QString> valid_result = this->validateBeforeFormat();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    this->setupFormatCmdList();
    m_mainwindow->startCmd(tr("Format Ok"));
}

void FormatWidget::on_toolButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void FormatWidget::on_radioButton_AutoFormat_clicked()
{
    ui->frame_ManualFormatSettings->setVisible(false);
}

void FormatWidget::on_radioButton_ManualFormat_clicked()
{
    ui->frame_ManualFormatSettings->setVisible(true);
}

QPair<bool, QString> FormatWidget::validateBeforeFormat() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    if (ui->radioButton_ManualFormat->isChecked()) {
        QRegExp regexp(valid_hex_address_expr);
        if (regexp.indexIn(ui->lineEdit_BeginAddress->text()) == -1) {
            result.first = false;
            result.second = QStringLiteral("Please fill the \"Begin Address\" field.");
        } else if (regexp.indexIn(ui->lineEdit_FormatLength->text()) == -1) {
            result.first = false;
            result.second = QStringLiteral("Please fill the \"Format Length\" field.");
        }
    }
    return result;
}

std::shared_ptr<CmdWrapper> FormatWidget::createCmdWrapper()
{
    if (ui->radioButton_AutoFormat->isChecked()) {
        return this->createAutoFormatCmdWrapper();
    }
    else {
        return this->createManualFormatCmdWrapper();
    }
}

std::shared_ptr<CmdWrapper> FormatWidget::createAutoFormatCmdWrapper()
{
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<EraseFlashCmdXML>());
    cmd_wrapper->setOperationProgress(std::bind(&FormatWidget::cbFormatProgress,
                                                this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

void FormatWidget::reStartFormat()
{
    this->setupFormatCmdList();
    m_mainwindow->startCmd();
}

CONN_END_STAGE FormatWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

CmdWrapperSPList FormatWidget::createCmdWrapperList()
{
    CmdWrapperSPList cmd_wrapper_list;
    cmd_wrapper_list.push_back(this->createCmdWrapper());
    cmd_wrapper_list.push_back(m_mainwindow->createWDTCmdWrapper());
    return cmd_wrapper_list;
}

void FormatWidget::refreshRegion(bool is_advanced_mode, bool is_nand_storage)
{
    if(is_nand_storage) {
        ui->comboBox_region->clear();
        ui->comboBox_region->addItems(is_advanced_mode ? m_nand_dummy_region_advanced : m_nand_dummy_region);
    }
}

std::shared_ptr<CmdWrapper> FormatWidget::createManualFormatCmdWrapper()
{
    std::shared_ptr<EraseFlashCmdXML> manualFmtCmdXml = std::make_shared<EraseFlashCmdXML>();
    manualFmtCmdXml->setPartition(ui->comboBox_region->currentText());
    manualFmtCmdXml->setOffSet(ui->lineEdit_BeginAddress->text());
    manualFmtCmdXml->setLength(ui->lineEdit_FormatLength->text());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(manualFmtCmdXml);
    cmd_wrapper->setOperationProgress(std::bind(&FormatWidget::cbFormatProgress,
                                                this, _1, _2, _3, _4));
    return cmd_wrapper;
}

void FormatWidget::setupFormatCmdList()
{
    m_mainwindow->addCmdWrapper(this->createCmdWrapperList());
}
