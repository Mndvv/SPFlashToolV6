#include "MainWindowCallBack.h"
#include <cassert>
#include "../Inc/SLA_Challenge.h"
#include "../UI/Src/MainWindow.h"

#include <QThread>
#include "../Logger/Log.h"

MainWindowCallBack *MainWindowCallBack::m_static_instance = nullptr;

MainWindowCallBack::MainWindowCallBack(MainWindow *mainwindow):
    m_mainwindow(mainwindow)
{
    m_static_instance = this;
}

MainWindowCallBack::~MainWindowCallBack()
{

}

void MainWindowCallBack::setupBromCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &MainWindowCallBack::cb_conn_brom_operation_process;
    fillCommonCallbackStruct(cbs);
}

void MainWindowCallBack::setupDACallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &MainWindowCallBack::cb_conn_da_operation_process;
    fillCommonCallbackStruct(cbs);
}

CB_COM_PORT_STATUS_FUNC MainWindowCallBack::getCBComportStatus()
{
    return MainWindowCallBack::cb_com_port_status;
}

CB_DA_CONNECTED_FUNC MainWindowCallBack::getCBDAConnect()
{
    return MainWindowCallBack::cb_da_connected;
}

CB_DEVICE_TRACKING_QUERY_FUNC MainWindowCallBack::getDeviceTracking()
{
    return MainWindowCallBack::cb_get_device_tracking;
}

void MainWindowCallBack::setupCmdCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    fillCommonCallbackStruct(cbs);
}

void MainWindowCallBack::setupStopCallbackStruct(callback_stop_t *cb_stop)
{
    assert(nullptr != cb_stop);
    cb_stop->_this = nullptr;
    cb_stop->cb_notify_stop = &MainWindowCallBack::cb_notify_stop;
}

void MainWindowCallBack::setupChksumCallbackStruct(callback_transfer_t *cb_chksum)
{
    assert(nullptr != cb_chksum);
    cb_chksum->_this = nullptr;
    cb_chksum->cb_notify_stop = &MainWindowCallBack::cb_notify_stop;
    cb_chksum->cb_op_progress = &MainWindowCallBack::cb_scatter_chksum_operation_process;
}

void MainWindowCallBack::fillCommonCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);

    cbs->trans._this = nullptr;
    cbs->trans.cb_notify_stop = &MainWindowCallBack::cb_notify_stop;

    setupSLACallbackStruct(&(cbs->cb_sla));
}

bool MainWindowCallBack::cb_notify_stop(void *_this)
{
    return m_static_instance->m_mainwindow->cb_notify_stop(_this);
}

void MainWindowCallBack::cb_conn_brom_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    emit m_static_instance->signal_conn_brom_operation_process(_this, progress, data_xferd, QString(info));
}

void MainWindowCallBack::cb_conn_da_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    emit m_static_instance->signal_conn_da_operation_process(_this, progress, data_xferd, QString(info));
}

void MainWindowCallBack::cb_scatter_chksum_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    emit m_static_instance->signal_scatter_chksum_operation_process(_this, progress, data_xferd, QString(info));
}

void MainWindowCallBack::cb_com_port_status(const std::string &friendly_port_name)
{
    emit m_static_instance->signal_cb_com_port_status(friendly_port_name);
}

void MainWindowCallBack::cb_da_connected(const QString &da_conn_xml)
{
    emit m_static_instance->signal_cb_da_connected(da_conn_xml);
}

void MainWindowCallBack::cb_get_device_tracking(int *p_out)
{
    emit m_static_instance->signal_get_device_tracking(p_out);
}
