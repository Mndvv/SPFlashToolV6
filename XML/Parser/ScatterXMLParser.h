#ifndef SCATTERXMLPARSER_H
#define SCATTERXMLPARSER_H

#include "IXMLParser.h"
#include "../../Inc/CFlashToolTypes.h"
#include "../../Utility/PartitionInfo.h"

class ScatterXMLParser: public IXMLParser
{
public:
    ScatterXMLParser();

    QString get_chip_name() const;
    DownloadPartitionInfoList get_scatter_partitions() const;
    HW_StorageType_E get_storage_type() const;
    bool isSkipPTOperation() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    bool parseGeneralNode(const QDomElement &general_node);
    bool parseStorageTypeNode(const QDomElement &storage_type_node);
    bool parsePartitionIndexNode(DownloadPartitionInfoList &partition_list, const QDomElement &partition_index_node);
    QString genNoChilElementError(const QDomElement &partition_index_node,
                               const std::string &sub_node_name) const;

private:
    QString m_chip_name;
    HW_StorageType_E m_storage_type;
    bool m_skip_pt_operation;
    DownloadPartitionInfoList m_scatter_list;
};

#endif // SCATTERXMLPARSER_H
