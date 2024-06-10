#include "Config.h"

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include "../Logger/Log.h"
#include <cassert>

namespace ConsoleMode {

static const QString CONFIG_XML_ROOT_NODE_NAME = "cflashtool-config";
static const QString CONFIG_XML_ROOT_NODE_ATTR_NAME = "version";
static const QString CONFIG_XML_ROOT_NODE_ATTR_VAL = "1.0";

Config::Config():
    m_general_setting(new GeneralSetting()),
    m_command_setting(new CommandSetting())
{

}

Config::Config(const std::shared_ptr<CommandLineArguments> &cmd_line_args)
{
    assert(cmd_line_args->getCurrentMode() != CommandLineArguments::ConsoleModeCurrentMode::UnknownMode);
    if (cmd_line_args->getCurrentMode() == CommandLineArguments::ConsoleModeCurrentMode::CmdArgumentMode)
    {
        LOGI("Init config from input arguments");
        m_general_setting = std::make_shared<GeneralSetting>(cmd_line_args);
        m_command_setting = std::make_shared<CommandSetting>(cmd_line_args);
    }
    else
    {
        LOGI("Init config from config file");
        loadFile(cmd_line_args->getConfigXMLFileName(), cmd_line_args->reboot_to_atm());
        QString com_port = cmd_line_args->getComPortName();
        if (!com_port.isEmpty()) {
            m_general_setting->getConnArgs()->set_com_port_name(com_port);
        }
    }
}

Config::~Config()
{

}

void Config::loadFile(const QString &xml_file_name, bool reboot_to_atm)
{
    if (xml_file_name.isEmpty()) {
        LOGE("load config xml file failed: file name is empty.");
        return ;
    }

    QFile file(xml_file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOGE("open config xml file failed: %s.", xml_file_name.toStdString().c_str());
        return ;
    }

    QDomDocument xml_dom_doc;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xml_dom_doc.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
        LOGE("load config xml file failed: error msg: %s, error line: %d, error column: %d",
            errorStr.toStdString().c_str(), errorLine, errorColumn);
        return ;
    }

    QDomElement root_node = xml_dom_doc.documentElement();
    assert(root_node.tagName() == CONFIG_XML_ROOT_NODE_NAME);
    assert(root_node.attribute(CONFIG_XML_ROOT_NODE_ATTR_NAME) == CONFIG_XML_ROOT_NODE_ATTR_VAL);

    if (!m_general_setting) {
        m_general_setting = std::make_shared<GeneralSetting>();
    }
    m_general_setting->loadXML(root_node);

    if (!m_command_setting) {
        m_command_setting = std::make_shared<CommandSetting>(reboot_to_atm, m_general_setting->getFlashXmlFile());
    }
    m_command_setting->loadXML(root_node);
}

void Config::saveFile(const QString &xml_file_name) const
{
    assert(m_general_setting);
    assert(m_command_setting);

    if (xml_file_name.isEmpty()) {
        LOGE("save config xml file failed: file name is empty.");
        return ;
    }
    QFile file(xml_file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        LOGE("save config xml file failed: %s open failed.", xml_file_name.toStdString().c_str());
        return ;
    }

    QTextStream text_stream(&file);

    QDomDocument xml_dom_doc;
    QString strHeader( "version=\"1.0\" encoding=\"utf-8\"" );
    xml_dom_doc.appendChild(xml_dom_doc.createProcessingInstruction("xml", strHeader) );
    QDomElement root_node = xml_dom_doc.createElement(CONFIG_XML_ROOT_NODE_NAME);
    root_node.setAttribute(CONFIG_XML_ROOT_NODE_ATTR_NAME, CONFIG_XML_ROOT_NODE_ATTR_VAL);
    xml_dom_doc.appendChild(root_node);

    m_general_setting->saveXML(xml_dom_doc, root_node);
    m_command_setting->saveXML(xml_dom_doc, root_node);

    xml_dom_doc.save(text_stream, 4);
}

void Config::setConnSetting(std::shared_ptr<ConnectionArgs> conn_args)
{
    m_general_setting->setConnArgs(conn_args);
}

void Config::setRuntimeParameterSetting(std::shared_ptr<SetRuntimeParaCmdXML> runtime_parameter)
{
    m_general_setting->setRuntimeParameterSetting(runtime_parameter);
}

void Config::setLogInfoSetting(std::shared_ptr<LogInfoSetting> log_info)
{
    m_general_setting->setLogInfoSetting(log_info);
}

void Config::addCmdWrapper(cmd_wrapper_ptr cmd_wrapper)
{
    m_command_setting->addCmdWrapper(cmd_wrapper);
}

void Config::addCmdWrapper(CmdWrapperSPList cmd_wrapper_list)
{
    m_command_setting->addCmdWrapper(cmd_wrapper_list);
}

}
