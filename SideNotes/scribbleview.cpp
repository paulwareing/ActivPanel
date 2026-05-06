#include <QGraphicsScene>
#include <QScrollBar>
#include <QPen>
#include <QTabletEvent>
#include <QLayout>

#include "scribbleview.h"

#define DEFAULT_SCRIBBLE_HEIGHT     200000

ScribbleView::ScribbleView(QWidget *parent)
    : QGraphicsView(parent)
{
    setStyleSheet("background: transparent;");
    viewport()->setStyleSheet("background: rgba(16, 16, 100, 200);");


    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);

    // Disable scrollbars — we control scrolling with gestures
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setAttribute(Qt::WA_AcceptTouchEvents);
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);

    setMouseTracking(true);

    auto scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, width(), DEFAULT_SCRIBBLE_HEIGHT);
    setScene(scene);

    _pen = QPen(Qt::white, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    connect(&_inertiaTimer, SIGNAL(timeout()), this, SLOT(onInertiaTimer()));
    _inertiaTimer.setInterval(16); // ~60 FPS
}

void ScribbleView::mousePressEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    _lastMousePos = event->pos();

    if (event->button() == Qt::LeftButton)
    {
        _lastScenePos = mapToScene(event->pos());
        _currentItemSceneOffset = _lastScenePos;

        QPainterPath path;
        _currentPathItem = scene()->addPath(path, _pen);

        // QGraphicsPathItems always start at 0,0, so the whole item needs translating to the scene start pos...
        _currentPathItem->setPos(_currentItemSceneOffset);
     }

    QGraphicsView::mousePressEvent(event);
}

void ScribbleView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    if (event->buttons() & Qt::LeftButton && _currentPathItem)
    {
        QPointF scenePos = mapToScene(event->pos());

        if (scenePos != _lastScenePos)
        {
            QPainterPath path = _currentPathItem->path();
            // And when we add new path lines we need to take the item/scene offset back into account.
            path.lineTo(scenePos - _currentItemSceneOffset);
            _currentPathItem->setPath(path);

            _lastScenePos = scenePos;
        }
    }

    if (event->buttons() & Qt::RightButton)
    {
        qreal dy = event->pos().y() - _lastMousePos.y();
        translate(0, dy);

        qint64 dt = _velocityClock.restart();
        if (dt > 0)
            _velocityY = dy * (16.0 / dt); // normalize

        _inertiaTimer.stop(); // user is in control
        _velocityClock.start();
    }

    _lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void ScribbleView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->source() != Qt::MouseEventNotSynthesized)
        return;

    if (event->button() == Qt::LeftButton)
        _currentPathItem = nullptr;

    if (event->button() == Qt::RightButton)
    {
        if (std::abs(_velocityY) > _minVelocity)
            _inertiaTimer.start();
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
        _lastScenePos = scenePos;
        _currentItemSceneOffset = _lastScenePos;

        QPainterPath path;
        _currentPathItem = scene()->addPath(path, _pen);

        // QGraphicsPathItems always start at 0,0, so the whole item needs translating to the scene start pos...
        _currentPathItem->setPos(_currentItemSceneOffset);

        break;
    }

    case QEvent::TabletMove:
    {
        if (!_currentPathItem)
            break;

        QPainterPath path = _currentPathItem->path();
        path.lineTo(scenePos - _currentItemSceneOffset);
        _currentPathItem->setPath(path);

        _lastScenePos = scenePos;
        break;
    }

    case QEvent::TabletRelease:
        _currentPathItem = nullptr;
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
            _velocityClock.start();
            _velocityY = 0.0;
            _inertiaTimer.stop();

            _lastTouchPos = pos;
        }
        else if (event->type() == QEvent::TouchUpdate)
        {
            qreal dy = pos.y() - _lastTouchPos.y();
            translate(0, dy);

            qint64 dt = _velocityClock.restart();
            if (dt > 0)
                _velocityY = dy * (16.0 / dt);

            _inertiaTimer.stop();
            _lastTouchPos = pos;
        }
        else if (event->type() == QEvent::TouchEnd)
        {
            if (std::abs(_velocityY) > _minVelocity)
                _inertiaTimer.start();
        }

        event->accept();
        return true;
    }

    return QGraphicsView::viewportEvent(event);
}

void ScribbleView::onInertiaTimer(void)
{
    if (std::abs(_velocityY) < _minVelocity)
    {
        _inertiaTimer.stop();
        return;
    }

    translate(0, _velocityY);
    _velocityY *= _friction;
}
