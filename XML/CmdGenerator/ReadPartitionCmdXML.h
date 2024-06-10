#ifndef READPARTITIONCMDXMLGENERATOR_H
#define READPARTITIONCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"


class ReadPartitionCmdXML : public ICmdXMLGenerator
{
public:
    ReadPartitionCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPartition(const QString &partition);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getCmdXMLValue() const;

private:
    QString m_partition;
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // READPARTITIONCMDXMLGENERATOR_H
