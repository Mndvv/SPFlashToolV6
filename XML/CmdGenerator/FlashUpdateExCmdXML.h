#ifndef FLASHUPDATEEXCMDXMLGENERATOR_H
#define FLASHUPDATEEXCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"
#include "Utility/PartitionInfo.h"

class FlashUpdateExCmdXML : public ICmdXMLGenerator
{
public:
    FlashUpdateExCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setPathSeparator(const QString &path_separator);
    void setBackupFolder(const QString &backup_folder);
    void setDLPartitionList(const DownloadPartitionInfoList &dl_partition_list);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString m_path_separator;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
    QString m_backupFolder;
    DownloadPartitionInfoList m_dl_partition_list;
};

#endif // FLASHUPDATEEXCMDXMLGENERATOR_H
