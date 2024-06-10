#ifndef IXMLPARSER_H
#define IXMLPARSER_H

#include <QDomDocument>
#include <cassert>

class IXMLParser
{
public:
    IXMLParser();
    virtual ~IXMLParser() {}

    bool parseXMLFile(const QString &xml_file_name);
    bool parseXMLString(const QString &xml_string);

    QString get_error_msg() const;
    virtual QString getXMLString();

protected:
    virtual bool parseDocument(const QDomElement &root_node) = 0;
    virtual bool isValidXMLSchema(const QString &file_name);

private:
    QString getFileNameOnly(const QString &file_name) const;

protected:
    QDomDocument m_xml_doc;
    QString m_error_msg;
    QString m_xml_file_path;
};

#endif // IXMLPARSER_H
