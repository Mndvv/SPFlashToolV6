#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <iostream>
#include <list>
#include <stdio.h>
#include <string>
#include <QString>
#include <functional>

enum E_CHECK_DIR_STATUS{
    DIR_EXIST,
    DIR_NOT_EXIST,
    CREATE_DIR_SUCCESS,
    CREATE_DIR_FAIL,
    DELETE_DIR_FAIL
};

namespace FileUtils {

    //Common Directory OPs
    E_CHECK_DIR_STATUS CheckDirectory(const std::string &dir, bool createIfNotExist = true);

    bool IsFileExist(const std::string &file);
    bool QDeleteDirectory(const std::string &dir);

    //Application related OPs
    static std::string g_app_path;
    const std::string &GetAppDirectory();
    void UpdateAppPath(const std::string &app_path);

    //make absolute path relative to tool dir
    std::string AbsolutePath(const char *name);
    std::string AbsolutePath(const std::string &name);

#define ABS_PATH        FileUtils::AbsolutePath
#define ABS_PATH_C(x)   ABS_PATH(x).c_str()

    //File Search OPs
    using COMPARE_METHOD = std::function<bool (const std::string &file_name, const std::string &pattern)>;
    bool FindFile(const std::string& path, std::list<std::string> &files, const std::string& pattern,
                  COMPARE_METHOD compare_m = NULL, bool recursive = false);

    void DebugLogsOn(void);
    void DebugLogsOff(void);

#if defined(_LINUX64)
    void copy_99ttyacms_file(bool need_check_first_run = false);
    bool IsValidKernelVersion();
    void runModemManagerCmd();
#endif

    // new added
    bool validFile(const QString &file_name, QString &error_msg);
    QString FWBackupFolder();
    QString generateRBFileName(const QString &file_name = QString());
}

#endif // FILEUTILS_H
