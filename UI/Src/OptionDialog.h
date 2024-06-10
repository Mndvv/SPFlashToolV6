#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <memory>

#include "../../Inc/CFlashToolTypes.h"

namespace Ui {
class OptionDialog;
}

class MainWindow;
class SetRuntimeParaCmdXML;
class LogInfoSetting;

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(MainWindow *mainwindow, QWidget *parent = nullptr);
    ~OptionDialog();

    std::shared_ptr<LogInfoSetting> createLogInfoSetting();
    void ShowCheckRPMBItem(bool show);
    void UpdateUI();
    void setLanguageIndex(int index);

    void fillRuntimeParameterSetting(
            std::shared_ptr<SetRuntimeParaCmdXML> &runtime_paramter_setting) const;
    bool isAutoPollingEnabled() const;
    int getAutoPollingUpperLimit() const;
    void updateToolLogStatus(bool checked);
    bool isUSBChksumEnabled() const;

signals:
    void signal_language_change();
    void signal_auto_polling_clicked(bool checked = false);

private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_comboBox_Language_activated(int index);
    void on_checkBox_ShowWelcome_clicked();
    void on_checkBox_check_update_clicked();
    void on_groupBox_enable_tool_log_clicked(bool checked = false);
    void on_checkBox_rpmb_clicked();
    void on_btn_logPath_clicked();
    void on_checkBox_StorageLifeCycle_clicked();
    void slot_checkbox_chksum_toggled();
    void slot_flash_xml_changed(bool is_nand_operation);

private:
    void InitSettings();
    void SaveSettings() const;
    void LoadGeneralSettings();
    void SaveGeneralSettings() const;
    void clearLogFiles();
    void InitUpdateOptions();
    void createConnections();
    ChkSumLevels getChkSumLevel() const;

private:
    Ui::OptionDialog *ui;
    MainWindow *m_mainwindow;
};

#endif // OPTIONDIALOG_H
