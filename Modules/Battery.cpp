#include "Battery.h"

Battery::Battery()
{
    scantimer.start(1000);
    connect(&scantimer, &QTimer::timeout, this, &Battery::scancycle);
    emit batteryLevel(remaining);
}

void Battery::scancycle() {
    float delta = 0;
    if (charging) {
        delta += 0.3;
    }
    if (drain) {
        delta -= 0.2;
    }

    float prev = remaining;
    remaining = RangeCompare::clamp(remaining + delta, 0.0f, 100.0f);

    if (remaining == 0 && prev > 0) {
            emit batteryEmpty();
        }
        else if (remaining <= 10.0f && prev > 10.0f) {
            emit batteryCritical();
        }
        else if (RangeCompare::inRange(remaining, 10.0f, 20.0f) && prev > 20.0f) {
            emit batteryLow();
        }

        emit batteryLevel(remaining);
    }
