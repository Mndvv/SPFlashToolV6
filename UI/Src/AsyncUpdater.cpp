#ifdef UNICODE
#undef UNICODE
// using ShellExecuteExA to startup and forward paramters for SP_Updater.exe program written by python.
#include <Windows.h>
#define UNICODE
#endif

#include "AsyncUpdater.h"
#include <QRunnable>
#include <QThreadPool>
#include <QDir>
#include <QStringList>
#include <QApplication>
#include <QTextStream>
#include <QSettings>
#include <algorithm>
#include <QDebug>
#include <QFuture>
#include <QtConcurrentRun>
#include <functional>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

#include "../../Utility/version.h"
#include "../../Utility/FileUtils.h"
#include "../../Host/Inc/RuntimeMemory.h"
#include "../../Utility/Utils.h"
#include "../../Utility/IniItem.h"
#include "../../Logger/Log.h"

#define STR_RELEASE_FOLDER  "Windows"
#define Tool_Version_key            "Version"
#define New_Feature_key             "New-Features/Enhancements"
#define Bug_Fixed_Key               "Bug-Fixed"
#define Limitation_Key              "Limitations"
#define Limitation_Bringup_SubKey   "Bringup"
#define Limitation_Update_SubKey    "Update"
#define Support_Platform_Key        "Support-Platforms"

using namespace std::placeholders;

class CheckUpdateRunnable : public QRunnable
{
public:
    CheckUpdateRunnable(AsyncUpdater *updater):updater_(updater)
    {

    }

    ~CheckUpdateRunnable()
    {

    }

    virtual void run();

private:
    AsyncUpdater *updater_;
};

void CheckUpdateRunnable::run()
{
    updater_->CheckUpdate();
    updater_->CheckFinish();
}

class UpdateRunnable:public QRunnable
{
public:
    UpdateRunnable(AsyncUpdater *updater):updater_(updater)
    {

    }

    ~UpdateRunnable()
    {

    }

    virtual void run();

    AsyncUpdater *updater_;
};

void UpdateRunnable::run()
{
    updater_->UpdateTool();
}

void AsyncUpdater::parserUpdateInfo()
{
    IniItem item(QStringLiteral("update.ini"), QStringLiteral("Exe Info"), QStringLiteral("path"));
    m_release_folder = item.GetStringValue().toStdString();
    item.SetItemName(QStringLiteral("name"));
    m_updater_exe = item.GetStringValue().toStdString();
    item.SetItemName(QStringLiteral("releaseNote"));
    m_release_note_file = item.GetStringValue().toStdString();
    item.SetItemName(QStringLiteral("ToolName"));
    m_tool_name = item.GetStringValue().toStdString();

    item.SetSectionName(QStringLiteral("BRING UP"));
    item.SetItemName(QStringLiteral("currentVersion"));
    m_current_version = item.GetStringValue().toStdString();
}

AsyncUpdater::AsyncUpdater(QObject *parent):
    QObject(parent),
    m_has_new_version(false)
{
    parserUpdateInfo();
}

AsyncUpdater::~AsyncUpdater()
{

}

void AsyncUpdater::AsyncUpdateTool()
{
    QRunnable *task = new UpdateRunnable(this);
    QThreadPool::globalInstance()->start(task);
}

void AsyncUpdater::UpdateTool()
{
    if (UpdateToolsPreCheck())
    {
        DoUpdateTool();
    }
    else {
        emit signal_update_failed(lack_of_update_files);
    }
}

void AsyncUpdater::CheckAsyncNewVersion()
{
    QRunnable *task = new CheckUpdateRunnable(this);
    future = QtConcurrent::run(task, &QRunnable::run);
}

void AsyncUpdater::waitForFinished()
{
    future.waitForFinished();
}

void AsyncUpdater::CheckUpdate()
{
    std::vector<std::string> allVersions;
    GetAllVersions(allVersions);
    if(allVersions.empty())
        return;

    std::sort(allVersions.begin(), allVersions.end());
    std::string latestVer = allVersions.back();

    bool bNeedUpdate = m_current_version.empty() ? latestVer.compare(ToolInfo::VersionNum()) > 0 : latestVer.compare(m_current_version) > 0;
    if(bNeedUpdate)
    {
        std::list<std::string> files;

        std::string tool_zip_folder = m_release_folder + QDir::separator().toLatin1() + latestVer + QDir::separator().toLatin1() + STR_RELEASE_FOLDER;
        FileUtils::FindFile(tool_zip_folder, files, "", std::bind(&AsyncUpdater::IsUpdatePackage, this, _1, _2));
        if(!files.empty())
        {
            m_has_new_version = true;
            m_new_version = latestVer;
            m_new_version_folder = tool_zip_folder;
            m_new_version_package = m_new_version_folder + QDir::separator().toLatin1() + files.front();

            ReadReleaseNotes(m_new_version_folder + QDir::separator().toLatin1() + m_release_note_file);
        }
    }
}

void AsyncUpdater::ConstructReleaseNotes(const QJsonObject &jsonObject)
{
    QString tool_version = jsonObject[Tool_Version_key].toString();
    if(!tool_version.isEmpty()){
        m_release_notes = QStringLiteral("Tool version: \n    ")  + tool_version + QStringLiteral("\n\n");
    }

    QJsonArray new_featureList = jsonObject[New_Feature_key].toArray();

    if (new_featureList.count() > 0) {
        m_release_notes += New_Feature_key;
        m_release_notes += QStringLiteral("\n");
        foreach(auto feature, new_featureList){
            m_release_notes += QStringLiteral("   ") + feature.toString() + QStringLiteral("\n\n");
        }
    }

    QJsonArray bug_fixedList = jsonObject[Bug_Fixed_Key].toArray();

    if(bug_fixedList.count() > 0){
        m_release_notes += Bug_Fixed_Key;
        m_release_notes += QStringLiteral("\n");

        foreach(auto bug, bug_fixedList){
            m_release_notes += QStringLiteral("   ") + bug.toString() + QStringLiteral("\n\n");
        }
    }
}

bool AsyncUpdater::UpdateToolsPreCheck() const
{
    std::string tool_path = ABS_PATH("update.ini");
    if (!FileUtils::IsFileExist(tool_path))
    {
        return false;
    }
    std::string updater_tool = ABS_PATH(m_updater_exe + ".exe");
    return FileUtils::IsFileExist(updater_tool);
}

void AsyncUpdater::DoUpdateTool()
{
    std::string updater = ABS_PATH(m_updater_exe);

    QString exePath = QString(updater.c_str());

    QStringList parameter;

    parameter << m_new_version_package.c_str();

    bool bSuccess = false;
    // QSysInfo::WV_6_1 means WV_WINDOWS7, Operating system version 6.1, corresponds to Windows 7 and Windows Server 2008 R2.
    if (QSysInfo::windowsVersion() > QSysInfo::WV_6_1)
    {
        bSuccess = this->RunAsAdmin(exePath, parameter);
    }
    else
    {
        QProcess process;
        QObject::connect(&process, SIGNAL(error(QProcess::ProcessError)),
                         this, SLOT(processError(QProcess::ProcessError)));
        bSuccess = process.startDetached(exePath, parameter);
    }

    if (bSuccess)
    {
        QApplication::exit(0);
    }
    else
    {
        emit signal_update_failed(start_update_exe_fail);
    }
}

bool AsyncUpdater::RunAsAdmin(const QString &appName, const QStringList &paramterList) const
{
    QString paramters_arg;
    for (int i = 0; i < paramterList.size(); ++i)
    {
        paramters_arg += QString("\"%1\"").arg(paramterList[i]);
        if (i != paramterList.size() - 1) {
            paramters_arg += QStringLiteral(" ");
        }
    }

    paramters_arg = QDir::toNativeSeparators(paramters_arg);
    LOG("paramter argument: %s", paramters_arg.toStdString().c_str());

    SHELLEXECUTEINFO execinfo;
    memset(&execinfo, 0, sizeof(execinfo));
    execinfo.lpFile         = appName.toStdString().c_str();
    execinfo.cbSize         = sizeof(execinfo);
    execinfo.lpVerb         = TEXT("runas");
    execinfo.fMask          = SEE_MASK_NO_CONSOLE;
    execinfo.nShow          = SW_SHOWDEFAULT;
    execinfo.lpParameters   = paramters_arg.toStdString().c_str();

    BOOL bSuccess = ShellExecuteEx(&execinfo);
    if (!bSuccess) {
        LOG("ShellExecuteEx error code: %d", GetLastError());
    }
    return static_cast<bool>(bSuccess);
}

void AsyncUpdater::ReadReleaseNotes(const std::string &path)
{
    QJsonObject jsonObject;

    if (!parseJsonFile(path, jsonObject)) {
        m_has_new_version = false;
        return ;
    }

    ConstructReleaseNotes(jsonObject);

    std::string curr_releaseFilePath = ABS_PATH_C("Release.json");
    QJsonObject currJsonObject;

    parseJsonFile(curr_releaseFilePath, currJsonObject);

    QJsonObject limitations = currJsonObject[Limitation_Key].toObject();

    QString bringup_chip = limitations[Limitation_Bringup_SubKey].toString();

    QJsonArray platforms = jsonObject[Support_Platform_Key].toArray();

    if(!bringup_chip.isEmpty() && !platforms.contains(bringup_chip))
        m_has_new_version = false;

    bool tempVersion_update = limitations[Limitation_Update_SubKey].toBool();
    if(!tempVersion_update)
        m_has_new_version = false;
}

bool AsyncUpdater::parseJsonFile(const std::string &json_file, QJsonObject &jsonObject) const
{
    const QString json_file_name(json_file.c_str());
    QString error_msg;
    if (!FileUtils::validFile(json_file_name, error_msg)) {
        LOG(qPrintable(error_msg));
        return false;
    }
    QFile file(json_file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG("open json file failed: %s", qPrintable(json_file_name));
        return false;
    }
    QByteArray json = file.readAll();

    QJsonParseError parse_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(json, &parse_error);
    if (parse_error.error != QJsonParseError::NoError) {
        LOG("parse json file error[%s].", qPrintable(parse_error.errorString()));
        return false;
    }

    assert(json_doc.isObject());
    jsonObject = json_doc.object();
    return true;
}

void AsyncUpdater::GetAllVersions(std::vector<std::string> &versions)
{
    QDir dir(QString::fromStdString(m_release_folder));
    if(!dir.exists())
        return;

    dir.setFilter(QDir::Dirs);
    QFileInfoList list = dir.entryInfoList();
    if(list.empty())
        return;

    int i = 0;
    do {
        QFileInfo info = list.at(i);
        versions.push_back(info.fileName().toStdString());
    } while(++i < list.size());
}

void AsyncUpdater::CheckFinish()
{
    emit signal_check_finish(m_has_new_version);
}

bool AsyncUpdater::IsUpdatePackage(const std::string &file_name,
                                   const std::string &/*pattern*/)
{
    return file_name.find(this->m_tool_name) != std::string::npos
            && file_name.find(".zip") != std::string::npos;
}

void AsyncUpdater::processError(QProcess::ProcessError err)
{
    switch(err)
    {
    case QProcess::FailedToStart:
        Utils::flashtool_message_box(nullptr, nullptr, INFORMATION_MSGBOX, tr("Smart Phone Flash Tool"), tr("Failed to Start Updater.exe!"), tr("OK"));
        break;

    case QProcess::Crashed:
        Utils::flashtool_message_box(nullptr, nullptr, INFORMATION_MSGBOX, tr("Smart Phone Flash Tool"), tr("Crashed!"), tr("OK"));
        break;

    case QProcess::Timedout:
        Utils::flashtool_message_box(nullptr, nullptr, INFORMATION_MSGBOX, tr("Smart Phone Flash Tool"), tr("Time out!"), tr("OK"));
        break;

    default:
        Utils::flashtool_message_box(nullptr, nullptr, INFORMATION_MSGBOX, tr("Smart Phone Flash Tool"), tr("Unknow Error!"), tr("OK"));
        break;
    }
}
