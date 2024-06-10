#include "LogInfoSetting.h"
#include "../Err/Exception.h"
#include "Utils.h"

static const QString LOG_INFO_COMMENT("log_on: log switch, enable log if true, otherwise false;\n"
                                   "\t\t\tlog_path: the directory in which the log files has been stored;\n"
                                   "\t\t\tclean_hours: the time setting to delete log files regularly, the unit is hours.");

LogInfoSetting::LogInfoSetting():
    m_logPath(Utils::GetSpecifalFolderPath().c_str()),
    m_cleanHours(7*24)
{

}

void LogInfoSetting::loadXML(const QDomElement &xml_node)
{
    QDomElement log_info_node = xml_node.firstChildElement(QStringLiteral("log-info"));
    if (log_info_node.isNull()) {
        THROW_APP_EXCEPTION(-1, QStringLiteral("log-info node not exists."))
    }

    m_logPath = log_info_node.attribute(QStringLiteral("log_path"));
    m_cleanHours = log_info_node.attribute(QStringLiteral("clean_hours")).toInt();
}

void LogInfoSetting::saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const
{
    QDomComment comment_node = xml_doc.createComment(LOG_INFO_COMMENT);
    xml_node.appendChild(comment_node);

    QDomElement log_info_node = xml_doc.createElement(QStringLiteral("log-info"));
    log_info_node.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    xml_node.appendChild(log_info_node);

    log_info_node.setAttribute(QStringLiteral("log_path"), m_logPath);
    log_info_node.setAttribute(QStringLiteral("clean_hours"), m_cleanHours);
}

void LogInfoSetting::setLogPath(const QString &sLogPath)
{
    m_logPath = sLogPath;
}

void LogInfoSetting::setCleanHours(qint64 nCleanHours)
{
    m_cleanHours = nCleanHours;
}

QString LogInfoSetting::getLogPath() const
{
    return m_logPath;
}

qint64 LogInfoSetting::getCleanHours() const
{
    return m_cleanHours;
}
