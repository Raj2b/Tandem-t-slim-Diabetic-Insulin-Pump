#ifndef OFFSCREEN_H
#define OFFSCREEN_H

#include <QWidget>
#include <QTimer>

#include "Modules/Battery.h"

namespace Ui {
class offscreen;
}

class offscreen : public QWidget
{
    Q_OBJECT

public:
    explicit offscreen(Battery& battery, QWidget *parent = nullptr);
    ~offscreen();
signals:
    void turnOnDevice();
private:
    void powerOn();

    void onButtonPressed();   // Called when button is pressed
    void onButtonReleased();  // Called when button is released
    void onLongPressDetected(); // Called if button is held for 5 seconds
    void connectCharger();

    Battery& b;

    Ui::offscreen *ui;
    QTimer *pressTimer;  // Timer to track button press duration
};

#endif // OFFSCREEN_H
