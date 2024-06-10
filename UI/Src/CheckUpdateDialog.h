#ifndef CHECKUPDATEDIALOG_H
#define CHECKUPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class CheckUpdateDialog;
}

class AsyncUpdater;
class MainWindow;

class CheckUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckUpdateDialog(MainWindow *mainwindow, QSharedPointer<AsyncUpdater> updater);
    ~CheckUpdateDialog();

    void SetCheckUpdateInfo(const QString &str);
    void SetOKButtonVisible(const bool& show);

    void UpdateUI();
    void CheckUpdate() const;
signals:
    void signal_check_finish(bool hasUpdate);

private slots:
    void on_pushButton_ok_clicked();

private:
    Ui::CheckUpdateDialog *ui;
    QSharedPointer<AsyncUpdater> m_async_checker;
    MainWindow *m_mainwindow;
};

#endif // CHECKUPDATEDIALOG_H
