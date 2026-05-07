#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>

#include "tab.h"

Tab::Tab(QSize size, QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background: rgba(5, 5, 5, 5);");
    setFixedSize(TAB_WIDTH, size.height());
}

void Tab::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}
