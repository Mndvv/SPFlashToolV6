#ifndef NETWORKINTF_H
#define NETWORKINTF_H

#include <QObject>
#include <QSharedPointer>
#include <atomic>

class NetworkInft: public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<NetworkInft> instance();

    bool network_online() const;
    void tryConnect();
    void tryConnectShow();
    void postData(const QString &device_id);

signals:
    void signal_network_connection_changed();

private slots:
    void slot_network_connected(bool connected);
    void slot_network_connected_show(bool connected);

private:
    NetworkInft();
    NetworkInft(const NetworkInft &) = delete;
    NetworkInft(const NetworkInft &&) = delete;
    NetworkInft & operator=(const NetworkInft &) = delete;
    NetworkInft & operator=(const NetworkInft &&) = delete;

private:
    static QSharedPointer<NetworkInft> m_instance;

    std::atomic<bool> m_network_online;
};

#endif // NETWORKINTF_H
