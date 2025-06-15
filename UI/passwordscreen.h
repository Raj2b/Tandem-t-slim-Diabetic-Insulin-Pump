#ifndef PASSWORDSCREEN_H
#define PASSWORDSCREEN_H

#include <QWidget>

namespace Ui {
class passwordscreen;
}

class passwordscreen : public QWidget
{
    Q_OBJECT

public:
    explicit passwordscreen(QWidget *parent = nullptr);
    ~passwordscreen();
    void resetPinInput();
    void showEvent(QShowEvent* event);



private:
    Ui::passwordscreen *ui;
    void checkPassword();
    QString enteredPin;
    void connectNumPad();     // add this
    void updateDisplay();

private slots:
    void handleDigitInput(const QString& digit);
    void checkEnteredPin();  // triggered by lockScreenCheck



signals:
    void successfulAuth();
};

#endif // PASSWORDSCREEN_H
