#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QTime>
#include <QElapsedTimer>
#include "Modules/Battery.h"
#include "Modules/Cartridge.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include "Modules/CgmDataManager.h"
#include <QtCharts/QAreaSeries>
#include <QtCharts/QScatterSeries>
#include <QLabel>
#include "./Person.h"


using namespace QtCharts;


namespace Ui {
class homescreen;
}

class homescreen : public QWidget
{
    Q_OBJECT

public:
    explicit homescreen(Battery& battery, Cartridge& cartridge, QWidget *parent = nullptr);
    ~homescreen();
    void setCgmManager(CgmDataManager* manager);
    Person* person = nullptr;
    void loadPersonData(const QMap<QDateTime, float>& realMap, const QMap<QDateTime, float>& predictedMap);

public slots:
    void receiveBatteryLevel(float level);
    void receiveCartridgeLevel(float level);
    void receiveDateTime(QDateTime);
    void receiveBolus(QString Amount, int Mode);
    void onNewCgmValues(QDateTime time, float real, float predicted);



signals:
    void clickBolus();
    void clickOptions();
    void manualCancelBolus();

private slots:
    void onNewCgmPoint(const GlucosePoint& point);
    void cycleTimeWindow();


private:
    void setupCgmGraph();

    Ui::homescreen *ui;
    QDateTime systemDateTime;
    bool bolusActivated = false;
    QElapsedTimer bolusTimer;

    Battery& b;
    Cartridge& c;

    void bolusButton();
    void optionsButton();
    void cancelBolus();
    QScatterSeries* series;

    QStringList timeWindows = {"1 HRS", "3 HRS", "6 HRS", "24 HRS"};
    int currentTimeWindowIndex = 1;
    QDateTimeAxis* axisX = nullptr;
    QValueAxis* axisY = nullptr;
    QAreaSeries* targetRange = nullptr;
    QLineSeries* highLimitLine;
    QLineSeries* lowLimitLine;
    void updateStatusIcon(float glucoseValue);
    QLabel* statusIconLabel;
    QLabel* bolusIndicator;
    QLineSeries* predictedSeries = nullptr;







};

#endif // HOMESCREEN_H
