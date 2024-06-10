#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QMenu>
#include "TabWidgetBase.h"
#include "../../Utility/PartitionInfo.h"
#include "../../Inc/CFlashToolTypes.h"
#include "../../Cmd/CmdWrapper.h"

namespace Ui {
class DownloadWidget;
}

class MainWindow;
class Connection;
class ICmdXMLGenerator;
class QEvent;
class QDragEnterEvent;
class QDropEvent;

class DownloadWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    using DOWNLOAD_PARTITION_LIST = std::vector<std::pair<std::string, std::string>>;

    explicit DownloadWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~DownloadWidget();

    DECLARE_TABWIDGET_VFUNCS()

    void cbDownloadProgress(void *_this, unsigned int progress,
                          unsigned long long data_xferd, const QString &info);
    void reStartDownload();
    CONN_END_STAGE getConnStage() const;
    CmdWrapperSPList createCmdWrapperList();

    void Enable_groupBox_boot_mode_flag(bool enable);
    bool hasImageSelected() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_pushButton_download_clicked();
    void on_pushButton_stop_clicked();
    void on_checkbox_set_boot_mode_to_meta_clicked();
    void slot_comboBox_Scene_currentIndexChanged(int index);
    //void slot_OnHeaderView_click(int column);
    void slot_enable_checked(bool checked);
    void slot_table_doubleClicked(const QModelIndex & index);
    void slot_table_clicked(const QModelIndex &index);
    void slot_all_items_selected(bool all_selected);
    void slot_after_rpmb_cmd(int *p_result, const std::shared_ptr<Connection> conn,
        const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_start_addr_action_toggled(bool checked);
    void slot_show_context_menu(const QPoint &pos);
    void slot_flash_xml_changed(const DownloadPartitionInfoList &download_partition_list);
    void slot_flash_xml_changed(bool is_nand_operation);
    void slot_after_get_DL_img_feedback_cmd(int *p_result, const std::shared_ptr<Connection> conn,
        const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);

private:
    void setUIState();
    void initTableWidget();
    void initDownloadSceneByConfig();
    void showRSCItem(bool show);
    void showBootModeInfo(bool show);
    void clear_RSC_Info();
    void init_RSC_list(const QStringList &rsc_proj_name_list);
    void connectSignalsAndSlots();
    QStringList getTableHeaderLabels() const;
    void resetTableWidget(const DownloadPartitionInfoList &download_partition_list);
    void resetDownloadSceneInfo();
    void resetRSCInfo();
    void replaceImageFile(const QModelIndex &index);
    void romEnabledChanged();
    void changeDownloadSceneToDefault();
    void updateScene();
    void updateImageList(bool checked_all);
    void process_rsc(const QString &rsc_file);
    bool validateBeforeDownload();
    std::pair<bool, bool> checkPGPTImage() const;
    void setupDownloadCmdList();
    CmdWrapperSPList createDownloadCmdWrapperList();
    std::list<std::shared_ptr<CmdWrapper>> createDLOnlyCmdWrapperList();
    std::list<std::shared_ptr<CmdWrapper>> createFmtDLCmdWrapperList();
    std::list<std::shared_ptr<CmdWrapper>> createFWCmdWrapperList();
    std::shared_ptr<CmdWrapper> createDLOnlyCmdWrapper(const DownloadPartitionInfoList &dl_partition_list);
    std::shared_ptr<CmdWrapper> createFmtDLCmdWrapper();
    std::shared_ptr<CmdWrapper> createFWCmdWrapper();
    std::shared_ptr<CmdWrapper> createRPMBCmdWrapper();
    std::shared_ptr<CmdWrapper> createMetaBootModeCmdWrapper() const;
    std::shared_ptr<CmdWrapper> createRSCCmdWrapper() const;
    std::shared_ptr<CmdWrapper> createDLImageFeedbackCmdWrapper() const;
    bool isRSCEnabled() const;
    BOOT_MODE_CONNECT_TYPE getBootModeConnType() const;
    void createContextMenu();
    void dragEnterEventHandler(QDragEnterEvent *event);
    void dropEventHandler(QDropEvent *event);
    QString getLoadChksumFile() const;
    DownloadPartitionInfoList filterNeedDLPartitionList() const;

private:
    Ui::DownloadWidget *ui;
    MainWindow *m_mainwindow;
    DOWNLOAD_SCENE m_download_scene;
    QMenu *m_context_menu;
    QAction *m_start_length_act;
};

#endif // DOWNLOADWIDGET_H
