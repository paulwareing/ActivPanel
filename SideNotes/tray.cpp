#include <QPropertyAnimation>
#include <QHBoxLayout>

#include "tray.h"
#include "tab.h"
#include "toolbar.h"
#include "scribbleview.h"

Tray::Tray(Qt::Alignment side, QWidget *parent)
    : QWidget{parent}
    , m_side(side)
{
    m_open = false;

    auto layout = new QHBoxLayout(this);
    setLayout(layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    auto toolbar = new Toolbar();
    auto scribbleview = new ScribbleView();
    auto tabLayout = new QVBoxLayout();

    if (m_side == Qt::AlignLeft)
    {
        layout->addWidget(toolbar);
        layout->addWidget(scribbleview);
        layout->addLayout(tabLayout);
    }
    else
    {
        layout->addLayout(tabLayout);
        layout->addWidget(scribbleview);
        layout->addWidget(toolbar);
    }

    auto w1 = new QWidget(this);
    tabLayout->addWidget(w1);

    auto tab = new Tab(this);
    tabLayout->addWidget(tab);
    connect(tab, SIGNAL(clicked()), this ,SLOT(onTogglePosition()));

    auto w2 = new QWidget(this);
    tabLayout->addWidget(w2);
}

void Tray::onTogglePosition()
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "pos");
    anim->setDuration(300);
    anim->setStartValue(pos());
    if (!m_open)
        if (m_side == Qt::AlignLeft)
            anim->setEndValue(QPoint(0,0));
        else
            anim->setEndValue(QPoint(width(),0));
    else
        if (m_side == Qt::AlignLeft)
            // ensure the tab is shown even when the tray is off-screen
            anim->setEndValue(QPoint(0-width()+TAB_WIDTH, 0));
        else
            anim->setEndValue(QPoint((width()*2)-TAB_WIDTH,0));
    m_open = !m_open;
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
