#ifndef ICONSOLEMODEXMLINTERFACE_H
#define ICONSOLEMODEXMLINTERFACE_H

#include <QDomElement>

class IConsoleModeXMLInterface
{
public:
    virtual ~IConsoleModeXMLInterface() {}

    virtual void loadXML(const QDomElement &xml_node) = 0;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const = 0;
};

#endif // ICONSOLEMODEXMLINTERFACE_H
