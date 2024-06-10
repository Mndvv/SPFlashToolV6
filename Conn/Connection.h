#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include "../Inc/CFlashToolTypes.h"
#include "../Inc/Flash.Mode.Struct.h"

class ConnectionArgs;
class ICmdCallBack;
class CmdWrapper;
class BromGetHWInfoCmdXML;
class DAGetHWInfoCmdXML;
class SetRuntimeParaCmdXML;

#define COM_PORT_NAME_MAX_LENGTH 512

class Connection: public std::enable_shared_from_this<Connection>
{
public:
    Connection(const std::shared_ptr<ConnectionArgs> conn_args,
               const std::shared_ptr<ICmdCallBack> mainwindow_callback,
               const std::shared_ptr<SetRuntimeParaCmdXML> p_runtime_parameter);
    virtual ~Connection();

    void connect();

    inline HSESSION get_hsession() const { return m_hsession; }

private:
#ifdef _WIN32
    void printDriverVersion() const;
#endif
    void scan_com_port();
    void com_port_status_callback();
    void conn_brom();
    ZHRESULT conn_da();

private:
    std::shared_ptr<ConnectionArgs> m_conn_args;
    std::shared_ptr<ICmdCallBack> m_cmd_callback;
    std::shared_ptr<CmdWrapper> m_conn_da_cmd_wrapper;
    std::shared_ptr<SetRuntimeParaCmdXML> m_runtime_paramter;
    HSESSION m_hsession;

    char m_symbolic_link[COM_PORT_NAME_MAX_LENGTH] = {0};
};

#endif // CONNECTION_H
