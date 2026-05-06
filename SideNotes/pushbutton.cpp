#include "pushbutton.h"

PushButton::PushButton(QWidget *parent)
    : QPushButton(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
    QWidget {
        background-color: rgba(0, 0, 0, 220);
        border-radius: 10px;
    }

    /* ===== BUTTON BASE ===== */
    QPushButton {
        color: white;
        background-color: rgba(25, 25, 25, 255);
        border: 1px solid rgb(255, 255, 255);
        border-radius: 6px;
        padding: 6px 14px;

        font-weight: 500;
    }

    /* ===== HOVER ===== */
    QPushButton:hover {
        background-color: rgba(60, 60, 60, 255);
        border-color: rgba(255, 255, 255, 180);
    }

    /* ===== PRESSED ===== */
    QPushButton:pressed {
        background-color: rgba(90, 90, 90, 255);
        border-color: rgba(255, 255, 255, 255);
        padding-top: 7px;
        padding-left: 15px;
    }

    /* ===== DISABLED (OPTIONAL) ===== */
    QPushButton:disabled {
        color: rgba(255, 255, 255, 80);
        border-color: rgba(100, 100, 100, 80);
        background-color: rgba(20, 20, 20, 200);
    }
    )");
}
