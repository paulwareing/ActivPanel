#include "response.h"

Response::Response()
: m_command(NULL)
{

}

void Response::setCommand(Command *command)
{
    m_command = command;
    m_bytes.clear();
}

QString Response::result(const QByteArray &packet)
{
    m_bytes = packet;

    if (m_command==NULL)
        return QString();

    if (m_command->responseType()=="string")
    {
        if (packet.size()<6)
            return QString("error!");

        QString s;
        for(int i=3 ; i<m_bytes.size()-2 ; ++i)
            s.append(m_bytes.at(i));

        return (m_command->name().remove("?") + " = " + s);
    }
    else if (m_command->responseType()=="enum")
    {
        if (m_bytes.size() < 4)
            return QString("error!");

        QString code = QString("%1").arg((unsigned char)m_bytes.at(3), 2, 16, QChar('0')).toUpper();
        QString s = m_command->enumResult(code);
        return (m_command->name().remove("?") + " = " + s);
    }
    else if (m_command->responseType() == "value")
    {
        if (m_bytes.size() < 4)
            return QString("error!");

        int value = 0;
        int count = m_bytes.size() - 5; // 5 = frame length
        int index = 3; // first value byte
        for (int i=index ; i<index+count; ++i)
        {
            value = value << 8;
            value += m_bytes.at(i);
        }

        return (m_command->name().remove("?") + " = " + QString::number(value) + " (" + QString::number(value, 16).toUpper() + "h)");
    }

    return QString();
}
