#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class AsyncUpdater;
class MainWindow;

class UpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UpdateDialog(MainWindow *mainwindow, QSharedPointer<AsyncUpdater> updater);
    ~UpdateDialog();

    void UpdateUI();
private slots:
    void on_pushButton_remind_clicked();

    void on_pushButton_install_clicked();

private:
    QSharedPointer<AsyncUpdater> m_async_updater;
    Ui::UpdateDialog *ui;
    MainWindow *m_mainwindow;
};

#endif // UPDATEDIALOG_H
