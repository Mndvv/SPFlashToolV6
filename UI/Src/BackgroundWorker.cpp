#include "BackgroundWorker.h"
#include <limits>
#include "../../Err/Exception.h"
#include "../../Logger/Log.h"
#include "../../Conn/Connection.h"
#include "../../Conn/ConnectionArgs.h"
#include "../../Cmd/MainWindowCallBack.h"
#include "../../XML/CmdGenerator/ICmdXMLGenerator.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Cmd/CmdExecutor.h"
#include "../../Inc/error_code.h"
#include "../../Err/Exception.h"
#include <QApplication>
#include <cassert>
#include <algorithm>

std::atomic_bool BackgroundWorker::m_interaction_finished(true);

BackgroundWorker::BackgroundWorker(const std::shared_ptr<MainWindowCallBack> mainwindow_callback):
    m_mainwindow_callback(mainwindow_callback),
    m_need_interaction(false)
{

}

BackgroundWorker::~BackgroundWorker()
{
    this->terminate();
    this->wait();
    resetCurrentJobStatus();
    slot_reset_common_jobs();
}

void BackgroundWorker::setConnArgs(std::shared_ptr<ConnectionArgs> p_conn_args)
{
    m_connArgs = p_conn_args;
}

void BackgroundWorker::setRuntimeParameter(std::shared_ptr<SetRuntimeParaCmdXML> p_runtime_parameter)
{
    m_rumtime_parameter = p_runtime_parameter;
}

void BackgroundWorker::AddCmdJob(const CmdWrapperSPList jobs)
{
    foreach(auto job, jobs) {
        m_jobs.push_back(job);
    }
}

void BackgroundWorker::addInteractionCmdJob(const CmdWrapperSPList jobs)
{
    foreach(auto job, jobs) {
        m_interaction_jobs.push_back(job);
    }
}

void BackgroundWorker::setNeedInteraction(bool need_interaction)
{
    m_need_interaction = need_interaction;
}

void BackgroundWorker::AddCmdJob(std::shared_ptr<CmdWrapper> job)
{
    m_jobs.push_back(job);
}

void BackgroundWorker::slot_reset_common_jobs()
{
    m_connArgs.reset();
    m_rumtime_parameter.reset();
}

void BackgroundWorker::run()
{
    assert(m_connArgs);
    assert(m_mainwindow_callback);
    assert(m_rumtime_parameter);
    bool isOK = true;
    QString error_msg;
    int error_code = 0;
    QString cmd_name;
    callbacks_struct_t cbs;
    try
    {
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(m_connArgs,
                                                                        m_mainwindow_callback,
                                                                        m_rumtime_parameter);
        conn->connect();

        m_mainwindow_callback->setupCmdCallbackStruct(&cbs);

        auto run_one_job = [&](const cmd_wrapper_ptr cmd_wrapper)->void {
            assert(cmd_wrapper);
            cmd_name = cmd_wrapper->get_cmd_xml_generator()->getCmdName();
            cbs.trans._this = static_cast<void *>(&cmd_name);
            cbs.trans.cb_op_progress = &CmdWrapper::cb_operation_process;
            CmdExecutor::execute(conn, cmd_wrapper, &cbs.trans);
        };

        std::for_each(m_jobs.cbegin(), m_jobs.cend(), run_one_job);

        if (m_need_interaction) {
            emit signal_user_interaction_mode();

            //do interaction jobs with user
            while (!m_interaction_finished.load()) {
                qApp->processEvents();
            }

            if (m_mainwindow_callback->cb_notify_stop(nullptr)) {
                error_code = E_CANCEL;
                isOK = false;
            }

            std::for_each(m_interaction_jobs.cbegin(), m_interaction_jobs.cend(), run_one_job);
        }
    } catch (const BaseException &e) {
        error_code = e.err_code();
        error_msg = e.err_msg();
        QString context_error_msg = e.err_msg() + QStringLiteral("\n") + e.context();
        LOG("caught BaseException: %s.", qPrintable(context_error_msg));
        isOK = false;
    } catch (const std::exception &e) {
        error_code = std::numeric_limits<int>::max();
        error_msg = e.what();
        LOG("caught std::exception: %s.", qPrintable(error_msg));
        isOK = false;
    } catch (...) {
        error_code = std::numeric_limits<int>::max();
        error_msg = tr("Unknown error ocurred.");
        LOG(qPrintable(error_msg));
        isOK = false;
    }

    emit signal_current_operation_finished();
    if (isOK) {
        emit signal_current_operation_ok();
    } else {
        emit signal_current_operation_failed(error_code, error_msg);
    }

    resetCurrentJobStatus();
}

void BackgroundWorker::resetCurrentJobStatus()
{
    m_jobs.clear();
    CmdWrapper::clearSelfMap();
    m_need_interaction = false;
    m_interaction_finished.store(true);
    m_interaction_jobs.clear();
}
