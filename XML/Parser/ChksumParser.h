#ifndef CHKSUMPARSER_H
#define CHKSUMPARSER_H

#include "IXMLParser.h"

#include <QHash>

class LoadChksumParser: public IXMLParser
{
public:
    LoadChksumParser();

    QHash<QString, QString> getChksumHash() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    QHash<QString, QString> m_chksum_hash;
};

class DeviceChksumParser: public IXMLParser
{
public:
    DeviceChksumParser();

    QHash<QString, QPair<QString, QString>> getChksumHash() const;

protected:
    virtual bool parseDocument(const QDomElement &root_node) override;

private:
    QString getFileNameOnly(const QString &file_name) const;

private:
    QHash<QString, QPair<QString, QString>> m_chksum_hash;
};

#endif // CHKSUMPARSER_H
