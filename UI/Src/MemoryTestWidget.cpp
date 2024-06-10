#include "MemoryTestWidget.h"
#include "ui_MemoryTestWidget.h"
#include "../../Inc/CFlashToolConst.h"
#include "../../XML/CmdGenerator/RamTestCmdXML.h"
#include "../../XML/CmdGenerator/DebugDRAMRepairCmdXML.h"
#include "../../XML/CmdGenerator/RamTestCmdXML.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Logger/Log.h"
#include "MainWindow.h"
#include "../../Utility/Utils.h"
#include <QFileDialog>
#include "../../Err/CFlashToolErrorCode.h"
#include "../../Cmd/ICmdCallBack.h"
#include "MultiLanguageMapping.h"

using namespace std::placeholders;

int MemoryTestWidget::m_current_tested_count = 0;

MemoryTestWidget::MemoryTestWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(MEMORYTEST_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_MEMORY_TEST_PAGE_NAME), parent),
    ui(new Ui::MemoryTestWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    setUIInitState();
}

MemoryTestWidget::~MemoryTestWidget()
{
    delete ui;
}

void MemoryTestWidget::cbMemoryTestProgress(void *_this, unsigned int progress,
                                            unsigned long long data_xferd,
                                            const QString &info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    Q_UNUSED(data_xferd)

    int n_count = ui->lineEdit_dramtest_stresscount->text().toInt();
    if (progress == 0 && n_count > 1) {
        LOG("dram flip test , current round: %u", m_current_tested_count);
        QString msg = QString("Current Round: %1").arg(m_current_tested_count);
        this->updateUI(msg, Qt::darkCyan);
    }

    QString msg = QString().sprintf("[FLIP]%d%%", progress);
    this->updateUI(msg);

    if (progress == 100 && n_count > 1) {
        LOG("Round %u Pass", m_current_tested_count);
        msg = QString("Round %1 Pass").arg(m_current_tested_count);
        this->updateUI(msg, Qt::darkGray);
        ++m_current_tested_count;
    }
}

void MemoryTestWidget::LockOnUI()
{
    ui->toolButton_start->setEnabled(false);
    ui->groupBox_dram_repair->setEnabled(false);
    ui->DRAM_FLIP_TEST->setEnabled(false);
    ui->chk_dram_interface_diagnose->setEnabled(false);
    ui->toolButton_stop->setEnabled(true);
    m_current_tested_count = 0;
}

void MemoryTestWidget::DoFinished()
{
    ui->toolButton_start->setEnabled(true);
    ui->groupBox_dram_repair->setEnabled(true);
    ui->DRAM_FLIP_TEST->setEnabled(true);
    ui->chk_dram_interface_diagnose->setEnabled(true);
    ui->toolButton_stop->setEnabled(false);
    m_current_tested_count = 0;
}

void MemoryTestWidget::ShowDRAMRepairInfo(bool isShow)
{
    ui->groupBox_dram_repair->setVisible(isShow);
    if (isShow) {
        ui->groupBox_dram_repair->setChecked(false);
    }
}

void MemoryTestWidget::slot_after_dram_repair_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    *p_result = ICmdCallBack::cb_after_dram_repair_cmd(conn, iCmdXMLGenerator);
}

void MemoryTestWidget::slot_after_dram_interface_diagnose_cmd(int *p_result, const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    std::shared_ptr<CalibrationRAMTestCmdXML> dramIntfDiagnoseCmdXML =
            std::dynamic_pointer_cast<CalibrationRAMTestCmdXML>(iCmdXMLGenerator);
    assert(dramIntfDiagnoseCmdXML);
    QString cmd_value = dramIntfDiagnoseCmdXML->getCmdXMLValue();
    this->updateUI("===============\tDRAM Interface Diagnose\t===============", Qt::blue);
    this->updateUI(cmd_value);
    *p_result = CFT_SUCCESS;
}

void MemoryTestWidget::updateUI(const QString &msg, QColor color/* = Qt::black*/)
{
    ui->MemoryTestReport->setUpdatesEnabled(false);
    ui->MemoryTestReport->setTextColor(color);
    ui->MemoryTestReport->append(msg);
    ui->MemoryTestReport->setUpdatesEnabled(true);
}

void MemoryTestWidget::setUIInitState()
{
    QRegExp regExpHex("0x[\\da-fA-F]{1,16}");
    ui->lineEdit_dramtest_address->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_dramtest_address));
    ui->lineEdit_dramtest_length->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_dramtest_length));
    ui->lineEdit_dramtest_stresscount->setValidator(new QIntValidator(1, 9999, ui->lineEdit_dramtest_stresscount));
    ShowDRAMRepairInfo(false);

    ui->label_dramtest_stresscount->setVisible(false);
    ui->lineEdit_dramtest_stresscount->setVisible(false);
}

void MemoryTestWidget::clearUIText()
{
    ui->MemoryTestReport->setUpdatesEnabled(false);
    ui->MemoryTestReport->clear();
    ui->MemoryTestReport->setUpdatesEnabled(true);
}

QPair<bool, QString> MemoryTestWidget::validBeforeMemoryTest() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    if (ui->radioButton_dramtest_manual->isChecked()) {
        QRegExp regexp(valid_hex_address_expr);
        if (regexp.indexIn(ui->lineEdit_dramtest_address->text()) == -1) {
            result.first = false;
            result.second = QStringLiteral("Please fill the \"Offset\" field.");
        } else if (regexp.indexIn(ui->lineEdit_dramtest_length->text()) == -1) {
            result.first = false;
            result.second = QStringLiteral("Please fill the \"Length\" field.");
        } else if (ui->lineEdit_dramtest_stresscount->text().isEmpty()) {
            result.first = false;
            result.second = QStringLiteral("Please fill the \"Stress Test Count\" field.");
        }
    }
    return result;
}

QPair<bool, QString> MemoryTestWidget::validBeforeDRAMRepair() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    if (ui->lineEdit_dram_repair->text().isEmpty()) {
        result.first = false;
        result.second = QStringLiteral("Please select \"dram repair file\" before dram repair operation.");
    }
    return result;
}

QPair<bool, QString> MemoryTestWidget::validBeforeDRAMInterfaceDiagnose() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
    }
    return result;
}

std::shared_ptr<CmdWrapper> MemoryTestWidget::createCmdWrapper()
{
    QString start_address = ui->lineEdit_dramtest_address->text().trimmed();
    QString length = ui->lineEdit_dramtest_length->text().trimmed();
    if (ui->radioButton_dramtest_auto->isChecked()) {
        start_address = QStringLiteral("0x0");
        length = QStringLiteral("MAX");
    }

    std::shared_ptr<FlipRAMTestCmdXML> flipRAMTestCmdXML = std::make_shared<FlipRAMTestCmdXML>();
    flipRAMTestCmdXML->setStartAddress(start_address);
    flipRAMTestCmdXML->setLength(length);
    flipRAMTestCmdXML->setRepeatCount(ui->lineEdit_dramtest_stresscount->text().toInt());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(flipRAMTestCmdXML);
    qRegisterMetaType<std::shared_ptr<Connection>>("std::shared_ptr<Connection>");
    QObject::connect(cmd_wrapper.get(), &CmdWrapper::signal_before_cmd,
                     this, &MemoryTestWidget::slot_before_memory_test_cmd, Qt::BlockingQueuedConnection);
    cmd_wrapper->setOperationProgress(std::bind(&MemoryTestWidget::cbMemoryTestProgress,
                                                this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> MemoryTestWidget::createDramRepairCmdWrapper()
{
    std::shared_ptr<DebugDRAMRepairCmdXML> dramRepairCmdXML = std::make_shared<DebugDRAMRepairCmdXML>();
    dramRepairCmdXML->setParam_file(ui->lineEdit_dram_repair->text());
    dramRepairCmdXML->setFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(dramRepairCmdXML);
    connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd, this, &MemoryTestWidget::slot_after_dram_repair_cmd,
            Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

std::shared_ptr<CmdWrapper> MemoryTestWidget::createDRAMInterfaceDiagnoseCmdWrapper()
{
    std::shared_ptr<CalibrationRAMTestCmdXML> dramIntfDiagnoseCmdCML = std::make_shared<CalibrationRAMTestCmdXML>();
    dramIntfDiagnoseCmdCML->setTargetFileType(FILE_TYPE::MEMORY_FILE);
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(dramIntfDiagnoseCmdCML);
    connect(cmd_wrapper.get(), &CmdWrapper::signal_after_cmd, this, &MemoryTestWidget::slot_after_dram_interface_diagnose_cmd,
            Qt::BlockingQueuedConnection);
    return  cmd_wrapper;
}

void MemoryTestWidget::doDramRepairOperation()
{
    QPair<bool, QString> valid_result = this->validBeforeDRAMRepair();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(CONN_END_STAGE::FIRST_DA_STAGE, false);
    m_mainwindow->addCmdWrapper(this->createDramRepairCmdWrapper());
    m_mainwindow->addCmdWrapper(m_mainwindow->createWDTCmdWrapper());
    m_mainwindow->startCmd(tr("DRAM repair"));
}

void MemoryTestWidget::doMemoryTestOperation()
{
    QPair<bool, QString> valid_result = this->validBeforeMemoryTest();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(CONN_END_STAGE::FIRST_DA_STAGE);
    m_mainwindow->addCmdWrapper(this->createCmdWrapper());
    m_mainwindow->addCmdWrapper(m_mainwindow->createWDTCmdWrapper());
    m_mainwindow->startCmd(tr("Memory Test Ok"));
}

void MemoryTestWidget::doDRAMInterfaceDiagnoseOperation()
{
    QPair<bool, QString> valid_result = this->validBeforeDRAMInterfaceDiagnose();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(CONN_END_STAGE::FIRST_DA_STAGE, false);
    m_mainwindow->addCmdWrapper(this->createDRAMInterfaceDiagnoseCmdWrapper());
    m_mainwindow->startCmd(tr("DRAM Interface Diagnose"));
}

void MemoryTestWidget::on_toolButton_start_clicked()
{
    clearUIText();
    if (ui->DRAM_FLIP_TEST->isChecked()) {
        doMemoryTestOperation();
    } else if (ui->chk_dram_interface_diagnose->isChecked()) {
        doDRAMInterfaceDiagnoseOperation();
    } else if (ui->groupBox_dram_repair->isChecked()){
        doDramRepairOperation();
    } else {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME,
                                     "Please select one operation!", STRING_OK);
    }
}

void MemoryTestWidget::on_pushButton_dram_repair_clicked()
{
    if (m_mainwindow->isFlashXMLLoaded()) {
        QString flash_xml = m_mainwindow->getFlashXMLFile();
        QString dram_repair_file = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                                QFileInfo(flash_xml).absolutePath(), "All File (*.*)");
        if (!dram_repair_file.isEmpty()) {
            ui->lineEdit_dram_repair->setText(dram_repair_file);
        }
    }
}

void MemoryTestWidget::on_toolButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void MemoryTestWidget::on_radioButton_dramtest_manual_clicked(bool checked)
{
    ui->lineEdit_dramtest_address->setEnabled(checked);
    ui->lineEdit_dramtest_length->setEnabled(checked);
    ui->lineEdit_dramtest_stresscount->setEnabled(checked);
}

void MemoryTestWidget::on_radioButton_dramtest_auto_clicked(bool checked)
{
    on_radioButton_dramtest_manual_clicked(!checked);
}

void MemoryTestWidget::on_DRAM_FLIP_TEST_clicked(bool checked)
{
    if (checked) {
        if (ui->groupBox_dram_repair->isVisible()) {
            ui->groupBox_dram_repair->setChecked(false);
        }
        ui->chk_dram_interface_diagnose->setChecked(false);
    }
}

void MemoryTestWidget::on_groupBox_dram_repair_clicked(bool checked)
{
    if (checked) {
        ui->DRAM_FLIP_TEST->setChecked(false);
        ui->chk_dram_interface_diagnose->setChecked(false);
    }
}

void MemoryTestWidget::on_chk_dram_interface_diagnose_clicked(bool checked)
{
    if (checked) {
        if (ui->groupBox_dram_repair->isVisible()) {
            ui->groupBox_dram_repair->setChecked(false);
        }
        ui->DRAM_FLIP_TEST->setChecked(false);
    }
}

void MemoryTestWidget::slot_before_memory_test_cmd(int *p_result, const std::shared_ptr<Connection> conn)
{
    Q_UNUSED(conn)
    updateUI("============\t DRAM Flip Test \t============\n", Qt::cyan);
    QString msg;
    long long ext_ram_size = m_mainwindow->get_ext_ram_size();
    if(0x20000 <= ext_ram_size)
    {
        msg = QString("\tDRAM Size = 0x%1 (%2MB/%3Mb)\n").arg(ext_ram_size, 8, 16, QLatin1Char('0'))
                .arg(ext_ram_size/1024/1024)
                .arg(ext_ram_size/1024*8/1024);

        updateUI(msg);
    }
    else
    {
        msg = QString("\tDRAM Size = 0x%1 (%2KB/%3Kb)\n").arg(ext_ram_size, 8, 16, QLatin1Char('0'))
                .arg(ext_ram_size/1024)
                .arg(ext_ram_size/1024*8);
        updateUI(msg);
    }
    *p_result = CFT_SUCCESS;
}

void MemoryTestWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_MEMORY_TEST_PAGE_NAME));
}
