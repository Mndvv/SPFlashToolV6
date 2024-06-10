#ifndef FORMATWIDGET_H
#define FORMATWIDGET_H

#include "TabWidgetBase.h"
#include "../../Inc/CFlashToolTypes.h"
#include "../../Cmd/CmdWrapper.h"

namespace Ui {
class FormatWidget;
}

class MainWindow;

class FormatWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit FormatWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~FormatWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbFormatProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    std::shared_ptr<CmdWrapper> createAutoFormatCmdWrapper();
    void reStartFormat();
    CONN_END_STAGE getConnStage() const;
    CmdWrapperSPList createCmdWrapperList();
    void refreshRegion(bool is_advanced_mode, bool is_nand_storage);
    QPair<bool, QString> validateBeforeFormat() const;

private slots:
    void on_toolButton_Start_clicked();
    void on_toolButton_stop_clicked();
    void on_radioButton_AutoFormat_clicked();
    void on_radioButton_ManualFormat_clicked();
    void slot_flash_xml_changed(bool is_nand_operation);

private:
    void setUIState();

private:
    std::shared_ptr<CmdWrapper> createManualFormatCmdWrapper();
    void setupFormatCmdList();
    std::shared_ptr<CmdWrapper> createCmdWrapper();

private:
    Ui::FormatWidget *ui;

    MainWindow *m_mainwindow;
    QStringList m_ufs_emmc_regions;
    QStringList m_nand_dummy_region;
    QStringList m_nand_dummy_region_advanced;
};

#endif // FORMATWIDGET_H
