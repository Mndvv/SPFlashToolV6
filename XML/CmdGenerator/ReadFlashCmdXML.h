#ifndef READFLASHCMDXMLGENERATOR_H
#define READFLASHCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"


class ReadFlashCmdXML : public ICmdXMLGenerator
{
public:
    ReadFlashCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPartition(const QString &partition);
    void setOffSet(const QString &offset);
    void setLength(const QString &length);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getCmdXMLValue() const;

private:
    QString m_partition;
    QString m_offset;
    QString m_length;
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // READFLASHCMDXMLGENERATOR_H
