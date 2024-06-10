#ifndef WRITEPARTITIONSCMDXMLGENERATOR_H
#define WRITEPARTITIONSCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"
#include "Utility/PartitionInfo.h"


class WritePartitionsCmdXML : public ICmdXMLGenerator
{
public:
    WritePartitionsCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

    void setDLPartitionList(const DownloadPartitionInfoList &dl_partition_list);

private:
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
    DownloadPartitionInfoList m_dl_partition_list;
};


#endif // WRITEPARTITIONSCMDXMLGENERATOR_H
