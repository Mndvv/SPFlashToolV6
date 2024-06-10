#include "Connection.h"
#include <cassert>
#include <iostream>

#include "../Inc/CFlashToolAPI.h"
#include "../Inc/error_code.h"
#include "ConnectionArgs.h"
#include "../Cmd/ICmdCallBack.h"
#include "../Cmd/CmdExecutor.h"
#include "../Cmd/CmdWrapper.h"
#include "../Logger/Log.h"
#include "../XML/CmdGenerator/GetHWInfoCmdXML.h"
#include "../XML/CmdGenerator/SetRuntimeParaCmdXML.h"
#include "../Utility/OptionIniFileMgr.h"
#include "../Utility/Utils.h"
#include "../Utility/version.h"
#include "../Err/Exception.h"
#include "../Err/CFlashToolErrorCode.h"
#ifdef _WIN32
#include "../Utility/PrintDriverVersion.h"
#include <QThreadPool>
#endif

Connection::Connection(const std::shared_ptr<ConnectionArgs> conn_args,
                       const std::shared_ptr<ICmdCallBack> mainwindow_callback,
                       const std::shared_ptr<SetRuntimeParaCmdXML> p_runtime_parameter):
    m_conn_args(conn_args),
    m_cmd_callback(mainwindow_callback),
    m_runtime_paramter(p_runtime_parameter)
{
    m_hsession = CFlashToolAPI::cflashtool_create_session();

    //temprary place
    m_conn_da_cmd_wrapper = std::make_shared<CmdWrapper>(std::make_shared<DAGetHWInfoCmdXML>());
}

Connection::~Connection()
{
    CFlashToolAPI::cflashtool_destroy_session(m_hsession);
}

void Connection::connect()
{
    assert(m_conn_args);
    assert(m_cmd_callback);
    assert(CONN_END_STAGE::CONN_UNKNOWN != m_conn_args->get_conn_end_strage());
    assert(COM_PORT_TYPE::COM_PORT_UNKNOWN != m_conn_args->get_com_port_type());

    scan_com_port();
    LOGI("scan device succeed, symbolic_link: %s", m_symbolic_link);

    conn_brom();

    if (m_conn_args->get_conn_end_strage() > CONN_END_STAGE::BROM_STAGE) {
        conn_da();
    }
#ifdef _WIN32
    this->printDriverVersion();
#endif
}

void Connection::scan_com_port()
{
    LOGI("scan device START.");
    LOGI("com port clue: %s.", m_conn_args->get_com_port_clue().toUtf8().data());
    if (COM_PORT_TYPE::COM_PORT_UART == m_conn_args->get_com_port_type()) {
        LOGI("UART com port, skip scan port!");
#ifdef _WIN32
        strcpy_s(m_symbolic_link, COM_PORT_NAME_MAX_LENGTH, m_conn_args->get_com_port_clue().toUtf8().data());
#else
        strcpy(m_symbolic_link, m_conn_args->get_com_port_clue().toUtf8().data());
#endif
        return ;
    }
    callback_stop_t cb_stop;
    m_cmd_callback->setupStopCallbackStruct(&cb_stop);
    HRESULT hresult = CFlashToolAPI::cflashtool_scan_device(m_conn_args->get_com_port_clue(),
                                                 m_symbolic_link, COM_PORT_NAME_MAX_LENGTH,
                                                 m_conn_args->getTimeout_ms(), &cb_stop);
    if (FAIL(hresult)) {
        QString error_msg = CFlashToolAPI::cflashtool_get_last_error_msg(m_hsession);
        LOGE("scan device com port failed, error code: %d, error msg: %s.",
            static_cast<int>(hresult), qPrintable(error_msg));
        THROW_APP_EXCEPTION(static_cast<int>(hresult), error_msg)
    }
    com_port_status_callback();
}

void Connection::com_port_status_callback()
{
    CB_COM_PORT_STATUS_FUNC cb_comport_status_func = m_cmd_callback->getCBComportStatus();
    if (cb_comport_status_func) {
        cb_comport_status_func(m_symbolic_link);
    }
}

void Connection::conn_brom()
{
    callbacks_struct_t cbs;
    m_cmd_callback->setupBromCallbackStruct(&cbs);
    LOGI("connect brom START, connection options: %s,\tauth file: %s,\tcert file: %s.",
        m_conn_args->get_brom_conn_options().toStdString().c_str(),
        m_conn_args->get_auth_file().toStdString().c_str(),
        m_conn_args->get_cert_file().toStdString().c_str());
    ZHRESULT hresult = CFlashToolAPI::cflashtool_connect_device(m_hsession,
                                                m_symbolic_link,
                                                m_conn_args->get_brom_conn_options(),
                                                m_conn_args->get_auth_file(),
                                                m_conn_args->get_cert_file(),
                                                &cbs);
    if (FAIL(hresult)) {
        QString error_msg = CFlashToolAPI::cflashtool_get_last_error_msg(m_hsession);
        LOGE("connect brom failed, error code: %d, error msg: %s.",
            static_cast<int>(hresult), qPrintable(error_msg));
        THROW_APP_EXCEPTION(static_cast<int>(hresult), error_msg)
    }
    LOGI("connect brom successed.");
}

ZHRESULT Connection::conn_da()
{
    callbacks_struct_t cbs;
    m_cmd_callback->setupDACallbackStruct(&cbs);
    std::string flash_xml_path;
    ZHRESULT hresult = E_OK;

    QString cmd_xml = m_runtime_paramter->toCmdString();
    QString flash_mode = m_conn_args->get_flash_mode();
    QString flash_xml = m_conn_args->get_conn_flash_xml_file();
    LOGI("enter DA mode START, flash DA mode: %s,\tflash XML: %s,\truntime parameter XML:\n%s",
        flash_mode.toStdString().c_str(),
        flash_xml.toStdString().c_str(),
        cmd_xml.toStdString().c_str());
    hresult = CFlashToolAPI::cflashtool_enter_mode(m_hsession, flash_mode, flash_xml, cmd_xml, &cbs.trans);
    if (FAIL(hresult)) {
        QString error_msg = CFlashToolAPI::cflashtool_get_last_error_msg(m_hsession);
        LOGE("enter DA mode failed, error code: %d, error msg: %s.",
            static_cast<int>(hresult), qPrintable(error_msg));
        THROW_APP_EXCEPTION(static_cast<int>(hresult), error_msg)
    }
    LOGI("enter DA mode successed.");

    // internal phone tracking feature, ONLY for internal tool
#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if (!ToolInfo::IsCustomerVer()) {
        int device_tracking = 1;
        bool is_customer_phone = false;
        CB_DEVICE_TRACKING_QUERY_FUNC device_tracking_func = m_cmd_callback->getDeviceTracking();
        if (is_customer_phone && device_tracking_func) {
            device_tracking_func(&device_tracking);
        }
        if (device_tracking == 0) {
            LOGE("Stopped operation due to NOT permitted collecting device  information.");
            THROW_APP_EXCEPTION(static_cast<int>(CMD_DEVICE_TRACKING_CANCEL), QStringLiteral(""))
        }
    }
#endif

    LOG("get DA HW info start");
    CmdExecutor::execute(shared_from_this(), m_conn_da_cmd_wrapper, &cbs.trans);
    const std::shared_ptr<DAGetHWInfoCmdXML> sp_da_get_hw_info_cmd_xml =
            std::dynamic_pointer_cast<DAGetHWInfoCmdXML>(m_conn_da_cmd_wrapper->get_cmd_xml_generator());
    assert(sp_da_get_hw_info_cmd_xml);
    QString cmd_value_xml = sp_da_get_hw_info_cmd_xml->getCmdValue();
    LOG("get DA HW info successed, XML: %s", qPrintable(cmd_value_xml));
    CB_DA_CONNECTED_FUNC da_connected_func = m_cmd_callback->getCBDAConnect();
    if (da_connected_func) {
        da_connected_func(cmd_value_xml);
    }
    return hresult;
}

#ifdef _WIN32
void Connection::printDriverVersion() const
{
    if(OptionIniFileMgr::getInstance()->getPrint_driver_version()) {
        PrintDriverVersion *driver_version = new PrintDriverVersion();
        // QThreadPool takes ownership and deletes 'driver_version' instance automatically
        QThreadPool::globalInstance()->start(driver_version);
    }
}
#endif
