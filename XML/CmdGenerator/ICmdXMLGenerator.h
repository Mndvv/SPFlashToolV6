#ifndef ICMDXMLGENERATOR_H
#define ICMDXMLGENERATOR_H

#include <QDomDocument>
#include <QMap>
#include <cassert>

#include "../../ConsoleMode/IConsoleModeXMLInterface.h"

#define XML_CMD_GENERATOR_VIRTUAL_FUNCS \
    virtual void fillArgNode(QDomElement * const arg_node) Q_DECL_OVERRIDE;

class ICmdXMLGenerator: public IConsoleModeXMLInterface
{
public:
    ICmdXMLGenerator(const QString &cmd_version_value,
                     const QString &cmd_name_value,
                     const QString &root_node_name);
    virtual ~ICmdXMLGenerator() Q_DECL_OVERRIDE;

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE { Q_UNUSED(xml_node) }
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE {
        Q_UNUSED(xml_node) Q_UNUSED(xml_doc)}

    QString toCmdString();
    QString getCmdName() const;

protected:
    virtual void fillArgNode(QDomElement * const arg_node) = 0;
    QDomElement createXMLSubNode(QDomNode * const parent_node, const QString &sub_node_name,
                          const QString &sub_node_value);
    QDomElement createNodeWithAttrs(QDomElement * const parent_node, const QString &node_name,
                                    const QMap<QString, QString> &attrs, const QString &node_value);

private:
    QDomDocument m_xml_doc;
    QString m_cmd_version_value;
    QString m_cmd_name_value;
    QString m_root_node_name;
    QString m_generated_xml_string;
};

#endif // ICMDXMLGENERATOR_H
