#ifndef HWINFOTOHTML_H
#define HWINFOTOHTML_H

#include <QString>
#include "../../XML/Parser/GetHWInfoXMLParser.h"

class HTMLConvertor
{
public:
    virtual QString toHTML(const QString &text) const = 0;
    virtual ~HTMLConvertor() {}
};

class HWInfoToHTML: public HTMLConvertor
{
public:
    HWInfoToHTML(const QString &body_titile);

    QString toHTML(const QString &text) const override;

private:
    QString getHTMLHeaderTitle() const;
    HWInfoTree::HWInfoTree_ptr parseHWInfo(const QString &text) const;
    QString generateTableContent(const QString &text) const;
    QString getHTMLTemplate() const;
    QString getTableRowTemplate(int row_index) const;
    QString getTableStyle() const;

private:
    QString m_html_body_title;
};

#endif // HWINFOTOHTML_H
