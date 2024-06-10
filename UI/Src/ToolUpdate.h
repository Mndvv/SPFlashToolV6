#ifndef TOOLUPDATE_H
#define TOOLUPDATE_H

#include <QObject>
#include <QSharedPointer>
#include "AsyncUpdater.h"

class CheckUpdateDialog;
class UpdateDialog;
class MainWindow;

class ToolUpdateMgr: public QObject
{
    Q_OBJECT

public:
    ToolUpdateMgr(MainWindow *parent);
    virtual ~ToolUpdateMgr();

    void startUpdate(bool show_update_dialog);
    void updateUI();

private slots:
    void slot_check_finish(bool hasUpdate);
    void slot_update_failed(AsyncUpdater::UpdateFailReason failReason);

private:
    void launchUpdateDialog();
    void showUpdateDialog();

private:
    MainWindow *m_mainwindow;
    QSharedPointer<AsyncUpdater> m_async_updater;
    QSharedPointer<CheckUpdateDialog> m_check_update_dialog;
    QSharedPointer<UpdateDialog> m_update_dialog;
};

#endif // TOOLUPDATE_H
