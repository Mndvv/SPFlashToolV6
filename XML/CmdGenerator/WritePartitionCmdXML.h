#ifndef WRITEPARTITIONCMDXMLGENERATOR_H
#define WRITEPARTITIONCMDXMLGENERATOR_H
#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"


class WritePartitionCmdXML : public ICmdXMLGenerator
{
public:
    WritePartitionCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPartition(const QString &partition);
    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString m_partition;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // WRITEPARTITIONCMDXMLGENERATOR_H
