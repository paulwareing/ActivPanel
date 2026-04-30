#ifndef RESPONSEINTERPRETER_H
#define RESPONSEINTERPRETER_H

#include <QByteArray>

#include "command.h"
#include "message.h"

class Response : public Message
{
public:
    Response();

    void setCommand(Command *);
    QString result(const QByteArray& packet);

private:
    Command *m_command;
};

#endif // RESPONSEINTERPRETER_H
