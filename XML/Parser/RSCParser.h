#ifndef RSCPARSER_H
#define RSCPARSER_H

#include "IXMLParser.h"

#include <QStringList>

class RSCParser: public IXMLParser
{
public:
    RSCParser();

    QStringList getRSCProjNames() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    bool parseMagicNode(const QDomElement &magic_node);
    bool parseProjInfoNode(QStringList &proj_name_list, const QDomElement &proj_info_node);
    bool parseProjItemNode(QStringList &proj_name_list, const QDomElement &proj_item_node);

private:
    QStringList m_rsc_proj_name_list;
};

#endif // RSCPARSER_H
