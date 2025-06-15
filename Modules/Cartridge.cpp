#include "Cartridge.h"

Cartridge::Cartridge() {
    connect(&scantimer, &QTimer::timeout, this, &Cartridge::scancycle);
    scantimer.start(1000);
}

void Cartridge::consume(float amount) {
    float prev = remaining;
    remaining = std::max(0.0f, remaining-amount);

    if (remaining == 0 & remaining > 0) {
        emit cartridgeEmpty();
    } else if (RangeCompare::inRange(remaining, 0.0f, 50.0f) && prev > 50) {
        emit cartridgeLow();
    }
}

void Cartridge::scancycle() {
    emit cartridgeLevel(remaining);
}
