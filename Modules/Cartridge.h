#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <QObject>
#include <QTimer>

#include "Utils/RNG.h"

class Cartridge : public QObject {
    Q_OBJECT
public:
    Cartridge();

    float getRemaining() { return remaining; }
    void replace(float amount, float maxCapacity) {
        remaining = amount;
        maxCap = maxCapacity;
    }
    void consume(float amount);
    float getMaxCapacity() { return maxCap; };

signals:
    void cartridgeLevel(float value);
    void cartridgeLow();
    void cartridgeEmpty();

private:
    void scancycle();

    float remaining = RNG::normalDistribution(0.0, 300.0, 250.0, 10.0);

    float maxCap = 300;
    QTimer scantimer;
};

#endif // CARTRIDGE_H
