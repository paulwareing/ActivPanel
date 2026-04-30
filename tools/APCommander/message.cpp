#include <QStringList>

#include "message.h"

Message::Message()
{

}

Message::~Message()
{

}

QByteArray Message::bytes()
{
    return m_bytes;
}

void Message::setBytes(const QByteArray& bytes)
{
    m_bytes = bytes;
}

QString Message::toString(const QByteArray& bytes)
{
    QString s;
    for(int i=0 ; i<bytes.size() ; ++i)
    {
        s.append(QString("%1").arg((unsigned char)bytes.at(i), 2, 16, QChar('0')).toUpper());
        if (i<bytes.size()-1)
            s.append(" ");
    }
    return s;
}

QByteArray Message::fromString(const QString& s)
{
    QByteArray bytes;
    QStringList byteList = s.split(" ");
    for(int i=0;i<byteList.size();i++)
        bytes.append(byteList.at(i).toInt(NULL,16));

    return bytes;
}
