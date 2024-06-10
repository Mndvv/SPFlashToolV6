#ifndef SCHEMAVALIDATOR_H
#define SCHEMAVALIDATOR_H

#include <QString>
#include <QtXmlPatterns>
#include <memory>

#include "../Err/Exception.h"

namespace ConsoleMode
{
class MessageHandler;
class SchemaValidator
{
public:
    SchemaValidator(const QString& _schema_file);
    ~SchemaValidator();

    void Validate(const QString& xml_file);

private:
    SchemaValidator(const SchemaValidator &)=delete;
    SchemaValidator & operator=(const SchemaValidator &)=delete;
    bool loadXSDFile();
    QString getErrorMsg(const QString &error_header) const;

private:
    QString m_schema_file;
    QXmlSchema m_schema_obj;
    std::shared_ptr<MessageHandler> m_msg_handler;
};

}

#endif // SCHEMAVALIDATOR_H
