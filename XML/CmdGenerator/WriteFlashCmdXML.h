#ifndef WRITEFLASHCMDXMLGENERATOR_H
#define WRITEFLASHCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"


class WriteFlashCmdXML : public ICmdXMLGenerator
{
public:
    WriteFlashCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPartition(const QString &partition);
    void setOffSet(const QString &offset);
    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString m_partition;
    QString m_offset;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // WRITEFLASHCMDXMLGENERATOR_H
