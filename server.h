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
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>

#include <algorithm>


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
    QString serverIP;
    QString getLocationServer();
    void loadSettings();
    void saveServerPort(quint16 port);
    void saveClientIP(const QString &ip);
    void readJSONFile(QString fileName);
    void writeJSONFile();
    void saveJSONFile(QByteArray arrayData);
};

#endif // SERVER_H
