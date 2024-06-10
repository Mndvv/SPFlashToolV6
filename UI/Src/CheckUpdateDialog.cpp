#include "CheckUpdateDialog.h"
#include "ui_CheckUpdateDialog.h"
#include "AsyncUpdater.h"
#include "MainWindow.h"
#include "../../Host/Inc/RuntimeMemory.h"
#include "MultiLanguageMapping.h"

CheckUpdateDialog::CheckUpdateDialog(MainWindow *mainwindow, QSharedPointer<AsyncUpdater> updater):
    QDialog(mainwindow),
    ui(new Ui::CheckUpdateDialog),
    m_async_checker(updater),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    SetOKButtonVisible(false);

    connect(m_async_checker.data(), SIGNAL(signal_check_finish(bool)),
            this,SIGNAL(signal_check_finish(bool)));

    UpdateUI();
}

CheckUpdateDialog::~CheckUpdateDialog()
{
    if(ui)
    {
        delete ui;
        ui = NULL;
    }
}

void CheckUpdateDialog::SetCheckUpdateInfo(const QString &str)
{
    ui->label_info->setText(str);
}

void CheckUpdateDialog::SetOKButtonVisible(const bool &show)
{
    ui->pushButton_ok->setVisible(show);

    if(show)
    {
       ui->progressBar->setMaximum(1);
       ui->progressBar->setValue(1);
    }
}

void CheckUpdateDialog::on_pushButton_ok_clicked()
{
    close();
}

void CheckUpdateDialog::UpdateUI()
{
    this->setWindowTitle(GET_TRANSLATE_STRING(IDS_STRING_UPDATE));
    if(ui->pushButton_ok->isHidden())
    {
        ui->label_info->setText(GET_TRANSLATE_STRING(IDS_STRING_CHECK_UPDATE));
        ui->pushButton_ok->setText(GET_TRANSLATE_STRING(IDS_STRING_OK));
    }
    else
    {
        ui->label_info->setText(GET_TRANSLATE_STRING(IDS_STRING_NO_UPDATE));
        ui->pushButton_ok->setText(GET_TRANSLATE_STRING(IDS_STRING_OK));
    }
}

void CheckUpdateDialog::CheckUpdate() const
{
    m_async_checker->CheckAsyncNewVersion();
}
