#ifndef WRITEEFUSECMDXMLGENERATOR_H
#define WRITEEFUSECMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class WriteEfuseCmdXML : public ICmdXMLGenerator
{
public:
    WriteEfuseCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // WRITEEFUSECMDXMLGENERATOR_H
