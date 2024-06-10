#ifndef MAINWINDOWCALLBACK_H
#define MAINWINDOWCALLBACK_H

#include "ICmdCallBack.h"

class MainWindow;

class MainWindowCallBack: public ICmdCallBack
{
    Q_OBJECT

public:
    MainWindowCallBack(MainWindow *mainwindow);
    virtual ~MainWindowCallBack();

signals:
    void signal_conn_brom_operation_process(void *_this, unsigned int progress,
                                      unsigned long long data_xferd, const QString &info);
    void signal_conn_da_operation_process(void *_this, unsigned int progress,
                                      unsigned long long data_xferd, const QString &info);
    void signal_scatter_chksum_operation_process(void *_this, unsigned int progress,
                                      unsigned long long data_xferd, const QString &info);
    void signal_cb_com_port_status(const std::string &friendly_port_name);
    void signal_cb_da_connected(const QString &da_conn_xml);
    void signal_get_device_tracking(int *p_out);

public:
    virtual void setupBromCallbackStruct(callbacks_struct_t *cbs);
    virtual void setupDACallbackStruct(callbacks_struct_t *cbs);
    virtual void setupCmdCallbackStruct(callbacks_struct_t *cbs);
    virtual void setupStopCallbackStruct(callback_stop_t *cb_stop);
    virtual void setupChksumCallbackStruct(callback_transfer_t *cb_chksum);
    virtual CB_COM_PORT_STATUS_FUNC getCBComportStatus();
    virtual CB_DA_CONNECTED_FUNC getCBDAConnect();
    virtual CB_DEVICE_TRACKING_QUERY_FUNC getDeviceTracking();

    static bool cb_notify_stop(void *_this);

private:
    //static bool cb_notify_stop(void *_this);
    static void cb_conn_brom_operation_process(void *_this, unsigned int progress,
                                  unsigned long long data_xferd, const char *info);
    static void cb_conn_da_operation_process(void *_this, unsigned int progress,
                                  unsigned long long data_xferd, const char *info);
    static void cb_scatter_chksum_operation_process(void *_this, unsigned int progress,
                                  unsigned long long data_xferd, const char *info);
    static void cb_com_port_status(const std::string &friendly_port_name);
    static void cb_da_connected(const QString &da_conn_xml);
    static void cb_get_device_tracking(int *p_out);

    void fillCommonCallbackStruct(callbacks_struct_t *cbs);

private:
    static MainWindowCallBack *m_static_instance;
    MainWindow *m_mainwindow;
};

#endif // MAINWINDOWCALLBACK_H
