#ifndef RAMTESTCMDXMLGENERATOR_H
#define RAMTESTCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class RAMTestCmdXML : public ICmdXMLGenerator
{
public:
    RAMTestCmdXML();

protected:
    virtual QString getFunctionValue() const = 0;
};

class FlipRAMTestCmdXML: public RAMTestCmdXML
{
public:
    FlipRAMTestCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setStartAddress(const QString &start_address);
    void setLength(const QString &length);
    void setRepeatCount(int repeat_count);

protected:
    virtual QString getFunctionValue() const override;

private:
    QString m_start_address;
    QString m_length;
    int m_repeat_count;
};

class CalibrationRAMTestCmdXML: public RAMTestCmdXML
{
public:
    CalibrationRAMTestCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setTargetFileType(FILE_TYPE file_type);
    void setTargetFileName(const QString &file_name);
    QString getCmdXMLValue() const;

protected:
    virtual QString getFunctionValue() const override;

private:
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

class SRAMTestCmdXML: public RAMTestCmdXML
{
public:
    XML_CMD_GENERATOR_VIRTUAL_FUNCS

protected:
    virtual QString getFunctionValue() const override;
};

#endif // RAMTESTCMDXMLGENERATOR_H
