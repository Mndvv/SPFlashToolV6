#include "ICmdCallBack.h"
#include "../Conn/Connection.h"
#include "../XML/CmdGenerator/EMMCControlCmdXML.h"
#include "../XML/CmdGenerator/DebugDRAMRepairCmdXML.h"
#include "../../XML/CmdGenerator/GetDLImageFeedbackCmdXML.h"
#include "../../XML/Parser/ChksumParser.h"
#include "../Err/CFlashToolErrorCode.h"
#include "../Utility/Utils.h"
#include "../Inc/SLA_Challenge.h"
#include "../Logger/Log.h"
#include <QMessageBox>
#include <cassert>

int ICmdCallBack::cb_after_rpmb_cmd(const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    Q_UNUSED(conn)
    std::shared_ptr<GetRPMBKeyStatusCmdXML> get_rpmb_status_xml = std::dynamic_pointer_cast<GetRPMBKeyStatusCmdXML>(iCmdXMLGenerator);
    assert(get_rpmb_status_xml);
    QString rpmb_status = get_rpmb_status_xml->getCmdXMLValue();
    return rpmb_status == QStringLiteral("NOT-EXIST") ? CFT_SUCCESS : CFT_RPMB_WRITTEN_BEFORE_DL;
}

int ICmdCallBack::cb_after_dram_repair_cmd(const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    Q_UNUSED(conn)
    std::shared_ptr<DebugDRAMRepairCmdXML> dramRepairXML =
            std::dynamic_pointer_cast<DebugDRAMRepairCmdXML>(iCmdXMLGenerator);
    assert(dramRepairXML);
    QString cmd_value = dramRepairXML->getCmdXMLValue();
    CFlashToolErrorCode result;
    if (cmd_value == QStringLiteral("SUCCEEDED")) {
        result = CFT_SUCCESS;
    } else if (cmd_value == QStringLiteral("NO-NEED")) {
        result = CFT_DRAM_REPAIR_NO_NEED;
    } else {
        result = CFT_DRAM_REPAIR_FAILED;
    }
    return result;
}

int ICmdCallBack::cb_after_check_storage_life_cycle_cmd(const std::shared_ptr<Connection> conn, const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator)
{
    Q_UNUSED(conn)
    std::shared_ptr<LifeCycleStatusCmdXML> checkStrogeLifeCycle =
            std::dynamic_pointer_cast<LifeCycleStatusCmdXML>(iCmdXMLGenerator);
    assert(checkStrogeLifeCycle);
    QString cmd_value = checkStrogeLifeCycle->getCmdXMLValue();
    CFlashToolErrorCode result;
    if (cmd_value == QStringLiteral("WARNING")) {
        int ret = Utils::flashtool_message_box(nullptr, nullptr, QUESTION_MSGBOX,
            tr("Warning"),
            tr("Notice that  90% of life time for storage in your device has passed, please change to use new device as soon as possible.\nDo you want to continue anyway?"),
            tr("Continue"),
            tr("&Cancel"));
        if (ret == QMessageBox::AcceptRole) {
            result = CFT_SUCCESS;
        } else {
            result = CFT_CHECK_STORAG_LIFE_CYCLE_WARN_CANCLE;
        }
    } else if (cmd_value == QStringLiteral("EXHAUSTED")) {
        result = CFT_CHECK_STORAG_LIFE_CYCLE_EXHAUST;
    } else {
        result = CFT_SUCCESS;
    }
    return result;
}

int ICmdCallBack::cb_after_get_dl_img_feedback_cmd(const std::shared_ptr<Connection> conn,
                                                   const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator
                                                   , const QString &load_chksum_file)
{
    Q_UNUSED(conn)
    std::shared_ptr<GetDLImageFeedbackCmdXML> dl_img_feedback_xml = std::dynamic_pointer_cast<GetDLImageFeedbackCmdXML>(iCmdXMLGenerator);
    assert(dl_img_feedback_xml);
    QString dl_feedback = dl_img_feedback_xml->getCmdValue();
    DeviceChksumParser device_chksum_parser;
    if (!device_chksum_parser.parseXMLString(dl_feedback)) {
        LOG("Parse device checksum failed: %s", device_chksum_parser.get_error_msg().toStdString().c_str());
        return CFT_DL_CHKSUM_FEEDBACK_PARSER_ERR;
    }
    LoadChksumParser load_chksum_parser;
    if (!load_chksum_parser.parseXMLFile(load_chksum_file)) {
        LOG("Parse load checksum failed: %s", load_chksum_parser.get_error_msg().toStdString().c_str());
        return CFT_LOAD_CHKSUM_PARSER_ERR;
    }
    QHash<QString, QPair<QString, QString>> device_chksum_hash = device_chksum_parser.getChksumHash();
    QHash<QString, QString> load_chksum_hash = load_chksum_parser.getChksumHash();
    bool is_chksum_cmp_fail = false;
    for (auto iter = device_chksum_hash.cbegin(); iter != device_chksum_hash.cend(); ++iter) {
        QString load_chksum = load_chksum_hash.value(iter.key());
        QString device_chksum = iter.value().first;
        if (load_chksum != device_chksum) {
            LOG("Downloaded Checksum Error: Partition[%s], image file[%s], device downloaded checksum[%s], load checksum[%s]",
                qPrintable(iter.value().second), qPrintable(iter.key()),qPrintable(iter.value().first), qPrintable(load_chksum_hash.value(iter.key())));
            is_chksum_cmp_fail = true;
        }
    }
    return is_chksum_cmp_fail ? CFT_DL_CHKSUM_FEEDBACK_ERR : CFT_SUCCESS;
}

void ICmdCallBack::setupSLACallbackStruct(sla_callbacks_t *sla_trans) const
{
    assert(nullptr != sla_trans);
    sla_trans->start_user_arg = nullptr;
    sla_trans->cb_start = SLA_Challenge;
    sla_trans->end_user_arg = nullptr;
    sla_trans->cb_end = SLA_Challenge_END;
}
