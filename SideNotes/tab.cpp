#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>

#include "tab.h"

Tab::Tab(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(false);
    setFixedSize(TAB_WIDTH, TAB_HEIGHT);

}

void Tab::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void Tab::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, TAB_WIDTH, TAB_HEIGHT), 10, 10);
    QPen pen(Qt::black, -1);
    p.setPen(pen);
    p.fillPath(path, QColor(128, 128, 128, 128));
    p.drawPath(path);
}
