#include "BasalPump.h"

BasalPump::BasalPump(DeviceSettings& deviceSettings)
    : ds(deviceSettings)
{
    scantimer.setInterval(1000);

    connect(&scantimer, &QTimer::timeout, this, &BasalPump::scancycle);
    connect(&ds, &DeviceSettings::profileChanged, this, &BasalPump::start);
}

void BasalPump::scancycle() {

    if (step == Step::CALIBRATE) {
        initialCalibration();
    } else if (step == Step::ACTIVE) {
        predictSugar();
        managePredictedSugar();
        manageCurrentSugar();
    }

    insulin = std::max(0.0f, insulin);
    bolus = std::max(0.0f, bolus);

    emit consumeInsulin(insulin, bolus);
    emit cgmData( CustomQtUtils::currentMajorDateTime(), getSugar(), getPredictedSugar(QTime(0,0,30)) );

    session.datapoints[CustomQtUtils::currentMajorDateTime()] = {
            .sugar = getSugar(),
            .insulin = insulin,
            .bolus = bolus };
}

void BasalPump::initialCalibration() {
    float adjustment = (getSugar() - getTargetSugar()) / getISF();

    //calculate insulin according to profile
    if (Thresholds::getThreshold(getSugar()) == Thresholds::Range::MAINTAIN) {
        step = Step::ACTIVE;
        calibratedInsulin = insulin;
        insulin = activeProfileTP().basalRate;
        qDebug() << "CALIBRATION DONE";
        sugarData.clear();

    } else {
        qDebug() << "CALIBRATING" << getSugar() << insulin;
        insulin = adjustment;
    }
}

void BasalPump::managePredictedSugar() {
    float insulinAdjustment = (getPredictedSugar(adjustmentWindow) - getTargetSugar()) / getISF();

    switch( Thresholds::getThreshold(getPredictedSugar(adjustmentWindow)) ) {
    case Thresholds::Range::HYPER:
        bolus = (bolus == 0) ? correctiveBolus() : bolus; //dont recalculate bolus if already set
        break;

    case Thresholds::Range::INCREASE:
        insulin = insulinAdjustment + getBasalRate();
        break;

    case Thresholds::Range::MAINTAIN:
        insulin = getBasalRate();
        bolus = 0;
        break;

    case Thresholds::Range::DECREASE:
        insulin = lowSugarAlert ? 0 : insulin + insulinAdjustment;
        bolus = 0;
        break;

    case Thresholds::Range::HYPO:
        insulin = 0;
        bolus = 0;
        break;

    default:
        break;
    }

    insulinData[CustomQtUtils::currentMajorDateTime()] = insulin;
}

void BasalPump::manageCurrentSugar() {
    //only log warnings once - do not log again until BG returns to MAINTAIN

    Thresholds::Range currentThreshold = Thresholds::getThreshold(getSugar());
    switch(currentThreshold) {
    case Thresholds::Range::HYPER:
        bolus = (bolus == 0) ? correctiveBolus() : bolus; //dont recalculate bolus if already set
        if (!highSugarAlert) {
            emit logMessage("BASAL PUMP: high blood sugar");
            emit warningHighBG();
        }
        break;

    case Thresholds::Range::INCREASE:
        break;

    case Thresholds::Range::MAINTAIN:
        break;

    case Thresholds::Range::DECREASE:
        break;

    case Thresholds::Range::HYPO:
        insulin = 0;
        bolus = 0;
        if(!lowSugarAlert) {
            emit logMessage("BASAL PUMP: low blood sugar");
            emit warningLowBG();
        }
        break;

    default:
        break;
    }

    highSugarAlert =
            (highSugarAlert || currentThreshold == Thresholds::Range::HYPER)
            && !RangeCompare::inRange(getSugar(), maintain);

    lowSugarAlert =
            (lowSugarAlert || currentThreshold == Thresholds::Range::HYPO)
            && !RangeCompare::inRange(getSugar(), maintain);
}
//--------------------------------------

float BasalPump::correctiveBolus() {
    float pretotal = (getSugar() - getTargetSugar()) / getISF();
    return pretotal - insulin;
}

void BasalPump::predictSugar() {
    ControlIQ ctrlIq;
    QDateTime k = CustomQtUtils::currentMajorDateTime().addSecs( QTime(0,0,0).secsTo(adjustmentWindow) );

    float v = ctrlIq.predict(sugarData, adjustmentWindow);
    predictedData[k] = v;
}

//-------CALL FROM THE UI--------------------------------------
void BasalPump::start() {
    if (isActive()) {
        emit recordSession(session);
        session = Session();
    }

    if (getSugar() <= 0) {
        emit logMessage("BASAL PUMP: could not detect blood sugar");
        abort();
        return;
    }

    step = Step::CALIBRATE;

    scantimer.start();
    emit logMessage("BASAL PUMP: started");
}

void BasalPump::stop() {
    if(isActive() && isRunning()) {
        scantimer.stop();
        emit logMessage("BASAL PUMP: stopped");
        emit recordSession(session);
        session = Session();
    }
}

void BasalPump::resume() {
    if (isActive() && !isRunning()) {
        scantimer.start();
        emit logMessage("BASAL PUMP: resumed");
    }
}

void BasalPump::abort() {
    emit logMessage("BASAL PUMP: aborted");
    stop();
    close();
}

void BasalPump::close() {
    session = Session();

    step = Step::SETUP;

    insulin = 0;
    bolus = 0;
    lowSugarAlert = false;
    highSugarAlert = false;

    sugarData.clear();
    insulinData.clear();

    scantimer.stop();
}
