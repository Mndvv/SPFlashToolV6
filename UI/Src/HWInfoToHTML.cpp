#include "HWInfoToHTML.h"
#include <memory>
#include <cassert>
#include "../../Logger/Log.h"

HWInfoToHTML::HWInfoToHTML(const QString &body_titile):
    m_html_body_title(body_titile)
{

}

QString HWInfoToHTML::toHTML(const QString &text) const
{
    QString html_template = this->getHTMLTemplate();

    QString header_title = this->getHTMLHeaderTitle();
    QString table_style = this->getTableStyle();
    QString table_content = this->generateTableContent(text);

    return html_template.arg(header_title).arg(table_style).arg(m_html_body_title).arg(table_content);
}

QString HWInfoToHTML::getHTMLHeaderTitle() const
{
    return QStringLiteral("HW INFO");
}

HWInfoTree::HWInfoTree_ptr HWInfoToHTML::parseHWInfo(const QString &text) const
{
    HWInfoTree::HWInfoTree_ptr hw_info_tree;
    std::shared_ptr<GetHWInfoXMLParser> sp_hw_xml_parser = std::make_shared<GetHWInfoXMLParser>();
    if (sp_hw_xml_parser->parseXMLString(text)) {
        hw_info_tree = sp_hw_xml_parser->get_hw_info();
    } else {
        LOG(qPrintable(sp_hw_xml_parser->get_error_msg()));
    }
    return hw_info_tree;
}

QString HWInfoToHTML::getTableRowTemplate(int row_index) const
{
    if (row_index % 2 == 0) {
        return QStringLiteral("<tr>"
                              "<td>%1</td>"
                              "<td>%2</td>"
                              "</tr>");
    } else {
        return QStringLiteral("<tr class=\"alt\">"
                              "<td>%1</td>"
                              "<td>%2</td>"
                              "</tr>");
    }
}

QString HWInfoToHTML::generateTableContent(const QString &text) const
{
    QString new_table;

    int row_index = 0;

    auto table_row_generator = [&](const QString &name, const QString &value)->void {
        QString table_row_template = this->getTableRowTemplate(row_index++);
        new_table += table_row_template.arg(name, value);
    };

    HWInfoTree::HWInfoTree_ptr hw_info_tree = this->parseHWInfo(text);

    for (int index = 0; index < hw_info_tree->count(); ++index) {
        HWInfoTree::HWInfoTree_ptr sub_node = hw_info_tree->getChildren(index);
        assert(sub_node);
        if (sub_node->hasChildren()) {
            for (int sub_index = 0; sub_index < sub_node->count(); ++sub_index) {
                HWInfoTree::HWInfoTree_ptr sub_tree = sub_node->getChildren(sub_index);
                assert(sub_tree);
                if (sub_tree->hasChildren()) {
                    continue;
                }
                table_row_generator(sub_tree->getName(), sub_tree->getValue());
            }
        } else {
            table_row_generator(sub_node->getName(), sub_node->getValue());
        }
    }
    return new_table;
}

QString HWInfoToHTML::getHTMLTemplate() const
{
    return QStringLiteral("<!DOCTYPE html>"
                          "<html>"
                          "  <head>"
                          "    <meta charset=\"utf-8\">"
                          "    <title>%1</title>"
                          "    <style>%2</style>"
                          "  </head>"
                          "  <body>"
                          "    <h3>%3:</h3>"
                          "    <table id=\"customers\">%4</table>"
                          "  </body>"
                          "</html>");
}

QString HWInfoToHTML::getTableStyle() const
{
    return QStringLiteral("#customers "
                          "{"
                          "  font-family:\"Trebuchet MS\", Arial, Helvetica, sans-serif;"
                          "  width:100%;"
                          "  border-collapse:collapse;"
                          "}"
                          "#customers td"
                          "{"
                          "  font-size:1.2em;"
                          "  border:1px solid #98bf21;"
                          "  padding:3px 7px 2px 7px;"
                          "}"
                          "#customers tr.alt td "
                          "{"
                          "  color:#000000;"
                          "  background-color:#EAF2D3;"
                          "}");
}
