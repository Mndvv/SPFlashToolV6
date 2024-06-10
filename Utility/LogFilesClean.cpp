#include "LogFilesClean.h"
#include <QDir>
#include <QStringList>
#include <QList>
#include <QThread>
#include <QDateTime>
#include "FileUtils.h"
#include "../Logger/Log.h"

#define TIME_START_INDEX 14

class LogCleanThread: public QThread
{
public:
    LogCleanThread(const QString &logPath, qint64 nHours, QObject *parent = nullptr);
    ~LogCleanThread();

protected:
    virtual void run();

private:
    QDateTime getReferenceDT() const;

private:
    QString m_logPath;
    qint64 m_hours;
};

LogCleanThread::LogCleanThread(const QString &logPath, qint64 nHours, QObject *parent):
    QThread(parent),
    m_logPath(logPath),
    m_hours(nHours)
{

}

LogCleanThread::~LogCleanThread()
{
    this->terminate();
    this->wait();
}

void LogCleanThread::run()
{
    QDir dir(m_logPath);
    QStringList nameFilters("SP_FT_V6_Dump_*");
    dir.setNameFilters(nameFilters);
    dir.setSorting(QDir::Reversed | QDir::Name);
    QStringList list = dir.entryList();

    QList<QString>::iterator it = list.begin();
    QDateTime referenceDT = this->getReferenceDT();
    LOGD(referenceDT.toString("MM-dd-yyyy-HH-mm-ss").toStdString().c_str());

    for(; it != list.end(); ++it)
    {
        QString sdatetime = (*it).mid (TIME_START_INDEX);//remove "SP_FT_V6_Dump_"
        QDateTime datetime = QDateTime::fromString(sdatetime, "MM-dd-yyyy-HH-mm-ss");
        LOGD(datetime.toString("MM-dd-yyyy-HH-mm-ss").toStdString().c_str());
        if(referenceDT >= datetime)
        {
            QString dir_path = m_logPath;
            dir_path = dir_path.append("/").append(*it);
            dir_path = QDir::toNativeSeparators(dir_path);

            if(FileUtils::QDeleteDirectory(dir_path.toStdString()) == false)
            {
                LOGD("remove file %s failed.", (*it).toLocal8Bit().constData());
            }
        }
    }
}

QDateTime LogCleanThread::getReferenceDT() const
{
    qint64 nSeconds = m_hours * 60 * 60;
    return QDateTime::currentDateTime().addSecs(nSeconds);
}

void LogCleaner::cleanLogFiles(const QString &log_path, qint64 clean_hours)
{
    LogCleanThread * p_workerThread = new LogCleanThread(log_path, clean_hours);
    QObject::connect(p_workerThread, &LogCleanThread::isFinished, p_workerThread, &QObject::deleteLater);
    p_workerThread->start(QThread::LowPriority);
}

void LogCleaner::cleanLogFiles(const QString &log_path, const QString &time_option)
{
    cleanLogFiles(log_path, GetHoursFromTimeOption(time_option));
}

qint64 LogCleaner::GetHoursFromTimeOption(const QString &time_option)
{
    QDateTime date = QDateTime::currentDateTime();
    if (time_option == QObject::tr("One Week")) {
        date = QDateTime::currentDateTime().addDays(-7);
    } else if (time_option == QObject::tr("One Month")) {
        date = QDateTime::currentDateTime().addMonths(-1);
    } else if (time_option == QObject::tr("Three Month")) {
        date = QDateTime::currentDateTime().addMonths(-3);
    } else {
        date = QDateTime::currentDateTime().addDays(-1);
    }
    qint64 nDays = QDateTime::currentDateTime().daysTo(date);
    return nDays * 24;
}
