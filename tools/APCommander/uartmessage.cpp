#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>

#include "uartmessage.h"

UARTMessage::UARTMessage()
{
    setWindowTitle("Generic UART Comms API Send");
    setModal(true);
    createUI();
    setMinimumWidth(800);
}

void UARTMessage::createUI()
{
    QVBoxLayout *l = new QVBoxLayout();
    setLayout(l);

    QHBoxLayout *ml = new QHBoxLayout();
    l->addLayout(ml);

    QLabel *l1 = new QLabel("Message:");
    ml->addWidget(l1);

    m_editMessage = new QLineEdit();
    m_editMessage->setText("Hello.  How are you?  Have you done your shopping today?");
    ml->addWidget(m_editMessage);
    m_editMessage->adjustSize();

    l->addSpacing(10);

    QHBoxLayout *tl = new QHBoxLayout();
    l->addLayout(tl);

    QLabel *l2 = new QLabel("Sending From:");
    tl->addWidget(l2);

    QVBoxLayout *tb = new QVBoxLayout();
    tl->addLayout(tb);

    QButtonGroup *bg = new QButtonGroup(this);

    m_btnLaptop = new QRadioButton("Laptop");
    tb->addWidget(m_btnLaptop);
    bg->addButton(m_btnLaptop);
    m_btnLaptop->setChecked(true);

    m_btnOPS = new QRadioButton("OPS");
    tb->addWidget(m_btnOPS);
    bg->addButton(m_btnOPS);

    tl->addStretch();

    QHBoxLayout *bl = new QHBoxLayout();
    l->addLayout(bl);

    bl->addStretch();

    QPushButton *bc = new QPushButton("Cancel");
    bl->addWidget(bc);
    connect(bc, SIGNAL(clicked()), this, SLOT(reject()));

    QPushButton *bok = new QPushButton("Ok");
    bl->addWidget(bok);
    connect(bok, SIGNAL(clicked()), this, SLOT(accept()));
}

QString UARTMessage::getMessage() const
{
    return m_editMessage->text();
}

QString UARTMessage::getSource() const
{
    if (m_btnLaptop->isChecked())
        return "Laptop";

    return "OPS";
}
