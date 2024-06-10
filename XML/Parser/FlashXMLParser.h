#ifndef FLASHXMLPARSER_H
#define FLASHXMLPARSER_H

#include "IXMLParser.h"

class FlashXMLParser: public IXMLParser
{
public:
    FlashXMLParser();

    QString getScatterFileName() const;
    QString getContact() const;
    void setDAFile(const QString &da_file);
    QString getXMLString() override;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;
    virtual bool isValidXMLSchema(const QString &file_name) override;

private:
    bool parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name);
    QString convertToAbsolutePath(const QString &file_path) const;
    void replaceDAFile();

private:
    QString m_xml_scatter_file;
    QString m_contact;
    QString m_da_file;
};

#endif // FLASHXMLPARSER_H
