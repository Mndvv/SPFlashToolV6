#ifndef SOURCEFILEASSISTANT_H
#define SOURCEFILEASSISTANT_H

#include <QString>
#include <QMap>
#include <memory>
#include "../../Inc/CFlashToolTypes.h"
#include "../XMLConst.h"
#include "../../ConsoleMode/IConsoleModeXMLInterface.h"

class XMLFileAssistant: public IConsoleModeXMLInterface
{
public:
    XMLFileAssistant();
    virtual ~XMLFileAssistant() {}

    void setFileType(FILE_TYPE file_type);
    void setFileName(const QString &file_name);
    QString getFileDesc();

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

protected:
    virtual QString getStartAddress() = 0;
    virtual QString getLength() const = 0;
    virtual QString getXMLNodeName() const = 0;

private:
    FILE_TYPE m_file_type;
    QString m_file_name;
    static QMap<QString, FILE_TYPE> m_file_type_map;
};

class XMLSourceFileAssistant: public XMLFileAssistant
{
public:
    XMLSourceFileAssistant() {}

    void setStartAddress(const QString &start_address);
    void setLength(const QString &length);

protected:
    virtual QString getStartAddress() override;
    virtual QString getLength() const override;
    virtual QString getXMLNodeName() const override;

private:
    QString m_start_address;
    QString m_length;
};

class XMLTargetFileAssistant: public XMLFileAssistant
{
public:
    XMLTargetFileAssistant() {}

    QString getCmdXMLValue() const;

protected:
    virtual QString getStartAddress() override;
    virtual QString getLength() const override;
    virtual QString getXMLNodeName() const override;

private:
    std::shared_ptr<char> m_xml_data;
};

#endif // SOURCEFILEASSISTANT_H
