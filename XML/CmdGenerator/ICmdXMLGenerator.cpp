#include "ICmdXMLGenerator.h"
#include "../XMLConst.h"

ICmdXMLGenerator::ICmdXMLGenerator(const QString &cmd_version_value,
                                   const QString &cmd_name_value,
                                   const QString &root_node_name):
    m_cmd_version_value(cmd_version_value),
    m_cmd_name_value(cmd_name_value),
    m_root_node_name(root_node_name)
{

}

ICmdXMLGenerator::~ICmdXMLGenerator()
{

}

QString ICmdXMLGenerator::toCmdString()
{
    if (m_generated_xml_string.isEmpty()) {
        QString strHeader( "version=\"1.0\" encoding=\"utf-8\"" );
        m_xml_doc.appendChild(m_xml_doc.createProcessingInstruction("xml", strHeader) );
        QDomElement root_node = m_xml_doc.createElement(m_root_node_name);
        m_xml_doc.appendChild(root_node);
        this->createXMLSubNode(&root_node, VERSION_NODE_NAME, m_cmd_version_value);
        this->createXMLSubNode(&root_node, COMMAND_NODE_NAME, m_cmd_name_value);
        QDomElement arg_node = m_xml_doc.createElement(ARG_NODE_NAME);
        root_node.appendChild(arg_node);
        this->fillArgNode(&arg_node);
        m_generated_xml_string = m_xml_doc.toString(4);
    }
    return m_generated_xml_string;
}

QString ICmdXMLGenerator::getCmdName() const
{
    return m_cmd_name_value;
}

QDomElement ICmdXMLGenerator::createXMLSubNode(QDomNode * const parent_node,
                                        const QString &sub_node_name,
                                        const QString &sub_node_value)
{
    QDomElement sub_name_node = m_xml_doc.createElement(sub_node_name);
    parent_node->appendChild(sub_name_node);
    if (!sub_node_value.isEmpty()) {
        QDomText sub_value_node = m_xml_doc.createTextNode(sub_node_value);
        sub_name_node.appendChild(sub_value_node);
    }
    return sub_name_node;
}

QDomElement ICmdXMLGenerator::createNodeWithAttrs(QDomElement * const parent_node, const QString &node_name,
                                                  const QMap<QString, QString> &attrs, const QString &node_value)
{
    QDomElement sub_name_node = m_xml_doc.createElement(node_name);
    parent_node->appendChild(sub_name_node);
    QDomText sub_value_node = m_xml_doc.createTextNode(node_value);
    sub_name_node.appendChild(sub_value_node);
    QStringList keys = attrs.keys();
    Q_FOREACH(auto attr_name, keys) {
        sub_name_node.setAttribute(attr_name, attrs.value(attr_name));
    }
    return sub_name_node;
}
