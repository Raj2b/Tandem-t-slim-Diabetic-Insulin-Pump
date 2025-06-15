#include "offscreen.h"
#include "ui_offscreen.h"
#include <QDebug>

offscreen::offscreen(Battery& battery, QWidget *parent) :
    QWidget(parent),
    b(battery),
    ui(new Ui::offscreen), pressTimer(new QTimer(this))
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::pressed, this, &offscreen::onButtonPressed);
    connect(ui->pushButton, &QPushButton::released, this, &offscreen::onButtonReleased);
    connect(ui->pushButton_2, &QPushButton::released, this, &offscreen::onLongPressDetected); //Temporary Bypass
    connect(pressTimer, &QTimer::timeout, this, &offscreen::onLongPressDetected);
    connect(ui->connectChargerButton, &QPushButton::released, this, &offscreen::connectCharger); //Connects Charger
    ui->label_6->hide();
    ui->label_6->setText("Device Empty.... Plug in device and try again...");
}

offscreen::~offscreen()
{
    delete ui;
}

void offscreen::powerOn(){
    if(b.getLevel()>0 || b.getChargingStatus()){emit turnOnDevice(); ui->label_6->hide();}
    else{qDebug() << "Device Empty.... Plug in device and try again...";ui->label_6->show();}

}

//When the button is pressed, start the timer
void offscreen::onButtonPressed()
{
    pressTimer->start(5000); //5000 milliseconds = 5 seconds
}

//If the button is released before 5 seconds, stop the timer
void offscreen::onButtonReleased()
{
    if (pressTimer->isActive()) {
        pressTimer->stop();
        if(pressTimer->remainingTime()>0){qDebug() << "Button released before 5 seconds."; return;}
    }
}

//Function triggered when the button is held for 5 seconds
void offscreen::onLongPressDetected()
{
     qDebug() << "Button held for 5 seconds! Powering on";
     powerOn();
}

//connects the charger
void offscreen::connectCharger(){
    if(b.getChargingStatus()==false){
        b.setCharging(true);
        qDebug("Charger Connected");
        ui->label_6->hide();
    }else{
        b.setCharging(false);
        qDebug("Charger Disconnected");
    }
}
