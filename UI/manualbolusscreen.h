#ifndef MANUALBOLUSSCREEN_H
#define MANUALBOLUSSCREEN_H

#include <QWidget>
#include <QPushButton>
#include "Controller.h"
#include "Profile.h"

namespace Ui {
class manualbolusscreen;
}

class manualbolusscreen : public QWidget
{
    Q_OBJECT

public:
    explicit manualbolusscreen(BolusPump& boluspump, DeviceSettings& deviceSettings, BasalPump& basalpump, QWidget *parent = nullptr);
    ~manualbolusscreen();
    void reset();

private:
    Ui::manualbolusscreen *ui;
    void switchView(int desired);
    void getHealthValues();

    bool confirmation =false;
    float carbValue = 0;
    float bgValue = 0;
    float unitValue = 1.11;
    float numpadValue =0.00;
    int currentPage = 0;

    float target = 0;
    float correction =0;
    QPushButton* currentButton;

    void unitsEntered();
    void confirmDelivery();
    void confirmBolus();
    void bolusEntered();
    void resetView();
    void connectNumPad();
    void hoursEntered();
    void confirmRequest();
    void bolusDelivered();
    void setupBolusConfirm();

    BolusPump& bolus;
    DeviceSettings& ds;
    BasalPump& basal;

signals:
    void clickHome();
    void bolusInitiated(QString, int);
};

#endif // MANUALBOLUSSCREEN_H
