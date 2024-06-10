#ifndef BOOTTOCMDXMLGENERATOR_H
#define BOOTTOCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class BootToCmdXML : public ICmdXMLGenerator
{
public:
    BootToCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setAtAddress(const QString &at_address);
    void setJmpAddress(const QString &jmp_address);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString m_at_address;
    QString m_jmp_address;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // BOOTTOCMDXMLGENERATOR_H
