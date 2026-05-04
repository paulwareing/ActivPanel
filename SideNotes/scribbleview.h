#ifndef SCRIBBLEVIEW_H
#define SCRIBBLEVIEW_H

#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QMouseEvent>

class ScribbleView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScribbleView(QWidget *parent = nullptr);

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
};

#endif // SCRIBBLEVIEW_H
