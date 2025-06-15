#ifndef BOLUSPUMP_H
#define BOLUSPUMP_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <stdexcept>

#include "Interfaces/Pump.h"
#include "DeviceSettings.h"

class BolusPump : public QObject, public Pump {
    Q_OBJECT
//    Q_INTERFACES(Pump)
public:
    struct Settings {
        enum class Mode {QUICK, EXTENDED};

        QTime dr;
        Mode m = Mode::QUICK;
        Profile p; //includes carb-insulin ratio
        float carbsConsumed = 0;
        float bolusNow = 0;
        float bolusLater = 0;
    };
    //---------------------------------------------
    BolusPump(DeviceSettings& deviceSettings);

    bool isActive() { return step == Step::ACTIVE; }
    bool isRunning() { return isActive() && scantimer.isActive(); }
    bool timelimit() { return qclock.elapsed() >= QTime(0,0,0,0).msecsTo(settings.dr); }

    QTime getQTimeStep() { return QTime::fromMSecsSinceStartOfDay(qclock.elapsed()/1000); };

signals:
    void consumeInsulin(float basal, float bolus);
    void logMessage(QString msg);
    void recordSession(Session& s);

public slots:
    void start();
    void stop();
    void resume();
    void abort();
    void close();

    void confirmSettings(Settings& setup) { if(!isActive()) { settings = setup; } };
    void iobReduction(float iob) { insulinReduction = iob; };
    void clearReduction() { insulinReduction = 0; };

private:
    void scancycle();
    float compute();

    enum class Step {SETUP, ACTIVE, SUMMARY} step = Step::SETUP;

    DeviceSettings& ds;
    Settings settings;

    float insulinReduction = 0;

    const int scanInterval = 1000;
    int nCycleCounter = 0;

    QTimer scantimer;
    QElapsedTimer qclock;
};

#endif // BOLUSPUMP_H
