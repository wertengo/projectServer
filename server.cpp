#include "server.h"


Server::Server()
{
    tcpServer = new QTcpServer(this);

    loadSettings();
//    readJSONFile();

    QHostAddress specIpAddress(serverIP);
    if (!(tcpServer->listen(specIpAddress, serverPort))) {
        qFatal("ERROR: Сервер поднят на 127.0.0.1 с портом 56789");
    }else{
        qDebug("INFO: Сервер запущен");
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(addClient()));

    message.append(QByteArray("Привет новый клиент"));
    message.append(QByteArray("Hello new client!"));
}

QString Server::getLocationServer(){
    //    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    //    for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
    //        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
    //            qDebug() << "My address server: " << address.toString() << ":" << tcpServer->serverPort();
    //        }
    //    }

    //    foreach(const QNetworkInterface &networkInterface, QNetworkInterface::allInterfaces()){
    //        QNetworkInterface::InterfaceFlags flags = networkInterface.flags();
    //        if ((bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)) {
    //            foreach(const QNetworkAddressEntry &adress, networkInterface.addressEntries()){
    //                if (adress.ip().protocol() == QAbstractSocket::IPv4Protocol) {
    //                    qDebug() << "My address server: " << adress.ip().toString() << ":" << tcpServer->serverPort();
    //                }
    //            }
    //        }
    //    }

        auto interfaceNetwork = QNetworkInterface::allInterfaces();

    //    std::for_each(interfaceNetwork.begin(),interfaceNetwork.end(),[](const QNetworkInterface &networkInterface){
    //        QNetworkInterface::InterfaceFlags flags = networkInterface.flags();
    //        if ((bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)) {
    //            std::for_each(networkInterface.addressEntries().begin(), networkInterface.addressEntries().end(), [](const QNetworkAddressEntry &address){
    //                if (address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
    ////                    qDebug() << "My address server: " << address.ip().toString() << ":" << tcpServer->serverPort();
    //                                        qDebug() << "My address server: " << address.ip().toString();
    //                }
    //            });
    //        }
    //    });

        for(QNetworkInterface &networkInterface : interfaceNetwork){
            QNetworkInterface::InterfaceFlags flags = networkInterface.flags();
            if ((bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)) {
                for (QNetworkAddressEntry &address : networkInterface.addressEntries()) {
                    if (address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        qDebug() << "My address server: " << address.ip().toString();
                        return address.ip().toString();
                    }
                }
            }
        }
}

void Server::addClient(){
    QTcpSocket *socket = tcpServer->nextPendingConnection();

    if (!(socket->waitForConnected())) {
        qDebug() << socket->errorString();
        return;
    }

    QString rawIP = socket->peerAddress().toString();
    if (rawIP.isEmpty()) {
        socket->disconnectFromHost();
        return;
    }

    saveClientIP(rawIP);

    clients.append(socket);
    qDebug("INFO: Клиент подключен, всего клиентов: %d" ,clients.size());
    QString infoLog = "Клиент подключен, всего клиентов: ";
    infoLog.append(QString::number(clients.size()));
    emit logMessage(infoLog);
//    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteClient()));
    socket->write(message.at(1));
//    socket->disconnectFromHost();
//    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteClient()));
}

void Server::sendMessage(QString message){
    QByteArray dataToSend = message.toUtf8();

    for(QTcpSocket *socket : clients){
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(dataToSend);
            socket->flush();
        }
    }
}

void Server::onReadyRead(){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if(!socket) return;

    QByteArray array = socket->readAll();

    if (array.startsWith('{') || array.startsWith('[')) {
        saveJSONFile(array);
    }else if(array.startsWith('T')){
        QString arrayAsString = QString::fromUtf8(array.mid(2));

        if (arrayAsString.isEmpty()) {
            return;
        }

        sendMessage(arrayAsString);
        qDebug()<<"INFO: Сокету пришли данные, готовые к чтению. Данные: " << arrayAsString << " Index: " << clients.indexOf(socket);
        emit logMessage("Получено от клиента " + arrayAsString + " Index: " + QString::number(clients.indexOf(socket)));
    }else{
        qWarning() << "Неизвестный формат данных: " << array;
    }
}

void Server::deleteClient(){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    clients.removeAll(socket);
    socket->close();
    qDebug("INFO: Пользователь отключился, осталось пользователей: %d", clients.size());
    emit logMessage("INFO: Пользователь отключился, осталось пользователей: " + QString::number(clients.size()));
}

void Server::loadSettings(){
    QString confPath = QDir::currentPath() + "/serverInit.ini";
    QSettings settings(confPath, QSettings::IniFormat);

    if (!settings.contains("Server/Port")) {
        settings.setValue("Server/Port", 56789);
        settings.sync();
        qDebug() << "ini файл должен создаться по этому пути: " << confPath;
    }

    if (!settings.contains("Server/IP")) {
        settings.setValue("Server/IP", "192.168.1.7");
        settings.sync();
    }

    serverPort = settings.value("Server/Port").toInt();
    serverIP = settings.value("Server/IP").toString();
    qDebug() << "Вывод порта из инит файла: " << serverPort;
    qDebug() << "Вывод IP адреса из инит файла: " << serverIP;
}

void Server::saveServerPort(quint16 port){
    QString confPath = QDir::currentPath() + "/serverInit.ini";
    QSettings settings(confPath, QSettings::IniFormat);

    settings.setValue("Server/Port", port);
    settings.sync();
    qDebug() << "Порт сервера сохранен в INI: " << port;
}

void Server::saveClientIP(const QString &ip){
    QString confPath = QDir::currentPath() + "/serverInit.ini";
    QSettings settings(confPath, QSettings::IniFormat);

    settings.beginGroup("LastClient");
    settings.setValue("IP", ip);
    settings.setValue("ConnectTime",QDateTime::currentDateTime().toString());
    settings.endGroup();
    settings.sync();
    qDebug() << "IP клиента сохранен в INI: " << ip;
}

void Server::writeJSONFile(){

}

void Server::readJSONFile(QString fileName){
    QString val;

    QFile fileJson(fileName);

    if (!fileJson.open(QFile::ReadOnly)) {
        qDebug() << "Не удалось открыть файл json для чтения";
        return;
    }

    val = fileJson.readAll();

    qDebug() << "Информация из JSON файла: " << val;
    fileJson.close();
}

void Server::saveJSONFile(QByteArray arrayData){
    QJsonDocument doc = QJsonDocument::fromJson(arrayData);
    QFile fileJson("clientJsonData.json");

    if (!fileJson.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи";
        return;
    }
    fileJson.write(doc.toJson());
    fileJson.close();
    qDebug() << "Пришел JSON файл с такой информацией: " ;
    readJSONFile("clientJsonData.json");
    qDebug() << "Файл JSON сохранен по этому пути: " << QDir::currentPath() + "/clientJsonData.json";
}

Server::~Server(){
    tcpServer->close();
    tcpServer->disconnect();
    tcpServer->deleteLater();
    clients.clear();
}


