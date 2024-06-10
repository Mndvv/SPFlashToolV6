#ifndef CONNECTIONSETTING_H
#define CONNECTIONSETTING_H

#include <QMap>

#include "../Inc/CFlashToolTypes.h"
#include "../ConsoleMode/IConsoleModeXMLInterface.h"

class ConnectionArgs: public IConsoleModeXMLInterface
{
public:
    ConnectionArgs();

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    const QString &get_flash_xml_file() const;
    QString get_conn_flash_xml_file() const;
    const QString &get_auth_file() const;
    const QString &get_cert_file() const;
    QString get_brom_conn_options() const;
    QString get_flash_mode() const;
    QString get_com_port_clue() const;
    COM_PORT_TYPE get_com_port_type() const;
    CONN_END_STAGE get_conn_end_strage() const;

    void set_flash_xml_file(const QString &flash_xml_file);
    void set_auth_file(const QString &auth_file);
    void set_cert_file(const QString &cert_file);
    void set_da_file(const QString &da_file);
    void set_com_port_type(COM_PORT_TYPE com_port_type);
    void set_baud_rate(const QString &baud_rate);
    void set_com_port_name(const QString &com_port_name);
    void set_conn_end_stage(CONN_END_STAGE conn_end_strage);

    int getTimeout_ms() const;

private:
    void parseNodeValue(QString &tag_value, const QDomElement &root_node, const QString &tag_name, bool b_must_exist=true) const;
    void appendChildTextNode(QDomDocument &xml_doc, QDomElement &parent_node,
                             const QString &tag_name, const QString &tag_value) const;

private:
    QString m_flash_xml_file;
    QString m_auth_file;
    QString m_cert_file;
    QString m_da_file;
    COM_PORT_TYPE m_com_port_type;
    QString m_com_port_name;
    QString m_baud_rate;
    CONN_END_STAGE m_conn_end_stage;
    int m_timeout_ms;

    static QMap<QString, COM_PORT_TYPE> m_com_port_type_map;
    static QMap<QString, CONN_END_STAGE> m_conn_stage_map;
};

#endif // CONNECTIONSETTING_H
