#ifndef GETHWINFOXMLPARSER_H
#define GETHWINFOXMLPARSER_H

#include <list>
#include <utility>

#include "IXMLParser.h"
#include "HWInfoTree.h"

class GetHWInfoXMLParser : public IXMLParser
{
public:
    GetHWInfoXMLParser();

    HWInfoTree::HWInfoTree_ptr get_hw_info() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;
    virtual bool parseElement(const QDomElement &element, QSharedPointer<HWInfoContainerNode> &container_node);

private:
     QSharedPointer<HWInfoContainerNode> m_hw_info_tree;
};

#endif // GETHWINFOXMLPARSER_H
