#ifndef READBACKWIDGET_H
#define READBACKWIDGET_H

#include <QMenu>
#include "TabWidgetBase.h"
#include "../../Inc/CFlashToolTypes.h"
#include "../../Cmd/CmdWrapper.h"

namespace Ui {
class ReadBackWidget;
}

class MainWindow;
class ReadbackBackup;

class ReadBackWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    explicit ReadBackWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~ReadBackWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbReadbackProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    void reStartReadback();
    CONN_END_STAGE getConnStage() const;
    CmdWrapperSPList createRBCmdWrapperList();

    void autoLoadRBSetting();
    void autoSaveRBSetting(const QString &chip_name, HW_StorageType_E storage_type) const;
    bool hasRBItemSelected() const;

signals:
    void signal_reset_rb_method(READBACK_METHOD rb_method);

private slots:
    void on_toolButton_readBack_clicked();
    void on_toolButton_read_pt_clicked();
    void on_toolButton_stop_clicked();
    void on_toolButton_add_clicked();
    void on_toolButton_remove_clicked();
    void slot_enable_checked(bool checked);
    void slot_table_doubleClicked(const QModelIndex & index);
    void slot_all_items_selected(bool all_selected);
    void slot_show_context_menu(const QPoint &pos);
    void slot_context_menu_about_to_show();
    void slot_after_read_partition_table_cmd(int *p_result, const std::shared_ptr<Connection> conn,
        const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_flash_xml_changed(bool is_nand_operation);
    void slot_RB_method_changed(bool checked = false);
    void slot_show_start_addr_toggled(bool checked);
    void slot_show_length_toggled(bool checked);

private:
    void setUIInitState();
    void initTableWidget();
    void adjustTableColWidth();
    void createContextMenu();
    QStringList getTableHeaderLabels() const;
    void replaceRBFile(const QModelIndex &index);
    QPair<bool, QString> validateBeforeReadback() const;
    void setupManualRBCmdList();
    CmdWrapperSPList createReadFlashCmdWrapperList();
    CmdWrapperSPList createReadPartitionCmdWrapperList();
    std::shared_ptr<CmdWrapper> createReadFlashCmdWrapper(const QString &region_text,
                                                          const QString &start_addr,
                                                          const QString &length,
                                                          const QString &file_name);
    std::shared_ptr<CmdWrapper> createReadPartitionCmdWrapper(const QString &partition_name,
                                                              const QString &file_name);
    cmd_wrapper_ptr createReadPartitionTableCmdWrapper();
    bool validReadBackXMLFormat(const QString &file_name) const;
    bool isRBXMLMatched(const ReadbackBackup &rb_backup) const;
    void updateBtnEnabledByRBData() const;
    void updateColumnVisible() const;
    void hideAllOptionalColumns() const;
    void showManualRBColumns() const;
    void showAutoRBColumns() const;
    void manualReadback();
    void autoReadbackWithReadPt();
    void autoReadbackWithoutReadPt();
    CmdWrapperSPList createManualRBCmdWrapperList();
    CmdWrapperSPList createAutoRBCmdWrapperList();

private:
    Ui::ReadBackWidget *ui;

    MainWindow *m_mainwindow;
    QMenu *m_context_menu;
    QAction *m_add_act;
    QAction *m_remove_act;
    QAction *m_show_start_addr;
    QAction *m_show_length;
    bool m_auto_RB_finished;
};

#endif // READBACKWIDGET_H
