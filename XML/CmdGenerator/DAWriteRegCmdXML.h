#ifndef DAWRITEREGISTERCMDXMLGENERATOR_H
#define DAWRITEREGISTERCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"


class DAWriteRegCmdXML : public ICmdXMLGenerator
{
public:
    DAWriteRegCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setRegisterBitWidth(REGISTER_BIT_WIDTH_TYPE register_bit_width);
    void setBaseAddress(const QString &base_address);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    void setMemInfo(const QString &start_address, const QString &length);

private:
    QString getRegisterBitWidth() const;

private:
    REGISTER_BIT_WIDTH_TYPE m_register_bit_width;
    QString m_base_address;
    QSharedPointer<XMLSourceFileAssistant> m_source_file_assist;
};

#endif // DAWRITEREGISTERCMDXMLGENERATOR_H
