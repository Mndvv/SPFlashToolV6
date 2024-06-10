#include "BromAdapterWidget.h"
#include "ui_BromAdapterWidget.h"
#include "../../Inc/CFlashToolConst.h"
#include "MainWindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include "../../Utility/FileUtils.h"
#include "../../Utility/Utils.h"
#include "../../XML/CmdGenerator/RunProgramCmdXML.h"
#include "../../Cmd/CmdWrapper.h"
#include "MultiLanguageMapping.h"

static const QString PBromAdapterColorBrown("QProgressBar::chunk{background-color: cyan} QProgressBar {text-align:center; border-radius:0px}");

using namespace std::placeholders;

BromAdapterWidget::BromAdapterWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(BROMADAPTER_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_BROM_ADAPTER_PAGE_NAME), parent),
    ui(new Ui::BromAdapterWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);
    QRegExp regExpHex("0x[\\da-fA-F]{1,8}");
    ui->lineEdit_startAddress->setValidator(new QRegExpValidator(regExpHex, ui->lineEdit_startAddress));
}

BromAdapterWidget::~BromAdapterWidget()
{
    delete ui;
}

void BromAdapterWidget::cbBromAdapterProgress(void *_this, unsigned int progress,
                                              unsigned long long data_xferd, const QString &info)
{
    QString progress_text = tr("Download Brom load %1%").arg(progress);
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, PBromAdapterColorBrown, progress_text, false);
}

std::shared_ptr<CmdWrapper> BromAdapterWidget::createCmdWrapper()
{
    std::shared_ptr<RunProgramCmdXML> runProgramCmdXML = std::make_shared<RunProgramCmdXML>();
    runProgramCmdXML->setAtAddress(ui->lineEdit_startAddress->text());
    runProgramCmdXML->setJmpAddress(ui->lineEdit_startAddress->text());
    runProgramCmdXML->setFileName(ui->lineEdit_dataPath->text());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(runProgramCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&BromAdapterWidget::cbBromAdapterProgress,
                                                this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

QPair<bool, QString> BromAdapterWidget::validDownloadFile() const
{
    QPair<bool, QString> result;
    result.first = false;
    if (ui->lineEdit_dataPath->text().isEmpty()) {
        result.second = QStringLiteral("Please select download file before operation.");
        return result;
    }
    QFileInfo file_info(ui->lineEdit_dataPath->text().trimmed());
    if (!file_info.exists()) {
        result.second = QStringLiteral("The selected download file is not exist yet, please select again.");
        return result;
    }

#define MAX_FILE_SIZE 10 * 1204 * 1024
    if (file_info.size() > MAX_FILE_SIZE) {
        result.second = QStringLiteral("The download file can NOT exceed 10M, please select again.");
        return result;
    }

    result.first = true;
    return result;
}

QPair<bool, QString> BromAdapterWidget::validBeforeOperation() const
{
    QPair<bool, QString> valid_result;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        valid_result.first = false;
        valid_result.second = STRING_SELECT_FLASH_XML;
        return valid_result;
    }

    valid_result = this->validDownloadFile();
    if (valid_result.first) {
        QRegExp regexp(valid_hex_address_expr);
        if (regexp.indexIn(ui->lineEdit_startAddress->text()) == -1) {
            valid_result.first = false;
            valid_result.second = QStringLiteral("Please fill \"Start Address\" field.");
        }
    }
    return valid_result;
}

void BromAdapterWidget::on_toolButton_download_clicked()
{
    QPair<bool, QString> valid_result = this->validBeforeOperation();
    if (!valid_result.first) {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }

    m_mainwindow->setupBeforeCmdOperation(CONN_END_STAGE::BROM_STAGE);
    m_mainwindow->addCmdWrapper(this->createCmdWrapper());
    m_mainwindow->startCmd(tr("BROM download OK."));
}

void BromAdapterWidget::on_toolButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void BromAdapterWidget::on_pushButton_data_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open Download Data File"),
                FileUtils::GetAppDirectory().c_str());

    if(!file_name.isEmpty())
    {
        file_name = QDir::toNativeSeparators(file_name);
        ui->lineEdit_dataPath->setText(file_name);
        ui->toolButton_download->setEnabled(true);
    }
}

void BromAdapterWidget::LockOnUI()
{
    lockUI(true);
}

void BromAdapterWidget::DoFinished()
{
    lockUI(false);
}

void BromAdapterWidget::lockUI(bool lock)
{
    ui->toolButton_download->setEnabled(!lock);
    ui->toolButton_stop->setEnabled(lock);
    ui->lineEdit_dataPath->setEnabled(!lock);
    ui->lineEdit_startAddress->setEnabled(!lock);
    ui->pushButton_data->setEnabled(!lock);
}

void BromAdapterWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_BROM_ADAPTER_PAGE_NAME));
}
