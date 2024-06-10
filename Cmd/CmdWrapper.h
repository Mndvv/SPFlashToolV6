#ifndef CMDWRAPPER_H
#define CMDWRAPPER_H

#include "../Cmd/CallbackTypes.h"
#include "../ConsoleMode/IConsoleModeXMLInterface.h"

class ICmdXMLGenerator;
class Connection;
#include <QObject>
#include <QMap>
#include <memory>

class CmdWrapper: public QObject, public IConsoleModeXMLInterface
{
    Q_OBJECT

public:
    CmdWrapper(const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    virtual ~CmdWrapper();

    const std::shared_ptr<ICmdXMLGenerator> get_cmd_xml_generator() const;
    void setOperationProgress(CB_OPERATION_PROGRESS_FUNC cb_operation_progress_func);

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

public:
    int cb_before_cmd(const std::shared_ptr<Connection> conn);
    int cb_after_cmd(const std::shared_ptr<Connection> conn);
    static void clearSelfMap();
    static void cb_operation_process(void *_this, unsigned int progress,
                                     unsigned long long data_xferd, const char *info);

signals:
    void signal_before_cmd(int *p_cmd_result, const std::shared_ptr<Connection> conn);
    void signal_after_cmd(int *p_cmd_result, const std::shared_ptr<Connection> conn,
                          const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator);
    void signal_operation_process(void *_this, unsigned int progress,
                                  unsigned long long data_xferd, const QString &info);

public slots:
    void slot_operation_progress(void *_this, unsigned int progress,
                                 unsigned long long data_xferd, const QString &info);

private:
    std::shared_ptr<ICmdXMLGenerator> m_cmd_xml_generator;
    static QMap<QString, CmdWrapper *> m_self_map;
    CB_OPERATION_PROGRESS_FUNC m_cb_operation_progress;
};

using cmd_wrapper_ptr = std::shared_ptr<CmdWrapper>;
using CmdWrapperSPList = std::list<cmd_wrapper_ptr>;

#endif // CMDWRAPPER_H
