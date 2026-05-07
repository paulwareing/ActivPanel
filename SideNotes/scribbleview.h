#ifndef SCRIBBLEVIEW_H
#define SCRIBBLEVIEW_H

#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QTimer>
#include <QElapsedTimer>

class ScribbleView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScribbleView(QWidget *parent = nullptr);

public slots:
    void onInertiaTimer(void);

signals:
    void horizontalSwipe(qreal deltaX);
    void swipeLeft();
    void swipeRight();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void tabletEvent(QTabletEvent *event) override;
    bool viewportEvent(QEvent *event) override;

private:
    enum class TouchMode {
        Unknown,
        VerticalScroll,
        HorizontalSwipe
    };

    TouchMode _touchMode = TouchMode::Unknown;

    static constexpr qreal gestureThreshold = 8.0; // pixels

    QPoint _lastMousePos;

    QPointF _touchStartPos;
    QPointF _lastTouchPos;

    QPointF _lastScenePos;
    QPointF _currentItemSceneOffset;
    QGraphicsPathItem *_currentPathItem = nullptr;

    QPen _pen;

    QTimer _inertiaTimer;
    qreal _velocityY = 0.0;
    QElapsedTimer _velocityClock;

    static constexpr qreal _friction = 0.90;   // slows motion
    static constexpr qreal _minVelocity = 0.5; // stop threshold

};

#endif // SCRIBBLEVIEW_H
