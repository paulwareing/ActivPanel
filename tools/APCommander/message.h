#ifndef TRANSFER_H
#define TRANSFER_H

#include <QByteArray>

class Message
{
public:
    Message();
    virtual ~Message();

    QByteArray bytes();
    void setBytes(const QByteArray&);

    static QString toString(const QByteArray&);
    static QByteArray fromString(const QString&);

protected:
    QByteArray m_bytes;
};

#endif // TRANSFER_H
