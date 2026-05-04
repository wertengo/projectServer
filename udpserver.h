#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

class udpServer : public QObject
{
Q_OBJECT
public:
    udpServer();
    void sendMessage(QString message);
    ~udpServer();

signals:
    void logMessage(QString message);
    void messageReceived(QString message);

private:
    QUdpSocket* m_pudp;

private slots:
    void slotSendDatagram();
    void slotProcessDatagrams();
};

#endif // UDPSERVER_H
