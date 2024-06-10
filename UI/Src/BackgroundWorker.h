#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <list>
#include <memory>

#include <QObject>
#include <QThread>
#include <atomic>

class ConnectionArgs;
class MainWindowCallBack;

#include "../../Cmd/CallbackTypes.h"
#include "../../Cmd/CmdWrapper.h"

class SetRuntimeParaCmdXML;

class BackgroundWorker: public QThread
{
    Q_OBJECT

public:
    static std::atomic_bool m_interaction_finished;

public:
    BackgroundWorker(const std::shared_ptr<MainWindowCallBack> mainwindow_callback);
    virtual ~BackgroundWorker();

    void setConnArgs(std::shared_ptr<ConnectionArgs> p_conn_args);
    void setRuntimeParameter(std::shared_ptr<SetRuntimeParaCmdXML> p_runtime_parameter);

    void AddCmdJob(std::shared_ptr<CmdWrapper> job);
    void AddCmdJob(const CmdWrapperSPList jobs);

    void addInteractionCmdJob(const CmdWrapperSPList jobs);

    void setNeedInteraction(bool need_interaction);

signals:
    void signal_current_operation_finished();
    void signal_current_operation_ok();
    void signal_current_operation_failed(int error_code, QString error_msg);
    void signal_user_interaction_mode();

public slots:
    void slot_reset_common_jobs();

protected:
    virtual void run();

private:
    void resetCurrentJobStatus();

private:
    std::shared_ptr<ConnectionArgs> m_connArgs;
    std::list<std::shared_ptr<CmdWrapper>> m_jobs;
    CmdWrapperSPList m_interaction_jobs;

    std::shared_ptr<MainWindowCallBack> m_mainwindow_callback;
    std::shared_ptr<SetRuntimeParaCmdXML> m_rumtime_parameter;
    bool m_need_interaction;
};

#endif // BACKGROUNDWORKER_H
