#ifndef CONFIG_H
#define CONFIG_H

#include <memory>

#include "GeneralSetting.h"
#include "CommandSetting.h"
#include "CommandLineArguments.h"

namespace ConsoleMode
{

class Config
{
public:
    Config();
    Config(const std::shared_ptr<CommandLineArguments> &cmd_line_args);
    virtual ~Config();

    void loadFile(const QString &xml_file_name, bool reboot_to_atm);
    void saveFile(const QString &xml_file_name) const;

    inline std::shared_ptr<GeneralSetting> getGeneralSetting() const { return m_general_setting; }
    inline std::shared_ptr<CommandSetting> getCommandSetting() const { return m_command_setting; }

    void setConnSetting(std::shared_ptr<ConnectionArgs> conn_args);
    void setRuntimeParameterSetting(std::shared_ptr<SetRuntimeParaCmdXML> runtime_parameter);
    void setLogInfoSetting(std::shared_ptr<LogInfoSetting> log_info);

    void addCmdWrapper(cmd_wrapper_ptr cmd_wrapper);
    void addCmdWrapper(CmdWrapperSPList cmd_wrapper_list);

private:
    std::shared_ptr<GeneralSetting> m_general_setting;
    std::shared_ptr<CommandSetting> m_command_setting;
};

}

#endif // CONFIG_H
