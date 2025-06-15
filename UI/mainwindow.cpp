#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <algorithm>

MainWindow::MainWindow(Controller& controller, QWidget *parent)
    : QMainWindow(parent),
      c(controller),
      currentScreen(),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(810, 460);  //Lock the window so no resizing

    //Create each screen
    homeScreen = new homescreen(c.getBattery(), c.getCartridge());
    connect(&c.getBasalPump(), &BasalPump::cgmData, homeScreen, &homescreen::onNewCgmValues);

    offScreen = new offscreen(c.getBattery());
    dataStorageScreen = new datastoragescreen(c.getSessionRecorder(), c);
    manualBolusScreen = new manualbolusscreen(c.getBolusPump(), c.getSettings(), c.getBasalPump());
    passwordScreen = new passwordscreen();
    personalProfileScreen = new personalprofilescreen(c.getProfileDB(), c.getSettings());
    pumpSettingsScreen = new pumpsettingsscreen(c.getBasalPump()/*, c.getSettings()*/);

    //vector of each screen
    screenList.push_back(homeScreen);
    screenList.push_back(offScreen);
    screenList.push_back(dataStorageScreen);
    screenList.push_back(manualBolusScreen);
    screenList.push_back(passwordScreen);
    screenList.push_back(personalProfileScreen);
    screenList.push_back(pumpSettingsScreen);

    for (QWidget* screen : screenList) {
        if (screen) {
            ui->stackedWidget->addWidget(screen);
        }
    }
    ui->stackedWidget->setStyleSheet(offScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(offScreen);
    //connect button for power
    connect(ui->powerButton, &QPushButton::clicked, this, &MainWindow::switchOffScreen);
    //Connect UI button signals to these funcs
    connect(offScreen, &offscreen::turnOnDevice, this, &MainWindow::powerOnDevice);
    connect(passwordScreen, &passwordscreen::successfulAuth, this, &MainWindow::switchHomeScreen);
    connect(homeScreen, &homescreen::clickBolus, this, &MainWindow::switchManualBolusScreen);
    connect(homeScreen, &homescreen::clickOptions, this, &MainWindow::switchPumpSettingsScreen);
    connect(manualBolusScreen, &manualbolusscreen::clickHome, this, &MainWindow::switchHomeScreen);
    //connect(personalProfileScreen, &personalprofilescreen::clickHome, this, &MainWindow::switchHomeScreen);
    connect(personalProfileScreen, &personalprofilescreen::goToPumpSettings, this, &MainWindow::switchPumpSettingsScreen);
    connect(this, &MainWindow::resetProfileState, personalProfileScreen, &personalprofilescreen::resetState);
    //Buttons for pump settings
    connect(pumpSettingsScreen->getMyPumpButton(), &QPushButton::clicked, this, &MainWindow::switchProfilesScreen);
    connect(pumpSettingsScreen, &pumpsettingsscreen::clickHome, this, &MainWindow::switchHomeScreen);
    //This is for the Home (T) button
    connect(ui->homeButton, &QPushButton::released, this, &MainWindow::switchHomeScreen);
    //Hide it initally
    ui->homeButton->hide();
    ui->errorLabel->setWordWrap(true);
    ui->errorLabel->setAlignment(Qt::AlignCenter);
    ui->powerButton->hide();

    //error message back screen
    connect(ui->backButton, &QPushButton::released, this, [this]() {ui->stackedWidget->setCurrentIndex(0); ui->stackedWidget->setCurrentWidget(currentScreen);});

    connect(&controller, &Controller::systemDateTime, homeScreen, &homescreen::receiveDateTime);

    connect(manualBolusScreen, &manualbolusscreen::bolusInitiated, homeScreen, &homescreen::receiveBolus);

    connect(pumpSettingsScreen, &pumpsettingsscreen::switchToLogs, this, &MainWindow::switchDataStorageScreen);

    //sys alerts
    connect(&c.getBattery(), &Battery::batteryLow, this, [this]() {receiveAlert(1);}); //battery low
    connect(&c.getBattery(), &Battery::batteryCritical, this, [this]() {receiveAlert(0);}); //battery Critical
    connect(&c.getBattery(), &Battery::batteryEmpty, this, [this]() {switchOffScreen();}); //battery empty
    connect(&c.getCartridge(), &Cartridge::cartridgeLow, this, [this]() {receiveAlert(2);}); //cartridge low
    connect(&c.getBasalPump(), &BasalPump::warningHighBG, this, [this]() {receiveAlert(3);}); // high BG


    connect(homeScreen, &homescreen::manualCancelBolus, &c.getBolusPump(), &BolusPump::abort);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switchHomeScreen(){
    emit resetProfileState();
    ui->stackedWidget->setStyleSheet(homeScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(homeScreen);
    ui->stackedWidget->setFixedSize(650,400);
    ui->homeButton->show();
}

void MainWindow::switchOffScreen() {
    c.powerOff();
    if (ui->stackedWidget->currentWidget() == offScreen) {
        powerOnDevice();
    } else {
        ui->stackedWidget->setStyleSheet(offScreen->styleSheet());
        this->setStyleSheet(offScreen->styleSheet());
        ui->stackedWidget->setCurrentWidget(offScreen);
        ui->homeButton->hide();
        ui->powerButton->hide();
    }
}

void MainWindow::switchDataStorageScreen(){
    ui->stackedWidget->setStyleSheet(dataStorageScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(dataStorageScreen);
    //ui->homeButton->hide();
};

void MainWindow::switchProfilesScreen(){
    ui->stackedWidget->setStyleSheet(personalProfileScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(personalProfileScreen);
};

void MainWindow::switchManualBolusScreen(){
    manualBolusScreen->reset();
    ui->stackedWidget->setStyleSheet(manualBolusScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(manualBolusScreen);
};

void MainWindow::switchPumpSettingsScreen(){
    ui->stackedWidget->setStyleSheet(pumpSettingsScreen->styleSheet());
    pumpSettingsScreen->showMainMenuPage();
    ui->stackedWidget->setCurrentWidget(pumpSettingsScreen);
};

void MainWindow::powerOnDevice() {
    std::cout << "[DEBUG] Device Powering On" << std::endl;
    ui->homeButton->show(); QFile file("pin.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString savedPin = in.readLine().trimmed();
        file.close();
        qDebug() << "[PIN File Content]:" << savedPin;
        if (!savedPin.isEmpty() && savedPin.length() >= 4 && savedPin.length() <= 6) {
            switchPasswordScreen();
            return;
        }
    }
    c.powerOn();
    ui->powerButton->show();
    this->setStyleSheet(homeScreen->styleSheet());
    switchHomeScreen();
}

void MainWindow::switchPasswordScreen(){
    ui->stackedWidget->setStyleSheet(passwordScreen->styleSheet());
    ui->stackedWidget->setCurrentWidget(passwordScreen);
};

//receives alert emissions from the central controller. Based on the code it displays an alert
void MainWindow::receiveAlert(int emergency){
    if(emergency == 0){
        if (std::find(ack.begin(), ack.end(), emergency) != ack.end()) {
            return;
        } else {
            qDebug() << "[DEBUG] 0 Alert Received";
            ui->errorLabel->setText("Battery Level is CRITICAL, Plug in device.              *Press Off Button and Charge Device*                      Note: This warning will not pop up again");
            ack.push_back(emergency);
        }
    }
    if(emergency == 1){
        if (std::find(ack.begin(), ack.end(), emergency) != ack.end()) {
            return;
        } else {
            qDebug() << "[DEBUG] 1 Alert Received";
            ui->errorLabel->setText("Battery Level is Low, Plug in device.                     *Press Off Button and Charge Device*                     Note: This warning will not pop up again");
            ack.push_back(emergency);
        }
    }
    if(emergency == 2){
        if (std::find(ack.begin(), ack.end(), emergency) != ack.end()) {
            return;
        } else {
            qDebug() << "[DEBUG] 2 Alert Received";
            ui->errorLabel->setText("Cartridge Level is Low.                                      Note: This warning will not pop up again");
            ack.push_back(emergency);
        }
    }
    if(emergency == 3){
        if (std::find(ack.begin(), ack.end(), emergency) != ack.end()) {
            return;
        } else {
            qDebug() << "[DEBUG] 3 Alert Received";
            ui->errorLabel->setText("Current BG has reached a HIGH Level.                              Note: This warning will not pop up again");
            ack.push_back(emergency);
        }
    }

    this->currentScreen = ui->stackedWidget->currentWidget();
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget->setStyleSheet(homeScreen->styleSheet());

}

