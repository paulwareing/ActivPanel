#include <QStringList>

#include "command.h"

QString Command::EndpointName(Endpoint ep)
{
    switch (ep)
    {
    case Scaler:
        return QString("scaler");
        break;

    case OPS:
        return QString("OPS");
        break;

    case Laptop:
        return QString("laptop");
        break;
    }
    return QString("unknown");
}

Command::Command(QString name, QString bytes)
: m_name(name)
, m_min(0)
, m_max(0)
{
    Message::setBytes(Message::fromString(bytes));
}

Command::~Command()
{

}

QString Command::name()
{
    return m_name;
}

QByteArray Command::bytes(char dataByte)
{
    QByteArray bytes = m_bytes;
    if (!m_inputType.isEmpty())
    {
        char *buffer = bytes.data();
        buffer[3] = dataByte;
        buffer[4] = (buffer[4] + dataByte) & 0xff;
    }
    return bytes;
}

QByteArray Command::bytes(Command::Endpoint from, Command::Endpoint to, const QString& message)
{
    // We're going to ignore m_bytes for a 'message' byte array, and build the byte array from scratch.
    QByteArray bytes;
    bytes.append((unsigned char)0xF6);
    bytes.append(0x3F);
    bytes.append((unsigned char)from);
    bytes.append((unsigned char)to);
    bytes.append(message.length());
    bytes.append(message.toUtf8());

    char cs = 0;
    for (int i=0 ; i<bytes.length() ; ++i)
    {
        cs += bytes.at(i);
    }
    bytes.append(cs);
    bytes.append(0x6F);
    return bytes;
}

QByteArray Command::bytes()
{
    return m_bytes;
}

void Command::setInputType(const QString& s)
{
    m_inputType = s;
}

QString Command::inputType() const
{
    return m_inputType;
}

void Command::addInputEnum(const QString& code, const QString& name)
{
    m_inputEnum.insert(code, name);
}

QString Command::inputEnumName(const QString& code) const
{
    return m_inputEnum.value(code);
}

QString Command::inputEnumCode(const QString& name) const
{
    return m_inputEnum.key(name);
}

QStringList Command::inputEnumCodes()
{
    QStringList codes = m_inputEnum.keys();
    codes.sort();
    return codes;
}

void Command::setDataMin(int min)
{
    m_min = min;
}

int Command::dataMin() const
{
    return m_min;
}

void Command::setDataMax(int max)
{
    m_max = max;
}

int Command::dataMax() const
{
    return m_max;
}

void Command::setResponseType(const QString& s)
{
    m_responseType = s;
}

QString Command::responseType() const
{
    return m_responseType;
}

void Command::addEnumResult(const QString& code, const QString& result)
{
    m_responseEnum.insert(code, result);
}

QString Command::enumResult(const QString& code) const
{
    return m_responseEnum.value(code);
}
