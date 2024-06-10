#include "BromConnSettingWidget.h"
#include "ui_BromConnSettingWidget.h"
#include "../../Com/AsyncComPortInfo.h"
#include "../../Logger/Log.h"
#include "../../Utility/version.h"
#include "../../Utility/OptionIniFileMgr.h"
#include "../../XML/CmdGenerator/SetRuntimeParaCmdXML.h"

#define AUTO_REBOOT_GROUPBOX_UNCHECKED_STYLE_SHEET QStringLiteral("" \
"QGroupBox {\n    " \
"border-color: rgb(156, 156, 156);\n    " \
"border-width: 0px;\n    " \
"border-style: solid;\n    " \
"margin-top: 0.5ex;\n}")

BromConnSettingWidget::BromConnSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BromConnSettingWidget)
{
    ui->setupUi(this);

    setUIState();

    createConnections();
}

BromConnSettingWidget::~BromConnSettingWidget()
{
    saveSettings();
    delete ui;
}

REBOOT_TYPE BromConnSettingWidget::getRebootType() const
{
    if (ui->groupBox_autoReboot->isChecked()) {
        if (ui->radioButton_cold_reset->isEnabled() && ui->radioButton_cold_reset->isChecked()) {
            return REBOOT_TYPE::PMIC_COLD_RESET;
        } else {
            return REBOOT_TYPE::IMMEDIATE;
        }
    } else {
        return REBOOT_TYPE::DISCONNECT;
    }
}

QString BromConnSettingWidget::getBaudrate() const
{
    QString s_baud_rate;
    if (ui->radioButton_uart->isChecked())
    {
        s_baud_rate = ui->comboBox_baud_rate->currentText().trimmed();
    }
    return s_baud_rate;
}

QString BromConnSettingWidget::getComPortName() const
{
    QString s_com_port_name;
    if (ui->radioButton_uart->isChecked())
    {
        s_com_port_name = ui->comboBox_com_port->currentText().trimmed();
    }
    return s_com_port_name;
}

COM_PORT_TYPE BromConnSettingWidget::getComPortType() const
{
    if (ui->radioButton_uart->isChecked()) {
        return COM_PORT_TYPE::COM_PORT_UART;
    } else {
        return COM_PORT_TYPE::COM_PORT_USB_PCIE;
    }
}

USB_POWER BromConnSettingWidget::getUSBPower() const
{
    if (ui->radioButton_with_battery->isChecked()) {
        return USB_POWER::WithBattery;
    } else if (ui->radioButton_without_battery->isChecked()) {
        return USB_POWER::WithoutBattery;
    } else {
        return USB_POWER::AutoDetect;
    }
}

void BromConnSettingWidget::updateAdvancedMode(bool advanced_mode)
{
    ui->groupBox_com_port->setVisible(advanced_mode);
    if (!advanced_mode) {
        ui->radioButton_usb_pcie->click();
        showUARTInfo(false);
    }
    this->showRebootOptions(advanced_mode && !ToolInfo::IsCustomerVer());
}

void BromConnSettingWidget::showUARTInfo(bool show)
{
    ui->groupBox_uart->setVisible(show);
}

void BromConnSettingWidget::showRebootOptions(bool show)
{
    ui->radioButton_warm_reset->setVisible(show);
    ui->radioButton_cold_reset->setVisible(show);
    if (!show) {
        ui->groupBox_autoReboot->setStyleSheet(AUTO_REBOOT_GROUPBOX_UNCHECKED_STYLE_SHEET);
    } else {
        ui->groupBox_autoReboot->setStyleSheet(QStringLiteral(""));
    }
}

void BromConnSettingWidget::setUIState()
{
    ui->groupBox_autoReboot->setChecked(!ToolInfo::IsCustomerVer());
    updateAdvancedMode(false);
    showUARTInfo(false);

    loadSettings();
}

void BromConnSettingWidget::loadSettings()
{
    int index = OptionIniFileMgr::getInstance()->getBaud_rate();
    if(index >= 0 && index < ui->comboBox_baud_rate->count()) {
        ui->comboBox_baud_rate->setCurrentIndex(index);
    }

    int usb_power_index = OptionIniFileMgr::getInstance()->getBattery();
    USB_POWER usb_power = USB_POWER::AutoDetect;
    if (usb_power_index >= static_cast<int>(USB_POWER::AutoDetect) &&
            usb_power_index <= static_cast<int>(USB_POWER::WithoutBattery)) {
        usb_power = static_cast<USB_POWER>(usb_power_index);
    }
    if (usb_power == USB_POWER::WithBattery) {
        ui->radioButton_with_battery->setChecked(true);
    } else if (usb_power == USB_POWER::WithoutBattery) {
        ui->radioButton_without_battery->setChecked(true);
    } else {
        ui->radioButton_auto_detect->setChecked(true);
    }
}

void BromConnSettingWidget::saveSettings()
{
    OptionIniFileMgr::getInstance()->setBaud_rate(ui->comboBox_baud_rate->currentIndex());

    USB_POWER usb_power = this->getUSBPower();
    int usb_power_index = static_cast<int>(usb_power);
    OptionIniFileMgr::getInstance()->setBattery(usb_power_index);
}

void BromConnSettingWidget::createConnections()
{
    connect(ui->radioButton_usb_pcie, &QRadioButton::clicked, this, &BromConnSettingWidget::slot_com_port_changed);
    connect(ui->radioButton_uart, &QRadioButton::clicked, this, &BromConnSettingWidget::slot_com_port_changed);
    connect(ui->radioButton_with_battery, &QRadioButton::clicked, this, &BromConnSettingWidget::signal_update_status_port_info);
    connect(ui->radioButton_without_battery, &QRadioButton::clicked, this, &BromConnSettingWidget::signal_update_status_port_info);
    connect(ui->radioButton_auto_detect, &QRadioButton::clicked, this, &BromConnSettingWidget::signal_update_status_port_info);
    connect(ui->comboBox_com_port, &QComboBox::currentTextChanged, this, &BromConnSettingWidget::signal_update_status_port_info);
    connect(ui->comboBox_baud_rate, &QComboBox::currentTextChanged, this, &BromConnSettingWidget::signal_update_status_port_info);
}

void BromConnSettingWidget::refreshCOMPortInfo()
{
    AsyncComPortInfo *get_com_info_task = new AsyncComPortInfo();
    qRegisterMetaType<QList<QString>>("QList<QString>");
    connect(get_com_info_task, &AsyncComPortInfo::signal_send_com_port_infos,
            this, &BromConnSettingWidget::slot_get_com_ports);
    connect(get_com_info_task, SIGNAL(finished()), get_com_info_task, SLOT(deleteLater()));
    get_com_info_task->start();
}

void BromConnSettingWidget::fillRuntimeParameterSetting(std::shared_ptr<SetRuntimeParaCmdXML> &runtime_paramter_setting) const
{
    runtime_paramter_setting->setBattery_exist(this->getUSBPower());
}

void BromConnSettingWidget::UpdateUI()
{
    ui->retranslateUi(this);
}

QString BromConnSettingWidget::getDescription() const
{
    QString info;
    if(ui->radioButton_uart->isChecked()) {
        info.append("UART: ").append(ui->comboBox_com_port->currentText())
                .append(", ").append(ui->comboBox_baud_rate->currentText());
    } else {
        info.append("USB: ");

        info.append("DA Download All(");

        if(ui->radioButton_with_battery->isChecked())
        {
            info.append("with bat)");
        }
        else if(ui->radioButton_without_battery->isChecked())
        {
            info.append("without bat)");
        }
        else if(ui->radioButton_auto_detect->isChecked())
        {
            info.append("auto detect)");
        }
    }
    return info;
}

bool BromConnSettingWidget::isUARTInfoVisible() const
{
    return ui->groupBox_uart->isVisible();
}

void BromConnSettingWidget::lockUI()
{
    this->setEnabled(false);
}

void BromConnSettingWidget::unLockUI()
{
    this->setEnabled(true);
}

void BromConnSettingWidget::slot_com_port_changed()
{
    bool is_uart_selected = false;
    QRadioButton *sender_btn = qobject_cast<QRadioButton *>(sender());
    if (sender_btn == ui->radioButton_uart) {
        this->refreshCOMPortInfo();
        this->showUARTInfo(true);
        is_uart_selected = true;
    } else {
        this->showUARTInfo(false);
    }
    emit signal_update_status_port_info();
}

void BromConnSettingWidget::slot_get_com_ports(const QList<QString> &com_ports)
{
    QString cur_com_port = ui->comboBox_com_port->currentText();
    ui->comboBox_com_port->clear();
    foreach (auto item, com_ports) {
        QString com_port = item;
#ifdef _LINUX64
        com_port = QString("/dev/%1").arg(com_port);
#endif
        ui->comboBox_com_port->addItem(com_port);
    }
    int cur_index = ui->comboBox_com_port->findText(cur_com_port);
    ui->comboBox_com_port->setCurrentIndex(cur_index == -1 ? 0 : cur_index);
}

void BromConnSettingWidget::on_groupBox_autoReboot_clicked(bool checked)
{
    ui->radioButton_warm_reset->setEnabled(checked);
    ui->radioButton_cold_reset->setEnabled(checked);
}
