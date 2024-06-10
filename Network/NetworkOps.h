#ifndef NETWORKOPS_H
#define NETWORKOPS_H

#include <QSharedPointer>

class QNetworkAccessManager;
class QNetworkReply;

class NetworkOps : public QObject
{
    Q_OBJECT
public:
    NetworkOps();
    virtual ~NetworkOps() {}

    bool get() const;
    bool post(const QByteArray &postData) const;

private:
    bool postFinished(QNetworkReply *reply) const;
    bool getFinished(QNetworkReply *reply) const;

private:
    QSharedPointer<QNetworkAccessManager> m_net_manager;
};

#endif // NETWORKOPS_H
