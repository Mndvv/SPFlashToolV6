#include "CFlashToolErrorCode.h"

QString ErrorLookup::CFlashToolErrorMessage(CFlashToolErrorCode error_code)
{
    QString error_msg;
    QString tip_msg;
    switch (error_code) {
    case CFT_UNKNOWN_ERROR:
        error_msg = QStringLiteral("un-specified error occured!");
        tip_msg = QStringLiteral("Please contact the tool owner for help.");
        break;
    case CFT_DRAM_REPAIR_NO_NEED:
        error_msg = QStringLiteral("DRAM repair address is NOT written.");
        break;
    case CFT_DRAM_REPAIR_FAILED:
        error_msg = QStringLiteral("DRAM repair operation failed.");
        break;
    case CFT_RPMB_WRITTEN_BEFORE_DL:
        error_msg = QStringLiteral("RPMB partition has been written before download");
        tip_msg = QStringLiteral("Please check it.");
        break;
    case CFT_CHECK_STORAG_LIFE_CYCLE_EXHAUST:
        error_msg = QStringLiteral("The storage in your device has reached end of life, " \
                                   "please change to use new device to do the operation.");
        break;
    case CFT_CHECK_STORAG_LIFE_CYCLE_WARN_CANCLE:
        error_msg = QStringLiteral("90% of life time for storage in your device has passed, " \
                                   "and the user canclled this opeartions");
        break;
    case CFT_DL_CHKSUM_FEEDBACK_ERR:
        error_msg = QStringLiteral("DL checksum verification failed.");
        break;
    case CFT_DL_CHKSUM_FEEDBACK_PARSER_ERR:
        error_msg = QStringLiteral("Parse device download checksum info failed.");
        break;
    case CFT_LOAD_CHKSUM_PARSER_ERR:
        error_msg = QStringLiteral("Parse load checksum info failed.");
        break;
    default:
        break;
    }
    if (!tip_msg.isEmpty()) {
        error_msg += QStringLiteral("\n[HINT]:\n");
        error_msg += tip_msg;
    }
    return error_msg;
}
