#ifndef LOGFILESCLEAN_H
#define LOGFILESCLEAN_H

#include <QString>

class LogCleaner
{
public:
    static void cleanLogFiles(const QString &log_path, qint64 clean_hours);
    static void cleanLogFiles(const QString &log_path, const QString &time_option);
    static qint64 GetHoursFromTimeOption(const QString &time_option);
};

#endif //LOGFILESCLEAN_H
