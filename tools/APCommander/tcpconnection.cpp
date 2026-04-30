#include <QNetworkInterface>

#include "tcpconnection.h"
#include "message.h"
#include "command.h"

TcpConnection::TcpConnection(QObject *parent)
: Connection{parent}
, m_tcpSocket(nullptr)
{

}

TcpConnection::~TcpConnection()
{

}

bool TcpConnection::open(const QString& settings)
{
    if (!settingsFromString(settings, m_settings))
        return false;

    if (m_tcpSocket != nullptr)
        delete m_tcpSocket;

    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onDataReady()));
    connect(m_tcpSocket, SIGNAL(aboutToClose()), this, SIGNAL(closed()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(opened()));

    //m_tcpSocket->open(QIODevice::ReadWrite);
    m_tcpSocket->connectToHost(m_settings.panelIpAddress, m_settings.panelPort);

    return true;
}

void TcpConnection::close()
{
    m_tcpSocket->close();
}

bool TcpConnection::isOpen() const
{
    return m_tcpSocket->isOpen();
}

QString TcpConnection::connectionSummary() const
{
    return QString("TCP/IP");
}

void TcpConnection::onDataReady()
{
    QByteArray a = m_tcpSocket->readAll();
    processReceived(a);
}

void TcpConnection::onError(QAbstractSocket::SocketError errorCode)
{
    QString s = QString("error: %1").arg((int)errorCode);
    emit error(s);
}

QByteArray TcpConnection::send(Command *command, char dataByte )
{
    QByteArray sent;

    if (m_tcpSocket==nullptr)
        return sent;

    if (!m_tcpSocket->isOpen())
        return sent;

    prepareResponse(command);

    QByteArray bytes = command->bytes(dataByte);
    qint64 ret = m_tcpSocket->write(bytes);

    if (ret >= 0)
        sent = QByteArray(bytes).left(ret);

    return sent;
}

QByteArray TcpConnection::send(Command *command, const QString& message, Command::Endpoint from )
{
    return QByteArray();
}

QString TcpConnection::settingsToString(const TcpSettings& settings)
{
    QString s;
    s.append(settings.panelIpAddress);
    s.append(",");
    s.append(QString::number(settings.panelPort));
    return s;
}

QString TcpConnection::settingsToString(const QString& ipAddr, int port)
{
    TcpSettings settings;
    settings.panelIpAddress = ipAddr;
    settings.panelPort = port;
    return settingsToString(settings);
}

bool TcpConnection::settingsFromString(const QString& s, TcpSettings& settings)
{
    QStringList parts = s.split(",");
    if (parts.size() != 2 )
        return false;

    settings.panelIpAddress = parts.at(0);
    settings.panelPort = parts.at(1).toInt();
    return true;
}
