#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDebug>
#include <QObject>
#include <QMainWindow>
#include <stack>
#include <memory>

#include "Modules/BasalPump.h"
#include "Modules/BolusPump.h"
#include "Modules/Injector.h"
#include "Modules/ProfileDB.h"
#include "Modules/SessionRecorder.h"
#include "Modules/Battery.h"
#include "Modules/Cartridge.h"
#include "Profile.h"
#include "DeviceSettings.h"

class Controller : public QObject {
    Q_OBJECT

public:
    Controller(Person& user);

    void powerOn();
    void powerOff();

    BasalPump& getBasalPump() { return *basalpump; };
    BolusPump& getBolusPump() { return *boluspump; };

    ProfileDB& getProfileDB() { return profileDb; };
    FileLogger& getLogger() { return logger; };
    SessionRecorder& getRecorder() { return recorder; };
    Battery& getBattery() { return battery; };
    Cartridge& getCartridge() { return cartridge; };

    DeviceSettings& getSettings() { return ds; }
    SessionRecorder& getSessionRecorder() { return recorder; }

signals:
    void stateOFF();
    void stateIDLE();

    void systemDateTime(QDateTime time);
    void userBG(float value);

public slots:
    void logMessage(QString msg);
    void recordSession(Session& session);

private:
    void scancycle();

    void consumeInsulin(float basal, float bolus);

    Person& u;

    DeviceSettings ds;

    //modules
    std::shared_ptr<BasalPump> basalpump;
    std::shared_ptr<BolusPump> boluspump;

    ProfileDB profileDb;
    FileLogger logger;
    SessionRecorder recorder;
    Battery battery;
    Cartridge cartridge;

    Injector injector;
    QTimer scantimer;
};

#endif // CONTROLLER_H
