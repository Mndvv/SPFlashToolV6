#ifndef GENERALSETTING_H
#define GENERALSETTING_H

#include <memory>

#include "IConsoleModeXMLInterface.h"
#include "../Conn/ConnectionArgs.h"
#include "../XML/CmdGenerator/SetRuntimeParaCmdXML.h"
#include "../Utility/LogInfoSetting.h"
#include "CommandLineArguments.h"

namespace ConsoleMode {

class GeneralSetting: public IConsoleModeXMLInterface
{
public:
    GeneralSetting();
    GeneralSetting(const std::shared_ptr<CommandLineArguments> &args);

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    inline std::shared_ptr<ConnectionArgs> getConnArgs() const { return m_conn_args; }
    inline std::shared_ptr<SetRuntimeParaCmdXML> getRuntimeParameterSetting() const {
        return m_runtime_parameter_setting; }

    inline void setConnArgs(std::shared_ptr<ConnectionArgs> conn_args) { m_conn_args = conn_args; }
    inline void setRuntimeParameterSetting(std::shared_ptr<SetRuntimeParaCmdXML> runtime_parameter_setting) {
        m_runtime_parameter_setting = runtime_parameter_setting; }
    inline void setLogInfoSetting(std::shared_ptr<LogInfoSetting> log_info) { m_log_info_setting = log_info; }

    static CONN_END_STAGE getConnStageByCommand(const QString &cmd_name);
    QString getFlashXmlFile() const;

private:
    void initConnArgsByCmdOptions(const std::shared_ptr<CommandLineArguments> &args);
    void initRuntimeParameterByCmdOptions(const std::shared_ptr<CommandLineArguments> &args);
    USB_POWER getUSBPowerByInputArg(const QString &battery_mode) const;
    bool needInitDramByCommand(const QString &cmd_name) const;

private:
    std::shared_ptr<ConnectionArgs> m_conn_args;
    std::shared_ptr<SetRuntimeParaCmdXML> m_runtime_parameter_setting;
    std::shared_ptr<LogInfoSetting> m_log_info_setting;
};

}

#endif // GENERALSETTING_H
