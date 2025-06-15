#ifndef BASALPUMP_H
#define BASALPUMP_H

#include <QTimer>
#include <QElapsedTimer>

#include "Interfaces/Pump.h"
#include "Profile.h"
#include "FileLogger.h"
#include "Cartridge.h"
#include "Utils/RangeCompare.h"
#include "Utils/RNG.h"
#include "Utils/CustomQtUtils.h"
#include "Session.h"
#include "Enums/Thresholds.h"
#include "DeviceSettings.h"
#include "Prediction/ControlIQ.h"

class BasalPump : public QObject, public Pump {
    Q_OBJECT
//    Q_INTERFACES(Pump)
public:
    //-----------------------------------------
    BasalPump(DeviceSettings& deviceSettings);
    bool isActive() { return step == Step::ACTIVE || step == Step::CALIBRATE; }
    bool isRunning() { return isActive() && scantimer.isActive(); }
    bool timelimit() { return false; }

    float getSugar() {
        if (sugarData.empty()) {
            return 0;
        } else if ( (sugarData.rbegin()->first).secsTo(QDateTime::currentDateTime()) > 1 ) {
            return 0;
        } else {
            return sugarData.rbegin()->second;
        }
    }

    float getPredictedSugar(QTime frame) {
        QDateTime k = CustomQtUtils::currentMajorDateTime().addSecs(QTime(0,0,0).secsTo(frame));
        return predictedData[ k ];
    }

    float getTargetSugar() { return activeProfileTP().targetSugar; }
    float getISF() { return activeProfileTP().correctionFactor; }
    float getInsulin() { return insulin; }
    float getBolus() { return bolus; }
    float getBasalRate() { return activeProfileTP().basalRate; }

    std::map<QDateTime, float> getSugarData() { return sugarData; };
    std::map<QDateTime, float> getPredictedData() { return predictedData; }

    void setSugar(float value) {
        sugarData[CustomQtUtils::currentMajorDateTime()] = value;
    }

public slots:
    void start();
    void stop();
    void resume();
    void abort();
    void close();

signals:
    void consumeInsulin(float basal, float bolus);
    void logMessage(QString msg);
    void recordSession(Session& s);

    void cgmData(QDateTime dt, float realSugar, float predictedSugar);
    void warningHighBG();
    void warningLowBG();

private:
    void scancycle();
    Profile::Timepoint activeProfileTP() { return ds.getProfile().activeTimepoint( QTime::currentTime() ); }

    void initialCalibration();
    void managePredictedSugar();
    void manageCurrentSugar();

    void predictSugar();
    float correctiveBolus();

    enum class Step {SETUP, CALIBRATE, ACTIVE} step = Step::SETUP;

    DeviceSettings& ds;
    Session session;
//    ControlIQ ctrlIq;

    float insulin = 0;
    float bolus = 0;
    bool lowSugarAlert = false;
    bool highSugarAlert = false;
    float calibratedInsulin = 0;

    std::map<QDateTime, float> sugarData;
    std::map<QDateTime, float> predictedData;
    std::map<QDateTime, float> insulinData;

    //copy from thresholds
    const std::pair<float, float> hyper = Thresholds::hyper;
    const std::pair<float, float> increase = Thresholds::increase;
    const std::pair<float, float> maintain = Thresholds::maintain;
    const std::pair<float, float> decrease = Thresholds::decrease;
    const std::pair<float, float> hypo = Thresholds::hypo;

    const QTime adjustmentWindow = QTime(0,0,10);

    QTimer scantimer;
};

#endif // BASALPUMP_H
