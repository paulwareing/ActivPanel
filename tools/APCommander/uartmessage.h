#ifndef UARTMESSAGE_H
#define UARTMESSAGE_H

#include <QObject>
#include <QWidget>
#include <QDialog>

class QLineEdit;
class QRadioButton;

class UARTMessage : public QDialog
{
    Q_OBJECT

public:
    UARTMessage();

    QString getMessage() const;
    QString getSource() const;

private:
    QLineEdit * m_editMessage;
    QRadioButton * m_btnOPS;
    QRadioButton * m_btnLaptop;

    void createUI();
};

#endif // UARTMESSAGE_H
