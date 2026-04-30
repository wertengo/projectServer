#include "server.h"

#include <QApplication>

Server::Server() : m_nNextBlockSize(0)
{
    tcpServer = new QTcpServer(this);
    // loadSettings();
//    parseComandLine();
//    writeJSONFileInfo();
//    readJSONFile();

    // QHostAddress specIpAddress(serverIP);

    // if (!(tcpServer->listen(specIpAddress, serverPort))) {
    //     qFatal("ERROR: Сервер поднят на 127.0.0.1 с портом 56789");
    // }else{
    //     qDebug("INFO: Сервер запущен");
    // }

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
        return QString();
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
    // socket->write(message.at(1));
//    socket->disconnectFromHost();
//    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteClient()));
}

// void Server::sendMessage(QString message){
//     QByteArray dataToSend = message.toUtf8();

//     for(QTcpSocket *socket : clients){
//         if (socket->state() == QAbstractSocket::ConnectedState) {
//             socket->write(dataToSend);
//             socket->flush();
//         }
//     }
// }

void Server::sendMessage(const QString &message)
{
    QByteArray payload = message.toUtf8();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << (quint64)0;
    out << payload;
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));

    for (QTcpSocket *socket : clients) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(block);
            socket->flush();
        }
    }
}

// void Server::onReadyRead(){
//     QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

//     if(!socket) return;

//     QByteArray array = socket->readAll();

//     if (array.startsWith('{') || array.startsWith('[')) {
//         saveJSONFile(array);
//     }else if(array.startsWith('T')){
//         QString arrayAsString = QString::fromUtf8(array.mid(2));

//         if (arrayAsString.isEmpty()) {
//             return;
//         }

//         sendMessage(arrayAsString);
//         qDebug()<<"INFO: Сокету пришли данные, готовые к чтению. Данные: " << arrayAsString << " Index: " << clients.indexOf(socket);
//         emit logMessage("Получено от клиента " + arrayAsString + " Index: " + QString::number(clients.indexOf(socket)));
//     }else{
//         qWarning() << "Неизвестный формат данных: " << array;
//     }
// }

void Server::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);

    for (;;) {
        if (m_nNextBlockSize == 0) {
            if (socket->bytesAvailable() < sizeof(quint64)) {
                return;
            }
            in >> m_nNextBlockSize;
        }

        if (socket->bytesAvailable() < m_nNextBlockSize) {
            return;
        }

        QByteArray payload;
        in >> payload;

        if (payload.startsWith('{') || payload.startsWith('[')) {
            qDebug() << "Получили JSON файл от клиента: ";
            saveJSONFile(payload);
        } else if (payload.startsWith('T')) {
            QString text = QString::fromUtf8(payload.mid(2));
            if (!text.isEmpty()) {
                sendMessage(text);
                qDebug() << "INFO: Сокету пришли данные, готовые к чтению. Данны:" << text << " Index: " << clients.indexOf(socket);
                emit logMessage("Получено от клиента " + text + " Index: " + QString::number(clients.indexOf(socket)));
            }
        } else {
            qWarning() << "Неизвестный формат данных:" << payload;
        }

        m_nNextBlockSize = 0;
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
//    QString confPath = QDir::currentPath() + "/serverInit.ini";
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");
    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/serverInit.ini";
    QSettings settings(filePath, QSettings::IniFormat);

    if (!settings.contains("Server/Port")) {
        settings.setValue("Server/Port", 56789);
        settings.sync();
        qDebug() << "ini файл должен создаться по этому пути: " << filePath;
    }

    if (!settings.contains("Server/IP")) {
        settings.setValue("Server/IP", "192.168.1.7");
        settings.sync();
    }

    serverPort = settings.value("Server/Port").toInt();
    serverIP = settings.value("Server/IP").toString();
    qDebug() << "Инит файл лежит: " << filePath;
    qDebug() << "Вывод порта из инит файла: " << serverPort;
    qDebug() << "Вывод IP адреса из инит файла: " << serverIP;
}

void Server::saveServerPort(quint16 port){
//    QString confPath = QDir::currentPath() + "/serverInit.ini";
//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");
    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/serverInit.ini";
    QSettings settings(filePath, QSettings::IniFormat);

    settings.setValue("Server/Port", port);
    settings.sync();
    qDebug() << "Порт сервера сохранен в INI: " << port;
}

void Server::saveToKeys(QSettings &settings){
    for (int i = 0; i < maxUser; ++i ) {
        settings.remove(QString("recentClient_%1").arg(i));
    }

    for (int i = 0; i < logList.size(); ++i ) {
        settings.setValue(QString("recentClient_%1").arg(i), logList[i]);
    }
    settings.sync();
}

void Server::saveClientIP(const QString &ip){
//    QString confPath = QDir::currentPath() + "/serverInit.ini";
//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/serverInit.ini";
    QSettings settings(filePath, QSettings::IniFormat);

//    if (userQueue.size() >= maxUser) {
//        userQueue.dequeue();
//    }

//    userQueue.enqueue("IP: " + ip + ", " + "ConnectTime: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
//    QStringList list;
//    for (const QString &item : userQueue)
//        list << item;

    settings.beginGroup("ClientHistory");
//    for(int i = maxUser - 2; i >= 0; --i){
//        QString old = settings.value(QString("recentClient_%1").arg(i)).toString();
//        if (!old.isEmpty()) {
//            settings.setValue(QString("recentClient_%1").arg(i + 1), old);
//        }
//    }
        QString record = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " - " + "IP :" + ip;
//        settings.setValue("recentClient_0", record);
//        settings.sync();

    logList.prepend(record);
    while (logList.size() > maxUser) {
        logList.removeFirst();
    }
    saveToKeys(settings);
    writeJSONFileInfo();

////    settings.setValue("recentClient", list);
////    settings.setValue("IP", ip);
////    settings.setValue("ConnectTime",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    settings.endGroup();
//    settings.sync();
    qDebug() << "IP клиента сохранен в INI: " << ip;
}

void Server::writeJSONFile(){

}

void Server::readJSONFile(QString fileName){
    QString val;

//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/" + fileName;

    QFile fileJson(filePath);

    if (!fileJson.open(QFile::ReadOnly)) {
        qDebug() << "Не удалось открыть файл json для чтения";
        return;
    }

    val = fileJson.readAll();

    qDebug() << "Информация из JSON файла: " << val;
    fileJson.close();
}

void Server::writeJSONFileInfo(){
    QJsonObject textObject;
    textObject["IP"] = serverIP;
    textObject["PORT"] = serverPort;

    QJsonArray textArray = QJsonArray::fromStringList(logList);
    textObject["ClientHistory"] = textArray;
//    textArray.append(textObject);
    saveJSONFileInfo(textObject);
}

void Server::saveJSONFileInfo(QJsonObject arrayData){
    QString fileName = "infoJsonData.json";
    QJsonDocument doc = QJsonDocument(arrayData);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/" + fileName;

    QFile fileJson(filePath);

    if (!fileJson.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл infoJsonData.json для записи";
        return;
    }
    fileJson.write(doc.toJson(QJsonDocument::Indented));
    fileJson.close();
    qDebug() << "Файл infoJsonData.json JSON сохранен по этому пути: " << filePath;
    saveYAMLFile();
}

void Server::saveJSONFile(QByteArray arrayData){
    QString fileName = "clientJsonData.json";
    QJsonDocument doc = QJsonDocument::fromJson(arrayData);

//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/" + fileName;

    QFile fileJson(filePath);

    if (!fileJson.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи";
        return;
    }
    fileJson.write(doc.toJson());
    fileJson.close();
    qDebug() << "Пришел JSON файл с такой информацией: " ;
    readJSONFile("clientJsonData.json");
    qDebug() << "Файл JSON сохранен по этому пути: " << filePath;
}

void Server::readYAMLFile(QString fileName){
    //    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString yamlFilePath = confPath + "/" + fileName;

    QFile yamlFile(yamlFilePath);

    if (!yamlFile.open(QFile::ReadOnly)) {
        qDebug() << "Не удалось открыть файл yaml для чтения";
        return;
    }
}

void Server::saveYAMLFile(){
    QString fileName = "clientYamlFile.yaml";

    //    YAML::Node root = YAML::LoadFile(yamlFilePath.toStdString());
    YAML::Node root;
//    root["ServerInfo"];
    root["ServerInfo"]["ip"] =  serverIP.toStdString();
    root["ServerInfo"]["port"] =  std::to_string(serverPort);
    root["ClientHistory"] = YAML::Node(YAML::NodeType::Sequence);

    for (const QString &client : logList) {
        root["ClientHistory"].push_back(client.toStdString());
    }

//    QString confPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString yamlFilePath = confPath + "/" + fileName;
    QFile yamlFile(yamlFilePath);
    if (yamlFile.open(QIODevice::WriteOnly)) {
        YAML::Emitter out;
        out << root;
        yamlFile.write(out.c_str());
        qDebug() << "Файл yaml создан: ";
        yamlFile.close();
    }
}

void Server::loadConfigFile(const QString &filePath) {
    if (filePath.isEmpty()) {
        qWarning("Server::loadConfigFile: empty path");
        return;
    }
    m_configFilePath = filePath;
    QFileInfo fi(filePath);
    if (!fi.exists()) {
        qWarning("Config file does not exist: %s", qPrintable(filePath));
        return;
    }
    QString suffix = fi.suffix().toLower();
    if (suffix == "ini") {
        loadIniConfig(filePath);
    } else if (suffix == "json") {
        loadJsonConfig(filePath);
    } else if (suffix == "yaml" || suffix == "yml") {
        loadYamlConfig(filePath);
    } else {
        qWarning("Unsupported config format: %s", qPrintable(suffix));
        return;
    }
    applySettingsAndRestartServer();
}

void Server::loadIniConfig(const QString &path) {
    QSettings settings(path, QSettings::IniFormat);
    if (!settings.contains("Server/Port")) {
        settings.setValue("Server/Port", 56789);
        settings.sync();
    }
    if (!settings.contains("Server/IP")) {
        settings.setValue("Server/IP", "192.168.1.7");
        settings.sync();
    }
    serverPort = settings.value("Server/Port").toInt();
    serverIP = settings.value("Server/IP").toString();
    qDebug() << "Loaded INI config:" << serverIP << serverPort;

    QHostAddress specIpAddress(serverIP);

    if (!(tcpServer->listen(specIpAddress, serverPort))) {
        qFatal("ERROR: Сервер поднят на 127.0.0.1 с портом 56789");
    }else{
        qDebug("INFO: Сервер запущен");
    }
}

void Server::loadJsonConfig(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open JSON config: %s", qPrintable(path));
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError) {
        qWarning("JSON parse error: %s", qPrintable(error.errorString()));
        return;
    }

    if (!doc.isObject()) {
        qWarning("JSON root is not an object");
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject info;

    if (root.contains("ServerInfo") && root["ServerInfo"].isObject()) {
        info = root["ServerInfo"].toObject();
    } else {
        info = root;
    }

    serverIP   = info["IP"].toString();
    serverPort = info["PORT"].toInt();
    qDebug() << "Loaded JSON config:" << serverIP << serverPort;

    QHostAddress specIpAddress(serverIP);

    if (!(tcpServer->listen(specIpAddress, serverPort))) {
        qFatal("ERROR: Сервер поднят на 127.0.0.1 с портом 56789");
    }else{
        qDebug("INFO: Сервер запущен");
    }
}

void Server::loadYamlConfig(const QString &path) {
    try {
        YAML::Node root = YAML::LoadFile(path.toStdString());
        auto serverInfo = root["ServerInfo"];
        serverIP = QString::fromStdString(serverInfo["ip"].as<std::string>("192.168.1.7"));
        serverPort = serverInfo["port"].as<int>(56789);
        qDebug() << "Loaded YAML config:" << serverIP << serverPort;
    } catch (const YAML::Exception &e) {
        qWarning() << "YAML parse error:" << e.what();
    }

    QHostAddress specIpAddress(serverIP);

    if (!(tcpServer->listen(specIpAddress, serverPort))) {
        qFatal("ERROR: Сервер поднят на 127.0.0.1 с портом 56789");
    }else{
        qDebug("INFO: Сервер запущен");
    }
}

void Server::applySettingsAndRestartServer() {
    tcpServer->close();

    qDeleteAll(clients);
    clients.clear();

    QHostAddress addr(serverIP);
    if (!tcpServer->listen(addr, serverPort)) {
        qFatal("Server failed to restart on %s:%d", qPrintable(serverIP), serverPort);
    }
    qDebug("Server restarted on %s:%d", qPrintable(serverIP), serverPort);

    // emit serverSettingsChanged();
}

Server::~Server(){
    tcpServer->close();
    tcpServer->disconnect();
    tcpServer->deleteLater();
    clients.clear();
    userQueue.clear();
    logList.clear();
}


