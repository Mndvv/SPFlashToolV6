#include "ConsoleModeRunner.h"
#include "../Logger/Log.h"
#include "../Conn/Connection.h"
#include "../Cmd/CmdExecutor.h"
#include "../Err/Exception.h"
#include "../Utility/Utils.h"
#include "../Inc/images_checker.h"
#include <cassert>

namespace ConsoleMode
{

ConsoleModeRunner::ConsoleModeRunner():
    m_cmd_callback(new ConsoleModeCallBack())
{

}

void ConsoleModeRunner::execute(const Config &config) const
{
    std::shared_ptr<ConnectionArgs> conn_args = config.getGeneralSetting()->getConnArgs();
    std::shared_ptr<SetRuntimeParaCmdXML> runtime_parameter_setting =
            config.getGeneralSetting()->getRuntimeParameterSetting();
    assert(conn_args);
    assert(runtime_parameter_setting);

    std::shared_ptr<Connection> conn = std::make_shared<Connection>(conn_args, m_cmd_callback, runtime_parameter_setting);
    conn->connect();

    QString cmd_name;
    callbacks_struct_t cbs;
    m_cmd_callback->setupCmdCallbackStruct(&cbs);

    std::list<std::shared_ptr<CmdWrapper>> cmd_wrapper_list = config.getCommandSetting()->getCmdWrapperList();
    for(auto iter = cmd_wrapper_list.cbegin(); iter != cmd_wrapper_list.cend(); ++iter)
    {
        std::shared_ptr<CmdWrapper> current_cmd_wrapper = *iter;
        assert(current_cmd_wrapper);
        cmd_name = current_cmd_wrapper->get_cmd_xml_generator()->getCmdName();
        cbs.trans._this = static_cast<void *>(&cmd_name);
        cbs.trans.cb_op_progress = &CmdWrapper::cb_operation_process;
        CmdExecutor::execute(conn, current_cmd_wrapper, &cbs.trans);
    }
}

void ConsoleModeRunner::imageChksumVerify(const QString &scatter_file) const
{
    struct callback_transfer_t cb_chksum;
    m_cmd_callback->setupChksumCallbackStruct(&cb_chksum);
    int result = flashtool_images_checker_verify(scatter_file.toStdString().c_str(), &cb_chksum);
    if (result == STATUS_IMAGE_CHK_SUCCEEDED || result == STATUS_IMAGE_CHK_XML_NOT_FOUND) {
        return ;
    }
    // deal with checksum error, included user cancel operation.
    if (result == STATUS_IMAGE_CHK_CANCELLED) {
        THROW_APP_EXCEPTION(result, QStringLiteral("load flash xml file cancelled."))
    } else {
        QString error_msg("image checksum mismatch");
        THROW_APP_EXCEPTION(result, error_msg)
    }
}

void ConsoleModeRunner::cbFormatProgress(void *_this, unsigned int progress,
                                         unsigned long long data_xferd, const QString &info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    static unsigned int last_percentage = 0;
    if (progress - last_percentage >= 5)
    {
        LOGI("%d%% of flash has been formatted(about %s).", progress, Utils::PrettyByteString(data_xferd).c_str());
        last_percentage = progress;
    }
}

void ConsoleModeRunner::cbDownloadProgress(void *_this, unsigned int progress, unsigned long long data_xferd, const QString &info)
{
    Q_UNUSED(_this)
    static unsigned int lastReportedProgress = 0;

    //change partition
    if (progress <= lastReportedProgress) {
        lastReportedProgress = progress;
    }

    unsigned int default_step = 5;
    if (info.contains(QStringLiteral("super")) || info.contains(QStringLiteral("userdata"))) {
        default_step = 1;
    }
    if (progress - lastReportedProgress >= default_step)
    {
        QString image_name = info;
        QString progress_text = QString("%1%% of [%2] data has been sent (about %3)").
                arg(progress).arg(image_name).arg(Utils::PrettyByteString(data_xferd).c_str());
        if (image_name.isEmpty()) {
            progress_text = QString("%1%% of image data has been sent (about %2)").
                            arg(progress).arg(Utils::PrettyByteString(data_xferd).c_str());
        }
        LOGI(qPrintable(progress_text));
        lastReportedProgress = progress;
    }
}

void ConsoleModeRunner::cbReadbackProgress(void *_this, unsigned int progress, unsigned long long data_xferd, const QString &info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    static unsigned int lastReportedProgress = 0;

    //change partition
    if (progress <= lastReportedProgress) {
        lastReportedProgress = progress;
    }

    if (progress - lastReportedProgress >= 5) {
        LOGI("%d%% of data readback to PC(about %s)", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

void ConsoleModeRunner::cbWriteMemoryProgress(void *_this, unsigned int progress, unsigned long long data_xferd, const QString &info)
{
    Q_UNUSED(_this)
    Q_UNUSED(info)
    static unsigned int lastReportedProgress = 0;
    if(progress - lastReportedProgress >= 5)
    {
        LOGI("%d%% of data write to memory(about %s).", progress, Utils::PrettyByteString(data_xferd).c_str());
        lastReportedProgress = progress;
    }
}

}
