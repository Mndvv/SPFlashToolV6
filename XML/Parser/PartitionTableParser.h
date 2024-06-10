#ifndef PARTITIONTABLEPARSER_H
#define PARTITIONTABLEPARSER_H

#include "IXMLParser.h"
#include "../../Utility/PartitionInfo.h"

class PartitionTableParser: public IXMLParser
{
public:
    PartitionTableParser();

    ReadbackPartitionInfoList getRBList() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    bool parsePartitionNode(ReadbackPartitionInfoList &rb_list,const QDomElement &pt_node);

private:
    ReadbackPartitionInfoList m_rb_list;
};

#endif // PARTITIONTABLEPARSER_H
