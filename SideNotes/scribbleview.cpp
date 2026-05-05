#include <QGraphicsScene>
#include <QScrollBar>
#include <QPen>
#include <QTabletEvent>

#include "scribbleview.h"

ScribbleView::ScribbleView(QWidget *parent)
    : QGraphicsView(parent)
{
    setStyleSheet("background: transparent;");
    viewport()->setStyleSheet("background: rgba(255, 255, 255, 40);");

    auto scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, width(), 200000);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);

    // Disable scrollbars — we control scrolling with gestures
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setAttribute(Qt::WA_AcceptTouchEvents);
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);

    setMouseTracking(true);

    _pen = QPen(Qt::black, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);


    connect(&inertiaTimer, SIGNAL(timeout()), this, SLOT(onInertiaTimer()));
    inertiaTimer.setInterval(16); // ~60 FPS

}

void ScribbleView::mousePressEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    lastMousePos = event->pos();

    if (event->button() == Qt::LeftButton)
    {
        lastScenePos = mapToScene(event->pos());
        currentItemSceneOffset = lastScenePos;

        QPainterPath path;
        currentPathItem = scene()->addPath(path, _pen);

        // QGraphicsPathItems always start at 0,0, so the whole item needs translating to the scene start pos...
        currentPathItem->setPos(currentItemSceneOffset);
     }

    QGraphicsView::mousePressEvent(event);
}

void ScribbleView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    if (event->buttons() & Qt::LeftButton && currentPathItem)
    {
        QPointF scenePos = mapToScene(event->pos());

        if (scenePos != lastScenePos)
        {
            QPainterPath path = currentPathItem->path();
            // And when we add new path lines we need to take the item/scene offset back into account.
            path.lineTo(scenePos - currentItemSceneOffset);
            currentPathItem->setPath(path);

            lastScenePos = scenePos;
        }
    }

    if (event->buttons() & Qt::RightButton)
    {
        qreal dy = event->pos().y() - lastMousePos.y();
        translate(0, dy);

        qint64 dt = velocityClock.restart();
        if (dt > 0)
            velocityY = dy * (16.0 / dt); // normalize

        inertiaTimer.stop(); // user is in control
        velocityClock.start();
    }

    lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void ScribbleView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    if (event->button() == Qt::LeftButton)
        currentPathItem = nullptr;

    if (event->button() == Qt::RightButton)
    {
        if (std::abs(velocityY) > minVelocity)
            inertiaTimer.start();
    }


    QGraphicsView::mouseReleaseEvent(event);
}

void ScribbleView::tabletEvent(QTabletEvent *event)
{
    if (event->pointerType() != QPointingDevice::PointerType::Pen &&
        event->pointerType() != QPointingDevice::PointerType::Eraser)
    {
        return;
    }

    const QPointF scenePos = mapToScene(event->position().toPoint());

    switch (event->type())
    {
    case QEvent::TabletPress:
    {
        lastScenePos = scenePos;
        currentItemSceneOffset = lastScenePos;

        QPainterPath path;
        currentPathItem = scene()->addPath(path, _pen);

        // QGraphicsPathItems always start at 0,0, so the whole item needs translating to the scene start pos...
        currentPathItem->setPos(currentItemSceneOffset);

        break;
    }

    case QEvent::TabletMove:
    {
        if (!currentPathItem)
            break;

        QPainterPath path = currentPathItem->path();
        path.lineTo(scenePos - currentItemSceneOffset);
        currentPathItem->setPath(path);

        lastScenePos = scenePos;
        break;
    }

    case QEvent::TabletRelease:
        currentPathItem = nullptr;
        break;

    default:
        break;
    }

    event->accept();
}

bool ScribbleView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::TouchUpdate ||
        event->type() == QEvent::TouchEnd)
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

        if (touchEvent->points().size() != 1)
            return true; // ignore multi-touch

        const QTouchEvent::TouchPoint &pt = touchEvent->points().first();
        const QPointF pos = pt.position();

        if (event->type() == QEvent::TouchBegin)
        {
            velocityClock.start();
            velocityY = 0.0;
            inertiaTimer.stop();

            lastTouchPos = pos;
        }
        else if (event->type() == QEvent::TouchUpdate)
        {
            qreal dy = pos.y() - lastTouchPos.y();
            translate(0, dy);

            qint64 dt = velocityClock.restart();
            if (dt > 0)
                velocityY = dy * (16.0 / dt);

            inertiaTimer.stop();
            lastTouchPos = pos;
        }
        else if (event->type() == QEvent::TouchEnd)
        {
            if (std::abs(velocityY) > minVelocity)
                inertiaTimer.start();
        }

        event->accept();
        return true;
    }

    return QGraphicsView::viewportEvent(event);
}

void ScribbleView::onInertiaTimer(void)
{
    if (std::abs(velocityY) < minVelocity)
    {
        inertiaTimer.stop();
        return;
    }

    translate(0, velocityY);
    velocityY *= friction;
}
