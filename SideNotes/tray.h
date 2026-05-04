#ifndef TRAY_H
#define TRAY_H

#include <QWidget>

class Tray : public QWidget
{
    Q_OBJECT

public:
    explicit Tray(Qt::Alignment, QWidget *parent = nullptr);

public slots:
    void onTogglePosition();

signals:

private:
    Qt::Alignment m_side;
    bool m_open;
};

#endif // TRAY_H
