#ifndef ICMDCALLBACK_H
#define ICMDCALLBACK_H

#include <QObject>
#include <memory>

#include "CallbackTypes.h"
#include "../Inc/Flash.Mode.Struct.h"

class Connection;
class ICmdXMLGenerator;

class ICmdCallBack: public QObject
{
    Q_OBJECT

public:
    virtual ~ICmdCallBack() {}

public:
    virtual void setupBromCallbackStruct(callbacks_struct_t *cbs) = 0;
    virtual void setupDACallbackStruct(callbacks_struct_t *cbs) = 0;
    virtual void setupCmdCallbackStruct(callbacks_struct_t *cbs) = 0;
    virtual void setupStopCallbackStruct(callback_stop_t *cb_stop) = 0;
    virtual void setupChksumCallbackStruct(callback_transfer_t *cb_chksum) = 0;
    virtual CB_COM_PORT_STATUS_FUNC getCBComportStatus() = 0;
    virtual CB_DA_CONNECTED_FUNC getCBDAConnect() = 0;
    virtual CB_DEVICE_TRACKING_QUERY_FUNC getDeviceTracking() = 0;

public:
    static int cb_after_rpmb_cmd(const std::shared_ptr<Connection> conn,
                              const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    static int cb_after_dram_repair_cmd(const std::shared_ptr<Connection> conn,
                                    const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    static int cb_after_check_storage_life_cycle_cmd(const std::shared_ptr<Connection> conn,
                                    const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    static int cb_after_get_dl_img_feedback_cmd(const std::shared_ptr<Connection> conn,
                                    const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator
                                                , const QString &load_chksum_file);

protected:
    void setupSLACallbackStruct(sla_callbacks_t *sla_trans) const;
};

#endif // ICMDCALLBACK_H
