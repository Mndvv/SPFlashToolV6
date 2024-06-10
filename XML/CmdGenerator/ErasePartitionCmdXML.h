#ifndef ERASEPARTITIONCMDXMLGENERATOR_H
#define ERASEPARTITIONCMDXMLGENERATOR_H

#include "ICmdXMLGenerator.h"


class ErasePartitionCmdXML : public ICmdXMLGenerator
{
public:
    ErasePartitionCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setPartition(const QString &partition);

private:
    QString m_partition;
};

#endif // ERASEPARTITIONCMDXMLGENERATOR_H
