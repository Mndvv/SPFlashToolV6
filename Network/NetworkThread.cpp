#include "NetworkThread.h"
#include <QDateTime>
#include "NetworkOps.h"

NetworkThread::NetworkThread(const QString &device_id):
    m_device_id(device_id),
    m_network_online(false)
{
    this->setObjectName(QStringLiteral("NetworkThread"));
}

NetworkThread::~NetworkThread()
{

}

bool NetworkThread::network_online() const
{
    return m_network_online.load();
}

void NetworkThread::run()
{
    NetworkOps network_ops;
    bool connected = false;
    if (m_device_id.isNull() || m_device_id.isEmpty()) {
        connected = network_ops.get();
        // This signal can NOT be sent to console mode main thread,
        // because console mode main thread will be blocked by usb com port search task.
        emit signal_network_connected(connected);
    } else {
        connected = network_ops.post(this->getPostData());
    }
    m_network_online = connected;
}

QByteArray NetworkThread::getPostData() const
{
    QByteArray dataArray;
    dataArray.append(QStringLiteral("record_date=%1&").arg(QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ssZ")));
    dataArray.append(QStringLiteral("storage_uid=%1").arg(m_device_id));
    return dataArray;
}
