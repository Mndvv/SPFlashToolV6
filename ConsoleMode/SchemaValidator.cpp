#include "SchemaValidator.h"
#include <QtGlobal>

#include "../Logger/Log.h"
#include "../Host/Inc/RuntimeMemory.h"
#include "../Err/Exception.h"
#include "../Inc/error_code.h"
#include "../Utility/FileUtils.h"

namespace ConsoleMode
{

class MessageHandler : public QAbstractMessageHandler
{
public:
    MessageHandler()
        : QAbstractMessageHandler(nullptr),
          m_messageType(QtDebugMsg)
    {
    }

    QString statusMessage() const
    {
        return m_description;
    }

    QString line() const
    {
        return QString::number(m_sourceLocation.line());
    }

    QString column() const
    {
        return QString::number(m_sourceLocation.column());
    }

protected:
    virtual void handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation)
    {
        Q_UNUSED(type)
        Q_UNUSED(identifier)

        m_messageType = type;
        m_description = description;
        m_sourceLocation = sourceLocation;
    }

private:
    QtMsgType m_messageType;
    QString m_description;
    QSourceLocation m_sourceLocation;
};

SchemaValidator::SchemaValidator(const QString& _schema_file)
    : m_schema_file(_schema_file), m_msg_handler(new MessageHandler())
{
    m_schema_obj.setMessageHandler(m_msg_handler.get());
}

SchemaValidator::~SchemaValidator()
{

}

void SchemaValidator::Validate(const QString& xml_file)
{
    LOGI("validating XML schema...");
    if (!loadXSDFile()) {
        QString error_msg = getErrorMsg(QStringLiteral("load XML Schema file failed!"));
        LOGE(error_msg.toLocal8Bit().constData());
        THROW_APP_EXCEPTION(E_ERROR, error_msg)
    }

    QXmlSchemaValidator validator(m_schema_obj);
    QFile file(xml_file);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString err_msg = QString("%1 open failed.").arg(xml_file);
        LOGE(qPrintable(err_msg));
        THROW_APP_EXCEPTION(E_ERROR, err_msg)
    }

    if(!validator.validate(&file))
    {
        QString error_msg = getErrorMsg(QStringLiteral("XML Schema validation failed: "));
        LOGE(error_msg.toLocal8Bit().constData());
        THROW_APP_EXCEPTION(E_ERROR, error_msg)
    }
    LOGI("XML schema validation passed!");
}

bool SchemaValidator::loadXSDFile()
{
    m_schema_file = QDir::toNativeSeparators(m_schema_file);
    QString err_msg;
    if (!FileUtils::validFile(m_schema_file, err_msg)) {
        LOGE(qPrintable(err_msg));
        return false;
    }
    QFile file(m_schema_file);
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        LOGE("schema file (%s) open failed!", m_schema_file.toLocal8Bit().data());
        return false;
    }
    return m_schema_obj.load(&file);
}

QString SchemaValidator::getErrorMsg(const QString &error_header) const
{
    QString msg(error_header);
    msg.append(m_msg_handler->statusMessage());
    msg.append("(line:").append(m_msg_handler->line());
    msg.append(", column:").append(m_msg_handler->column()).append(")");
    return msg;
}

}
