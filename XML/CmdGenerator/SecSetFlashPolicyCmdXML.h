#ifndef SECURITYSETFLASHPOLICYCMDXMLGENERATOR_H
#define SECURITYSETFLASHPOLICYCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class SecSetFlashPolicyCmdXML : public ICmdXMLGenerator
{
public:
    SecSetFlashPolicyCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // SECURITYSETFLASHPOLICYCMDXMLGENERATOR_H
