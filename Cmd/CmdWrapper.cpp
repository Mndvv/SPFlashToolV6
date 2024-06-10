#include "CmdWrapper.h"
#include "../XML/CmdGenerator/ICmdXMLGenerator.h"
#include <QMetaMethod>
#include "../Err/CFlashToolErrorCode.h"

QMap<QString, CmdWrapper *> CmdWrapper::m_self_map;

CmdWrapper::CmdWrapper(const std::shared_ptr<ICmdXMLGenerator> iCmdXMLGenerator):
    m_cmd_xml_generator(iCmdXMLGenerator)
{
    m_self_map.insert(m_cmd_xml_generator->getCmdName(), this);
    connect(this, &CmdWrapper::signal_operation_process,
            this, &CmdWrapper::slot_operation_progress);
}

CmdWrapper::~CmdWrapper()
{

}

const std::shared_ptr<ICmdXMLGenerator> CmdWrapper::get_cmd_xml_generator() const
{
    return m_cmd_xml_generator;
}

void CmdWrapper::setOperationProgress(CB_OPERATION_PROGRESS_FUNC cb_operation_progress_func)
{
    m_cb_operation_progress = cb_operation_progress_func;
}

void CmdWrapper::loadXML(const QDomElement &xml_node)
{
    m_cmd_xml_generator->loadXML(xml_node);
}

void CmdWrapper::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    m_cmd_xml_generator->saveXML(xml_doc, xml_node);
}

int CmdWrapper::cb_before_cmd(const std::shared_ptr<Connection> conn)
{
    int cmd_result = CFT_CMD_NOT_SUPPORT;
    const QMetaMethod before_cmd_signal = QMetaMethod::fromSignal(&CmdWrapper::signal_before_cmd);
    if (isSignalConnected(before_cmd_signal)) {
        emit signal_before_cmd(&cmd_result, conn);
    }
    return cmd_result;
}

int CmdWrapper::cb_after_cmd(const std::shared_ptr<Connection> conn)
{
    int cmd_result = CFT_CMD_NOT_SUPPORT;
    const QMetaMethod after_cmd_signal = QMetaMethod::fromSignal(&CmdWrapper::signal_after_cmd);
    if (isSignalConnected(after_cmd_signal)) {
        emit signal_after_cmd(&cmd_result, conn, m_cmd_xml_generator);
    }
    return cmd_result;
}

void CmdWrapper::clearSelfMap()
{
    m_self_map.clear();
}

void CmdWrapper::cb_operation_process(void *_this, unsigned int progress,
                                      unsigned long long data_xferd, const char *info)
{
    if (nullptr != _this) {
        QString cmd_name = *(static_cast<QString *>(_this));
        if (m_self_map.contains(cmd_name)) {
            CmdWrapper * const p_current_cmd_wrapper = m_self_map.value(cmd_name, nullptr);
            if (p_current_cmd_wrapper != nullptr) {
                emit p_current_cmd_wrapper->signal_operation_process(_this, progress, data_xferd, QString(info));
            }
        }
    }
}

void CmdWrapper::slot_operation_progress(void *_this, unsigned int progress, unsigned long long data_xferd, const QString &info)
{
    if (m_cb_operation_progress) {
        m_cb_operation_progress(_this, progress, data_xferd, info);
    }
}
