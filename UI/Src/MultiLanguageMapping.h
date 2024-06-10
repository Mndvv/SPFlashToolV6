#ifndef MULTILANGUAGEMAPPING_H
#define MULTILANGUAGEMAPPING_H

#include <QSharedPointer>
#include <QMap>

enum class LANGUAGE_TAG: unsigned char {
    TT_ENG = 0,
    TT_ZH_CH,
    TT_ZH_TW
};

enum STRING_TAG: unsigned int {
    IDS_STRING_FMT_ALL_DL = 0,
    IDS_STRING_FIRMWARE_UPGRADE,
    IDS_STRING_DL_ONLY,
    IDS_STRING_SHOW_WELCOME_PAGE,
    IDS_STRING_HIDE_WELCOME_PAGE,
    IDS_STRING_SHOW_START_LENGTH_COLUMN,
    IDS_STRING_HIDE_START_LENGTH_COLUMN,
    IDS_STRING_WELCOME_PAGE_NAME,
    IDS_STRING_BROM_ADAPTER_PAGE_NAME,
    IDS_STRING_WRITE_MEMORY_PAGE_NAME,
    IDS_STRING_FORMAT_PAGE_NAME,
    IDS_STRING_DOWNLOAD_PAGE_NAME,
    IDS_STRING_READ_BACK_PAGE_NAME,
    IDS_STRING_MEMORY_TEST_PAGE_NAME,
    IDS_STRING_TOOL_NAME,
    IDS_STRING_ABOUT_TOOL,
    IDS_STRING_DA_INFO,
    IDS_STRING_COPY_RIGHT,
    IDS_STRING_CUSTOM_RELEASE,
    IDS_STRING_INTERNEL_RELEASE,
    IDS_STRING_BUILD,
    IDS_STRING_UPDATE,
    IDS_STRING_CHECK_UPDATE,
    IDS_STRING_NO_UPDATE,
    IDS_STRING_OK,
    IDS_STRING_NEW_DETECT,
    IDS_STRING_REMIND,
    IDS_STRING_INSTALL_UPDATE,
    IDS_STRING_UPDATE_TITLE,
    IDS_STRING_UPDATE_MULTI_PROCESS,
    IDS_STRING_LACKING_OF_UPDATE_FILES,
    IDS_STRING_START_UPDATE_TOOL_FAILED,
    IDS_STRING_UPDATE_FAILED,
    IDS_STRING_TRACE_MODE,
    IDS_STRING_ADVANCE_MODE,
    IDS_STRING_SECURITY_MODE,
    IDS_STRING_SHOW_LENGTH_COLUMN,
    IDS_STRING_HIDE_LENGTH_COLUMN,
    IDS_STRING_DL_CERT_PAGE_NAME,
    IDS_STRING_FFU_PAGE_NAME
};

class MultiLanguageMapping
{
public:
    static QSharedPointer<MultiLanguageMapping> getInstance();

    void setLanguageTag(LANGUAGE_TAG languange);
    QString getString(STRING_TAG string_tag);

private:
    MultiLanguageMapping();
    MultiLanguageMapping(const MultiLanguageMapping &) = delete;
    MultiLanguageMapping(const MultiLanguageMapping &&) = delete;
    MultiLanguageMapping & operator=(const MultiLanguageMapping &) = delete;
    MultiLanguageMapping & operator=(const MultiLanguageMapping &&) = delete;

    void initEngMapping();
    void initZhCHMapping();
    void initZhTWMapping();

private:
   static QSharedPointer<MultiLanguageMapping> m_instance;
   QMap<STRING_TAG, QString> m_eng_mapping;
   QMap<STRING_TAG, QString> m_zh_CH_mapping;
   QMap<STRING_TAG, QString> m_zh_TW_mapping;
   LANGUAGE_TAG m_language_tag;
};

#define SET_TRANSLATE_TAG(translate_tag) MultiLanguageMapping::getInstance()->setLanguageTag(translate_tag)
#define GET_TRANSLATE_STRING(string_tag) MultiLanguageMapping::getInstance()->getString(string_tag)

#endif // MULTILANGUAGEMAPPING_H
