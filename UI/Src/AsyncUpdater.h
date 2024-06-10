#ifndef ASYNCUPDATER_H
#define ASYNCUPDATER_H

#include <QObject>
#include <QString>
#include <QtCore/QThread>
#include <vector>
#include <QProcess>
#include <QFuture>
#include <QJsonObject>

class AsyncUpdater : public QObject
{
    Q_OBJECT

public:
    typedef enum _UpdateFailReason
    {
        unknown_reason = 0,
        lack_of_update_files,
        start_update_exe_fail
    } UpdateFailReason;

public:
    explicit AsyncUpdater(QObject *parent);
    ~AsyncUpdater();

    bool IsUpdatePackage(const std::string &file_name, const std::string &pattern);
    bool hasNewVersion() {return m_has_new_version;}

    void CheckAsyncNewVersion();
    void CheckUpdate();
    void CheckFinish();

    void AsyncUpdateTool();
    void UpdateTool();

    const QString &GetReleaseNotes() const { return m_release_notes;}
    const std::string &GetVersionInfo() const { return m_new_version;}

    void waitForFinished();

private:
    void GetAllVersions(std::vector<std::string> &versions);
    void processError(QProcess::ProcessError err);
    void ReadReleaseNotes(const std::string &path);
    bool parseJsonFile(const std::string &json_file, QJsonObject &jsonObject) const;
    void ConstructReleaseNotes(const QJsonObject &jsonObject);
    bool UpdateToolsPreCheck() const;
    void DoUpdateTool();
    bool RunAsAdmin(const QString &appName, const QStringList &paramterList) const;
    void parserUpdateInfo();

private:
    // variables from update.ini file
    std::string m_release_folder;
    std::string m_updater_exe;
    std::string m_release_note_file;
    std::string m_current_version;
    std::string m_tool_name;

    bool m_has_new_version;
    std::string m_new_version;
    std::string m_new_version_folder;
    std::string m_new_version_package;
    QString m_release_notes;

    QFuture<void> future;

signals:
    void signal_check_finish(bool bUpdate);
    void signal_update_finish();
    void signal_update_failed(AsyncUpdater::UpdateFailReason);
};

#endif // ASYNCUPDATER_H
