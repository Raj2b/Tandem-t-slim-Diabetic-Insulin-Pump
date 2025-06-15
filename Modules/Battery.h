#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>
#include <QTimer>
#include "Utils/RNG.h"
#include "Utils/RangeCompare.h"

class Battery : public QObject {
    Q_OBJECT
public:
    Battery();
    void on() { drain = true; }
    void off() { drain = false; }
    bool getChargingStatus() { return charging; };
    void setCharging(bool value) { charging = value; };
    void plug() { charging = true; }
    void unplug() { charging = false; }
    float getLevel(){return remaining;}

signals:
    void batteryLevel(float value);
    void batteryLow();
    void batteryEmpty();
    void batteryCritical();

private:
    void scancycle();

    QTimer scantimer;
    float remaining = RNG::normalDistribution(0.0, 100.0, 60.0, 10.0);
    bool drain = false;
    bool charging = false;
};

#endif // BATTERY_H
