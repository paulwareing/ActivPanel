#include <QPropertyAnimation>
#include <QHBoxLayout>

#include "tray.h"
#include "tab.h"
#include "toolbar.h"
#include "scribbleview.h"

Tray::Tray(Qt::Alignment side, QWidget *parent)
    : QWidget{parent}
    , _side(side)
    , _open(false)
{
    _open = false;

    auto layout = new QHBoxLayout(this);
    setLayout(layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    auto scribbleview = new ScribbleView();
    connect(scribbleview, SIGNAL(swipeLeft()), this, SLOT(onTogglePosition()));

    auto tabLayout = new QVBoxLayout();

    if (_side == Qt::AlignLeft)
    {
        layout->addWidget(scribbleview);
        layout->addLayout(tabLayout);
    }
    else
    {
        layout->addLayout(tabLayout);
        layout->addWidget(scribbleview);
    }

    auto w1 = new QWidget(this);
    tabLayout->addWidget(w1);

    auto tab = new Tab(this);
    tabLayout->addWidget(tab);
    connect(tab, SIGNAL(clicked()), this ,SLOT(onTogglePosition()));

    auto w2 = new QWidget(this);
    tabLayout->addWidget(w2);

    _toolbar = new Toolbar(scribbleview);
    _toolbar->raise();
}

void Tray::showEvent(QShowEvent */*event*/)
{
    if (_side == Qt::AlignRight)
       _toolbar->move(width()-_toolbar->width()-TAB_WIDTH-10, (height()/2)-(_toolbar->height()/2));
    else
        _toolbar->move(10, (height()/2)-(_toolbar->height()/2));
}

void Tray::onTogglePosition()
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "pos");
    anim->setDuration(300);
    anim->setStartValue(pos());
    if (!_open)
        if (_side == Qt::AlignLeft)
            anim->setEndValue(QPoint(0,0));
        else
            anim->setEndValue(QPoint(width(),0));
    else
        if (_side == Qt::AlignLeft)
            // ensure the tab is shown even when the tray is off-screen
            anim->setEndValue(QPoint(0-width()+TAB_WIDTH, 0));
        else
            anim->setEndValue(QPoint((width()*2)-TAB_WIDTH,0));
    _open = !_open;
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
