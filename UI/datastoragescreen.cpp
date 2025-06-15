#include "datastoragescreen.h"
#include "ui_datastoragescreen.h"
#include <QStringList>

datastoragescreen::datastoragescreen(SessionRecorder& session, Controller& controller, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::datastoragescreen),
    currSession(session),
    currController(controller),
    currLogger(controller.getLogger())
{
    //UI setup
    ui->setupUi(this);
    ui->backgroundButton->setEnabled(false);
    ui->scrollArea->hide();
    ui->scrollPageMainLabel->hide();
    ui->nextPageButton->hide();
    ui->previousPageButton->hide();
    ui->currentPageLabel->hide();

    //Connections
   connect(ui->viewSystemLogsButton, &QPushButton::released, this, [this]() {retrieveSysLogs(0);});
   connect(ui->viewHealthLogsButton, &QPushButton::released, this, [this]() {retrieveHealthLogs(0);});
   connect(ui->nextPageButton, &QPushButton::released, this, [this]() {pageOperation(desiredLogs, 1);});
   connect(ui->previousPageButton, &QPushButton::released, this, [this]() {pageOperation(desiredLogs, 0);});
   connect(ui->resetButton, &QPushButton::released, this, [this]() {reset();});

   //for dynamically created buttons on health value retrieval
   container = new QWidget(this);
   layout = new QVBoxLayout(container);

    //Init state
    defaultState();
}

datastoragescreen::~datastoragescreen()
{
    delete ui;
}

//resets state
void datastoragescreen::reset(){
    for (QPushButton* button : foundSessions) {
        if (button) {
            button->setParent(nullptr);
            button->deleteLater();
        }
    }

    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    }

    foundSessions.clear();
    ui->scrollArea->hide();
    ui->scrollPageMainLabel->hide();
    defaultState();
    ui->viewHealthLogsButton->show();
    ui->viewSystemLogsButton->show();
    ui->nextPageButton->hide();
    ui->previousPageButton->hide();
    ui->currentPageLabel->hide();
}

//used when the user clicks the view system logs button
void datastoragescreen::retrieveSysLogs(int){
    reset();
    ui->nextPageButton->show();
    ui->previousPageButton->show();
    ui->currentPageLabel->show();
    desiredLogs = 1;
    currentPage = 1;
    ui->currentViewLabel->setText("Currently Viewing: System Logs");
    pageOperation(desiredLogs, -1);
    ui->viewSystemLogsButton->hide();
}

//used when the user clicks the view health logs button
void datastoragescreen::retrieveHealthLogs(int){
    reset();
    ui->scrollArea->show();
    ui->viewHealthLogsButton->hide();

    //retrieve all sessions, add them to the scroll box
    QStringList fetchedSessions = currSession.fetchSessionsNew();

     ui->scrollPageMainLabel->setText("Select Session From Options Below");
     ui->scrollPageMainLabel->show();

    for (const QString& name : fetchedSessions) {
        QPushButton* button = new QPushButton(name, container);

        connect(button, &QPushButton::clicked, this, [this, name]() {
            qDebug() << "Button clicked (Session):" << name;
            retrieveSessionValues(name);
        });

        foundSessions.push_back(button);
        layout->addWidget(button);
    }

    container->setLayout(layout);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(container);
}

//puts the sessions values on the screen
void datastoragescreen::retrieveSessionValues(QString name){
    reset();
    ui->scrollArea->show();
    ui->scrollPageMainLabel->show();
    ui->viewHealthLogsButton->hide();

    //recover the sessions, extract the values and put them on screen
    Session recoveredSession = currSession.recoverSession(name);

    ui->scrollPageMainLabel->setText("Session: " + name);

    //this is for the first general information chunk

    std::map<QDateTime , float> insulin = recoveredSession.getInsulin();
    std::map<QDateTime , float> cgm = recoveredSession.getSugar();
    std::map<QDateTime , float> bolus = recoveredSession.getBolus();

    QTime latestTime = recoveredSession.getStartDT().time();
    Profile::Timepoint tp = recoveredSession.profile.activeTimepoint(latestTime);
    float correctionFactor = tp.correctionFactor;
    float basalRate = tp.basalRate;
    float targetSugar = tp.targetSugar;
    float carbInsulinRatio = tp.carbInsulinRatio;

    QVBoxLayout* chunkLayoutTemp = new QVBoxLayout();

    QLabel* duration1 = new QLabel(container);
    duration1->setText(QString("Total Session Duration:  " + recoveredSession.getDuration().toString()));
    chunkLayoutTemp->addWidget(duration1);

    QLabel* insulinLabel1 = new QLabel(container);
    insulinLabel1->setText(QString("Session-Profile Last Correction Factor Target:  %1").arg(correctionFactor, 0, 'f', 2));
    chunkLayoutTemp->addWidget(insulinLabel1);

    //insulin label. Constructs the label, adds it to the layout and increases the iterator
    QLabel* basalRate1 = new QLabel(container);
    basalRate1->setText(QString("Session-Profile Last Basal Rate Target:  %1").arg(basalRate, 0, 'f', 2));
    chunkLayoutTemp->addWidget(basalRate1);

    QLabel* targetSugar1 = new QLabel(container);
    targetSugar1->setText(QString("Session-Profile Last Sugar Target:  %1").arg(targetSugar, 0, 'f', 2));
    chunkLayoutTemp->addWidget(targetSugar1);

    QLabel* carbRatio1 = new QLabel(container);
    carbRatio1->setText(QString("Session-Profile Last Carb-Insulin Ratio Target:  %1").arg(carbInsulinRatio, 0, 'f', 2));
    chunkLayoutTemp->addWidget(carbRatio1);

    QWidget* chunkWidgetTemp = new QWidget(container);
    chunkWidgetTemp->setLayout(chunkLayoutTemp);
    layout->addWidget(chunkWidgetTemp);
    layout->addSpacing(10);

    //then add all values in chunks

    auto it = recoveredSession.datapoints.begin();
    for(int i = 0; i < 100 && it != recoveredSession.datapoints.end(); i++, it++) {
        QDateTime timestamp = it->first;
        Session::Datapoint datapoint = it->second;

        QVBoxLayout* chunkLayout = new QVBoxLayout();

        //insulin label. Constructs the label, adds it to the layout and increases the iterator
        QLabel* insulinLabel = new QLabel(container);
        insulinLabel->setText(QString("Insulin Value at: [%1]  | %2").arg(timestamp.toString("yyyy-MM-dd HH:mm:ss")).arg(datapoint.insulin, 0, 'f', 2));
        chunkLayout->addWidget(insulinLabel);

        //CGM label
        QLabel* sugarLabel = new QLabel(container);
        sugarLabel->setText(QString("CGM Value at: [%1]  | %2").arg(timestamp.toString("yyyy-MM-dd HH:mm:ss")).arg(datapoint.sugar, 0, 'f', 2));
        chunkLayout->addWidget(sugarLabel);

        //bolus label
        QLabel* bolusLabel = new QLabel(container);
        bolusLabel->setText(QString("Bolus Value at: [%1]  | %2").arg(timestamp.toString("yyyy-MM-dd HH:mm:ss")).arg(datapoint.bolus, 0, 'f', 2));
        chunkLayout->addWidget(bolusLabel);

        //Wrap chunk in a widget
        QWidget* chunkWidget = new QWidget(container);
        chunkWidget->setLayout(chunkLayout);
        layout->addWidget(chunkWidget);
        layout->addSpacing(5);

      }

}

//has recent heatlh values
void datastoragescreen::defaultState(){
    desiredLogs=0;
    currentPage=1;

    ui->currentPageLabel->setText("Current Page: 1");
    ui->currentViewLabel->setText("Currently Viewing: Current Status Page (recent)");

    //retrieve recent values from most recent session
    QStringList sessions = currSession.fetchSessionsNew();
    //qDebug() << sessions;
    if (sessions.isEmpty()) {
        ui->entry1Label->setText("No session data found.");
        ui->entry2Label->clear();
        ui->entry3Label->clear();
        ui->entry4Label->clear();
        ui->entry5Label->clear();
        return;
    }
    Session recoveredSession = currSession.recoverSession(sessions.back());

    std::map<QDateTime , float> insulin = recoveredSession.getInsulin();
    if (insulin.empty()) {
        ui->entry1Label->setText("Most Recent Session is Corrupted. No Datapoints are present");
        ui->entry2Label->clear();
        ui->entry3Label->clear();
        ui->entry4Label->clear();
        ui->entry5Label->clear();
        return;
    }

    auto basalEntry = *insulin.rbegin();
    float basalValue = basalEntry.second; //Basal Value

    std::map<QDateTime , float> cgm = recoveredSession.getSugar();
    auto cgmEntry = *cgm.rbegin();
    float cgmValue = cgmEntry.second; //cgm Value

    std::map<QDateTime , float> bolus = recoveredSession.getBolus();
    auto bolusEntry = *bolus.rbegin();
    QDateTime firstTime = bolusEntry.first; //Bolus Time
    float bolusValue = bolusEntry.second; //Bolus Value

    QTime latestTime = recoveredSession.getStartDT().time();
    Profile::Timepoint tp = recoveredSession.profile.activeTimepoint(latestTime);
    float correctionFactor = tp.correctionFactor;

    ui->entry1Label->setText("Last Bolus Time: " + firstTime.toString("dd-MM-yyyy HH:mm:ss") + "  |  Bolus Value: " + QString::number(bolusValue));
    ui->entry2Label->setText("Last Basal Value: " + QString::number(basalValue));
    ui->entry3Label->setText("Last Sugar Value (CGM): " + QString::number(cgmValue));
    ui->entry4Label->setText("Last Session Duration: " + recoveredSession.getDuration().toString());
    ui->entry5Label->setText("Correction Factor: " + QString::number(correctionFactor));

}

//somewhat legacy. Only used in system logs now. Logtype will always be 1
//flips through the syslog pages
void datastoragescreen::pageOperation(int logType, int pageDirection){
    //logType = 1 = Sys logs
    //logType = 0 = Health logs
    //Page Direction: 1 = Forward (Retrieve Older), 0 = Back (Retrieve Newer). -1 (Switch to page 0)

    if(pageDirection==-1){
        ui->currentPageLabel->setText("Current Page: " + QString::number(currentPage));
    }else if(pageDirection==1){
        currentPage++;
        ui->currentPageLabel->setText("Current Page: " + QString::number(currentPage));
    }else if(pageDirection==0){
        if(currentPage==1){return;} //return if already on page 0
        currentPage--;
        ui->currentPageLabel->setText("Current Page: " + QString::number(currentPage));
    }

    //Retrieve Sys Logs
    if(logType==1){

        //Fetches an increment of 5 logs
        //Current page (1, 2 ..) *  5 (e.g Page 1 (home) * 5 = First 5 logs)
        QStringList sysLogs = currLogger.fetchLinesEndingAt((currentPage * 5));

        //Update the Labels based on the returned contents above
        for (int i = 0; i < sysLogs.size(); ++i) {
            QString labelName = QString("entry%1Label").arg(i + 1);
            QLabel* label = this->findChild<QLabel*>(labelName);

            if (label) {
                label->setText(sysLogs.at(i));
            }
        }
        return;
    }

    if(logType==0){
        //Todo Scrap This

        return;
    }

    return;

}
