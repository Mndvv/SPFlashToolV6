#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QAbstractNativeEventFilter>
#include <QAbstractButton>

#include "Cmd/CallbackTypes.h"
#include "Inc/CFlashToolTypes.h"
#include "Inc/error_code.h"
#include "OkDialog.h"
#include "../../ConsoleMode/Config.h"
#include "../../Utility/PartitionInfo.h"
#include "AsyncFlashXMLLoader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class FormatWidget;
class DownloadWidget;
class ReadBackWidget;
class MemoryTestWidget;
class BromAdapterWidget;
class WriteMemoryWidget;
class WelcomeWidget;
class DLCertWidget;
class FFUWidget;
class BackgroundWorker;
//class ICmdXMLGenerator;
class CmdWrapper;
class MainWindowCallBack;
class OptionDialog;
class AboutDialog;
class ConnectionArgs;
class SetRuntimeParaCmdXML;
#ifdef _WIN32
class ToolUpdateMgr;
#endif
class ProcessingDialog;
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public QAbstractNativeEventFilter
{
    Q_OBJECT

    using GetConnStageFunc = std::function<CONN_END_STAGE ()>;
    using GetCmdWrapperListFunc = std::function<CmdWrapperSPList ()>;

signals:
    void signal_language_change(int index);
    void signal_reset_common_jobs();
    void signal_flash_xml_changed(const DownloadPartitionInfoList &download_partition_list);
    void signal_flash_xml_changed(bool is_nand_operation);

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    // functions related to command execution begin
    void setupBeforeCmdOperation(CONN_END_STAGE conn_end_strage, bool initial_dram = true);
    void addCmdWrapper(const std::shared_ptr<CmdWrapper> cmd_wrapper);
    void addCmdWrapper(const CmdWrapperSPList cmd_wrapper_list);
    void addInteractionCmdWrapperList(const CmdWrapperSPList cmd_wrapper_list);
    void setNeedInteraction(bool need_interaction);
    cmd_wrapper_ptr createWDTCmdWrapper() const;
    void startCmd();
    void startCmd(const QString &success_title);
    void stopCmd(bool is_stopped_by_user = true);
    // functions related to command execution begin

    OkDialog *GetOkDialog(){return m_ok_dialog;}

    //callback
    bool cb_notify_stop(void *_this);

    void operation_progress(void *_this, unsigned int progress,
                               unsigned long long data_xferd, const QString &info,
                            const QString &backColor, const QString &progress_text,
                            bool update_speed_info);

    void LockOnUI(); //called in UI thread
    void unLockUI();
    void ResetStatus();

    void LoadLastFlashXMLFile();
    void LoadLastAuthFile();
    void LoadLastCertFile();
    void LoadLastDAFile();

    void ChangeLanguage(int index);

    HW_StorageType_E getStorageType() const;
    QString getChipName() const;
    bool isFlashXMLLoaded() const;
    long long int get_ext_ram_size() const;
    QString getFlashXMLFile() const;
    QAbstractButton** getConnectButton();
    bool checkRPMBEnable() const;
    bool advancedModeOn() const;
    FormatWidget *getFormatWidget() const;
    QString getScatterFileName() const;
    bool isSkipPTOperate() const;
    ProcessingDialog *processing_dialog() const{ return m_processing_dialog; }
    void logSwitch();
    void refreshTabText(const QWidget * const tab_widget, const QString &tab_text);
    void EnableLog(bool enable) const;
    void detectNetworkConnected();
    void updateFlashXMLFile(const QString &file_name);
    bool isUSBChksumEnabled() const;

public:
    virtual bool nativeEventFilter(const QByteArray & eventType, void * message, long * result) override;

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void closeEvent(QCloseEvent * event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    // UI slots
    void on_pushButton_flash_xml_clicked();
    void on_pushButton_CertFile_clicked();
    void on_pushButton_authFile_clicked();
    void on_pushButton_daFile_clicked();
    void on_comboBox_flash_xml_file_activated(const QString &arg1);
    void on_comboBox_authFilePath_activated(int index);
    void on_comboBox_certFilePath_activated(int index);
    void on_comboBox_daFilePath_activated(int index);
    void slot_setMenuItemStatus();
    void on_actionUSB_UART_options_triggered();
    void on_actionWrite_Memory_triggered();
    void on_actionField_Firmware_Update_triggered();
    //callback for operation, like format/download/readback
    void slot_current_operation_ok();
    void slot_current_operation_finished();
    void slot_operation_failed(int error_code, QString error_msg);
    //callback for connect brom and da
    void slot_conn_brom_operation_progress(void *_this, unsigned int progress,
                                         unsigned long long data_xferd, const QString &info);
    void slot_conn_da_operation_progress(void *_this, unsigned int progress,
                                         unsigned long long data_xferd, const QString &info);
    void slot_scatter_chksum_operation_process(void *_this, unsigned int progress,
                                         unsigned long long data_xferd, const QString &info);
    void slot_com_port_status(const std::string &symbolic_port_name);
    void slot_da_connected(const QString &da_conn_xml);
    void slot_get_device_tracking(int *p_out);
    //shortcut slots
    void toggleEnableAdvanceMode();
    void toggleBromAdapterWidget();
    void toggleLogging();
    void toggleDLCertWidget();

    //menu slots
    void on_actionSecurity_Mode_triggered();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void on_actionExportFormat_triggered();
    void on_actionExportDownload_triggered();
    void on_actionExportReadback_triggered();
    void on_actionExportWriteMemory_triggered();
    void on_actionOpen_Logs_Folder_triggered();
    void on_actionShow_Welcome_triggered();
    void on_action_SoftwareUpdate_triggered();
    void on_actionNetwork_Test_triggered();

    //other slots
    void slot_update_status_port_info();
    void slot_language_changed(int index);
    void slot_UpdateTimeLabel();
    void slot_after_check_storage_life_cycle_cmd(int *p_result, const std::shared_ptr<Connection> conn,
        const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void slot_current_tab_page_changed(int index);
    void slot_user_interaction_mode();
    void slot_enable_auto_polling_clicked(bool checked = false);
    void slot_parse_flash_xml_successed(FlashXMLDataLoader flash_xml_data_loader);
    void slot_parse_flash_xml_failed(const QString &error_msg, const QString &flash_xml_file);
    void slot_parse_flash_xml_cancelled();
    void slot_UserCancelLoadFlashXml();
    void slot_start_timer();
    void slot_show_network_connected();

private:
    //functions related to command operation begin
    std::shared_ptr<ConnectionArgs> createConnSetting(CONN_END_STAGE conn_end_strage) const;
    std::shared_ptr<SetRuntimeParaCmdXML> createRuntimeParamterSetting(bool initial_dram) const;
    std::shared_ptr<CmdWrapper> createCheckStorageLifeCycleCmdWrapper() const;
    //functions related to command operation end
    void CreateTabWidgets();
    void connectSignals();
    void initMembers();
    void initShortcuts();
    bool operation_support_auto_polling() const;
    void setUIInitState();
    void refreshMenuItemsEnabled();
    void refreshkMainUIEnabled();
    void loadFlashXMLFile(const QString &flash_xml_file_name);
    bool LoadAuthFile(const QString& file_name);
    bool LoadCertFile(const QString& file_name);
    bool LoadDAFile(const QString& file_name);
    bool isNANDStorage() const;
    void updateSpeedInfo(unsigned long long bytes);
    void setCertUIVisible(bool visible);
    void clearFlashXMLInfo();
    void exportConfigXMLFile(const QString &default_file,
                             const GetCmdWrapperListFunc get_cmd_wrapper_list,
                             const GetConnStageFunc get_conn_stage_func, bool initial_dram = true) const;
    void buildConsoleModeGeneral(ConsoleMode::Config *p_config,
                                 CONN_END_STAGE conn_stage, bool initial_dram) const;
    bool needShowErrDlgByErrCode(int error_code) const;
    void parseInfoFromDAHwInfo(const QString &da_conn_xml);
    void updateWriteMemoryTab();
    void updateFFUTab();
    void saveReadBackSetting();
    void UpdateWindowTitle(bool enableTrace = false);

private:
    Ui::MainWindow *ui;

    FormatWidget *m_format_widget;
    DownloadWidget *m_download_widget;
    ReadBackWidget *m_readback_widget;
    MemoryTestWidget *m_memorytest_widget;
    BromAdapterWidget *m_bromadapter_widget;
    WriteMemoryWidget *m_writememory_widget;
    WelcomeWidget *m_welcome_widget;
    DLCertWidget *m_dl_cert_widget;
    FFUWidget *m_ffu_widget;

    OkDialog *m_ok_dialog;
    OptionDialog    *m_option_dialog;
    AboutDialog *m_about_dialog;
    ProcessingDialog *m_processing_dialog;

    std::shared_ptr<BackgroundWorker> m_backgroundworker;
    std::shared_ptr<MainWindowCallBack> m_mainwindow_callback;
    bool m_stop_flag;
    bool m_lockui_on;
    bool m_advance_on;
    bool m_Log_on;
    bool m_security_on;
    bool m_bromAdapter_on;
    bool m_show_welcome;
    bool m_dl_cert_on;
    int m_current_auto_polling_count;
    HW_StorageType_E m_storage_type;
    QTime m_speed_clock;
    QTime m_start_clock;
    QTimer *m_time_label_timer;
    QString m_scatter_file_name;
    bool m_skip_pt_operate;

    QStringList m_flash_xml_file_history_list;
    QStringList m_auth_file_history_list;
    QStringList m_cert_file_history_list;
    QStringList m_da_file_history_list;

    long long int m_ext_ram_size;

    QAbstractButton* m_pconnectButton;
#ifdef _WIN32
    ToolUpdateMgr *m_tool_update;
#endif

    //CB_OPERATION_PROGRESS_FUNC m_cb_operation_progress;
};
#endif // MAINWINDOW_H
