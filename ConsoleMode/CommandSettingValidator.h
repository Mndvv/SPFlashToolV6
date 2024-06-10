#ifndef COMMANDSETTINGVALIDATOR_H
#define COMMANDSETTINGVALIDATOR_H

#include "Config.h"
#include "CommandLineArguments.h"

namespace ConsoleMode
{

class CommandSettingValidator
{
public:
    CommandSettingValidator(const Config &config, const std::shared_ptr<CommandLineArguments> &args);
    virtual ~CommandSettingValidator();

    bool validate() const;
    bool hasRSCXMLFile() const;

private:
    bool validateRSCSetting() const;

private:
    const Config &m_config;
    const std::shared_ptr<CommandLineArguments> m_args;
};

}

#endif // COMMANDSETTINGVALIDATOR_H

