#-------------------------------------------------
#
# Project created by QtCreator 2017-03-16T14:05:46
#
#-------------------------------------------------

QT += core gui serialport xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = apcommander
TEMPLATE = app

win32:RC_ICONS = images/remote.ico

SOURCES += main.cpp\
    mainwindow.cpp \
    command.cpp \
    commandfile.cpp \
    response.cpp \
    tcpconnection.cpp \
    udpconnection.cpp \
    serialconnection.cpp \
    connection.cpp \
    ipctrl.cpp \
    message.cpp \
    uartmessage.cpp

HEADERS  += mainwindow.h \
    command.h \
    commandfile.h \
    response.h \
    tcpconnection.h \
    udpconnection.h \
    serialconnection.h \
    connection.h \
    ipctrl.h \
    message.h \
    uartmessage.h

RESOURCES += \
    apcommander.qrc
