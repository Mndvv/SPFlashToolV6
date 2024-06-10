#ifndef CONSOLEMODEENTRY_H
#define CONSOLEMODEENTRY_H

#include <QCoreApplication>
#include <QSharedPointer>
#include "../Utility/LogFilesClean.h"

class NetworkThread;

namespace ConsoleMode
{

class ConsoleModeEntry
{
public:
    ConsoleModeEntry();
    virtual ~ConsoleModeEntry();

    int run(const QStringList &arguments);
    bool network_online() const;

private:
    ConsoleModeEntry(const ConsoleModeEntry &);
    ConsoleModeEntry & operator=(const ConsoleModeEntry &);

    ConsoleModeEntry(const ConsoleModeEntry &&);
    ConsoleModeEntry & operator=(const ConsoleModeEntry &&);

private:
    QSharedPointer<NetworkThread> m_network_thread;
};

}

#endif // CONSOLEMODEENTRY_H
