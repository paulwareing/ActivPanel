#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScreen>
#include <QHBoxLayout>

#include "mainwindow.h"
#include "tray.h"
#include "tab.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _leftTray(nullptr)
    , _rightTray(nullptr)
{
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    if (primaryScreen)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);

        QSize screenSize = primaryScreen->size();
        int width = screenSize.width();
        int height = screenSize.height();

        // Align the mainWindow geometry with the screen
        setGeometry(0,0,width, height);
    }
    show();
}

MainWindow::~MainWindow() = default;

void MainWindow::showEvent(QShowEvent */*event*/)
{
    if (_leftTray == nullptr)
    {
        // Add the left hand tray, and hang it off the LHS with only the tray tab visible.
        _leftTray = new Tray(Qt::AlignLeft, size(), this);
        _leftTray->setFixedSize(QSize((width()/2), height()));
        _leftTray->move(_leftTray->mapToParent(QPoint(0-(width()/2)+TAB_WIDTH, 0)));
        _leftTray->show();
    }

    if (_rightTray == nullptr)
    {
        // Add the right hand tray, and hang it off the RHS with only the tray tab visible.
        _rightTray = new Tray(Qt::AlignRight, size(), this);
        _rightTray->setFixedSize(QSize((width()/2), height()));
        _rightTray->move(_rightTray->mapToParent(QPoint(width() - TAB_WIDTH, 0)));
        _rightTray->show();
    }
}
