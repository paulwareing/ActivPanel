#ifndef COMMANDFILE_H
#define COMMANDFILE_H

#include <QList>
#include <QString>
#include <QDomNode>
#include <QObject>

class QString;
class QDomNode;
class Command;

class CommandFile : public QObject
{
    Q_OBJECT

public:
    CommandFile();
    virtual ~CommandFile();

    bool open(const QString&filePath);
    void close();

    Command *command(const QString& name);

    const QString& text() const;

signals:
    void commandOpened(Command *);
    void portSettings(const QString&);

private:
    bool parseCommandFile(const QString& filePath);
    QString parseRootNode(const QDomNode&);
    Command * parseCommandNode(const QDomNode&);
    void parsePortNode(const QDomNode&);
    void deleteCommands();

    QList<Command *> m_commands;
    QString m_fileContents;

};

#endif // COMMANDFILE_H
