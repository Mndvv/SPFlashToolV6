#include "NetworkIntf.h"
#include "NetworkThread.h"
#include "NetworkOps.h"

QSharedPointer<NetworkInft> NetworkInft::m_instance = QSharedPointer<NetworkInft>();

NetworkInft::NetworkInft():
    m_network_online(false)
{

}

bool NetworkInft::network_online() const
{
    return m_network_online.load();
}

void NetworkInft::tryConnect()
{
    NetworkThread *network_thread = new NetworkThread(QString());
    connect(network_thread, SIGNAL(signal_network_connected(bool)), this, SLOT(slot_network_connected(bool)));
    connect(network_thread, SIGNAL(finished()), network_thread, SLOT(deleteLater()));
    network_thread->start();
}

void NetworkInft::tryConnectShow()
{
    NetworkThread *network_thread = new NetworkThread(QString());
    connect(network_thread, SIGNAL(signal_network_connected(bool)), this, SLOT(slot_network_connected_show(bool)));
    connect(network_thread, SIGNAL(finished()), network_thread, SLOT(deleteLater()));
    network_thread->start();
}

void NetworkInft::postData(const QString &device_id)
{
    if (m_network_online) {
        NetworkThread *network_thread = new NetworkThread(device_id);
        connect(network_thread, SIGNAL(finished()), network_thread, SLOT(deleteLater()));
        network_thread->start();
    }
}

void NetworkInft::slot_network_connected(bool connected)
{
    m_network_online.store(connected);
}

void NetworkInft::slot_network_connected_show(bool connected)
{
    slot_network_connected(connected);
    emit signal_network_connection_changed();
}

QSharedPointer<NetworkInft> NetworkInft::instance()
{
    if (!m_instance) {
        m_instance = QSharedPointer<NetworkInft>(new NetworkInft());
    }
    return m_instance;
}
