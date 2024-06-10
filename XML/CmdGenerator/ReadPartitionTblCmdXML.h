#ifndef READPARTITIONTABLECMDXMLGENERATOR_H
#define READPARTITIONTABLECMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class ReadPartitionTblCmdXML: public ICmdXMLGenerator
{
public:
    ReadPartitionTblCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    QString getCmdValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // READPARTITIONTABLECMDXMLGENERATOR_H
