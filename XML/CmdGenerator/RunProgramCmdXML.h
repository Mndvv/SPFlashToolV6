#ifndef RUNPROGRAMCMDXMLGENERATOR_H
#define RUNPROGRAMCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class RunProgramCmdXML : public ICmdXMLGenerator
{
public:
    RunProgramCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setAtAddress(const QString &at_address);
    void setJmpAddress(const QString &jmp_address);
    void setSignatureOffset(const QString &signature_offset);
    void setSignatureLength(const QString &signature_length);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString m_at_address;
    QString m_jmp_address;
    QString m_signature_offset;
    QString m_signature_length;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // RUNPROGRAMCMDXMLGENERATOR_H
