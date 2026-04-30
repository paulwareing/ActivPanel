#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QHash>
#include <QChar>

#include "message.h"

class Command : public Message
{
public:
    enum Endpoint
    {
        Scaler = 1,
        OPS = 2,
        Laptop = 3
    };

    static QString EndpointName(Endpoint);

    Command(QString name, QString bytes);
    virtual ~Command();

    QString name();
    QByteArray bytes(char dataByte);
    QByteArray bytes(Endpoint from, Endpoint to, const QString& message);
    QByteArray bytes();

    void setInputType(const QString&);
    QString inputType() const;

    void setDataMin(int);
    int dataMin() const;

    void setDataMax(int);
    int dataMax() const;

    void addInputEnum(const QString& code, const QString& name);
    QString inputEnumName(const QString& code) const;
    QString inputEnumCode(const QString& name) const;
    QStringList inputEnumCodes();

    void setResponseType(const QString& );
    QString responseType() const;

    void addEnumResult(const QString& code, const QString& result);
    QString enumResult(const QString& code) const;

private:
    QString m_name;

    QString m_inputType;
    int m_min;
    int m_max;
    QHash<QString, QString> m_inputEnum;

    QString m_responseType;
    QHash<QString, QString> m_responseEnum;
};

#endif // COMMAND_H
