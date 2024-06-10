#ifndef EMMCCONTROLCMDXMLGENERATOR_H
#define EMMCCONTROLCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class EMMCControlCmdXML : public ICmdXMLGenerator
{
public:
    EMMCControlCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setTargetFileType(FILE_TYPE file_type);
    void setTargetFileName(const QString &file_name);
    QString getCmdXMLValue() const;

protected:
    virtual QString getFunctionFieldVal() const = 0;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

class GetRPMBKeyStatusCmdXML: public EMMCControlCmdXML
{
public:
    GetRPMBKeyStatusCmdXML();

protected:
    virtual QString getFunctionFieldVal() const;
};

class LifeCycleStatusCmdXML: public EMMCControlCmdXML
{
public:
    LifeCycleStatusCmdXML();

protected:
    virtual QString getFunctionFieldVal() const;
};

#endif // EMMCCONTROLCMDXMLGENERATOR_H
