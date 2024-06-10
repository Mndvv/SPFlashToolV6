#ifndef GETDAINFOCMDXMLGENERATOR_H
#define GETDAINFOCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class GetDAInfoCmdXML: public ICmdXMLGenerator
{
public:
    GetDAInfoCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    QString getCmdValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // GETDAINFOCMDXMLGENERATOR_H
