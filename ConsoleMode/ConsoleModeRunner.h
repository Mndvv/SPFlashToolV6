#ifndef CONSOLEMODERUNNER_H
#define CONSOLEMODERUNNER_H

#include "Config.h"
#include "../Cmd/ConsoleModeCallBack.h"

namespace ConsoleMode {

class ConsoleModeRunner
{
public:
    ConsoleModeRunner();

    void execute(const Config &config) const;
    void imageChksumVerify(const QString &scatter_file) const;

public:
    //console mode progress callback
    static void cbFormatProgress(void *_this, unsigned int progress,
                                 unsigned long long data_xferd, const QString &info);
    static void cbDownloadProgress(void *_this, unsigned int progress,
                                 unsigned long long data_xferd, const QString &info);
    static void cbReadbackProgress(void *_this, unsigned int progress,
                                 unsigned long long data_xferd, const QString &info);
    static void cbWriteMemoryProgress(void *_this, unsigned int progress,
                                 unsigned long long data_xferd, const QString &info);

private:
    std::shared_ptr<ConsoleModeCallBack> m_cmd_callback;
};

}

#endif // CONSOLEMODERUNNER_H
