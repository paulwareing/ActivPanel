#include <QSerialPortInfo>

#include "serialconnection.h"
#include "command.h"

#define COM_SETTINGS_PARTS_COUNT        5

SerialConnection::SerialConnection(QObject *parent)
: Connection(parent)
, m_port(NULL)
{

}

SerialConnection::~SerialConnection()
{

}

QStringList SerialConnection::availablePorts()
{
    QStringList portNames;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo portInfo, ports)
        portNames.append(portInfo.portName());
    return portNames;
}

bool SerialConnection::open(const QString& s)
{
    if (m_port!=NULL)
        delete m_port;

    ComSettings settings;
    if (!settingsFromString(s, settings))
        return false;

    m_port = new QSerialPort(settings.portName, this);
    m_port->setBaudRate(settings.baudRate);
    m_port->setParity(settings.parity);
    m_port->setDataBits(settings.dataBits);
    m_port->setStopBits(settings.stopBits);

    connect(m_port, SIGNAL(readyRead()), this, SLOT(onPortReadyRead()));
    connect(m_port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onPortError(QSerialPort::SerialPortError)));
    connect(m_port, SIGNAL(aboutToClose()), this, SIGNAL(closed()));

    if (m_port->open(QIODevice::ReadWrite))
    {
        emit opened();
        return true;
    }

    delete m_port;
    m_port = NULL;
    return false;
}

void SerialConnection::close()
{
    if (m_port==NULL)
        return;

    if (m_port->isOpen())
        m_port->close();
}

bool SerialConnection::isOpen() const
{
    if (m_port && m_port->isOpen())
        return true;

    return false;
}

QString SerialConnection::connectionSummary() const
{
    if (m_port==NULL)
        return QString();

    return settingsToString(m_port->portName(), m_port->baudRate(), m_port->parity(), m_port->dataBits(), m_port->stopBits());
}

QByteArray SerialConnection::send(Command *command, char dataByte)
{
    QByteArray sent;

    if (m_port==NULL)
        return sent;

    if (!m_port->isOpen())
        return sent;

    prepareResponse(command);

    QByteArray bytes = command->bytes(dataByte);
    qint64 ret = m_port->write(bytes);

    if (ret >= 0)
        sent = QByteArray(bytes).left(ret);

    return sent;
}

QByteArray SerialConnection::send(Command *command, const QString& message, Command::Endpoint from )
{
    QByteArray sent;

    if (m_port==NULL)
        return sent;

    if (!m_port->isOpen())
        return sent;

    QByteArray bytes = command->bytes(from, Command::Scaler, message);

    qint64 ret = m_port->write(bytes);

    if (ret >= 0)
        sent = QByteArray(bytes).left(ret);

    return sent;
}

void SerialConnection::onPortError(QSerialPort::SerialPortError errorCode)
{
    if (errorCode==QSerialPort::NoError)
        return;

    QString s = QString("error: %1").arg((int)errorCode);
    emit error(s);
}

void SerialConnection::onPortReadyRead()
{
    QByteArray a = m_port->readAll();
    processReceived(a);
}

QString SerialConnection::settingsToString(const QString& portName, int baudRate, QSerialPort::Parity parity, QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits)
{
    QString settings;
    settings.append(portName);
    settings.append(",");
    settings.append(QString::number(baudRate));
    settings.append(",");
    settings.append(parityToString(parity));
    settings.append(",");
    settings.append(dataBitsToString(dataBits));
    settings.append(",");
    settings.append(stopBitsToString(stopBits));
    return settings;
}

QString SerialConnection::baudRateToString(QSerialPort::BaudRate b)
{
    switch(b)
    {
    case QSerialPort::Baud1200:
        return("1200");
        break;

    case QSerialPort::Baud2400:
        return("2400");
        break;

    case QSerialPort::Baud4800:
        return("4800");
        break;

    case QSerialPort::Baud9600:
        return("9600");
        break;

    case QSerialPort::Baud19200:
        return("19200");
        break;

    case QSerialPort::Baud38400:
        return("38400");
        break;

    case QSerialPort::Baud57600:
        return("57600");
        break;

    case QSerialPort::Baud115200:
        return("115200");
        break;
    }
    return("??");
}

QString SerialConnection::baudRateToString(quint32 b)
{
    return QString::number(b);
}

QString SerialConnection::parityToString(QSerialPort::Parity p)
{
    switch (p)
    {
    case QSerialPort::NoParity:
        return("N");
        break;

    case QSerialPort::EvenParity:
        return("E");
        break;

    case QSerialPort::OddParity:
        return("O");
        break;

    case QSerialPort::MarkParity:
        return("M");
        break;

    case QSerialPort::SpaceParity:
        return("S");
        break;
    }
    return QString("?");
}

QString SerialConnection::dataBitsToString(QSerialPort::DataBits d)
{
    switch(d)
    {
    case QSerialPort::Data5:
        return("5");
        break;

    case QSerialPort::Data6:
        return("6");
        break;

    case QSerialPort::Data7:
        return("7");
        break;

    case QSerialPort::Data8:
        return("8");
        break;
    }
    return("X");
}

QString SerialConnection::stopBitsToString(QSerialPort::StopBits s)
{
    switch(s)
    {
    case QSerialPort::OneStop:
        return("1");
        break;

    case QSerialPort::OneAndHalfStop:
        return("1.5");
        break;

    case QSerialPort::TwoStop:
        return("2");
        break;
    }
    return("?");
}

bool SerialConnection::settingsFromString(const QString& s, SerialConnection::ComSettings& settings)
{
    QStringList parts = s.split(",");
    if (parts.count() != COM_SETTINGS_PARTS_COUNT )
        return false;

    settings.portName = parts.at(0);
    settings.baudRate = baudRateFromString(parts.at(1));
    settings.parity = parityFromString(parts.at(2));
    settings.dataBits = dataBitsFromString(parts.at(3));
    settings.stopBits = stopBitsFromString(parts.at(4));
    return true;
}

quint32 SerialConnection::baudRateFromString(const QString& b)
{
    return b.toInt();
}

QSerialPort::Parity SerialConnection::parityFromString(const QString& p)
{
    if (p.toUpper()=="E")
        return QSerialPort::EvenParity;
    else if (p.toUpper()=="N")
        return QSerialPort::NoParity;
    else if (p.toUpper()=="O")
        return QSerialPort::OddParity;
    else if (p.toUpper()=="M")
        return QSerialPort::MarkParity;
    else if (p.toUpper()=="S")
        return QSerialPort::SpaceParity;

    return QSerialPort::NoParity;
}

QSerialPort::DataBits SerialConnection::dataBitsFromString(const QString& d)
{
    if (d.toUpper()=="5")
        return QSerialPort::Data5;
    else if (d.toUpper()=="6")
        return QSerialPort::Data6;
    else if (d.toUpper()=="7")
        return QSerialPort::Data7;
    else if (d.toUpper()=="8")
        return QSerialPort::Data8;

    return QSerialPort::Data8;
}

QSerialPort::StopBits SerialConnection::stopBitsFromString(const QString& s)
{
    if (s=="1")
        return QSerialPort::OneStop;
    else if (s=="1.5")
        return QSerialPort::OneAndHalfStop;
    else if (s=="2")
        return QSerialPort::TwoStop;

    return QSerialPort::OneStop;
}
