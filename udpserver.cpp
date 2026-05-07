#include "udpserver.h"

udpServer::udpServer()
{
    m_pudp = new QUdpSocket(this);

    m_pudp->bind(QHostAddress("172.16.202.129"), 2425);


    // QTimer* ptimer = new QTimer(this);
    // ptimer->setInterval(10000);
    // ptimer->start();
    // connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
    connect(m_pudp, SIGNAL(readyRead()), SLOT(slotProcessDatagrams()));
}

void udpServer::slotSendDatagram()
{
    // QByteArray baDatagram;
    // QDataStream out(&baDatagram, QIODevice::WriteOnly);
    // out.setVersion(QDataStream::Qt_5_0);
    // QString message = "UDP Клиенту отправлено сообщение: ";
    // QDateTime dt = QDateTime::currentDateTime();
    // qDebug() << message + "Текущее время: " + dt.toString();
    // emit logMessage(message + "Текущее время: " + dt.toString());
    // out << dt;
    // m_pudp->writeDatagram(baDatagram, QHostAddress("172.16.202.129"), 2424);
}

void udpServer::slotProcessDatagrams()
{
    QByteArray baDatagram;

    // do{
    //     baDatagram.resize(m_pudp->pendingDatagramSize());
    //     m_pudp->readDatagram(baDatagram.data(), baDatagram.size());
    // }while(m_pudp->hasPendingDatagrams());

    while (m_pudp->hasPendingDatagrams()) {
        QByteArray baDatagramTest;
        baDatagramTest.resize(m_pudp->pendingDatagramSize());
        m_pudp->readDatagram(baDatagramTest.data(), baDatagramTest.size());
        QByteArray payload;
        QDataStream stream(&baDatagramTest, QIODevice::ReadOnly);
        quint8 type;
        quint32 packetNumber;
        stream >> type;

        if (type == 0x01) {
            int remainingSize = baDatagramTest.size() - sizeof(quint32);
            payload = baDatagramTest.right(remainingSize);
            QString message = QString::fromUtf8(payload);
            qDebug() << "Client UDP: " + QString::fromUtf8(payload);
            emit logMessage("Client UDP: " + QString::fromUtf8(payload));
        }else if(type == 0x02){
            stream >> packetNumber;
            payload = baDatagramTest.mid(stream.device()->pos());
            packetHistory.insert(packetNumber, QString::fromUtf8(payload));
            qDebug() << "Пришел пакет файла №" << packetNumber;
        }else{
            qDebug() << "ERROR: Не корректный тип данных";
        }

        // stream >> packetNumber;

        // int remainingSize = baDatagramTest.size() - sizeof(int);
        // payload = baDatagramTest.right(remainingSize);
    }

    if (!packetHistory.isEmpty()) {
        QMap<quint32, QString>::iterator it_packetHistory = packetHistory.begin();
        QString text;

        for (; it_packetHistory != packetHistory.end(); ++it_packetHistory) {
            qDebug() <<"Читаем packetHistory по ключу: " << it_packetHistory.key() << " Text по ключу: " << it_packetHistory.value();
            text.append(it_packetHistory.value());
        }

        saveFileUDP(text);
        qDebug() << "Пришел файл !!!";
    }
}

void udpServer::sendMessage(QString message)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << message.toUtf8();
    qDebug() << message;
    emit logMessage(message);

    m_pudp->writeDatagram(baDatagram, QHostAddress("172.16.202.129"), 2424);
}

void udpServer::saveFileUDP(QString text)
{
    QString fileName = "text.txt";
    QString confPath = QString("%1/%2").arg(qApp->applicationDirPath()).arg("data");

    QDir dir(confPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = confPath + "/" + fileName;

    QFile fileText(filePath);
    if (!fileText.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл infoJsonData.json для записи";
        return;
    }
    QTextStream streamText(&fileText);
    streamText << text;
    // fileText.write(text);
    fileText.close();
    qDebug() << "Файл текста сохранен по этому пути: " << filePath;
}

udpServer::~udpServer()
{
    m_pudp->deleteLater();
}
