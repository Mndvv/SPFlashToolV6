#ifndef REBOOTCMDXMLGENERATOR_H
#define REBOOTCMDXMLGENERATOR_H

#include <QMap>
#include "ICmdXMLGenerator.h"
#include "../../Inc/CFlashToolTypes.h"

class RebootCmdXML: public ICmdXMLGenerator
{
public:
    RebootCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setRebootType(REBOOT_TYPE reboot_type);

private:
    REBOOT_TYPE m_reboot_type;

    static QMap<QString, REBOOT_TYPE> m_reboot_map;
};

#endif // REBOOTCMDXMLGENERATOR_H
