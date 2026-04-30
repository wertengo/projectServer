#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QList>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>
#include <QQueue>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <iostream>
#include <string>
#include <algorithm>
#include <yaml-cpp/yaml.h>
//#include <yaml-cpp/emitter.h>
//#include <yaml-cpp/node/node.h>
//#include <yaml-cpp/node/impl.h>


class Server : public QObject
{
Q_OBJECT

public:
    Server();
    ~Server();

    void loadConfigFile(const QString &filePath);
    QString currentConfigFile() const;
    void reloadServer();

signals:
    void logMessage(QString message);
    void button_clicked_signal();

public slots:
    void sendMessage(const QString &message);

private slots:

    void onReadyRead();
    void addClient();
    void deleteClient();

private:
    quint64 m_nNextBlockSize;
    const int maxUser = 10;
    QStringList logList;
    QQueue<QString> userQueue;
    QTcpServer *tcpServer;
    QList<QByteArray> message;
    QList<QTcpSocket*> clients;
    quint16 serverPort;
    QString serverIP;
    QString getLocationServer();
    void saveToKeys(QSettings &settings);
    void loadSettings();
    void saveServerPort(quint16 port);
    void saveClientIP(const QString &ip);
    void readJSONFile(QString fileName);
    void writeJSONFile();
    void writeJSONFileInfo();
    void saveJSONFile(QByteArray arrayData);
    void saveJSONFileInfo(QJsonObject arrayData);
    void readYAMLFile(QString fileName);
    void saveYAMLFile();

    QString m_configFilePath;
    void loadIniConfig(const QString &path);
    void loadJsonConfig(const QString &path);
    void loadYamlConfig(const QString &path);
    void applySettingsAndRestartServer();
//    void parseComandLine();
};

#endif // SERVER_H
