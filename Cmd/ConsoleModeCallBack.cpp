#include "ConsoleModeCallBack.h"
#include "../Logger/Log.h"
#include "../Utility/Utils.h"
#include "../XML/Parser/GetHWInfoXMLParser.h"
#include "../Utility/version.h"
#include "../Network/NetworkThread.h"
#include <cassert>

ConsoleMode::ConsoleModeEntry * ConsoleModeCallBack::m_console_mode_entry = nullptr;

void ConsoleModeCallBack::setupBromCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &ConsoleModeCallBack::cb_conn_brom_operation_process;
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupDACallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    cbs->trans.cb_op_progress = &ConsoleModeCallBack::cb_conn_da_operation_process;
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupCmdCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);
    fillCallbackStruct(cbs);
}

void ConsoleModeCallBack::setupStopCallbackStruct(callback_stop_t *cb_stop)
{
    assert(nullptr != cb_stop);
    cb_stop->_this = nullptr;
    cb_stop->cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;
}

void ConsoleModeCallBack::setupChksumCallbackStruct(callback_transfer_t *cb_chksum)
{
    assert(nullptr != cb_chksum);
    cb_chksum->_this = nullptr;
    cb_chksum->cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;
    cb_chksum->cb_op_progress = &ConsoleModeCallBack::cb_scatter_chksum_operation_process;
}

CB_COM_PORT_STATUS_FUNC ConsoleModeCallBack::getCBComportStatus()
{
    return ConsoleModeCallBack::cb_com_port_status;
}

CB_DA_CONNECTED_FUNC ConsoleModeCallBack::getCBDAConnect()
{
    return ConsoleModeCallBack::cb_da_connected;
}

CB_DEVICE_TRACKING_QUERY_FUNC ConsoleModeCallBack::getDeviceTracking()
{
    return ConsoleModeCallBack::cb_get_device_tracking;
}

void ConsoleModeCallBack::setConsoleModeEntry(ConsoleMode::ConsoleModeEntry *entry)
{
    m_console_mode_entry = entry;
}

bool ConsoleModeCallBack::cb_notify_stop(void *_this)
{
    Q_UNUSED(_this)
    return false;
}

void ConsoleModeCallBack::cb_conn_brom_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of BROM data has been sent(about %s)", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_conn_da_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)

    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of DA data has been sent(about %s)", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_scatter_chksum_operation_process(void *_this, unsigned int progress, unsigned long long data_xferd, const char *info)
{
    Q_UNUSED(_this)
    static unsigned int lastReportedProgress = 0;

    if (progress - lastReportedProgress >= 5 && progress != lastReportedProgress)
    {
        LOGI("%d%% of %s has been calculate checksum value(about %s)", progress, std::string(info).c_str(), Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeCallBack::cb_com_port_status(const std::string &friendly_port_name)
{
    LOG("com port friendly name: %s", friendly_port_name.c_str());
}

void ConsoleModeCallBack::cb_da_connected(const QString &da_conn_xml)
{
    Q_ASSERT(nullptr != m_console_mode_entry);

    LOG("DA HW info is:\n%s", qPrintable(da_conn_xml));
#ifdef _INTERNAL_PHONE_TRACKING_MODE
    if(ToolInfo::IsCustomerVer()) {
        LOGI("No need collect device tracking information for customer version!");
        return ;
    }

    if (!m_console_mode_entry->network_online()) {
        LOGI("No need collect device tracking information due to NO network!");
        return ;
    }

    QString device_id;
    std::shared_ptr<GetHWInfoXMLParser> sp_hw_xml_parser = std::make_shared<GetHWInfoXMLParser>();
    if (sp_hw_xml_parser->parseXMLString(da_conn_xml)) {
        HWInfoTree::HWInfoTree_ptr hw_info_tree = sp_hw_xml_parser->get_hw_info();
        for (int i = 0; i < hw_info_tree->count(); ++i) {
            HWInfoTree::HWInfoTree_ptr tree_node = hw_info_tree->getChildren(i);
            assert(tree_node);
            if (tree_node->getName() == QStringLiteral("device_id")) {
                device_id = tree_node->getValue();
            }
        }
    } else {
        LOG(qPrintable(sp_hw_xml_parser->get_error_msg()));
    }

    if (!device_id.isEmpty()) {
        NetworkThread *network_thread = new NetworkThread(device_id);
        connect(network_thread, SIGNAL(finished()), network_thread, SLOT(deleteLater()));
        network_thread->start();
    }
#endif
}

void ConsoleModeCallBack::cb_get_device_tracking(int *p_out)
{
    assert(nullptr != p_out);

/*
#define MTK_VID QStringLiteral("vid_0e8d")

    *p_out = 1;

    if(ToolInfo::IsCustomerVer()) {
        LOGI("Stopped! No need collect device tracking information for customer version!");
        return ;
    }

    if (!m_console_mode_entry->network_online()) {
        LOGI("Stopped! No need collect device tracking information due to NO network!");
        return ;
    }

    QString vid;
    QString port_name = QString::fromStdString(friendly_port_name);
    QStringList port_name_list = port_name.split(QStringLiteral("@"));
    if (port_name_list.count() > 3) {
        vid = port_name_list.at(3).split(QStringLiteral("&")).at(0);
    }
    if (vid.toLower() != MTK_VID) {
        *p_out = 0;
        LOGI("mismatch vendor id: %s", vid.toStdString().c_str());
    }
*/
    *p_out = 0;
}

void ConsoleModeCallBack::fillCallbackStruct(callbacks_struct_t *cbs)
{
    assert(nullptr != cbs);

    cbs->trans._this = nullptr;
    cbs->trans.cb_notify_stop = &ConsoleModeCallBack::cb_notify_stop;

    setupSLACallbackStruct(&(cbs->cb_sla));
}
