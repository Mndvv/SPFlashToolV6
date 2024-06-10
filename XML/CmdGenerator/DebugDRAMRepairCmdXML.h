#ifndef DEBUGDRAMREPAIRCMDXMLGENERATOR_H
#define DEBUGDRAMREPAIRCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class DebugDRAMRepairCmdXML : public ICmdXMLGenerator
{
public:
    DebugDRAMRepairCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setParam_file(const QString &param_file);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getCmdXMLValue() const;

private:
    QString m_param_file;
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // DEBUGDRAMREPAIRCMDXMLGENERATOR_H
