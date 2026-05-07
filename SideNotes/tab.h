#ifndef TAB_H
#define TAB_H

#include <QWidget>

#define TAB_WIDTH       2

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QSize size, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *) override;

signals:
    void clicked();
};

#endif // TAB_H
