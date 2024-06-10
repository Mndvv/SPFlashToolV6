#include "FileUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <sys/io.h>
#include <sys/utsname.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cerrno>
#include <cstring>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include "../Host/Inc/RuntimeMemory.h"
#include "../Utility/Utils.h"
#include "../Utility/OptionIniFileMgr.h"
#include "../Logger/Log.h"
#include "../Inc/CFlashToolAPI.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

namespace FileUtils{
using std::string;

#if defined(_LINUX64)
#define RULES_PATH "/etc/udev/rules.d"
#define TTYACMS_FILE "99-ttyacms.rules"
#define KERNEL_INVALID_MIN_VERSION std::string("4.11.12")
#define KERNEL_INVALID_MAX_VERSION std::string("4.19.14")
#endif
#define HISTORY_INI_FILE "history.ini"

static const std::string kBackupFolder("BackupData");
static const QString kDefaultFilePrefix = "ROM_";
static const QString kReadbackFolder("Readback");
}

E_CHECK_DIR_STATUS FileUtils::CheckDirectory(const std::string &dir, bool createIfNotExist) {
    QDir dir_obj(QString::fromStdString(dir));
    if(dir_obj.exists()){
        LOG("%s(): the dir(%s) already exists.", __FUNCTION__, qPrintable(dir_obj.absolutePath()));
        return DIR_EXIST;
    }

    if(!createIfNotExist){
        LOG("%s(): the dir(%s) does not exist.", __FUNCTION__, qPrintable(dir_obj.absolutePath()));
        return DIR_NOT_EXIST;
    }

    if(!dir_obj.mkpath(dir_obj.absolutePath())){
        LOG("%s(): create the dir(%s) failed.", __FUNCTION__, qPrintable(dir_obj.absolutePath()));
        return CREATE_DIR_FAIL; //create directory fail
    }
    else
    {
#ifdef _WIN32
        _chmod(dir.c_str(), _S_IREAD | _S_IWRITE);
#else
        chmod(dir.c_str(),  S_IXUSR |S_IRUSR | S_IWUSR |
              S_IXGRP |S_IRGRP | S_IWGRP |
              S_IXOTH |S_IROTH | S_IWOTH );
#endif
        LOG("%s(): create the dir(%s) succeed.", __FUNCTION__, qPrintable(dir_obj.absolutePath()));
        return CREATE_DIR_SUCCESS; //create directory successfully
    }
}

/**
 * Check whether the file exists
 *
 * @param file to be checked
 *
 * @return whether the file exists
 */
bool FileUtils::IsFileExist(const  string & file) {
    struct stat fileInfo;
    int ret = stat(file.c_str(), &fileInfo);
    return ( (ret == 0) && ((fileInfo.st_mode & S_IFMT) == S_IFREG));
}

/**
 * Delete a directory
 *
 * @param directory to be deleted
 *
 * @return whether the directory is deleted
 */

bool FileUtils::QDeleteDirectory(const std::string &dir_name)
{
    bool result = true;
    QString dir_temp(dir_name.c_str());
    QDir dir(dir_temp);

    if (dir.exists(dir_temp)) {
        Q_FOREACH(QFileInfo info,
                  dir.entryInfoList(QDir::NoDotAndDotDot
                                    | QDir::System
                                    | QDir::Hidden
                                    | QDir::AllDirs
                                    | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = QDeleteDirectory(info.absoluteFilePath().toLocal8Bit().constData());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dir_temp);
    }

    return result;
}

const std::string &FileUtils::GetAppDirectory()
{
    return g_app_path;
}

void FileUtils::UpdateAppPath(const std::string &app_path) {
    g_app_path = app_path;
}

std::string FileUtils::AbsolutePath(const char *name)
{
    return g_app_path + QDir::separator().toLatin1() + name;
}

std::string FileUtils::AbsolutePath(const std::string &name)
{
    return g_app_path + QDir::separator().toLatin1() + name;
}

bool FileUtils::FindFile(const std::string& path, std::list<std::string> &files,
                         const std::string& pattern, COMPARE_METHOD compare_m, bool recursive)
{
    QDir dir(path.c_str());

    if(!dir.exists())
        return false;

    dir.setFilter(QDir::Dirs | QDir::Files);

    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    bool isDirectory;
    std::string fileName;

    if(list.empty())
        return false;

    do
    {
        QFileInfo fileInfo = list.at(i);

        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {
            i++;
            continue;
        }

        isDirectory = fileInfo.isDir();

        if(isDirectory && recursive)
        {
            FindFile(fileInfo.filePath().toStdString(), files, pattern, compare_m, recursive);
        }
        else
        {
            fileName = fileInfo.fileName().toStdString();

            if(compare_m)
            {
                if(compare_m(fileName, pattern))
                {
                    files.push_back(fileName);
                }
            }
            else
            {
                if(pattern == "")
                {
                    files.push_back(fileName);
                }
                else if(pattern == fileName)
                {
                    files.push_back(fileName);
                }
            }
        }

        i++;
    }while(i < list.size());

    return true;
}

/**
 * Set debug log file on
 *
 * @param NULL
 *
 * @return NULL
 */
void FileUtils::DebugLogsOn( void )
{
    //std::string dll_log;
    Logger::SetSPFlashToolDumpFileFolder();
    Logger::DebugOn();

    //lib log path
    QString log_folder = QString::fromStdString(Logger::GetSPFlashToolDumpFileFolder());
    CFlashToolAPI::cflashtool_env_set_log(logging_level_e::kdebug, log_folder);
}

/**
 * Set debug log file on
 *
 * @param NULL
 *
 * @return NULL
 */
void FileUtils::DebugLogsOff( void ) {
    Logger::DebugOff();
    QString log_folder = QString::fromStdString(Logger::GetSPFlashToolDumpFileFolder());
    CFlashToolAPI::cflashtool_env_set_log(logging_level_e::koff, log_folder);
}

#if defined(_LINUX64)
void FileUtils::copy_99ttyacms_file(bool need_check_first_run/* = false*/)
{
    if (need_check_first_run) {
        QString history_file = QString::fromStdString(AbsolutePath(std::string(HISTORY_INI_FILE)));
        history_file = QDir::toNativeSeparators(history_file);
        if (IsFileExist(history_file.toStdString())) { // not the first to run flash tool GUI, skip copy
            LOG("skip to copy 99-ttyacms.rules file, reason: the tool not the first time to run!");
            return;
        }
    }
    // linux modemmanager issue fixed: copy 99-ttyacms.rules file to /etc/udev/rules.d directory.
    QString src_file = QString::fromStdString(AbsolutePath(std::string(TTYACMS_FILE)));
    src_file = QDir::toNativeSeparators(src_file);
    if (IsFileExist(src_file.toStdString())) {
        if (QDir(std::string(RULES_PATH).c_str()).exists() && QFileInfo(QString(RULES_PATH)).isWritable()) {
            QString dest_file = QString(RULES_PATH) + QDir::separator().toLatin1() + QString(TTYACMS_FILE);
            dest_file = QDir::toNativeSeparators(dest_file);
            if (!IsFileExist(dest_file.toStdString())) {
                if (QFile::copy(src_file, dest_file)) {
                    LOG("copy 99-ttyacms.rules file successfully!");
                } else {
                    LOG("copy 99-ttyacms.rules file failed, reason: %s", strerror(errno));
                }
            } else {
                LOG("skip to copy 99-ttyacms.rules file, reason: %s has already exist!", dest_file.toStdString().c_str());
            }
        } else {
            LOG("skip to copy 99-ttyacms.rules file, reason: %s has no write permission!", std::string(RULES_PATH).c_str());
        }
    } else {
        LOG("skip to copy 99-ttyacms.rules file, reason: %s not exist!", src_file.toStdString().c_str());
    }
}

void DumpKernelInfo(const struct utsname &kernel_info)
{
    printf("sysname:%s\n nodename:%s\n release:%s\n version:%s\n machine:%s\n \n ",\
                        kernel_info.sysname,\
                        kernel_info.nodename,\
                        kernel_info.release,\
                        kernel_info.version,\
                        kernel_info.machine);
#if _UTSNAME_DOMAIN_LENGTH - 0
#ifdef __USE_GNU
    printf(" domainame:%s\n ",kernel_info.domainname);
    //char domainname[_UTSNAME_DOMAIN_LENGTH];
#else
    printf(" __domainame:%s\n ",kernel_info.__domainname);
    //char __domainname[_UTSNAME_DOMAIN_LENGTH];
#endif
#endif
}

bool FileUtils::IsValidKernelVersion()
{
    struct utsname buf;
    int ret = uname(&buf);
    if (ret < 0) {
        perror("uname function call failed.");
        return true; //ignore to check linux kernel version
    }
    else {
        DumpKernelInfo(buf);

        std::string kernel_release = buf.release;
        std::size_t found = kernel_release.find('-');
        if (found == std::string::npos) {
            perror("invalid kernel release string");
            return true;
        }
        std::string cur_kernel_version = kernel_release.substr(0, found);
        std::cout << "cur_kernel_version = " << cur_kernel_version << std::endl;
        bool is_valid_kernel = (KERNEL_INVALID_MIN_VERSION > cur_kernel_version
                || cur_kernel_version >= KERNEL_INVALID_MAX_VERSION);
        return is_valid_kernel;
    }
}

void FileUtils::runModemManagerCmd()
{
    if (OptionIniFileMgr::getInstance()->getRun_modemManager_cmd()) {
        std::string modemmanager_cmd_file = FileUtils::AbsolutePath("modemmanagercmd.sh");
        if (QFileInfo(QString::fromStdString(modemmanager_cmd_file)).exists()) {
            system("sudo chmod +x modemmanagercmd.sh");
            system("sudo ./modemmanagercmd.sh &");
            LOG("running \"sudo ./modemmanagercmd.sh &\" cmd in background!");
        } else {
            LOG("modemmanagercmd.sh file is NOT exists.");
        }
    } else {
        LOG("NO need to run \"sudo ./modemmanagercmd.sh &\" cmd.");
    }
}
#endif


//new added
bool FileUtils::validFile(const QString &file_name, QString &error_msg)
{
    QFileInfo fileinfo(file_name);
    if (file_name.isEmpty()) {
        error_msg = QObject::tr("filename is empty.").arg(file_name);
        return false;
    }
    if (!fileinfo.exists(file_name)) {
        error_msg = QObject::tr("%1 is not exist.").arg(file_name);
        return false;
    }
    if (!fileinfo.isFile()) {
        error_msg = QObject::tr("%1 is not a regular file.").arg(file_name);
        return false;
    }
    if (!fileinfo.isReadable()) {
        error_msg = QObject::tr("%1 is not a readable file.").arg(file_name);
        return false;
    }
    return true;
}

QString FileUtils::FWBackupFolder()
{
    return FileUtils::AbsolutePath(FileUtils::kBackupFolder).c_str();
}

QString FileUtils::generateRBFileName(const QString &file_name)
{
    QString path = FileUtils::GetAppDirectory().c_str();
    path += QDir::separator() + kReadbackFolder;
    QDir().mkdir(path);
    if (file_name.isEmpty()) {
        static int file_index = 0;
        path += QDir::separator() + kDefaultFilePrefix + QString::number(file_index++);
    } else {
        path += QDir::separator() + file_name;
    }
    path = QDir::toNativeSeparators(path);
    return path;
}
