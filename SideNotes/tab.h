#ifndef TAB_H
#define TAB_H

#include <QWidget>

#define TAB_WIDTH       20
#define TAB_HEIGHT      100

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:
    void clicked();
};

#endif // TAB_H
