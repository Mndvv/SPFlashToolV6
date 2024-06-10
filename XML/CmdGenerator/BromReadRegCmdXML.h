#ifndef BROMREADREGISTERCMDXMLGENERATOR_H
#define BROMREADREGISTERCMDXMLGENERATOR_H

#include <QSharedPointer>
#include "ICmdXMLGenerator.h"
#include "XMLFileAssistant.h"

class BromReadRegCmdXML : public ICmdXMLGenerator
{
public:
    BromReadRegCmdXML();

    XML_CMD_GENERATOR_VIRTUAL_FUNCS

    void setRegisterBitWidth(REGISTER_BIT_WIDTH_TYPE register_bit_width);
    void setBaseAddress(const QString &base_address);

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getCmdXMLValue() const;

private:
    QString getRegisterBitWidth() const;

private:
    REGISTER_BIT_WIDTH_TYPE m_register_bit_width;
    QString m_base_address;
    QSharedPointer<XMLTargetFileAssistant> m_target_file_assist;
};

#endif // BROMREADREGISTERCMDXMLGENERATOR_H
