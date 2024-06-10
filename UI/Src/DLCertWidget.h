#ifndef DLCERTWIDGET_H
#define DLCERTWIDGET_H

#include "TabWidgetBase.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Inc/CFlashToolTypes.h"

namespace Ui {
class DLCertWidget;
}

class MainWindow;

class DLCertWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit DLCertWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~DLCertWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbDLCertProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    CONN_END_STAGE getConnStage() const;
    QPair<bool, QString> validBeforeDLCert() const;

private:
    std::shared_ptr<CmdWrapper> createCmdWrapper();

private slots:
    void on_toolButton_dl_cert_clicked();
    void on_toolButton_stop_clicked();
    void on_toolButton_openFile_clicked();

private:
    Ui::DLCertWidget *ui;
    MainWindow *m_mainwindow;
    QStringList m_ufs_emmc_regions;
    QStringList m_nand_dummy_region;
};

#endif // DLCERTWIDGET_H
