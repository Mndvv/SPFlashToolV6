#ifndef BROMCONNSETTINGWIDGET_H
#define BROMCONNSETTINGWIDGET_H

#include <QWidget>
#include <memory>

#include "../../Inc/CFlashToolTypes.h"

namespace Ui {
class BromConnSettingWidget;
}

class SetRuntimeParaCmdXML;

class BromConnSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BromConnSettingWidget(QWidget *parent = nullptr);
    ~BromConnSettingWidget();

    void lockUI();
    void unLockUI();

    REBOOT_TYPE getRebootType() const;
    QString getBaudrate() const;
    QString getComPortName() const;
    COM_PORT_TYPE getComPortType() const;
    USB_POWER getUSBPower() const;
    QString getDescription() const;
    bool isUARTInfoVisible() const;

    void updateAdvancedMode(bool advanced_mode);
    void refreshCOMPortInfo();
    //new
    void fillRuntimeParameterSetting(
            std::shared_ptr<SetRuntimeParaCmdXML> &runtime_paramter_setting) const;
    void UpdateUI();

signals:
    void signal_update_status_port_info();

private slots:
    void slot_com_port_changed();
    void slot_get_com_ports(const QList<QString> &com_ports);
    void on_groupBox_autoReboot_clicked(bool checked);

private:
    void setUIState();
    void loadSettings();
    void saveSettings();
    void createConnections();
    void showUARTInfo(bool show);
    void showRebootOptions(bool show);

private:
    Ui::BromConnSettingWidget *ui;
};

#endif // BROMCONNSETTINGWIDGET_H
