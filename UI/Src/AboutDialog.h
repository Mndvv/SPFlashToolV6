#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
namespace Ui
{
class AboutDialog;
}

class MainWindow;
class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(MainWindow *parent);
    ~AboutDialog();

    void UpdateUI();

private:
    void UpdateVersionInfo();
    void showDADLLInfo(bool show);

private:
    Ui::AboutDialog *ui;
    MainWindow *m_main_window;
};

#endif // ABOUTDIALOG_H
