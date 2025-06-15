#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UI/homescreen.h"
#include "UI/offscreen.h"
#include "UI/datastoragescreen.h"
#include "UI/manualbolusscreen.h"
#include "UI/passwordscreen.h"
#include "UI/personalprofilescreen.h"
#include "UI/pumpsettingsscreen.h"
#include "Controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Controller& controller, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Controller& c;

    QWidget* currentScreen;
//    Person* person;
    Ui::MainWindow *ui;
    homescreen *homeScreen;
    offscreen *offScreen;
    datastoragescreen *dataStorageScreen;
    manualbolusscreen *manualBolusScreen;
    passwordscreen *passwordScreen;
    personalprofilescreen *personalProfileScreen;
    pumpsettingsscreen *pumpSettingsScreen;
    std::vector<QWidget*> screenList;

    std::vector<int> ack;

    void switchHomeScreen();
    void switchOffScreen();
    void switchDataStorageScreen();
    void switchProfilesScreen();
    void switchManualBolusScreen();
    void switchPumpSettingsScreen();
    void switchPasswordScreen();
    void receiveAlert(int emergency);

public slots:
    void powerOnDevice();

signals:
    void resetProfileState();
};
#endif // MAINWINDOW_H
