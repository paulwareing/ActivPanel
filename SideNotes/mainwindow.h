#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Tray;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void showEvent(QShowEvent *event) override;

private:
    Tray * _leftTray;
    Tray * _rightTray;
};

#endif // MAINWINDOW_H
