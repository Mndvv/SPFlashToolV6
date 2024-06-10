#ifndef SECURITYSETALLINONESIGNATURECMDXMLGENERATOR_H
#define SECURITYSETALLINONESIGNATURECMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class SecSetAllInOneSignatureCmdXML : public ICmdXMLGenerator
{
public:
    SecSetAllInOneSignatureCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // SECURITYSETALLINONESIGNATURECMDXMLGENERATOR_H
