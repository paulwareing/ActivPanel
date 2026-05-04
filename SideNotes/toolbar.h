#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

#define TOOLBAR_WIDTH   30

class Toolbar : public QWidget
{
    Q_OBJECT

public:
    explicit Toolbar(QWidget *parent = nullptr);

signals:

};

#endif // TOOLBAR_H
