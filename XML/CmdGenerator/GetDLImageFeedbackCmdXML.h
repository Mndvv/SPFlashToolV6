#ifndef GETDLIMAGEFEEDBACK_H
#define GETDLIMAGEFEEDBACK_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class GetDLImageFeedbackCmdXML: public ICmdXMLGenerator
{
public:
    GetDLImageFeedbackCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setFileType(FILE_TYPE file_type);

    QString getCmdValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // GETDLIMAGEFEEDBACK_H
