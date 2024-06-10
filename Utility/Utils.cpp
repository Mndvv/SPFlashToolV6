#include "Utils.h"

#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QAbstractButton>
#include <QDir>
#include <QFileInfoList>
#include <unordered_map>
#include <algorithm>
#include <QTextCodec>

#include "IniItem.h"
#include "FileUtils.h"
#include "version.h"
#include "../Logger/Log.h"
#include "../Inc/CFlashToolConst.h"

#ifdef _WIN32
#include <Windows.h>
#include <MAPI.h>
#include <TlHelp32.h>
#include <wchar.h>
#endif

#define IDC_BUTTON_HELP                     1
#define IDC_BUTTON_SENDREPORT               2

static const std::unordered_map<std::string, HW_StorageType_E> storage_hash = {
    std::make_pair("UFS", HW_StorageType_E::HW_STORAGE_UFS),
    std::make_pair("EMMC", HW_StorageType_E::HW_STORAGE_EMMC),
    std::make_pair("SDMMC", HW_StorageType_E::HW_STORAGE_SDMMC),
    std::make_pair("NAND", HW_StorageType_E::HW_STORAGE_NAND),
    std::make_pair("NOR", HW_StorageType_E::HW_STORAGE_NOR)
};

std::string Utils::ReplaceAllSubstring(const std::string &ori_str,
                                       const std::string &sub_str,
                                       const std::string &new_sub_str)
{
    std::string temp(ori_str);
    size_t pos(0);
    size_t len_of_sub_str(sub_str.length());
    size_t len_of_new_sub_str(new_sub_str.length());
    while(1) {
        pos = temp.find(sub_str, pos);
        if (pos == std::string::npos) {
            break;
        } else {
            temp.replace(pos, len_of_sub_str, new_sub_str);
            pos += len_of_new_sub_str;
        }
    }
    return temp;
}

std::string Utils::GetSpecifalFolderPath()
{
#ifdef _WIN32
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                       QSettings::NativeFormat);
    QString commonAppDir = reg.value("Common AppData").toString();



    if (!commonAppDir.isEmpty()) {
        return commonAppDir.toStdString() + "\\SP_FT_V6_Logs";
    } else {
        return ABS_PATH("SP_FT_V6_Logs");
    }
#else
    return "/tmp/SP_FT_V6_Logs";
#endif
}

HW_StorageType_E Utils::getHWStorageTypeByName(const std::string &storage_name)
{
    HW_StorageType_E en_hw_storage_type = HW_StorageType_E::HW_STORAGE_NONE;
    if (!storage_name.empty()) {
        std::string upper_storage_name = storage_name;
        std::transform(storage_name.cbegin(), storage_name.cend(), upper_storage_name.begin(), toupper);
        auto iter = storage_hash.find(storage_name);
        if (iter != storage_hash.cend()) {
            en_hw_storage_type = iter->second;
        }
    }
    return en_hw_storage_type;
}

std::string Utils::getStorageNameByType(HW_StorageType_E storage_type)
{
    using storage_name_type__item_type = std::unordered_map<std::string, HW_StorageType_E>::value_type;
    std::string storage_name;
    auto iter = std::find_if(storage_hash.cbegin(), storage_hash.cend(),
                  [&](const storage_name_type__item_type &item)->bool{
        return storage_type == item.second ? true : false;
    });
    if (iter != storage_hash.cend()) {
        storage_name = iter->first;
    }
    return storage_name;
}

std::string Utils::PrettyByteString(unsigned long long bytes)
{
    size_t i = 0;
    char buf[32];
    const char unit[] = "BKMGT";

    // potential overflow!
    bytes *= 100;

    while (bytes >= 1000*100 && i<sizeof(unit)-1)
    {
        bytes /= 1024;
        ++ i;
    }

    sprintf(buf, "%d.%02d%c",
            (int)bytes/100,
            (int)bytes%100,
            (char)unit[i]);

    return buf;
}

void Utils::grabWindow()
{
    QFile file("c:\\errorImage.jpg");
    if (file.exists()){
        file.remove();
    }

    WId winID = QApplication::activeWindow()->winId();
    QPixmap map = QPixmap::grabWindow(winID);
    if(!map.save("c:\\errorImage.jpg"))
        LOGE("grabWindow Fail!");
}

static int message_box_information(QWidget *parent,
                                    const QString &title,
                                    const QString &errMsg,
                                    const QString &btn0_txt)
{
    return QMessageBox::information(parent, title, errMsg, btn0_txt);
}

static int message_box_warning(QWidget *parent,
                               QAbstractButton** pConnectButton,
                               const QString &title,
                               const QString &errMsg,
                               const QString &btn0_txt,
                               const QString &btn1_txt,
                               const QString &btn2_txt,
                               bool report)
{
    int ret = -1;

#ifdef _WIN32
    if(report && !ToolInfo::IsCustomerVer())
    {
        QMessageBox warning_box(QMessageBox::Warning,
                                title,
                                errMsg,
                                QMessageBox::NoButton,
                                parent);
        warning_box.addButton(btn0_txt, QMessageBox::AcceptRole);
        if (!btn1_txt.trimmed().isEmpty())
        {
            warning_box.addButton(btn1_txt, QMessageBox::HelpRole);
        }
        if (!btn2_txt.trimmed().isEmpty())
        {
            warning_box.addButton(btn2_txt, QMessageBox::ActionRole);
        }

        if(pConnectButton && !btn1_txt.trimmed().isEmpty() && !btn2_txt.trimmed().isEmpty())
        {
            *pConnectButton = warning_box.buttons()[2];
            if(*pConnectButton)
            {
                (*pConnectButton)->installEventFilter(parent);
            }
        }

        ret = warning_box.exec();

    }
    else
        ret =  QMessageBox::warning(parent,
                                    title,
                                    errMsg,
                                    btn0_txt, //ok
                                    btn1_txt); //help
#else
    ret =  QMessageBox::warning(parent,
                                title,
                                errMsg,
                                btn0_txt, //ok
                                btn1_txt); //help
#endif

    return ret;
}

static int message_box_critical(QWidget *parent,
                     QAbstractButton** pConnectButton,
                     const QString &title,
                     const QString &errMsg,
                     const QString &btn0_txt,
                     const QString &btn1_txt)
{
    int ret = -1;

#ifdef _WIN32
    if(!ToolInfo::IsCustomerVer())
    {
        QMessageBox critical_box(QMessageBox::Critical,
                                title,
                                errMsg,
                                QMessageBox::NoButton,
                                parent);
        critical_box.addButton(btn0_txt, QMessageBox::AcceptRole);
        if (!btn1_txt.trimmed().isEmpty())
        {
            critical_box.addButton(btn1_txt, QMessageBox::ActionRole);
        }

        if(pConnectButton && !btn1_txt.trimmed().isEmpty())
        {
            *pConnectButton = critical_box.buttons()[1];
            if(*pConnectButton)
            {
                (*pConnectButton)->installEventFilter(parent);
            }
        }

        ret = critical_box.exec();
    }
    else
        ret =  QMessageBox::critical(parent,
                                    title,
                                    errMsg,
                                    btn0_txt); //ok

#else
    ret =  QMessageBox::critical(parent,
                                title,
                                errMsg,
                                btn0_txt); //ok
#endif

    return ret;
}

static int message_box_question(QWidget *parent,
                     const QString &title,
                     const QString &errMsg,
                     const QString &btn0_txt,
                     const QString &btn1_txt)
{
    return QMessageBox::question(parent,
                                 title,
                                 errMsg,
                                 btn0_txt,  //yes, ok
                                 btn1_txt); //no, cancel
}

#ifdef _WIN32
std::vector<std::string> getLogFile(){
    int i = 0;
    QString log = QString::fromLocal8Bit(Logger::GetSPFlashToolLogFolder().c_str());
    QDir dir(log);
    QStringList nameFilter("SP_FT_Dump_*");
    dir.setNameFilters(nameFilter);
    dir.setSorting(QDir::Reversed | QDir::Name);
    QString logDir = dir.entryList().first();

    std::vector<std::string> fileList;

    logDir = log.append("\\").append(logDir);

    LOGI(logDir.toStdString().c_str());

    QDir subDir(logDir);
    QFileInfoList file_list = subDir.entryInfoList(QDir::Files);

    for(; i < file_list.size(); i++){
        QString name = file_list.at(i).absoluteFilePath();
        LOGI(name.toStdString().c_str());
        fileList.push_back(name.toStdString());
    }

    return fileList;
}

static void sendmail(std::string receiver_name)
{
    HINSTANCE hMapI = ::LoadLibraryA("MAPI32.dll");
    if(!hMapI){
        LOGE("Load MAPI32.DLL error");
        return;
    }

    ULONG (PASCAL *SendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
    (FARPROC&)SendMail = GetProcAddress(hMapI, "MAPISendMail");

    if(!SendMail){
        LOGE("Get SendMail function error");
        ::FreeLibrary(hMapI);
        return ;
    }

    std::vector<std::string> logFiles = getLogFile();
    logFiles.push_back("c:\\errorImage.jpg");
    LOGI("Log file count:%d", logFiles.size());

    std::vector<MapiFileDesc> filedesc;

  //  MapiFileDesc fileDesc[5];
    std::string pathName[5];
    std::string fileName[5];

    for(uint i = 0; i < logFiles.size(); i++){
        MapiFileDesc fileDesc;
        ::ZeroMemory(&fileDesc, sizeof(fileDesc));

        fileDesc.nPosition = (ULONG)-1;
        pathName[i] = logFiles.at(i);
        char* path1 = (char*)pathName[i].c_str();

        fileDesc.lpszPathName = path1;
        QFileInfo file(pathName[i].c_str());
        fileName[i] = file.fileName().toStdString();
        char*name1 = (char*)fileName[i].c_str();

        fileDesc.lpszFileName = name1;

        filedesc.push_back(fileDesc);

    }

    MapiMessage message;
    ::ZeroMemory(&message, sizeof(message));
    message.lpszSubject = const_cast<char *>("Flash tool issue");
    message.lpszNoteText = const_cast<char *>("Issue submit");
    message.lpFiles = &filedesc[0];
    message.nFileCount = static_cast<ULONG>(filedesc.size());

    MapiRecipDesc recv = {0};
    recv.ulRecipClass = MAPI_TO;
    std::string receiver_mail_addr = "SMTP:"+receiver_name+"@mediatek.com";
    LOGI(receiver_mail_addr.c_str());
    recv.lpszAddress = const_cast<char*>(receiver_mail_addr.c_str());
    recv.lpszName = const_cast<char*>(receiver_name.c_str());

    message.lpRecips = &recv;
    message.nRecipCount = 1;

    int ret = SendMail(0, 0, &message, /*MAPI_LOGON_UI|*/MAPI_DIALOG, 0);
    if(ret != SUCCESS_SUCCESS )
    {
        LOGE("Failed to send mail, error code: %d", ret);
        if(ret == MAPI_E_ATTACHMENT_OPEN_FAILURE)
        {
            LOGE("Failed to open attachment file.");
        }
    }

    ::FreeLibrary(hMapI);
}
#endif

int Utils::flashtool_message_box(QWidget *parent, QAbstractButton **pConnectButton,
                                 MessageBox_Type msgbox_type, const QString &title,
                                 const QString &errMsg, const QString &btn0_txt,
                                 const QString &btn1_txt, const QString &btn2_txt,
                                 std::string receiver_name, bool report)
{
    int ret = -1;

    switch(msgbox_type)
    {
    case STANDRAD_MSGBOX:
    case INFORMATION_MSGBOX:
        return message_box_information(parent,
                                title,
                                errMsg,
                                btn0_txt);

    case WARNING_MSGBOX:
        ret = message_box_warning(parent,
                                  pConnectButton,
                                  title,
                                  errMsg,
                                  btn0_txt,
                                  btn1_txt,
                                  btn2_txt,
                                  report);
    #ifdef _WIN32
        if(report && ret == IDC_BUTTON_SENDREPORT)
        {
            sendmail(receiver_name);
        }
    #endif
        return ret;

    case CRITICAL_MSGBOX:
        ret = message_box_critical(parent,
                                   pConnectButton,
                                   title,
                                   errMsg,
                                   btn0_txt,
                                   btn1_txt);
    #ifdef _WIN32
        if(report && ret == 1) //report
        {
            sendmail(receiver_name);
        }
    #endif

        return ret;

    case QUESTION_MSGBOX:
        return message_box_question(parent,
                                    title,
                                    errMsg,
                                    btn0_txt,
                                    btn1_txt);

    default:
        return ret;
    }
}

#ifdef _WIN32
bool Utils::IsMoreThanOneProcessByName(const QString &processName)
{
    int nProcessCnt = 0;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        LOGD("CreateToolhelp32Snapshot fail");
        return nProcessCnt > 1;
    }

    PROCESSENTRY32 processEntry32;
    processEntry32.dwSize = sizeof(PROCESSENTRY32);
    bool bProcess = Process32First(hProcessSnap, &processEntry32);
    while (bProcess)
    {
        if (_wcsicmp(processEntry32.szExeFile, processName.toStdWString().c_str()) == 0)
        {
            LOGD("ProcessName: %s, ProcessID: %d",
                 QString::fromWCharArray(processEntry32.szExeFile).toStdString().c_str(), processEntry32.th32ProcessID);
            ++nProcessCnt;
        }
        bProcess = Process32Next(hProcessSnap, &processEntry32);
    }
    LOGD("Flash Tool total opened process count: %d", nProcessCnt);
    CloseHandle(hProcessSnap);
    return nProcessCnt > 1;
}
#endif

QString Utils::getSystemOS()
{
    QString system_os("Unknown");
#ifdef Q_OS_WIN
    system_os = "WINDOWS";
#elif defined(Q_OS_LINUX)
    system_os = "LINUX";
#endif
    return system_os;
}

void Utils::setTextCodec()
{
#ifdef _WIN32
    QTextCodec *coder = QTextCodec::codecForName("System");
#else
    QTextCodec *coder = QTextCodec::codecForName("UTF-8");
#endif
    QTextCodec::setCodecForLocale(coder);
}

QStringList Utils::getEMMCUFSRegionList()
{
    static QStringList region_lables = {EMMC_BOOT1_VAL, EMMC_BOOT2_VAL, EMMC_USER_VAL,
                                 UFS_LUA0_VAL, UFS_LUA1_VAL, UFS_LUA2_VAL};
    return region_lables;
}

QStringList Utils::getNANDRegionList()
{
    static QStringList region_lables = {NAND_WHOLE_VAL};
    return region_lables;
}

QStringList Utils::getNANDBMTRegionList()
{
    static QStringList region_lables = {NAND_WHOLE_VAL, NAND_BMT_VAL}; // NAND-BMT only for NAND format operation.
    return region_lables;
}

QString Utils::transBoolToYESNO(bool bVal)
{
    return bVal ? QStringLiteral("YES") : QStringLiteral("NO");
}
