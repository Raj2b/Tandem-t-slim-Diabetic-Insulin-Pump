#include "Controller.h"

Controller::Controller(Person& user)
    : u(user),
      ds(),
      basalpump( std::make_shared<BasalPump>(ds) ),
      boluspump( std::make_shared<BolusPump>(ds) ),
      profileDb("Profiles/", ".json"),
      logger("Logs/", "Log.txt"),
      recorder("Sessions/", ".json"),
      battery(),
      cartridge(),
      injector(*basalpump, *boluspump)
{

    scantimer.start(100);

    connect(&scantimer, &QTimer::timeout, this, &Controller::scancycle);

    connect(&battery, &Battery::batteryLow, [this](){ logMessage("DEVICE NOTICE: battery low"); });
    connect(&battery, &Battery::batteryCritical, [this](){ logMessage("DEVICE NOTICE: battery critical"); });
    connect(&battery, &Battery::batteryEmpty, [this](){
        logMessage("DEVICE NOTICE: battery empty");
        powerOff();
    });

    connect(&cartridge, &Cartridge::cartridgeLow, [this](){ logMessage("DEVICE NOTICE: cartridge low"); });
    connect(&cartridge, &Cartridge::cartridgeEmpty, [this](){
        logMessage("DEVICE NOTICE: cartridge empty");
        basalpump->stop();
        boluspump->stop();
    });

    connect(&injector, &Injector::logMessage, this, &Controller::logMessage);
    connect(&injector, &Injector::consumeInsulin, this, &Controller::consumeInsulin);
    connect(&injector, &Injector::recordSession, this, &Controller::recordSession);
}

void Controller::scancycle() {
    emit systemDateTime(QDateTime::currentDateTime());

    float sugar = u.getSugar();
    emit userBG(sugar);
    basalpump->setSugar(sugar);
}

void Controller::consumeInsulin(float basal, float bolus) {
    //qDebug() << "CONSUMING INSULIN:" << basal << bolus;
    float remaining = cartridge.getRemaining();
    cartridge.consume(basal + bolus);

    if (basal + bolus <= remaining) {
        u.injectBolus(bolus);
        u.injectBasal(basal);

    } else {
        float availableBolus = std::min(bolus, remaining); //inject as much bolus as possible
        remaining -= availableBolus;
        float availableBasal = std::min(basal, remaining); //remaining injected as basal

        u.injectBolus(availableBolus);
        u.injectBasal(availableBasal);
    }
}

//-------------------------------------
void Controller::powerOn() {
    battery.on();
    emit stateIDLE();
}

void Controller::powerOff() {
    basalpump->abort();
    basalpump->close();

    boluspump->abort();
    boluspump->close();

    battery.off();

    emit stateOFF();
}

//--------------------------------------
void Controller::logMessage(QString msg) {
    qDebug() << "LOG MESSAGE:" << msg;
    logger.log(msg);
}

void Controller::recordSession(Session& session) {
    recorder.record(session);
}
