#ifndef LOGINFOSETTING_H
#define LOGINFOSETTING_H

#include <QString>
#include "../ConsoleMode/IConsoleModeXMLInterface.h"

class LogInfoSetting: public IConsoleModeXMLInterface
{
public:
    LogInfoSetting();

    virtual void loadXML(const QDomElement &xml_node) Q_DECL_OVERRIDE;
    virtual void saveXML(QDomDocument &xml_doc, QDomElement &xml_node) const Q_DECL_OVERRIDE;

    void setLogPath(const QString &sLogPath);
    void setCleanHours(qint64 nCleanHours);

    QString getLogPath() const;
    qint64 getCleanHours() const;

private:
    QString m_logPath;
    qint64 m_cleanHours;
};

#endif // LOGINFOSETTING_H

