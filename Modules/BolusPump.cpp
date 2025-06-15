#include "BolusPump.h"

BolusPump::BolusPump(DeviceSettings& deviceSettings)
    : ds(deviceSettings)
{
    QObject::connect(&scantimer, &QTimer::timeout, this, &BolusPump::scancycle);
    scantimer.setInterval(scanInterval);
}

void BolusPump::scancycle() {
    if(timelimit()) {
        scantimer.stop();
        emit logMessage("MANUAL BOLUS: complete");
        close();
        return;
    }

    try {
        float bolus = std::min(0.0f, compute() - insulinReduction);
        emit consumeInsulin( 0, bolus );
    }  catch (std::exception& e) {
        qDebug() << e.what();
    }

    nCycleCounter++;
}

float BolusPump::compute() {
    if( qclock.elapsed()/1000 <= 1 ) {
        return settings.bolusNow;

    } else {
        return settings.bolusLater;

    }
//    float cir = settings.getProfile().activeTimepoint(getElapsedTime()).carbInsulinRatio;
//    float carbs = settings.getCarbsConsumed();
//    bool extended = (settings.getMode() == Settings::Mode::EXTENDED);

//    if (cir <= 0) {
//        throw std::runtime_error("invalid carbs:insulin ratio");
//    }

//    float foodBolus = carbs / cir;
//    return foodBolus * (extended ? 0.4 : 0.6);
}

//----------------------------------
void BolusPump::start() {
    scantimer.start();
    emit logMessage("MANUAL BOLUS: started");

//    float carbs = settings.getCarbsConsumed();
//    float cir = settings.getProfile().activeTimepoint(getElapsedTime()).carbInsulinRatio;

//    if (carbs < 0) {
//        throw std::runtime_error("invalid carbs");
//    } else if (cir <= 0) {
//        throw std::runtime_error("invalid carbs:insulin ratio");
//    }
}

void BolusPump::stop() {
    if (isActive() && isRunning()) {
        scantimer.stop();
        emit logMessage("MANUAL BOLUS: stopped");
    }
}

void BolusPump::resume() {
    if (isActive() && !isRunning()) {
        scantimer.start();
        emit logMessage("MANUAL BOLUS: resumed");
    }
}

void BolusPump::abort() {
    emit logMessage("MANUAL BOLUS: aborted");
    stop();
    close();
}

void BolusPump::close() {
    settings = Settings();

    insulinReduction = 0;

    nCycleCounter = 0;
}
