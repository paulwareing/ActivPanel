#include <QDebug>

#include "connection.h"
#include "command.h"
#include "response.h"

Connection::Connection(QObject *parent)
: QObject(parent)
, m_response(NULL)
{

}

Connection::~Connection()
{
    delete m_response;
}

bool Connection::isOpen() const
{
   return false;
}

QString Connection::connectionSummary() const
{
    return QString();
}

void Connection::prepareResponse(Command *command)
{
    delete m_response;
    m_response = NULL;
    if (!command->responseType().isEmpty())
    {
        m_response = new Response();
        m_response->setCommand(command);
    }
}

void Connection::processReceived(const QByteArray& a)
{
    //First - we need to build a framed, if indeed we're receiving a framed packet.
    m_buffer.append(a);

    // For now we'll assume that if the received parts don't start with a 0xf6 char, then it's not framed.
    if (!isStartOfPacket(m_buffer))
    {
        // So we'll spit out the result, and wait for the next partial..
        emit received(m_buffer);
        m_buffer.clear();
        return;
    }

    //Next - let's check if the LHS of the buffer (upto and including the whole thing) contains a fully framed packet.
    bool csok = false;
    int packetLength = containsACompletePacket(m_buffer, &csok);
    if (packetLength <= 0)
    {
        // we don't recognise a correctly framed packet in the buffer.

        // how large is the buffer?
        if (m_buffer.size() > 256)
        {
            // ok - our buffer is too full.  we're gonna dump what we have and start again.
            emit received(m_buffer);
            m_buffer.clear();
        }
        // else
            // wait for more bytes and check for a complete packet next time we receive something.

        return;
    }

    // Check the returned packet length can be contained in the buffer size.
    if (m_buffer.length() < packetLength)
    {
        //oops - something went wrong;
        // So we'll spit out the result, and wait for the next partial..
        emit received(m_buffer);
        m_buffer.clear();
        return;
    }

    // Snap off the packet - leave the rest in m_buffer...
    QByteArray packet = m_buffer.left(packetLength);
    m_buffer = m_buffer.right(m_buffer.length() - packetLength);

    // Now process the packet...
    if (m_response != NULL)
    {
        emit received(packet);

        QString s = m_response->result(packet);

        if (!s.isEmpty())
        {
            if (!csok)
                s=s.append("   (BAD CHECKSUM!?)");

            emit result(s);
        }
        delete m_response;
        m_response = NULL;
    }
    else if (packet.at(1) == 0x3f)
    {
        // the packet is a generic UART packet...

        unsigned char from = packet.at(2);
        //unsigned char to = packet.at(3);
        unsigned char length = packet.at(4);
        QString message = packet.mid(5, length).trimmed().simplified();

        QString res = ("Received '" + message + "' from " + Command::EndpointName((Command::Endpoint)from));
        emit result(res);
        emit received(packet);
    }
    else
    {
        // we don't recognise the packet.  so we'll just spit it out.
        emit received(packet);
    }
}

bool Connection::isStartOfPacket(const QByteArray& buffer)
{
    return (buffer.startsWith(0xf6));
}

// returns the length of the complete packet af the front of 'buffer', if there is one.  otherwise returns 0
int Connection::containsACompletePacket(const QByteArray& buffer, bool *csok)
{
    if (!isStartOfPacket(buffer))
        return 0;

    *csok = false;

    bool looking = true;
    bool found= false;
    int indexOfEOF = buffer.indexOf(0x6f);
    while (looking)
    {
        if (indexOfEOF < 0)
        {
            looking = false;
            found = false;
        }
        else
        {
            char cs = buffer.at(indexOfEOF-1);
            if (cs == checksumOfPacket(buffer, indexOfEOF))
            {
                found = true;
                looking = false;
                *csok = true;
            }
            else
            {
                if (buffer.length() > indexOfEOF+1)
                    indexOfEOF = buffer.indexOf(0x6f, indexOfEOF+1);
                else if (buffer.length() == indexOfEOF+1)
                {
                    // full packet with bad checksum;
                    found = true;
                    looking = false;
                }
                else
                {
                    looking = false;
                    found = false;
                }
            }
        }
    }

    if (!found)
        return 0;

    return indexOfEOF+1;
}

char Connection::checksumOfPacket(const QByteArray& buffer, int indexOfEOF)
{
    char cs = 0;
    for( int i=0 ; i<indexOfEOF-1 ; ++i)
    {
        cs += buffer.at(i);
    }
    return cs;
}
