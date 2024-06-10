#ifndef SECURITYGETDEVFWINFOCMDXMLGENERATOR_H
#define SECURITYGETDEVFWINFOCMDXMLGENERATOR_H


#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class SecGetDevFWInfoCmdXML : public ICmdXMLGenerator
{
public:
    SecGetDevFWInfoCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getCmdXMLValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // SECURITYGETDEVFWINFOCMDXMLGENERATOR_H
