#include "MultiLanguageMapping.h"

QSharedPointer<MultiLanguageMapping> MultiLanguageMapping::m_instance = QSharedPointer<MultiLanguageMapping>();

void MultiLanguageMapping::setLanguageTag(LANGUAGE_TAG languange)
{
    m_language_tag = languange;
}

QString MultiLanguageMapping::getString(STRING_TAG string_tag)
{
    switch (m_language_tag) {
    case LANGUAGE_TAG::TT_ENG:
        return m_eng_mapping.value(string_tag);
    case LANGUAGE_TAG::TT_ZH_CH:
        return m_zh_CH_mapping.value(string_tag);
    case LANGUAGE_TAG::TT_ZH_TW:
        return m_zh_TW_mapping.value(string_tag);
    }
    return QString();
}

MultiLanguageMapping::MultiLanguageMapping():
    m_language_tag(LANGUAGE_TAG::TT_ENG)
{
    initEngMapping();
    initZhCHMapping();
    initZhTWMapping();
}

void MultiLanguageMapping::initEngMapping()
{
    m_eng_mapping = {
        {IDS_STRING_FMT_ALL_DL, QStringLiteral("Format All + Download")},
        {IDS_STRING_FIRMWARE_UPGRADE, QStringLiteral("Firmware Upgrade")},
        {IDS_STRING_DL_ONLY, QStringLiteral("Download Only")},
        {IDS_STRING_SHOW_WELCOME_PAGE, QStringLiteral("Show Welcome")},
        {IDS_STRING_HIDE_WELCOME_PAGE, QStringLiteral("Hide Welcome")},
        {IDS_STRING_SHOW_START_LENGTH_COLUMN, QStringLiteral("Show \"Start Address\" Column")},
        {IDS_STRING_HIDE_START_LENGTH_COLUMN, QStringLiteral("Hide \"Start Address\" Column")},
        {IDS_STRING_WELCOME_PAGE_NAME, QStringLiteral("W&elcome")},
        {IDS_STRING_BROM_ADAPTER_PAGE_NAME, QStringLiteral("&Brom Adapter")},
        {IDS_STRING_WRITE_MEMORY_PAGE_NAME, QStringLiteral("Write &Memory")},
        {IDS_STRING_FORMAT_PAGE_NAME, QStringLiteral("Form&at")},
        {IDS_STRING_DOWNLOAD_PAGE_NAME, QStringLiteral("&Download")},
        {IDS_STRING_READ_BACK_PAGE_NAME, QStringLiteral("&Readback")},
        {IDS_STRING_MEMORY_TEST_PAGE_NAME, QStringLiteral("Memory&Test")},
        {IDS_STRING_FFU_PAGE_NAME, QStringLiteral("&Field Firmware Update")},
        {IDS_STRING_TOOL_NAME, QStringLiteral("Smart Phone Flash Tool V6")},
        {IDS_STRING_ABOUT_TOOL, QStringLiteral("About Smart Phone Flash Tool V6")},
        {IDS_STRING_DA_INFO, QStringLiteral("DA and Library Info")},
        {IDS_STRING_COPY_RIGHT, QStringLiteral("Copyright (c) 2020-2021, MediaTek Inc. All rights reserved.")},
        {IDS_STRING_CUSTOM_RELEASE, QStringLiteral("Comment: [Customer Release] ")},
        {IDS_STRING_INTERNEL_RELEASE, QStringLiteral("Comment: [Internal Release] ")},
        {IDS_STRING_BUILD, QStringLiteral("Build: ")},
        {IDS_STRING_UPDATE, QStringLiteral("Software Update")},
        {IDS_STRING_CHECK_UPDATE, QStringLiteral("Checking for update...")},
        {IDS_STRING_NO_UPDATE, QStringLiteral("Your Software is up to date now.")},
        {IDS_STRING_OK, QStringLiteral("&OK")},
        {IDS_STRING_NEW_DETECT, QStringLiteral("<html><head/><body><p>A new version of Flash tool is availabel</p></body></html>")},
        {IDS_STRING_REMIND, QStringLiteral("&Remind Me Later")},
        {IDS_STRING_INSTALL_UPDATE, QStringLiteral("&Install Update")},
        {IDS_STRING_UPDATE_TITLE, QStringLiteral("Update")},
        {IDS_STRING_UPDATE_MULTI_PROCESS, QStringLiteral("Please close other Flash Tool which is running before installing update.")},
        {IDS_STRING_LACKING_OF_UPDATE_FILES, QStringLiteral("Lacking of update files.\nPlease re-install the tool or contact the tool owner.")},
        {IDS_STRING_START_UPDATE_TOOL_FAILED, QStringLiteral("Start update tool failed.\nPlease re-install the tool or contact the tool owner.")},
        {IDS_STRING_UPDATE_FAILED, QStringLiteral("Update tool failed.\nPlease re-install the tool or contact the tool owner.")},
        {IDS_STRING_TRACE_MODE, QStringLiteral("(Runtime Trace Mode)")},
        {IDS_STRING_ADVANCE_MODE, QStringLiteral("(Advanced Mode)")},
        {IDS_STRING_SECURITY_MODE, QStringLiteral("(Security Mode)")},
        {IDS_STRING_SHOW_LENGTH_COLUMN, QStringLiteral("Show \"Length\" Column")},
        {IDS_STRING_HIDE_LENGTH_COLUMN, QStringLiteral("Hide \"Length\" Column")},
        {IDS_STRING_DL_CERT_PAGE_NAME, QStringLiteral("DLC&ert")}
    };
}

void MultiLanguageMapping::initZhCHMapping()
{
    m_zh_CH_mapping = {
        {IDS_STRING_FMT_ALL_DL, QStringLiteral("全部格式化和下载")},
        {IDS_STRING_FIRMWARE_UPGRADE, QStringLiteral("固件升级")},
        {IDS_STRING_DL_ONLY, QStringLiteral("下载")},
        {IDS_STRING_SHOW_WELCOME_PAGE, QStringLiteral("显示欢迎界面")},
        {IDS_STRING_HIDE_WELCOME_PAGE, QStringLiteral("隐藏欢迎界面")},
        {IDS_STRING_SHOW_START_LENGTH_COLUMN, QStringLiteral("显示“开始地址”列")},
        {IDS_STRING_HIDE_START_LENGTH_COLUMN, QStringLiteral("隐藏“开始地址”列")},
        {IDS_STRING_WELCOME_PAGE_NAME, QStringLiteral("欢迎(&E)")},
        {IDS_STRING_BROM_ADAPTER_PAGE_NAME, QStringLiteral("&BROM适配器")},
        {IDS_STRING_WRITE_MEMORY_PAGE_NAME, QStringLiteral("写存储(&W)")},
        {IDS_STRING_FORMAT_PAGE_NAME, QStringLiteral("格式化(&A)")},
        {IDS_STRING_DOWNLOAD_PAGE_NAME, QStringLiteral("下载(&D)")},
        {IDS_STRING_READ_BACK_PAGE_NAME, QStringLiteral("回读(&R)")},
        {IDS_STRING_MEMORY_TEST_PAGE_NAME, QStringLiteral("内存测试(&T)")},
        {IDS_STRING_FFU_PAGE_NAME, QStringLiteral("固件升级(&F)")},
        {IDS_STRING_TOOL_NAME, QStringLiteral("智能手机下载工具V6")},
        {IDS_STRING_ABOUT_TOOL, QStringLiteral("关于智能手机下载工具V6")},
        {IDS_STRING_DA_INFO, QStringLiteral("DA 和 Library 信息")},
        {IDS_STRING_COPY_RIGHT, QStringLiteral("版权 2020-2021 联发科技有限公司。保留所有权利。")},
        {IDS_STRING_CUSTOM_RELEASE, QStringLiteral("备注 [客户版本] ")},
        {IDS_STRING_INTERNEL_RELEASE, QStringLiteral("备注：[内部版本] ")},
        {IDS_STRING_BUILD, QStringLiteral("构建：")},
        {IDS_STRING_UPDATE, QStringLiteral("软件更新")},
        {IDS_STRING_CHECK_UPDATE, QStringLiteral("检查更新版本...")},
        {IDS_STRING_NO_UPDATE, QStringLiteral("你的软件已经是最新版本了。")},
        {IDS_STRING_OK, QStringLiteral("确定(&O)")},
        {IDS_STRING_NEW_DETECT, QStringLiteral("<html><head/><body><p>一个新的Flash Tool 版本可用</p></body></html>")},
        {IDS_STRING_REMIND, QStringLiteral("下次提醒(&R)")},
        {IDS_STRING_INSTALL_UPDATE, QStringLiteral("安装更新(&I)")},
        {IDS_STRING_UPDATE_TITLE, QStringLiteral("更新")},
        {IDS_STRING_UPDATE_MULTI_PROCESS, QStringLiteral("更新之前请先闭关其他打开的Flash Tool")},
        {IDS_STRING_LACKING_OF_UPDATE_FILES, QStringLiteral("缺少必要的更新文件导致更新失败！\n请重新安装Flash Tool或联系Tool Owner。")},
        {IDS_STRING_START_UPDATE_TOOL_FAILED, QStringLiteral("启动升级工具失败！\n请重新安装Flash Tool或联系Tool Owner。")},
        {IDS_STRING_UPDATE_FAILED, QStringLiteral("升级失败！\n请重新安装Flash Tool或联系Tool Owner。")},
        {IDS_STRING_TRACE_MODE, QStringLiteral("(运行时跟踪模式)")},
        {IDS_STRING_ADVANCE_MODE, QStringLiteral("(高级模式)")},
        {IDS_STRING_SECURITY_MODE, QStringLiteral("(安全模式)")},
        {IDS_STRING_SHOW_LENGTH_COLUMN, QStringLiteral("显示“长度”列")},
        {IDS_STRING_HIDE_LENGTH_COLUMN, QStringLiteral("隐藏“长度”列")},
        {IDS_STRING_DL_CERT_PAGE_NAME, QStringLiteral("CERT下载(&E)")}
    };
}

void MultiLanguageMapping::initZhTWMapping()
{
    m_zh_TW_mapping = {
        {IDS_STRING_FMT_ALL_DL, QStringLiteral("全部格式化和下載")},
        {IDS_STRING_FIRMWARE_UPGRADE, QStringLiteral("固件升級")},
        {IDS_STRING_DL_ONLY, QStringLiteral("下載")},
        {IDS_STRING_SHOW_WELCOME_PAGE, QStringLiteral("顯示歡迎界面")},
        {IDS_STRING_HIDE_WELCOME_PAGE, QStringLiteral("隱藏歡迎界面")},
        {IDS_STRING_SHOW_START_LENGTH_COLUMN, QStringLiteral("顯示“開始地址”列")},
        {IDS_STRING_HIDE_START_LENGTH_COLUMN, QStringLiteral("隱藏“開始地址”列")},
        {IDS_STRING_WELCOME_PAGE_NAME, QStringLiteral("歡迎(&E)")},
        {IDS_STRING_BROM_ADAPTER_PAGE_NAME, QStringLiteral("&BROM適配器")},
        {IDS_STRING_WRITE_MEMORY_PAGE_NAME, QStringLiteral("寫存儲(&W)")},
        {IDS_STRING_FORMAT_PAGE_NAME, QStringLiteral("格式化(&A)")},
        {IDS_STRING_DOWNLOAD_PAGE_NAME, QStringLiteral("下載(&D)")},
        {IDS_STRING_READ_BACK_PAGE_NAME, QStringLiteral("回讀(&R)")},
        {IDS_STRING_MEMORY_TEST_PAGE_NAME, QStringLiteral("內存測試(&T)")},
        {IDS_STRING_FFU_PAGE_NAME, QStringLiteral("固件升級(&F)")},
        {IDS_STRING_TOOL_NAME, QStringLiteral("智能手機下載工具V6")},
        {IDS_STRING_ABOUT_TOOL, QStringLiteral("關於智能手機下載工具V6")},
        {IDS_STRING_DA_INFO, QStringLiteral("DA 和 Library 信息")},
        {IDS_STRING_COPY_RIGHT, QStringLiteral("版權 2020-2021, 聯發科技有限公司。保留所有權利。")},
        {IDS_STRING_CUSTOM_RELEASE, QStringLiteral("備註 [客戶版本]")},
        {IDS_STRING_INTERNEL_RELEASE, QStringLiteral("備註 [內部版本]")},
        {IDS_STRING_BUILD, QStringLiteral("")},
        {IDS_STRING_UPDATE, QStringLiteral("軟件升級")},
        {IDS_STRING_CHECK_UPDATE, QStringLiteral("檢查更新版本...")},
        {IDS_STRING_NO_UPDATE, QStringLiteral("你的軟體已經是最新版本了。")},
        {IDS_STRING_OK, QStringLiteral("確定(&O)")},
        {IDS_STRING_NEW_DETECT, QStringLiteral("<html><head/><body><p>一個新的Flash Tool 版本可用</p></body></html>")},
        {IDS_STRING_REMIND, QStringLiteral("下次提醒(&R)")},
        {IDS_STRING_INSTALL_UPDATE, QStringLiteral("安裝更新(&I)")},
        {IDS_STRING_UPDATE_TITLE, QStringLiteral("更新")},
        {IDS_STRING_UPDATE_MULTI_PROCESS, QStringLiteral("更新之前請先關閉其他打開的Flash Tool")},
        {IDS_STRING_LACKING_OF_UPDATE_FILES, QStringLiteral("缺少必要的更新文件導致更新失敗！\n請重新安裝Flash Tool或聯繫Tool Owner。")},
        {IDS_STRING_START_UPDATE_TOOL_FAILED, QStringLiteral("啟動升級工具失敗！\n請重新安裝Flash Tool或聯繫Tool Owner。")},
        {IDS_STRING_UPDATE_FAILED, QStringLiteral("升級失敗！\n請重新安裝Flash Tool或聯繫Tool Owner。")},
        {IDS_STRING_TRACE_MODE, QStringLiteral("(運行時跟蹤模式)")},
        {IDS_STRING_ADVANCE_MODE, QStringLiteral("(高級模式)")},
        {IDS_STRING_SECURITY_MODE, QStringLiteral("(安全模式)")},
        {IDS_STRING_SHOW_LENGTH_COLUMN, QStringLiteral("顯示“長度”列")},
        {IDS_STRING_HIDE_LENGTH_COLUMN, QStringLiteral("隱藏“長度”列")},
        {IDS_STRING_DL_CERT_PAGE_NAME, QStringLiteral("CERT下載(&E)")}
    };
}

QSharedPointer<MultiLanguageMapping> MultiLanguageMapping::getInstance()
{
    // ONLY used in main thread, so NO NEED to care multi-thread condition.
    if (!m_instance) {
        m_instance = QSharedPointer<MultiLanguageMapping>(new MultiLanguageMapping());
    }
    return m_instance;
}
