#ifndef TRAY_H
#define TRAY_H

#include <QWidget>

class Toolbar;

class Tray : public QWidget
{
    Q_OBJECT

public:
    explicit Tray(Qt::Alignment, QWidget *parent = nullptr);

public slots:
    void onTogglePosition();

protected:
    void showEvent(QShowEvent *) override;

signals:

private:
    Toolbar * _toolbar;
    Qt::Alignment _side;
    bool _open;
};

#endif // TRAY_H
