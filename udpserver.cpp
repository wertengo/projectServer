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

    do{
        baDatagram.resize(m_pudp->pendingDatagramSize());
        m_pudp->readDatagram(baDatagram.data(), baDatagram.size());
    }while(m_pudp->hasPendingDatagrams());

    // QDateTime dateTime;
    QByteArray dateTime;
    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in >> dateTime;
    // QString text = QString::fromUtf8(dateTime);
    qDebug() << "Client UDP: " + QString::fromUtf8(dateTime);
    emit logMessage("Client UDP: " + QString::fromUtf8(dateTime));
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

udpServer::~udpServer()
{
    m_pudp->deleteLater();
}
