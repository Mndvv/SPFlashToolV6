#ifndef XMLCONST_H
#define XMLCONST_H

#define CMD_VERSION_VALUE "1.0"
#define CMD_VERSION_1_1_VALUE "1.1"
#define CMD_VERION_2_VALUE "2.0"
// command version and command name values
//DA commands
#define WRITEFLASH_CMD_VER_VAL                    CMD_VERSION_VALUE
#define WRITE_FLASH_CMD_NAME_VAL                   "CMD:WRITE-FLASH"
#define WRITEPARTITION_CMD_VER_VAL                    CMD_VERSION_VALUE
#define WRITE_PARTITION_CMD_NAME_VAL                   "CMD:WRITE-PARTITION"
#define READFLASH_CMD_VER_VAL                    CMD_VERSION_VALUE
#define READ_FLASH_CMD_NAME_VAL                   "CMD:READ-FLASH"
#define READPARTITION_CMD_VER_VAL                    CMD_VERSION_VALUE
#define READ_PARTITION_CMD_NAME_VAL                   "CMD:READ-PARTITION"
#define FLASH_ALL_CMD_VER_VAL                    CMD_VERSION_VALUE
#define FLASH_ALL_CMD_NAME_VAL                   "CMD:FLASH-ALL"
#define FLASH_ALL_EX_CMD_VER_VAL                    CMD_VERION_2_VALUE
#define FLASH_ALL_EX_CMD_NAME_VAL                   FLASH_ALL_CMD_NAME_VAL
#define WRITE_PARTITIONS_CMD_VER_VAL                    CMD_VERSION_VALUE
#define WRITE_PARTITIONS_CMD_NAME_VAL                   "CMD:WRITE-PARTITIONS"
#define FLASH_UPDATE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define FLASH_UPDATE_CMD_NAME_VAL                   "CMD:FLASH-UPDATE"
#define FLASH_UPDATE_EX_CMD_VER_VAL                    CMD_VERION_2_VALUE
#define FLASH_UPDATE_EX_CMD_NAME_VAL                   FLASH_UPDATE_CMD_NAME_VAL
#define ERASEFLASH_CMD_VER_VAL                    CMD_VERSION_VALUE
#define ERASE_FLASH_CMD_NAME_VAL                   "CMD:ERASE-FLASH"
#define ERASEPARTITION_CMD_VER_VAL                    CMD_VERSION_VALUE
#define ERASEPARTITION_CMD_NAME_VAL                   "CMD:ERASE-PARTITION"
#define DA_GET_HW_CMD_VER_VAL                 CMD_VERSION_VALUE
#define DA_GET_HW_CMD_NAME_VAL                "CMD:GET-HW-INFO"
#define REBOOT_CMD_VER_VAL                    CMD_VERSION_VALUE
#define REBOOT_CMD_NAME_VAL                   "CMD:REBOOT"
#define SET_RUNTIME_PARAMETER_CMD_VER_VAL     CMD_VERSION_1_1_VALUE
#define SET_RUNTIME_PARAMETER_CMD_NAME_VAL    "CMD:SET-RUNTIME-PARAMETER"
#define NOTIFY_INIT_DRAM_CMD_VER_VAL                    CMD_VERSION_VALUE
#define NOTIFY_INIT_DRAM_CMD_NAME_VAL                   "CMD:NOTIFY-INIT-DRAM"
#define BOOT_TO_CMD_VER_VAL                    CMD_VERSION_VALUE
#define BOOT_TO_CMD_NAME_VAL                   "CMD:BOOT-TO"
#define WRITE_EFUSE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define WRITE_EFUSE_CMD_NAME_VAL                   "CMD:WRITE-EFUSE"
#define READ_EFUSE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define READ_EFUSE_CMD_NAME_VAL                   "CMD:READ-EFUSE"
#define RAM_TEST_CMD_VER_VAL                    CMD_VERSION_VALUE
#define RAM_TEST_CMD_NAME_VAL                   "CMD:RAM-TEST"
#define DA_READ_REGISTER_CMD_VER_VAL                    CMD_VERSION_VALUE
#define DA_READ_REGISTER_CMD_NAME_VAL                   "CMD:READ-REGISTER"
#define DA_WRITE_REGISTER_CMD_VER_VAL                    CMD_VERSION_VALUE
#define DA_WRITE_REGISTER_CMD_NAME_VAL                   "CMD:WRITE-REGISTER"
#define EMMC_CONTROL_CMD_VER_VAL                    CMD_VERSION_VALUE
#define EMMC_CONTROL_CMD_NAME_VAL                   "CMD:EMMC-CONTROL"
#define SET_HOST_INFO_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SET_HOST_INFO_CMD_NAME_VAL                   "CMD:SET-HOST-INFO"
#define SECURITY_GET_DEV_FW_INFO_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SECURITY_GET_DEV_FW_INFO_CMD_NAME_VAL                   "CMD:SECURITY-GET-DEV-FW-INFO"
#define SECURITY_SET_FLASH_POLICY_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SECURITY_SET_FLASH_POLICY_CMD_NAME_VAL                   "CMD:SECURITY-SET-FLASH-POLICY"
#define SECURITY_SET_ALLINONE_SIGNATURE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SECURITY_SET_ALLINONE_SIGNATURE_CMD_NAME_VAL                   "CMD:SECURITY-SET-ALLINONE-SIGNATURE"
#define SET_RSC_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SET_RSC_CMD_NAME_VAL                   "CMD:SET-RSC"
#define SET_BOOT_MODE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define SET_BOOT_MODE_CMD_NAME_VAL                   "CMD:SET-BOOT-MODE"
#define WRITE_PRIVATE_CERT_CMD_VER_VAL                    CMD_VERSION_VALUE
#define WRITE_PRIVATE_CERT_CMD_NAME_VAL                   "CMD:WRITE-PRIVATE-CERT"
#define DEBUG_DRAM_REPAIR_CMD_VER_VAL                    CMD_VERSION_VALUE
#define DEBUG_DRAM_REPAIR_CMD_NAME_VAL                   "CMD:DEBUG:DRAM-REPAIR"
#define GET_DA_INFO_CMD_VER_VAL                    CMD_VERSION_VALUE
#define GET_DA_INFO_CMD_NAME_VAL                   "CMD:GET-DA-INFO"
#define READ_PARTITION_TABLE_CMD_VER_VAL                    CMD_VERSION_VALUE
#define READ_PARTITION_TABLE_CMD_NAME_VAL                   "CMD:READ-PARTITION-TABLE"
#define DEBUG_UFS_CMD_VER_VAL                    CMD_VERSION_VALUE
#define DEBUG_UFS_CMD_NAME_VAL                   "CMD:DEBUG:UFS"
#define GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_VER_VAL                    CMD_VERSION_VALUE
#define GET_DOWNLOADED_IMAGE_FEEDBACK_CMD_NAME_VAL                   "CMD:GET-DOWNLOADED-IMAGE-FEEDBACK"

//brom commands
#define BROM_GET_HW_CMD_VER_VAL               CMD_VERSION_VALUE
#define BROM_GET_HW_CMD_NAME_VAL              "CMD:GET-HW-INFO"
#define BROM_READ_REGISTER_CMD_VER_VAL                    CMD_VERSION_VALUE
#define BROM_READ_REGISTER_CMD_NAME_VAL                   "CMD:READ-REGISTER"
#define BROM_WRITE_REGISTER_CMD_VER_VAL                    CMD_VERSION_VALUE
#define BROM_WRITE_REGISTER_CMD_NAME_VAL                   "CMD:WRITE-REGISTER"
#define RUN_PROGRAM_CMD_VER_VAL                    CMD_VERSION_VALUE
#define RUN_PROGRAM_CMD_NAME_VAL                   "CMD:RUN-PROGRAM"

#define EXTRACT_CMD_NAME_ONLY(cmd_name) QStringLiteral(cmd_name).mid(4)

#define Add_CMD_PREFIX(cmd_name) QStringLiteral("CMD:") + cmd_name

#define LIFE_CYCLE_CHECK QStringLiteral("LIFE-CYCLE-STATUS")
#define GET_RPMB_KEY_STATUS QStringLiteral("GET-RPMB-KEY-STATUS")

#define UPDATE_FIRMWARE QStringLiteral("UPDATE-FIRMWARE")
#define SETTING QStringLiteral("SETTING")
#define SPLIT_LAST_LUA QStringLiteral("SPLIT-LAST-LUA")

// command node name
#define BROM_NODE_NAME "brom"
#define DA_NODE_NAME "da"
#define VERSION_NODE_NAME "version"
#define COMMAND_NODE_NAME "command"
#define ARG_NODE_NAME "arg"
#define PARTITION_NODE_NAME "partition"
#define PARTITION_INDEX_NODE_NAME "partition_index"
#define PARTITION_NAME_NODE_NAME "partition_name"
#define PHYSICAL_START_ADDR_NODE_NAME "physical_start_addr"
#define FILE_NAME_NODE_NAME "file_name"
#define IS_DOWNLOAD_NODE_NAME "is_download"
#define OFFSET_NODE_NAME "offset"
#define LENGTH_NODE_NAME "length"
#define OPTION_NODE_NAME "option"
#define OPTIONS_NODE_NAME "options"
#define SOURCE_FILE_NODE_NAME "source_file"
#define TARGET_FILE_NODE_NAME "target_file"
#define CHECKSUM_LEVEL_NODE_NAME "checksum_level"
#define BATTERY_EXIST_NODE_NAME "battery_exist"
#define DA_LOG_LEVEL_NODE_NAME "da_log_level"
#define LOG_CHANNEL_NODE_NAME "log_channel" //todo
#define HOST_OS_NODE_NAME "host_os"
#define SYSTEM_OS_NODE_NAME "system_os"
#define BROM_HW_INFO_NODE_NAME "brom_hw_info"
#define CHIP_HW_CODE_NODE_NAME "chip_hw_code"
#define CHIP_HW_SUB_CODE_NODE_NAME "chip_hw_sub_code"
#define CHIP_HW_VERSION_NODE_NAME "chip_hw_version"
#define CHIP_SW_VERSION_NODE_NAME "chip_sw_version"
#define ME_ID_NODE_NAME "me_id"
#define SOC_ID_NODE_NAME "soc_id"
#define SECURITY_BOOT_ENABLED_NODE_NAME "security_boot_enabled"
#define SECURITY_DA_AUTH_ENABLED_NODE_NAME "security_da_auth_enabled"
#define SECURITY_SLA_ENABLED_NODE_NAME "security_sla_enabled"
#define AT_ADDRESS_NODE_NAME "at_address"
#define JMP_ADDRESS_NODE_NAME "jmp_address"
#define SIGNATURE_OFFSET_NODE_NAME "signature_offset"
#define SIGNATURE_LENGTH_NODE_NAME "signature_length"
#define BIT_WIDTH_NODE_NAME "bit_width"
#define BASE_ADDRESS_NODE_NAME "base_address"
#define COUNT_NODE_NAME "count"
#define FUNCTION_NODE_NAME "function"
#define SCATTER_NODE_NAME "scatter"
#define CONTACT_NODE_NAME "contact"
#define GENERAL_NODE_NAME "general"
#define CHIP_NODE_NAME    "chip"
#define SECTIONS_NODE_NAME "sections"
#define SECTION_NODE_NAME  "section"
#define NAME_NODE_NAME     "name"
#define FILE_NODE_NAME     "file"
#define MODE_NODE_NAME     "mode"
#define PATH_SEPARATOR_NODE_NAME "path_separator"
#define ACTION_NODE_NAME "action"
#define START_ADDRESS_NODE_NAME "start_address"
#define REPEAT_NODE_NAME "repeat"
#define INFO_NODE_NAME "info"
#define PLATFORM_NODE_NAME "platform"
#define STORAGE_TYPE_NODE_NAME "storage_type"
#define MAGIC_NODE_NAME "magic"
#define PROJ_INFO_NODE_NAME "proj_info"
#define PROJ_ITEM_NODE_NAME "proj_item"
#define INDEX_NODE_NAME "index"
#define KEY_NODE_NAME "key"
#define SKIP_PT_OPERATE_NODE_NAME "skip_pt_operate"
#define CONNECT_TYPE_NODE_NAME "connect_type"
#define MOBILE_LOG_NODE_NAME "mobile_log"
#define ADB_NODE_NAME "adb"
#define BACKUP_FOLDER_NODE_NAME "backup_folder"
#define PARAM_FILE_NODE_NAME "param_file"
#define PT_NODE_NAME QStringLiteral("pt")
#define START_NODE_NAME QStringLiteral("start")
#define SIZE_NODE_NAME QStringLiteral("size")
#define TYPE_NODE_NAME QStringLiteral("type")
#define FLASH_LIST_NODE_NAME QStringLiteral("flash_list")
#define PT_NODE_NAME QStringLiteral("pt")
#define TW_NODE_NAME QStringLiteral("write_turbo")
#define HPB_NODE_NAME QStringLiteral("hpb")
#define SPLIT_LAST_LUA_NODE_NAME QStringLiteral("split_last_lua")
#define REDUCTION_NODE_NAME QStringLiteral("reduction")
#define PRSI_NODE_NAME QStringLiteral("pinned_region_start_index")
#define PRC_NODE_NAME QStringLiteral("pinned_region_count")
#define ADV_NODE_NAME QStringLiteral("adv")
#define INITIALIZE_DRAM_NODE_NAME QStringLiteral("initialize_dram")

#define STORAGE_ATTR_NAME "storage"
#define NAME_ATTR_NAME    "name"

//console mode xml node names
#define GENERAL_NODE_NAME    "general"
#define COMMANDS_NODE_NAME    "commands"

//console mode user used command name defination
#define FORMAT_ALL_CMD_NAME       QStringLiteral("format")
#define DL_ONLY_CMD_NAME          QStringLiteral("download")
#define FMT_ALL_DL_CMD_NAME       QStringLiteral("format-download")
#define FIRMWARE_UPGRADE_CMD_NAME QStringLiteral("firmware-upgrade")
#define DRAM_REPAIR_CMD_NAME      QStringLiteral("dram-repair")
#define WRITE_EFUSE_CMD_NAME      QStringLiteral("write-efuse")
#define READ_EFUSE_CMD_NAME       QStringLiteral("read-efuse")

#define NODE_VALUE_MAX_LENGTH 256
#define ADDRESS_MAX_LENGTH 9
#define XML_DATA_MAX_LENGTH 0x200000

#endif // XMLCONST_H
