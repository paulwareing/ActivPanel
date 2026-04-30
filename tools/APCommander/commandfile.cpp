#include <QtCore>
#include <QtXml>

#include "commandfile.h"
#include "command.h"

CommandFile::CommandFile()
{

}

CommandFile::~CommandFile()
{
    deleteCommands();
}

bool CommandFile::open(const QString& filePath)
{
    deleteCommands();
    return parseCommandFile(filePath);
}

void CommandFile::close()
{
    deleteCommands();
}

Command * CommandFile::command(const QString& name)
{
    foreach(Command *command, m_commands)
    {
        if(command->name() == name)
            return command;
    }
    return NULL;
}

const QString& CommandFile::text() const
{
    return m_fileContents;
}

bool CommandFile::parseCommandFile(const QString& filePath)
{
    QFileInfo info(filePath);
    if (!info.exists())
        return false;

    QDomDocument doc(info.filePath());
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }

    file.reset();
    m_fileContents = file.readAll();

    file.close();

    QDomNode root = doc.documentElement();
    if (root.nodeName() != "apcommands")
        return false;

    QString panel = parseRootNode(root);

    QDomNode node = root.firstChild();
    while(!node.isNull())
    {
        if (node.nodeName()=="command")
        {
            Command *command = parseCommandNode(node);
            if (command != NULL)
            {
                m_commands.append(command);
                emit commandOpened(command);
            }
        }
        else if (node.nodeName() == "port")
        {
            parsePortNode(node);
        }
        node = node.nextSibling();
    }
    return true;
}

QString CommandFile::parseRootNode(const QDomNode& node)
{
    QDomNamedNodeMap attrs = node.attributes();
    QString panel = attrs.namedItem("panel").nodeValue();
    return panel;
}

Command * CommandFile::parseCommandNode(const QDomNode& node)
{
    QDomNamedNodeMap attrs = node.attributes();
    QString name = attrs.namedItem("name").nodeValue();
    QString bytes = attrs.namedItem("bytes").nodeValue();

    if (name.isEmpty())
        return NULL;

    if (bytes.isEmpty())
        return NULL;

    Command *command = new Command(name, bytes);

    QString input = attrs.namedItem("input").nodeValue();
    command->setInputType(input);

    if (input=="enum" && node.hasChildNodes())
    {
        QDomNode child = node.firstChild();
        while(!child.isNull())
        {
            if (child.nodeName()=="enum")
            {
                QDomNamedNodeMap attrs = child.attributes();
                QString value = attrs.namedItem("value").nodeValue();
                QString result = attrs.namedItem("result").nodeValue();
                if (!value.isEmpty() && !result.isEmpty())
                    command->addInputEnum(value, result);
            }
            child = child.nextSibling();
        }
    }

    QString min = attrs.namedItem("min").nodeValue();
    if (!min.isEmpty())
        command->setDataMin(min.toInt());

    QString max = attrs.namedItem("max").nodeValue();
    if (!max.isEmpty())
        command->setDataMax(max.toInt());

    QString responseType = attrs.namedItem("response").nodeValue();
    if (!responseType.isEmpty())
    {
        command->setResponseType(responseType);
        if (responseType=="enum" && node.hasChildNodes())
        {
            QDomNode child = node.firstChild();
            while(!child.isNull())
            {
                if (child.nodeName()=="enum")
                {
                    QDomNamedNodeMap attrs = child.attributes();
                    QString value = attrs.namedItem("value").nodeValue();
                    QString result = attrs.namedItem("result").nodeValue();
                    if (!value.isEmpty() && !result.isEmpty())
                        command->addEnumResult(value, result);
                }
                child = child.nextSibling();
            }
        }
    }
    return command;
}

void CommandFile::parsePortNode(const QDomNode& node)
{
    QDomNamedNodeMap attrs = node.attributes();
    QString baudRate = attrs.namedItem("baudRate").nodeValue();
    QString parity = attrs.namedItem("parity").nodeValue();
    QString dataBits = attrs.namedItem("dataBits").nodeValue();
    QString stopBits = attrs.namedItem("stopBits").nodeValue();

    if (!baudRate.isEmpty() && !parity.isEmpty() & !dataBits.isEmpty() & !stopBits.isEmpty())
    {
        QString port = baudRate + "," + parity + "," + dataBits + "," + stopBits;
        emit portSettings(port);
    }
}

void CommandFile::deleteCommands()
{
    foreach(Command *command, m_commands)
        delete command;
    m_commands.clear();
}
