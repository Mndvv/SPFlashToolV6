#ifndef ERASEFLASHCMDXMLGENERATOR_H
#define ERASEFLASHCMDXMLGENERATOR_H

#include "ICmdXMLGenerator.h"


//default value used for auto format operation.
class EraseFlashCmdXML : public ICmdXMLGenerator
{
public:
    EraseFlashCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPartition(const QString &partition);
    void setOffSet(const QString &offset);
    void setLength(const QString &length);

private:
    QString m_partition;
    QString m_offset;
    QString m_length;
};

#endif // ERASEFLASHCMDXMLGENERATOR_H
