#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QApplication>
#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QDir>

class udpServer : public QObject
{
Q_OBJECT
public:
    udpServer();
    void sendMessage(QString message);
    void saveFileUDP(QByteArray textInByte);
    ~udpServer();

signals:
    void logMessage(QString message);
    void messageReceived(QString message);

private:
    QUdpSocket* m_pudp;
    QMap<quint32, QByteArray> packetHistory;

private slots:
    void slotSendDatagram();
    void slotProcessDatagrams();
};

#endif // UDPSERVER_H
