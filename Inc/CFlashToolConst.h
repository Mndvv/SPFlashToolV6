#ifndef CFLASHTOOLCONST_H
#define CFLASHTOOLCONST_H

#include <QString>

#define WELCOME_WIDGET_INDEX 0
#define FORMAT_WIDGET_INDEX 1
#define DOWNLOAD_WIDGET_INDEX 2
#define READBACK_WIDGET_INDEX 3
#define MEMORYTEST_WIDGET_INDEX 4
#define WRITEMEMORY_WIDGET_INDEX 5
#define BROMADAPTER_WIDGET_INDEX 6
#define DLCERT_WIDGET_INDEX 7
#define FFU_WIDGET_INDEX 8

#define STRING_TOOL_NAME QStringLiteral("Smart Phone Flash Tool V6")
#define STRING_OK QStringLiteral("&OK")
#define STRING_CANCEL QStringLiteral("&Cancel")
#define STRING_YES QStringLiteral("&Yes")
#define STRING_NO QStringLiteral("&No")
#define STRING_HELP QStringLiteral("&Help")
#define STRING_SEND_REPORT QStringLiteral("&Report")
#define STRING_DOWNLOAD_OK QStringLiteral("Download Ok")
#define STRING_SELECT_FLASH_XML QStringLiteral("Please select flash xml before operation.")

static const QString context_menu_style_sheet = "QMenu {"
                                        "background-color: #ffffff; /* sets background of the menu */"
                                        "}"
                                        "QMenu::item:selected { /* when user selects item using mouse or keyboard */"
                                        "    background-color: #80c4ff;"
                                        "}";

static const QString comboBox_style_sheet = "QComboBox {"
                                                "selection-background-color: #80c4ff;"
                                            "}";

static const QString valid_hex_address_expr = "^0x(?:[\\da-fA-F]+)$";

static const QString EMMC_BOOT1_VAL = QStringLiteral("EMMC-BOOT1");
static const QString EMMC_BOOT2_VAL = QStringLiteral("EMMC-BOOT2");
static const QString EMMC_USER_VAL  = QStringLiteral("EMMC-USER");
static const QString UFS_LUA0_VAL   = QStringLiteral("UFS-LUA0");
static const QString UFS_LUA1_VAL   = QStringLiteral("UFS-LUA1");
static const QString UFS_LUA2_VAL   = QStringLiteral("UFS-LUA2");
static const QString NAND_WHOLE_VAL = QStringLiteral("NAND-WHOLE");
static const QString NAND_BMT_VAL   = QStringLiteral("NAND-BMT");

#define UFS_LUA2_INDEX 5

#endif // CFLASHTOOLCONST_H
