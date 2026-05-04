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
    , m_leftTray(nullptr)
    , m_rightTray(nullptr)
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
    if (m_leftTray == nullptr)
    {
        // Add the left hand tray, and hang it off the LHS with only the tray tab visible.
        m_leftTray = new Tray(Qt::AlignLeft, this);
        m_leftTray->setFixedSize(QSize((width()/2), height()));
        m_leftTray->move(m_leftTray->mapToParent(QPoint(0-(width()/2)+TAB_WIDTH, 0)));
        m_leftTray->show();
    }

    if (m_rightTray == nullptr)
    {
        // Add the right hand tray, and hang it off the RHS with only the tray tab visible.
        m_rightTray = new Tray(Qt::AlignRight, this);
        m_rightTray->setFixedSize(QSize((width()/2), height()));
        m_rightTray->move(m_rightTray->mapToParent(QPoint(width() - TAB_WIDTH, 0)));
        m_rightTray->show();
    }
}
