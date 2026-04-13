#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <QString>
#include <algorithm>
#include <QDir>
#include <QSettings>
#include <QDateTime>

#define PORT 56789

class Server : public QObject
{
Q_OBJECT

public:
    Server();
    ~Server();

signals:
    void logMessage(QString message);
    void button_clicked_signal();

public slots:
    void sendMessage(QString message);

private slots:

    void onReadyRead();
    void addClient();
    void deleteClient();

private:
    QTcpServer *tcpServer;
    QList<QByteArray> message;
    QList<QTcpSocket*> clients;
    quint16 serverPort;
    void loadSettings();
    void saveServerPort(quint16 port);
    void saveClientIP(const QString &ip);
};

#endif // SERVER_H
