#include "DLCertWidget.h"
#include "ui_DLCertWidget.h"
#include "../../Inc/CFlashToolConst.h"
#include "MainWindow.h"
#include <QFileDialog>
#include "../../Utility/FileUtils.h"
#include "../../Utility/Utils.h"
#include "../../XML/CmdGenerator/WritePrivateCertCmdXML.h"
#include "../../Cmd/CmdWrapper.h"
#include "MultiLanguageMapping.h"

static const QString PDLCertColorDarkCyan (" QProgressBar::chunk{background-color: darkCyan} QProgressBar {text-align: center; border-radius: 0px}");

using namespace std::placeholders;

DLCertWidget::DLCertWidget(MainWindow *mainwindow, QTabWidget *parent) :
    TabWidgetBase(DLCERT_WIDGET_INDEX, GET_TRANSLATE_STRING(IDS_STRING_DL_CERT_PAGE_NAME), parent),
    ui(new Ui::DLCertWidget),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);
}

DLCertWidget::~DLCertWidget()
{
    delete ui;
}

void DLCertWidget::cbDLCertProgress(void *_this, unsigned int progress,
                                              unsigned long long data_xferd,
                                              const QString &info)
{
    QString progress_text = tr("DL Cert %1%").arg(progress);
    m_mainwindow->operation_progress(_this, progress, data_xferd, info, PDLCertColorDarkCyan, progress_text, true);
}

CONN_END_STAGE DLCertWidget::getConnStage() const
{
    return CONN_END_STAGE::SECOND_DA_STAGE;
}

std::shared_ptr<CmdWrapper> DLCertWidget::createCmdWrapper()
{
    std::shared_ptr<WritePrivateCertCmdXML> writeCertCmdXML = std::make_shared<WritePrivateCertCmdXML>();
    writeCertCmdXML->setFileName(ui->lineEdit_FilePath->text().trimmed());
    std::shared_ptr<CmdWrapper> cmd_wrapper = std::make_shared<CmdWrapper>(writeCertCmdXML);
    cmd_wrapper->setOperationProgress(std::bind(&DLCertWidget::cbDLCertProgress, this, _1, _2, _3, _4));
    return  cmd_wrapper;
}

QPair<bool, QString> DLCertWidget::validBeforeDLCert() const
{
    QPair<bool, QString> result;
    result.first = true;
    if (!m_mainwindow->isFlashXMLLoaded()) {
        result.first = false;
        result.second = STRING_SELECT_FLASH_XML;
        return result;
    }

    if (ui->lineEdit_FilePath->text().isEmpty()) {
        result.first = false;
        result.second = QStringLiteral("Please select a file before operation.");
    }
    return result;
}

void DLCertWidget::on_toolButton_dl_cert_clicked()
{
    QPair<bool, QString> valid_result = this->validBeforeDLCert();
    if (!valid_result.first)
    {
        Utils::flashtool_message_box(m_mainwindow, nullptr, WARNING_MSGBOX, STRING_TOOL_NAME, valid_result.second, STRING_OK);
        return ;
    }
    m_mainwindow->setupBeforeCmdOperation(getConnStage());
    m_mainwindow->addCmdWrapper(this->createCmdWrapper());
    m_mainwindow->addCmdWrapper(m_mainwindow->createWDTCmdWrapper());
    m_mainwindow->startCmd(tr("DL Cert Ok"));
}

void DLCertWidget::on_toolButton_stop_clicked()
{
    m_mainwindow->stopCmd();
}

void DLCertWidget::on_toolButton_openFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(
                this,
                tr("Open Cert File"),
                FileUtils::GetAppDirectory().c_str(),
                tr("All Files (*.cert)"));

    if(!file_name.isEmpty()) {
        QFileInfo fileInfo(file_name);
        ui->lineEdit_FilePath->setText(file_name);
    }
    else {
        ui->lineEdit_FilePath->setText("");
    }
}

void DLCertWidget::LockOnUI()
{
    ui->toolButton_dl_cert->setEnabled(false);
    ui->toolButton_stop->setEnabled(true);
    ui->groupBox_dl_cert_parameter->setEnabled(false);
}

void DLCertWidget::DoFinished()
{
    ui->toolButton_dl_cert->setEnabled(true);
    ui->toolButton_stop->setEnabled(false);
    ui->groupBox_dl_cert_parameter->setEnabled(true);
}

void DLCertWidget::UpdateUI()
{
    ui->retranslateUi(this);
    m_mainwindow->refreshTabText(this, GET_TRANSLATE_STRING(IDS_STRING_DL_CERT_PAGE_NAME));
}
