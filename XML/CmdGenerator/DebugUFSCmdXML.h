#ifndef DEBUGUFSCMDXML_H
#define DEBUGUFSCMDXML_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class DebugUFSCmdXML : public ICmdXMLGenerator
{
public:
    DebugUFSCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

protected:
    virtual QString getFunctionFieldVal() const = 0;
};

class UFSUpdateFirmwareCmdXML: public DebugUFSCmdXML
{
public:
    UFSUpdateFirmwareCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

protected:
    virtual QString getFunctionFieldVal() const Q_DECL_OVERRIDE;

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

class UFSConfigSettingCmdXML: public DebugUFSCmdXML
{
public:
    UFSConfigSettingCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setTWSize(uint32_t tw_size);
    void setTWReduction(bool need_reduction);
    void setHPBSize(uint32_t hpb_size);
    void setHPBPinnedIndex(uint16_t hpb_index);
    void setHPBPinnedCnt(uint16_t hpb_cnt);
    void setLu3Size(uint32_t lu3_size);
    void setLu3TypeIndex(uint16_t lu3_type_index);

protected:
    virtual QString getFunctionFieldVal() const Q_DECL_OVERRIDE;

private:
    //write_turbo settings
    uint32_t m_tw_size_mb;
    bool m_tw_reduction;
    //HPB settings
    uint32_t m_hpb_size_mb;
    uint16_t m_hpb_pinned_index;
    uint16_t m_hpb_pinned_cnt;
    //split_last_lua settings
    uint32_t m_lu3_size_mb;
    uint16_t m_lu3_type_index;
};

#endif // DEBUGUFSCMDXML_H
