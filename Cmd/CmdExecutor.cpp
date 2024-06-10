#include "CmdExecutor.h"
#include "../XML/CmdGenerator/ICmdXMLGenerator.h"
#include "../Cmd/CmdWrapper.h"
#include "../Conn/Connection.h"
#include "../Inc/CFlashToolAPI.h"
#include "../Inc/error_code.h"
#include "../Logger/Log.h"
#include "../Err/Exception.h"
#include "../Err/CFlashToolErrorCode.h"
#include <cassert>

void CmdExecutor::execute(const std::shared_ptr<Connection> conn,
                              const std::shared_ptr<CmdWrapper> cmdWrapper,
                              const callback_transfer_t *callbacks)
{
    int cmd_result = CFT_SUCCESS;
    QString cmd_name = cmdWrapper->get_cmd_xml_generator()->getCmdName();
    cmd_result = cmdWrapper->cb_before_cmd(conn);
    if (cmd_result == CFT_SUCCESS) {
        LOGI("callback before %s command execute successed.", qPrintable(cmd_name));
    } else if (cmd_result == CFT_CMD_NOT_SUPPORT) {
        LOG("callback before %s command ignored.", qPrintable(cmd_name));
    } else {
        LOGE("callback before %s command execute failed.", qPrintable(cmd_name));
        THROW_APP_EXCEPTION(cmd_result, QStringLiteral(""))
    }

    assert(cmdWrapper->get_cmd_xml_generator());
    QString cmd_xml = cmdWrapper->get_cmd_xml_generator()->toCmdString();
    LOG("current command name: %s,\tcommand XML:\n%s", qPrintable(cmd_name), cmd_xml.toStdString().c_str());
    HRESULT hresult = CFlashToolAPI::cflashtool_execute_command(conn->get_hsession(), cmd_xml, callbacks);
    if (FAIL(hresult)) {
        QString error_msg = CFlashToolAPI::cflashtool_get_last_error_msg(conn->get_hsession());
        LOGE("%s command execute failed, return error code: %d, error msg: %s.",
            qPrintable(cmd_name), static_cast<int>(hresult), qPrintable(error_msg));
        THROW_APP_EXCEPTION(static_cast<int>(hresult), error_msg)
    }
    LOGI("%s command execute successed.", qPrintable(cmd_name));

    cmd_result = cmdWrapper->cb_after_cmd(conn);
    if (cmd_result == CFT_SUCCESS) {
        LOGI("callback after %s command execute successed.", qPrintable(cmd_name));
    } else if (cmd_result == CFT_CMD_NOT_SUPPORT) {
        LOG("callback after %s command ignored.", qPrintable(cmd_name));
    } else {
        LOGE("callback after %s command execute failed.", qPrintable(cmd_name));
        THROW_APP_EXCEPTION(cmd_result, QStringLiteral(""))
    }
}
