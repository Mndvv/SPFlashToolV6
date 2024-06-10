#include "GeneralSetting.h"
#include "../Err/Exception.h"
#include "../XML/Parser/FlashXMLParser.h"
#include "../Utility/FileUtils.h"
#include "../Logger/Log.h"
#include "../Inc/error_code.h"
#include "../XML/XMLConst.h"
#include <cassert>
#include <QSet>

namespace ConsoleMode
{

GeneralSetting::GeneralSetting():
    m_conn_args(nullptr),
    m_runtime_parameter_setting(nullptr),
    m_log_info_setting(nullptr)
{

}

GeneralSetting::GeneralSetting(const std::shared_ptr<CommandLineArguments> &args):
    m_conn_args(new ConnectionArgs()),
    m_runtime_parameter_setting(new SetRuntimeParaCmdXML()),
    m_log_info_setting(new LogInfoSetting())
{
    initConnArgsByCmdOptions(args);
    initRuntimeParameterByCmdOptions(args);
}

void GeneralSetting::loadXML(const QDomElement &xml_node)
{
    QDomElement general_node = xml_node.firstChildElement("general");
    if (general_node.isNull()) {
        THROW_APP_EXCEPTION(E_ERROR, QStringLiteral("general node not exists."))
    }

    if (!m_conn_args) {
        m_conn_args = std::make_shared<ConnectionArgs>();
    }
    m_conn_args->loadXML(general_node);
    if (!m_runtime_parameter_setting) {
        m_runtime_parameter_setting = std::make_shared<SetRuntimeParaCmdXML>();
    }
    m_runtime_parameter_setting->loadXML(general_node);
    if (!m_log_info_setting) {
        m_log_info_setting = std::make_shared<LogInfoSetting>();
    }
    m_log_info_setting->loadXML(general_node);
}

void GeneralSetting::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    assert(m_conn_args);
    assert(m_runtime_parameter_setting);
    assert(m_log_info_setting);

    QDomElement general_node = xml_doc.createElement("general");
    xml_node.appendChild(general_node);

    m_conn_args->saveXML(xml_doc, general_node);
    m_runtime_parameter_setting->saveXML(xml_doc, general_node);
    m_log_info_setting->saveXML(xml_doc, general_node);
}

void GeneralSetting::initConnArgsByCmdOptions(const std::shared_ptr<CommandLineArguments> &args)
{
    assert(args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode);
    m_conn_args->set_flash_xml_file(args->getFlashXMLFilename());
    m_conn_args->set_auth_file(args->getAuthFilename());
    m_conn_args->set_cert_file(args->getCertFilename());
    m_conn_args->set_com_port_type(COM_PORT_TYPE::COM_PORT_USB_PCIE);
    m_conn_args->set_com_port_name(args->getComPortName());
    QString cmd_name = args->getCommandName();
    m_conn_args->set_conn_end_stage(this->getConnStageByCommand(cmd_name));
}

void GeneralSetting::initRuntimeParameterByCmdOptions(const std::shared_ptr<CommandLineArguments> &args)
{
    assert(args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode);
    m_runtime_parameter_setting->setBattery_exist(getUSBPowerByInputArg(args->getBatteryOption()));
    m_runtime_parameter_setting->setDa_log_channel(args->getDALogChannel());
    m_runtime_parameter_setting->setInitial_dram(needInitDramByCommand(args->getCommandName()));
}

CONN_END_STAGE GeneralSetting::getConnStageByCommand(const QString &cmd_name)
{
    CONN_END_STAGE conn_stage = CONN_END_STAGE::SECOND_DA_STAGE;
    //expand this function if add new command needed connected to 1st DA
    if (cmd_name == DRAM_REPAIR_CMD_NAME) {
        conn_stage = CONN_END_STAGE::FIRST_DA_STAGE;
    }
    return conn_stage;
}

QString GeneralSetting::getFlashXmlFile() const
{
    return m_conn_args->get_flash_xml_file();
}

USB_POWER GeneralSetting::getUSBPowerByInputArg(const QString &battery_mode) const
{
    if (battery_mode == "with") {
        return USB_POWER::WithBattery;
    } else if (battery_mode == "without") {
        return USB_POWER::WithoutBattery;
    } else {
        return USB_POWER::AutoDetect;
    }
}

bool GeneralSetting::needInitDramByCommand(const QString &cmd_name) const
{
    static QSet<QString> not_init_dram_cmd_set = { DRAM_REPAIR_CMD_NAME };
    return !not_init_dram_cmd_set.contains(cmd_name);
}

}
