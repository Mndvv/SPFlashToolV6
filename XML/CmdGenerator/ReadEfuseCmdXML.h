#ifndef READEFUSECMDXMLGENERATOR_H
#define READEFUSECMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class ReadEfuseCmdXML : public ICmdXMLGenerator
{
public:
    ReadEfuseCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);

    QString getCmdValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // READEFUSECMDXMLGENERATOR_H
