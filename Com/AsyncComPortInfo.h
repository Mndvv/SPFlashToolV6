#ifndef ASYNCCOMPORTINFO_H
#define ASYNCCOMPORTINFO_H

#include <QThread>

class AsyncComPortInfo : public QThread
{
    Q_OBJECT

public:
    AsyncComPortInfo();
    ~AsyncComPortInfo();

protected:
    virtual void run();

signals:
    void signal_send_com_port_infos(const QList<QString> &uart_port_list);
};

#endif // ASYNCCOMPORTINFO_H
