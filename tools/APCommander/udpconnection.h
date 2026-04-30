#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H

#include <QUdpSocket>

#include "connection.h"

class UdpConnection : public Connection
{
    Q_OBJECT

public:
    struct UdpSettings
    {
        QString localIpAddress;
        int localPort;
        QString panelIpAddress;
        int panelPort;
    };

    UdpConnection(QObject *parent = NULL);
    virtual ~UdpConnection();

    bool open(const QString& settings);
    void close();

    bool isOpen() const;
    QString connectionSummary() const;

    QByteArray send(Command *command, char dataByte);
    QByteArray send(Command *command, const QString& message, Command::Endpoint from );

    static QString settingsToString(const UdpSettings&);
    static QString settingsToString(const QString& localIpAddress, int localPort, const QString& panelIpAddress, int panelPort);
    bool settingsFromString(const QString&, UdpSettings&);

    static QStringList localIpAddresses();

private slots:
    void onDatagramReady();
    void onError(QAbstractSocket::SocketError);

private:
    QUdpSocket *m_socket;
    UdpSettings m_settings;

    static QString explodeReceived(const QString& );
};

#endif // UDPCONNECTION_H
