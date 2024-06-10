#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"

#include <QThreadPool>
#include "AsyncUpdater.h"
#include "MainWindow.h"
#include "../../Host/Inc/RuntimeMemory.h"
#include "DownloadWidget.h"
#include "../../Utility/Utils.h"
#include "MultiLanguageMapping.h"

UpdateDialog::UpdateDialog(MainWindow *mainwindow, QSharedPointer<AsyncUpdater> updater) :
    QDialog(mainwindow),
    m_async_updater(updater),
    ui(new Ui::UpdateDialog),
    m_mainwindow(mainwindow)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->textBrowser_note->setText(m_async_updater->GetReleaseNotes());
    UpdateUI();
}

UpdateDialog::~UpdateDialog()
{
    if(ui)
    {
        delete ui;
        ui = nullptr;
    }
}

void UpdateDialog::on_pushButton_remind_clicked()
{
    //TODO: When to re update

    close();
}

#define CURRENT_PROCESS_NAME "SPFlashToolV6.exe"
void UpdateDialog::on_pushButton_install_clicked()
{
    if (Utils::IsMoreThanOneProcessByName(CURRENT_PROCESS_NAME))
    {
        Utils::flashtool_message_box(this, nullptr, INFORMATION_MSGBOX,
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_TITLE),
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_MULTI_PROCESS),
                              GET_TRANSLATE_STRING(IDS_STRING_OK));
    }
    else
    {
        //todo
        //m_mainwindow->downloadWidget()->UserCancelLoadScatter();
        QThreadPool::globalInstance()->waitForDone();
        m_async_updater->AsyncUpdateTool();
    }
}

void UpdateDialog::UpdateUI()
{
    this->setWindowTitle(GET_TRANSLATE_STRING(IDS_STRING_UPDATE));
    ui->label->setText(GET_TRANSLATE_STRING(IDS_STRING_NEW_DETECT));
    ui->pushButton_remind->setText(GET_TRANSLATE_STRING(IDS_STRING_REMIND));
    ui->pushButton_install->setText(GET_TRANSLATE_STRING(IDS_STRING_INSTALL_UPDATE));
}
