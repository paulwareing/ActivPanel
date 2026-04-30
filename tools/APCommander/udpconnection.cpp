
#include <QNetworkInterface>

#include "udpconnection.h"
#include "message.h"
#include "command.h"

UdpConnection::UdpConnection(QObject *parent)
: Connection(parent)
{

}

UdpConnection::~UdpConnection()
{

}

bool UdpConnection::open(const QString& settings)
{
    if (!settingsFromString(settings, m_settings))
        return false;

    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onDatagramReady()));
    connect(m_socket, SIGNAL(aboutToClose()), this, SIGNAL(closed()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_socket->open(QIODevice::ReadWrite);

    if (m_socket->bind(QHostAddress(m_settings.localIpAddress), m_settings.localPort))
    {
        emit opened();
        return true;
    }
    return false;
}

bool UdpConnection::isOpen() const
{
    return m_socket->isOpen();
}

QString UdpConnection::connectionSummary() const
{
    return "UDP";
}

void UdpConnection::close()
{
    m_socket->close();
}

void UdpConnection::onError(QAbstractSocket::SocketError errorCode)
{
    QString s = QString("error: %1").arg((int)errorCode);
    emit error(s);
}

QByteArray UdpConnection::send(Command *command, char dataByte)
{
    QByteArray sent;

    if (m_socket==NULL)
        return sent;

    if (!m_socket->isOpen())
        return sent;

    prepareResponse(command);

    QByteArray bytes = command->bytes(dataByte);
    QString s = Message::toString(bytes);
    qint64 ret = m_socket->writeDatagram(s.toLocal8Bit(), QHostAddress(m_settings.panelIpAddress), m_settings.panelPort);

    if (ret >= 0)
        sent = QByteArray(bytes).left(ret);

    return sent;
}

QByteArray UdpConnection::send(Command *command, const QString& message, Command::Endpoint from )
{
    QByteArray sent;

    if (m_socket==NULL)
        return sent;

    if (!m_socket->isOpen())
        return sent;

    QByteArray bytes = command->bytes(from, Command::Scaler, message);
    QString s = Message::toString(bytes);
    qint64 ret = m_socket->writeDatagram(s.toLocal8Bit(), QHostAddress(m_settings.panelIpAddress), m_settings.panelPort);

    if (ret >= 0)
        sent = QByteArray(bytes).left(ret);

    return sent;
}

void UdpConnection::onDatagramReady()
{
    QByteArray a;
    while (m_socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        a.append(datagram);
    }
    QString s = explodeReceived(QString(a));
    processReceived(Message::fromString(s));
}

QString UdpConnection::explodeReceived(const QString& s)
{
    QString compact = s;
    QString exploded;
    int count = s.length()/2;
    for(int i=0 ; i< count ; ++i)
    {
        exploded.append(compact.left(2));
        if (i < (count-1))
            exploded.append(" ");

        compact = compact.right(compact.length()-2);
    }
    return exploded;
}

QString UdpConnection::settingsToString(const UdpConnection::UdpSettings& settings)
{
    QString s;
    s.append(settings.localIpAddress);
    s.append(",");
    s.append(QString::number(settings.localPort));
    s.append(",");
    s.append(settings.panelIpAddress);
    s.append(",");
    s.append(QString::number(settings.panelPort));
    return s;
}

QString UdpConnection::settingsToString(const QString& localIpAddress, int localPort, const QString& panelIpAddress, int panelPort)
{
    UdpSettings settings;
    settings.localIpAddress = localIpAddress;
    settings.localPort = localPort;
    settings.panelIpAddress = panelIpAddress;
    settings.panelPort = panelPort;
    return settingsToString(settings);
}

bool UdpConnection::settingsFromString(const QString& s, UdpSettings& settings)
{
    QStringList parts = s.split(",");
    if (parts.size() != 4 )
        return false;

    settings.localIpAddress = parts.at(0);
    settings.localPort = parts.at(1).toInt();
    settings.panelIpAddress = parts.at(2);
    settings.panelPort = parts.at(3).toInt();
    return true;
}

QStringList UdpConnection::localIpAddresses()
{
    QStringList addrList;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, addresses)
    {
        if ((addr.protocol() == QAbstractSocket::IPv4Protocol) && (!addr.isLoopback()))
            addrList.append(addr.toString());
    }
    return addrList;
}
