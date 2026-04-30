#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QStringList>
#include <QSerialPort>

#include "connection.h"

class Command;
class Response;

class SerialConnection : public Connection
{
    Q_OBJECT

public:
    struct ComSettings
    {
        QString portName;
        int baudRate;
        QSerialPort::Parity parity;
        QSerialPort::DataBits dataBits;
        QSerialPort::StopBits stopBits;
    };

    SerialConnection(QObject *parent = 0);
    virtual ~SerialConnection();

    bool open(const QString& settings);
    void close();

    bool isOpen() const;
    QString connectionSummary() const;
    
    QByteArray send(Command *command, char dataByte);
    QByteArray send(Command *command, const QString& message, Command::Endpoint from );

    static QStringList availablePorts();

    static bool settingsFromString(const QString&, ComSettings&);
    static QString settingsToString(const QString& portName, int baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits);

    static QString baudRateToString(QSerialPort::BaudRate);
    static QString baudRateToString(quint32);
    static QString parityToString(QSerialPort::Parity);
    static QString dataBitsToString(QSerialPort::DataBits);
    static QString stopBitsToString(QSerialPort::StopBits);
    static quint32 baudRateFromString(const QString&);
    static QSerialPort::Parity parityFromString(const QString&);
    static QSerialPort::DataBits dataBitsFromString(const QString&);
    static QSerialPort::StopBits stopBitsFromString(const QString&);

private slots:
    void onPortError(QSerialPort::SerialPortError error);
    void onPortReadyRead();
    
private:
    QSerialPort * m_port;

};

#endif // SERIALCONNECTION_H
