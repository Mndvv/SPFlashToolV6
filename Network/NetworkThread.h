#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include <atomic>

class NetworkThread: public QThread
{
    Q_OBJECT

public:
    explicit NetworkThread(const QString &device_id);
    virtual ~NetworkThread();

    bool network_online() const;

signals:
    void signal_network_connected(bool connected);

protected:
    virtual void run();

private:
    QByteArray getPostData() const;

private:
    QString m_device_id;

    // The second way to get the network status. Add for console mode usage.
    std::atomic<bool> m_network_online;
};

#endif // NETWORKTHREAD_H
