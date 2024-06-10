#include "ToolUpdate.h"
#include "CheckUpdateDialog.h"
#include "UpdateDialog.h"
#include "MainWindow.h"
#include "../../Utility/Utils.h"
#include "MultiLanguageMapping.h"

ToolUpdateMgr::ToolUpdateMgr(MainWindow *parent):
    QObject(parent),
    m_mainwindow(parent),
    m_async_updater(new AsyncUpdater(this)),
    m_check_update_dialog(nullptr),
    m_update_dialog(nullptr)
{
    connect(m_async_updater.data(), SIGNAL(signal_check_finish(bool)),
            this, SLOT(slot_check_finish(bool)));
    qRegisterMetaType<AsyncUpdater::UpdateFailReason>("AsyncUpdater::UpdateFailReason");
    connect(m_async_updater.data(), SIGNAL(signal_update_failed(AsyncUpdater::UpdateFailReason)),
            this, SLOT(slot_update_failed(AsyncUpdater::UpdateFailReason)));
}

ToolUpdateMgr::~ToolUpdateMgr()
{
    m_async_updater->waitForFinished();
}

void ToolUpdateMgr::startUpdate(bool show_update_dialog)
{
    if (show_update_dialog) {
        showUpdateDialog();
    } else {
        m_async_updater->CheckAsyncNewVersion();
    }
}

void ToolUpdateMgr::updateUI()
{
    if (m_update_dialog) {
        m_update_dialog->UpdateUI();
    }
    if (m_check_update_dialog) {
        m_check_update_dialog->UpdateUI();
    }
}

void ToolUpdateMgr::slot_check_finish(bool hasUpdate)
{
    if(hasUpdate) {
        launchUpdateDialog();
    } else {
        if(m_check_update_dialog)
        {
            m_check_update_dialog->SetCheckUpdateInfo(GET_TRANSLATE_STRING(IDS_STRING_NO_UPDATE));
            m_check_update_dialog->SetOKButtonVisible(true);
        }
    }
}

void ToolUpdateMgr::slot_update_failed(AsyncUpdater::UpdateFailReason failReason)
{
    switch (failReason)
    {
    case AsyncUpdater::unknown_reason:
        Utils::flashtool_message_box(nullptr, nullptr, WARNING_MSGBOX,
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_TITLE),
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_FAILED),
                              GET_TRANSLATE_STRING(IDS_STRING_OK));
        break;
    case AsyncUpdater::lack_of_update_files:
        Utils::flashtool_message_box(nullptr, nullptr, WARNING_MSGBOX,
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_TITLE),
                              GET_TRANSLATE_STRING(IDS_STRING_LACKING_OF_UPDATE_FILES),
                              GET_TRANSLATE_STRING(IDS_STRING_OK));
        break;
    case AsyncUpdater::start_update_exe_fail:
        Utils::flashtool_message_box(nullptr, nullptr, WARNING_MSGBOX,
                              GET_TRANSLATE_STRING(IDS_STRING_UPDATE_TITLE),
                              GET_TRANSLATE_STRING(IDS_STRING_START_UPDATE_TOOL_FAILED),
                              GET_TRANSLATE_STRING(IDS_STRING_OK));
        break;
    }
}

void ToolUpdateMgr::launchUpdateDialog()
{
    if(m_check_update_dialog)
        m_check_update_dialog->close();

    if(!m_update_dialog)
    {
        m_update_dialog = QSharedPointer<UpdateDialog>(new UpdateDialog(m_mainwindow, m_async_updater));
    }
    m_update_dialog->show();
}

void ToolUpdateMgr::showUpdateDialog()
{
    if(!m_check_update_dialog)
    {
        m_check_update_dialog = QSharedPointer<CheckUpdateDialog>(new CheckUpdateDialog(m_mainwindow, m_async_updater));

        connect(m_check_update_dialog.data(), SIGNAL(signal_check_finish(bool)),
                this, SLOT(slot_check_finish(bool)));
    }

    m_check_update_dialog->CheckUpdate();
    m_check_update_dialog->show();
}
