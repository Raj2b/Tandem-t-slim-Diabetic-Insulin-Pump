#include "homescreen.h"
#include "ui_homescreen.h"
#include <iostream>
#include <QDebug>
#include <QDate>
#include <QTimer>
#include <QTime>


homescreen::homescreen(Battery& battery, Cartridge& cartridge, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homescreen),
    b(battery),
    c(cartridge),
    series(new QScatterSeries)

{
    ui->setupUi(this);


    predictedSeries = new QLineSeries();
    predictedSeries->setColor(Qt::cyan);
    QPen dashedPen(Qt::cyan);
    dashedPen.setStyle(Qt::DashLine);
    dashedPen.setWidth(2);
    predictedSeries->setPen(dashedPen);



    statusIconLabel = findChild<QLabel*>("statusIconLabel");
    statusIconLabel->setPixmap(QPixmap(":/icons/icons/maintains.png").scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    bolusIndicator = findChild<QLabel*>("bolusIndicator");
    bolusIndicator->hide(); // hidden until bolus starts



    QChart* chart = new QChart();

    chart->legend()->hide();
    chart->setTitle("");
    chart->setBackgroundBrush(QBrush(Qt::black));

    axisX = new QDateTimeAxis;
    axisY = new QValueAxis;

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignRight);

    axisX->setFormat("hh:mm");
    axisX->setLabelsVisible(false);
    axisX->setTickCount(0);
    axisX->setGridLineVisible(false);
    axisX->setRange(QDateTime::currentDateTime().addSecs(-3 * 3600), QDateTime::currentDateTime());

    axisY->setRange(2.0, 22.0);
    axisY->setTickCount(6);
    axisY->setLabelFormat("%.0f");
    axisY->setLabelsColor(Qt::gray);
    axisY->setLinePenColor(Qt::gray);
    axisY->setGridLineVisible(false);
    axisY->setTickAnchor(2.0);
    axisY->setTickInterval(4.0);


    QLineSeries* upper = new QLineSeries();
    QLineSeries* lower = new QLineSeries();

    qint64 startMs = QDateTime::currentDateTime().addSecs(-3 * 3600).toMSecsSinceEpoch();
    qint64 endMs   = QDateTime::currentDateTime().toMSecsSinceEpoch();



    lower->append(startMs, 3.9);
    lower->append(endMs, 3.9);
    upper->append(startMs, 10.0);
    upper->append(endMs, 10.0);

    targetRange = new QAreaSeries(upper, lower);
    targetRange->setColor(QColor(255, 255, 255, 80));
    targetRange->setPen(Qt::NoPen);
    targetRange->setUseOpenGL(false);

    chart->addSeries(targetRange);
    targetRange->attachAxis(axisX);
    targetRange->attachAxis(axisY);



    highLimitLine = new QLineSeries();
    lowLimitLine = new QLineSeries();
    highLimitLine->append(startMs, 10.0);
    highLimitLine->append(endMs, 10.0);
    highLimitLine->setColor(Qt::yellow);
    highLimitLine->setPen(QPen(Qt::yellow, 2, Qt::SolidLine));

    lowLimitLine->append(startMs, 3.9);
    lowLimitLine->append(endMs, 3.9);
    lowLimitLine->setColor(Qt::red);
    lowLimitLine->setPen(QPen(Qt::red, 2, Qt::SolidLine));

    // Add to chart
    chart->addSeries(highLimitLine);
    chart->addSeries(lowLimitLine);
    highLimitLine->attachAxis(axisX);
    highLimitLine->attachAxis(axisY);
    lowLimitLine->attachAxis(axisX);
    lowLimitLine->attachAxis(axisY);




    QPen dataPen(Qt::white);
    dataPen.setWidth(3);
    series->setPen(dataPen);
    series->setPointsVisible(true);
    series->setUseOpenGL(true);
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setMarkerSize(3.0);
    series->setColor(Qt::white);
    series->setBorderColor(Qt::white);

    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);


    chart->setAnimationOptions(QChart::NoAnimation);

QChartView* chartView = findChild<QChartView*>("glucoseChartView");
if (!chartView) {
    qCritical() << "[CRASH DEBUG] glucoseChartView not found in UI!";
    return;
}
chartView->setChart(chart);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(QBrush(Qt::black));
    chartView->repaint();

    findChild<QChartView*>("glucoseChartView")->setChart(chart);
    findChild<QChartView*>("glucoseChartView")->setRenderHint(QPainter::Antialiasing);
    findChild<QChartView*>("glucoseChartView")->setBackgroundBrush(QBrush(Qt::black));


    connect(ui->bolusButton, &QPushButton::released, this, &homescreen::bolusButton);
    connect(ui->optionsButton, &QPushButton::released, this, &homescreen::optionsButton);
    connect(ui->bolusActivatedStopButton, &QPushButton::released, this, &homescreen::cancelBolus);
    connect(ui->timeWindowButton, &QPushButton::clicked, this, &homescreen::cycleTimeWindow);


    connect(&b, &Battery::batteryLevel, this, &homescreen::receiveBatteryLevel);
    connect(&c, &Cartridge::cartridgeLevel, this, &homescreen::receiveCartridgeLevel);


    ui->chargingIcon->hide();
    ui->bolusIcon->setStyleSheet("");
    ui->bolusIcon->setIcon(QIcon(":/icons/icons/droplet.png"));
    ui->bolusIcon->setIconSize(QSize(55, 55));
    ui->bolusIcon->setFixedSize(61, 61);
    ui->bolusIcon->setFlat(true);
    ui->bolusIcon->setText("");

    ui->optionsIcon->setStyleSheet("");
    ui->optionsIcon->setIcon(QIcon(":/icons/icons/gear.png"));
    ui->optionsIcon->setIconSize(QSize(57, 57));
    ui->optionsIcon->setFixedSize(61, 61);
    ui->optionsIcon->setFlat(true);
    ui->optionsIcon->setText("");

    ui->bolusActivatedDotsLabel->hide();
    ui->bolusActivatedLabel->hide();
    ui->bolusActivatedRequestingLabel->hide();
    ui->bolusActivatedStopButton->hide();

    bolusTimer.start();
    chart->setAnimationOptions(QChart::NoAnimation);
    targetRange->setUseOpenGL(false);

    findChild<QChartView*>("glucoseChartView")->repaint();
}


homescreen::~homescreen()
{
    delete ui;
}

//emits and tells the UI controller to switch the view
void homescreen::bolusButton(){
    emit clickBolus();
}

//emits and tells the UI controller to switch the view
void homescreen::optionsButton(){
    emit clickOptions();
}

//connection from the controller to here giving us the battery leve
void homescreen::receiveBatteryLevel(float level){
    //std::cout << "[DEBUG] Battery Level Recieved " <<level<< std::endl;
    int percent = static_cast<int>(level);
    ui->batteryBar->setValue(percent);
    QString fillColor;

    //sets bar colour to red if battery level is less than 20
    if (percent < 20) {
        fillColor = "#d32f2f";
    } else {
        fillColor = "#00c853";
    }

    if(b.getChargingStatus() == true){
        ui->chargingIcon->show();
    }else{
        ui->chargingIcon->hide();
    }

    QString style = QString("QProgressBar { border: 1px solid gray; text-align: center; color: black; }"" QProgressBar::chunk { background-color: %1; }").arg(fillColor);
    ui->batteryBar->setStyleSheet(style);
}

//connection from the controller to here giving us the cartridge level for insulin
void homescreen::receiveCartridgeLevel(float level){
    //std::cout << "[DEBUG] Cartridge Level Recieved " <<level<< std::endl;

    int percent = level /*/ c.getMaxCapacity()*/;

    ui->insulinBar->setRange(0, c.getMaxCapacity());
    ui->insulinBar->setFormat(QString::number((int)level));
    ui->insulinBar->setValue(level);

    QString fillColor;

    //sets bar colour to red if battery level is less than 20

    if (percent < 20) {
        fillColor = "#d32f2f";
    } else {
        fillColor = "#00c853";
    }
    QString style = QString("QProgressBar { border: 1px solid gray; text-align: center; color: black; }"" QProgressBar::chunk { background-color: %1; }").arg(fillColor);
    ui->insulinBar->setStyleSheet(style);

}

//receives the system time from the backend controller and displays it on the UI
void homescreen::receiveDateTime(QDateTime sysTime){
    ui->timeLabel->setText( sysTime.time().toString("hh:mm") );
    ui->dateLabel->setText( sysTime.date().toString("yyyy MMMM dd") );

    //creates the animation for the moving dots on request
    if(bolusActivated && bolusTimer.elapsed() >= 800){
        QString currentDots = ui->bolusActivatedDotsLabel->text();
        if(currentDots == "- QUICK"){
        }
        if(currentDots == "•◦◦"){
            ui->bolusActivatedDotsLabel->setText("◦•◦");
        }
        if(currentDots == "◦◦•"){
            ui->bolusActivatedDotsLabel->setText("•◦◦");
        }
        if(currentDots == "◦•◦"){
            ui->bolusActivatedDotsLabel->setText("◦◦•");
        }
        bolusTimer.restart();
    }
}

//initalizes the state after receiving a bolus
void homescreen::receiveBolus(QString Amount, int Mode){
    std::cout << "[DEBUG] Bolus Started in Homescreen"<<std::endl;

    ui->bolusActivatedDotsLabel->show();
    ui->bolusActivatedLabel->show();
    ui->bolusActivatedRequestingLabel->show();
    ui->bolusActivatedStopButton->show();
    ui->bolusButton->setText("");
    if(Mode == 1){
        ui->bolusActivatedRequestingLabel->setText("Requesting " + Amount + " u Bolus..");
        ui->bolusActivatedDotsLabel->setText("◦◦•");
    }else{
        ui->bolusActivatedRequestingLabel->setText("Delivering " + Amount + " u Bolus..");
        ui->bolusActivatedDotsLabel->setText("- QUICK");
    }
    bolusActivated = true;
    ui->bolusButton->setEnabled(false);
    if (bolusIndicator) {
        bolusIndicator->setPixmap(QPixmap(":/icons/icons/droplet.png")
                                      .scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        bolusIndicator->show();
    }


}

void homescreen::cancelBolus(){
    bolusActivated = false;
    ui->bolusActivatedRequestingLabel->setText("Cancelling Bolus...");

    //using Qtimer to stop single thread blocking
    QTimer::singleShot(4000, this, [this]() {
            ui->bolusActivatedDotsLabel->hide();
            ui->bolusActivatedLabel->hide();
            ui->bolusActivatedRequestingLabel->hide();
            ui->bolusActivatedStopButton->hide();
            ui->bolusButton->setText("BOLUS");
            ui->bolusButton->setEnabled(true);
            if (bolusIndicator) {
                bolusIndicator->hide();
            }
            //emit a signal to abort the bolus - connected in UI control
            emit manualCancelBolus();
    });
}
/*
void homescreen::setCgmManager(CgmDataManager* manager) {
    connect(manager, &CgmDataManager::newCgmPoint,
            this,    &homescreen::onNewCgmPoint);

    for (const auto& point : manager->getHistory()) {
        series->append(point.time.toMSecsSinceEpoch(), point.value);
    }
}
*/

void homescreen::onNewCgmPoint(const GlucosePoint& point) {
   // qDebug() << "[Chart] Received CGM point:" << point.value << "at" << point.time.toString();

    // Append new data point to the glucose series
    series->append(point.time.toMSecsSinceEpoch(), point.value);

    auto axisX = qobject_cast<QDateTimeAxis*>(series->attachedAxes().at(0));

    if (axisX) {

        QDateTime end = point.time;
        QDateTime start = end.addSecs(-120);

        axisX->setRange(start, end);


        qint64 startMs = start.toMSecsSinceEpoch();
        qint64 endMs = end.toMSecsSinceEpoch();


        targetRange->upperSeries()->replace(QVector<QPointF>{
    QPointF(startMs, 10.0),
    QPointF(endMs, 10.0)
});


        targetRange->lowerSeries()->replace(QVector<QPointF>{
    QPointF(startMs, 3.9),
    QPointF(endMs, 3.9)
});



        highLimitLine->replace(QVector<QPointF>{
    QPointF(startMs, 10.0),
    QPointF(endMs, 10.0)
});


        lowLimitLine->replace(QVector<QPointF>{
    QPointF(startMs, 3.9),
    QPointF(endMs, 3.9)
});

    }


    ui->latestReadingLabel->setText(QString::number(point.value, 'f', 1));
    updateStatusIcon(point.value);
   // updateStatusIcon(3.8f);  // should show "stops"

}


void homescreen::cycleTimeWindow() {
    // Move to next time window
    currentTimeWindowIndex = (currentTimeWindowIndex + 1) % timeWindows.size();
    QString newLabel = timeWindows[currentTimeWindowIndex];
    ui->timeWindowButton->setText(newLabel);

    // Parse hours from label
    int hours = newLabel.split(" ").first().toInt();

    // Update X axis range
    QDateTime now = QDateTime::currentDateTime();
    axisX->setRange(now.addSecs(-hours * 3600), now);
}


void homescreen::updateStatusIcon(float glucoseValue) {
    QString iconPath;

    if (glucoseValue >= 10.0f) {
        iconPath = ":/icons/icons/delivers.png";
    } else if (glucoseValue >= 8.9f) {
        iconPath = ":/icons/icons/increases.png";
    } else if (glucoseValue >= 6.25f) {
        iconPath = ":/icons/icons/maintains.png";
    } else if (glucoseValue >= 3.9f) {
        iconPath = ":/icons/icons/decreases.png";
    } else {
        iconPath = ":/icons/icons/stops.png";
    }

    if (statusIconLabel) {
        statusIconLabel->setPixmap(QPixmap(iconPath).scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void homescreen::loadPersonData(const QMap<QDateTime, float>& realMap,
                                const QMap<QDateTime, float>& predictedMap)
{
    series->clear();
    predictedSeries->clear();

    for (const auto& dt : realMap.keys()) {
        series->append(dt.toMSecsSinceEpoch(), realMap[dt]);
    }

    for (const auto& dt : predictedMap.keys()) {
        predictedSeries->append(dt.toMSecsSinceEpoch(), predictedMap[dt]);
    }

    // Auto scroll to latest timestamp
    if (!realMap.isEmpty()) {
        QDateTime latest = realMap.lastKey();
        axisX->setRange(latest.addSecs(-120), latest);
    }
}

void homescreen::onNewCgmValues(QDateTime time, float real, float predicted) {

    static int count = 0;
    count++;
    if (count % 2 != 0) return;  // only plot every 2nd reading

    series->append(time.toMSecsSinceEpoch(), real);
    predictedSeries->append(time.toMSecsSinceEpoch(), predicted);

    axisX->setRange(time.addSecs(-120), time);
    qint64 startMs = time.addSecs(-120).toMSecsSinceEpoch();
    qint64 endMs   = time.toMSecsSinceEpoch();

    highLimitLine->clear();
    lowLimitLine->clear();
    highLimitLine->append(startMs, 10.0);
    highLimitLine->append(endMs, 10.0);
    lowLimitLine->append(startMs, 3.9);
    lowLimitLine->append(endMs, 3.9);

    targetRange->upperSeries()->clear();
    targetRange->lowerSeries()->clear();
    targetRange->upperSeries()->append(startMs, 10.0);
    targetRange->upperSeries()->append(endMs, 10.0);
    targetRange->lowerSeries()->append(startMs, 3.9);
    targetRange->lowerSeries()->append(endMs, 3.9);


    ui->latestReadingLabel->setText(QString::number(real, 'f', 1));
    updateStatusIcon(real);
}

