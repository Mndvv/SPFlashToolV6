#ifndef CMDEXECUTOR_H
#define CMDEXECUTOR_H
#include <memory>
#include <QString>

#include "../Inc/Flash.Mode.Struct.h"

class ICmdXMLGenerator;
class Connection;
class CmdWrapper;

class CmdExecutor
{
public:
    static void execute(const std::shared_ptr<Connection> conn,
                     const std::shared_ptr<CmdWrapper> cmdWrapper,
                 const struct callback_transfer_t *callbacks);
};

#endif // CMDEXECUTOR_H
