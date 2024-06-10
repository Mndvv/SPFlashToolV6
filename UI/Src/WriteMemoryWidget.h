#ifndef WRITEMEMORYWIDGET_H
#define WRITEMEMORYWIDGET_H

#include "TabWidgetBase.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Inc/CFlashToolTypes.h"

namespace Ui {
class WriteMemoryWidget;
}

class MainWindow;

class WriteMemoryWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit WriteMemoryWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~WriteMemoryWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbWriteMemoryProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    CONN_END_STAGE getConnStage() const;
    CmdWrapperSPList createCmdWrapperList();
    QPair<bool, QString> validBeforeWriteMemeory() const;

private:
    std::shared_ptr<CmdWrapper> createCmdWrapper();

private slots:
    void on_toolButton_writeMemory_clicked();
    void on_toolButton_stop_clicked();
    void on_toolButton_openFile_clicked();
    void slot_flash_xml_changed(bool is_nand_operation);

private:
    Ui::WriteMemoryWidget *ui;
    MainWindow *m_mainwindow;
    QStringList m_ufs_emmc_regions;
    QStringList m_nand_dummy_region;
};

#endif // WRITEMEMORYWIDGET_H
