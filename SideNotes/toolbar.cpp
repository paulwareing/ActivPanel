#include <QCoreApplication>
#include <QVBoxLayout>
#include <QIcon>

#include "toolbar.h"
#include "pushbutton.h"

Toolbar::Toolbar(QWidget *parent)
    : QWidget{parent}
{
    setFixedWidth(32);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    setLayout(layout);

    auto *homeButton = new PushButton(this);
    homeButton->setIcon(QIcon(":/icons/icons/home.png"));
    homeButton->setIconSize(QSize(16, 16));
    homeButton->setToolTip("Home position");
    homeButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(homeButton);

    auto *openButton = new PushButton(this);
    openButton->setIcon(QIcon(":/icons/icons/open.png"));
    openButton->setIconSize(QSize(16, 16));
    openButton->setToolTip("Open a SideNote");
    openButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(openButton);

    auto *saveButton = new PushButton(this);
    saveButton->setIcon(QIcon(":/icons/icons/save.png"));
    saveButton->setIconSize(QSize(16, 16));
    saveButton->setToolTip("Save this SideNote");
    saveButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(saveButton);

    layout->addSpacing(8);

    auto *clearButton = new PushButton(this);
    clearButton->setIcon(QIcon(":/icons/icons/eraser.png"));
    clearButton->setIconSize(QSize(16, 16));
    clearButton->setToolTip("Clear Canvas");
    clearButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(clearButton);

    auto *paletteButton = new PushButton(this);
    paletteButton->setIcon(QIcon(":/icons/icons/palette.png"));
    paletteButton->setIconSize(QSize(16, 16));
    paletteButton->setToolTip("Pen size and color");
    paletteButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(paletteButton);

    auto *undoButton = new PushButton(this);
    undoButton->setIcon(QIcon(":/icons/icons/undo.png"));
    undoButton->setIconSize(QSize(16, 16));
    undoButton->setToolTip("Undo");
    undoButton->setCursor(Qt::PointingHandCursor);

    layout->addWidget(undoButton);

    auto *redoButton = new PushButton(this);
    redoButton->setIcon(QIcon(":/icons/icons/redo.png"));
    redoButton->setIconSize(QSize(16, 16));
    redoButton->setToolTip("Redo");
    redoButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(redoButton);

    layout->addSpacing(8);

    auto *exitButton = new PushButton(this);
    exitButton->setIcon(QIcon(":/icons/icons/exit.png"));
    exitButton->setIconSize(QSize(16, 16));
    exitButton->setToolTip("Exit SideNotes");
    exitButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(exitButton);

    connect(exitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}
