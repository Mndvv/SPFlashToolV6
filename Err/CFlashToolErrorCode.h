#ifndef CFLASHTOOLERRORCODE_H
#define CFLASHTOOLERRORCODE_H

#include <QString>

//CFlash Tool error code definition, range from 1000 to 2000 to avoid conflict with lib error code.
enum CFlashToolErrorCode {
    CFT_SUCCESS = 1000,
    CFT_DRAM_REPAIR_NO_NEED,
    CFT_DRAM_REPAIR_FAILED,
    CFT_RPMB_WRITTEN_BEFORE_DL,
    CFT_CHECK_STORAG_LIFE_CYCLE_EXHAUST,
    CFT_CHECK_STORAG_LIFE_CYCLE_WARN_CANCLE,
    CFT_NOT_SUPPORTED_CONSOLE_MODE_CMD,
    CFT_CMD_NOT_SUPPORT,
    CMD_DEVICE_TRACKING_CANCEL,
    CFT_DL_CHKSUM_FEEDBACK_ERR,
    CFT_DL_CHKSUM_FEEDBACK_PARSER_ERR,
    CFT_LOAD_CHKSUM_PARSER_ERR,

    CFT_UNKNOWN_ERROR = 2000
};

class ErrorLookup {
public:
    static QString CFlashToolErrorMessage(CFlashToolErrorCode error_code);
};

#endif // CFLASHTOOLERRORCODE_H
