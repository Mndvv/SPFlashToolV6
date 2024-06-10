#ifndef GETHWINFOCMDXMLGENERATOR_H
#define GETHWINFOCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class BromGetHWInfoCmdXML : public ICmdXMLGenerator
{
public:
    BromGetHWInfoCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    QString getCmdValue() const;

private:
        QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

class DAGetHWInfoCmdXML : public ICmdXMLGenerator
{
public:
    DAGetHWInfoCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    QString getCmdValue() const;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // GETHWINFOCMDXMLGENERATOR_H
