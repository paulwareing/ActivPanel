#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

#include "command.h"

class Response;

class Connection : public QObject
{
    Q_OBJECT

public:
    Connection(QObject *parent = 0);
    virtual ~Connection();

    virtual bool open(const QString& settings) = 0;
    virtual void close() = 0;

    virtual bool isOpen() const;
    virtual QString connectionSummary() const;

    virtual QByteArray send(Command *command, char dataByte ) = 0;
    virtual QByteArray send(Command *command, const QString& message, Command::Endpoint from ) = 0;

signals:
    void opened();
    void closed();
    void received(const QByteArray&);
    void result(const QString&);
    void error(const QString&);

protected:
    Response * m_response;

    void prepareResponse(Command *);
    void processReceived(const QByteArray&);

private:
    QByteArray m_buffer;

    static bool isStartOfPacket(const QByteArray& buffer);
    static int containsACompletePacket(const QByteArray& buffer, bool *csok);
    static char checksumOfPacket(const QByteArray& buffer, int indexOfEOF);
};

#endif // CONNECTION_H
