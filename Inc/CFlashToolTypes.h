#ifndef CFLASHTOOLTYPES_H
#define CFLASHTOOLTYPES_H

#include <QMetaType>

enum class CONN_END_STAGE: unsigned char {
    CONN_UNKNOWN = 0,
    BROM_STAGE,
    FIRST_DA_STAGE,
    SECOND_DA_STAGE
};

enum class COM_PORT_TYPE: unsigned char {
    COM_PORT_UNKNOWN = 0,
    COM_PORT_USB_PCIE,
    COM_PORT_UART
};

enum class USB_POWER: unsigned char {
    AutoDetect,
    WithBattery,
    WithoutBattery
};

enum class HW_StorageType_E: unsigned char {

    HW_STORAGE_NONE = 0
    ,HW_STORAGE_NOR
    ,HW_STORAGE_NAND
    ,HW_STORAGE_EMMC
    ,HW_STORAGE_SDMMC
    ,HW_STORAGE_UFS
};

enum class FILE_TYPE: unsigned char {
    LOCAL_FILE = 0,
    REMOTE_FILE,
    MEMORY_FILE
};

enum class REGISTER_BIT_WIDTH_TYPE: unsigned char {
    REGISTER_32_BIT = 0,
    REGISTER_64_BIT
};

enum class REBOOT_TYPE: unsigned char {
    IMMEDIATE = 0,
    WARM_RESET = 0, //new name for IMMEDIATE
    PMIC_COLD_RESET,
    DISCONNECT
};

enum class BOOT_MODE_CONNECT_TYPE: unsigned char {
    CONN_TYPE_USB = 0,
    CONN_TYPE_UART,
    CONN_TYPE_WIFI
};

enum DOWNLOAD_COLUMN: unsigned char {
    dct_ENABLE = 0,
    dct_NAME,
    dct_Addr,
    dct_FILE_NAME
};

enum DOWNLOAD_SCENE: unsigned char {
    FORMAT_ALL_DOWNLOAD = 0,
    FIRMWARE_UPGRADE,
    DOWNLOAD_ONLY,
    UNKOWN_DOWNLOAD_SCENE
};

enum READBACK_COLUMN: unsigned char{
    rbc_Enabled = 0,
    rbc_Name,
    rbc_Addr,
    rbc_Length,
    rbc_Region,
    rbc_File
};

enum class READBACK_METHOD: unsigned char {
    RB_MANUAL = 0,
    RB_AUTO
};

enum ChkSumLevel {
    Chksum_NONE = 0x01,
    Chksum_USB = 0x02,
    Chksum_STORAGE = 0x04
};
Q_DECLARE_FLAGS(ChkSumLevels, ChkSumLevel)
Q_DECLARE_OPERATORS_FOR_FLAGS(ChkSumLevels)

enum class DRAM_REPARE_STATUS: unsigned char {
    DRAM_REPAIR_SUCCESS = 0,
    DRAM_REPAIR_NO_NEED,
    DRAM_REPARE_FAILED
};

#endif // CFLASHTOOLTYPES_H
