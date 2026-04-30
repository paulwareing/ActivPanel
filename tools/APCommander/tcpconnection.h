#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QTcpSocket>

#include "connection.h"

class TcpConnection : public Connection
{
    Q_OBJECT

public:
    struct TcpSettings
    {
        QString panelIpAddress;
        int panelPort;
    };

    explicit TcpConnection(QObject *parent = nullptr);
    virtual ~TcpConnection();

    virtual bool open(const QString& settings);
    virtual void close();

    virtual bool isOpen() const;
    virtual QString connectionSummary() const;

    virtual QByteArray send(Command *command, char dataByte );
    virtual QByteArray send(Command *command, const QString& message, Command::Endpoint from );

    static QString settingsToString(const TcpSettings&);
    static QString settingsToString(const QString& ipAddr, int port);
    bool settingsFromString(const QString&, TcpSettings&);

private slots:
    void onDataReady();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket * m_tcpSocket;
    TcpSettings m_settings;
};

#endif // TCPCONNECTION_H
