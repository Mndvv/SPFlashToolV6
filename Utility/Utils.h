#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <QAbstractButton>
#include "../Inc/CFlashToolTypes.h"

typedef enum{
    STANDRAD_MSGBOX = 0,
    INFORMATION_MSGBOX = 1,
    WARNING_MSGBOX = 2,
    CRITICAL_MSGBOX = 3,
    QUESTION_MSGBOX = 4
}MessageBox_Type;

namespace Utils
{
std::string ReplaceAllSubstring(const std::string &ori_str, const std::string &sub_str, const std::string &new_sub_str);
std::string GetSpecifalFolderPath();
HW_StorageType_E getHWStorageTypeByName(const std::string &storage_name);
std::string getStorageNameByType(HW_StorageType_E storage_type);
std::string PrettyByteString(unsigned long long bytes);

void grabWindow();

int flashtool_message_box(QWidget *parent,
                          QAbstractButton **pConnectButton,
                           MessageBox_Type msgbox_type,
                           const QString &title,
                           const QString &errMsg,
                           const QString &btn0_txt,
                           const QString &btn1_txt = "",
                           const QString &btn2_txt = "",
                           std::string receiver_name = "",
                           bool report = false);

#ifdef _WIN32
bool IsMoreThanOneProcessByName(const QString &processName);
#endif
QString getSystemOS();
void setTextCodec();
QStringList getEMMCUFSRegionList();
QStringList getNANDRegionList();
QStringList getNANDBMTRegionList();
QString transBoolToYESNO(bool bVal);
}

#endif // UTILS_H
