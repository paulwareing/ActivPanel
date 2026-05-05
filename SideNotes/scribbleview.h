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

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void tabletEvent(QTabletEvent *event) override;
    bool viewportEvent(QEvent *event) override;

private:
    QPoint lastMousePos;
    QPointF lastTouchPos;

    QPointF lastScenePos;
    QPointF currentItemSceneOffset;
    QGraphicsPathItem *currentPathItem = nullptr;

    QPen _pen;

    QTimer inertiaTimer;
    qreal velocityY = 0.0;
    QElapsedTimer velocityClock;

    static constexpr qreal friction = 0.90;   // slows motion
    static constexpr qreal minVelocity = 0.5; // stop threshold

};

#endif // SCRIBBLEVIEW_H
