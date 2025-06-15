#include "manualbolusscreen.h"
#include "ui_manualbolusscreen.h"
#include <QDebug>

manualbolusscreen::manualbolusscreen(BolusPump& boluspump, DeviceSettings& deviceSettings, BasalPump& basalpump, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::manualbolusscreen), confirmation(false),
    bolus(boluspump),
    ds(deviceSettings),
    basal(basalpump)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    //page 0 bolus homescreen
    connect(ui->page0Home, &QPushButton::released, this, [this]() {emit clickHome();});
    connect(ui->page0InsulinButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("grams"); currentButton = ui->page0InsulinButton; currentPage = 0; switchView(9);});
    connect(ui->page0AddBgButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("mmol/L"); currentButton =ui->page0AddBgButton; currentPage = 0; switchView(9);});
    connect(ui->generalBackButton, &QPushButton::released, this, [this]() {switchView(0);}); //TEMP FOR TESTING
    connect(ui->page0Check, &QPushButton::released, this, [this]() {switchView(4); confirmDelivery();});
    connect(ui->page0UnitsButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("units"); currentButton =ui->page0UnitsButton; currentPage = 0; switchView(9);});

    connect(ui->page3CheckButton, &QPushButton::released, this, [this]() {qDebug() <<"[DEBUG] Confirming High Dosage"; confirmation = true; switchView(0);}); //continue to delivery confirmation
    connect(ui->page3CancelButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled

    connect(ui->page4CheckButton, &QPushButton::released, this, [this]() {confirmBolus(); switchView(5);}); //continue to delivery confirmation
    connect(ui->page4CancelButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled


    connect(ui->page5CheckButton, &QPushButton::released, this, [this]() {bolusEntered();}); //continue to delivery confirmation
    connect(ui->page5CancelButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled

    connect(ui->page6CheckButton, &QPushButton::released, this, [this]() {confirmRequest(); switchView(7);}); //continue to delivery confirmation
    connect(ui->page6BackButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled
    connect(ui->page6DeliverNowButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("% Now"); currentButton = ui->page6DeliverNowButton; currentPage = 6; switchView(9);});
    connect(ui->page6DeliverLaterButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("% Later"); currentButton = ui->page6DeliverLaterButton; currentPage = 6; switchView(9);});
    connect(ui->page6DurationButton, &QPushButton::released, this, [this]() {ui->page9MainLabel_2->setText("hour minute"); currentButton = ui->page6DurationButton; currentPage = 6; switchView(9); ui->pushButton_PlusEquals->setText(":");});


    connect(ui->page7CheckButton, &QPushButton::released, this, [this]() {setupBolusConfirm(); switchView(10);}); //continue to delivery confirmation
    connect(ui->page7BackButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled

    connect(ui->page10CheckButton, &QPushButton::released, this, [this]() {bolusDelivered(); switchView(8);}); //continue to delivery confirmation
    connect(ui->page10CancelButton, &QPushButton::released, this, [this]() {switchView(0);}); //go home if cancelled

    connectNumPad();
    ui->page8Graphic->setIcon(QIcon(":/icons/icons/bolusStartedIcon.png"));
    ui->page8Graphic->setIconSize(QSize(240, 110));
    ui->page8Graphic->setFlat(true);
}

manualbolusscreen::~manualbolusscreen()
{
    delete ui;
}


//called to switch the view of the UI
void manualbolusscreen::switchView(int desired){
    ui->stackedWidget->setCurrentIndex(desired);
}

void manualbolusscreen::reset(){
    //resets the state and retrieves values from the CGM
    switchView(0);
    getHealthValues();
    ui->page0InsulinButton->setText("0");
    ui->page0UnitsButton->setText("0");
    ui->page6DeliverNowButton->setText("50");
    ui->page6DeliverLaterButton->setText("50");
    ui->page6DurationButton->setText("2:00 hrs");
    confirmation=false;
    ui->page5CheckButton->setChecked(false);
}

void manualbolusscreen::getHealthValues(){
    qDebug() <<"[DEBUG] Getting Health Values to Populate Bolus Screen";

    //cgm value gets the target sugar and current blood sugar levels
    ui->page0AddBgButton->setText(QString::number(basal.getSugar(), 'f', 2));
    target = basal.getTargetSugar();

}

void manualbolusscreen::unitsEntered(){
    //todo Remove
}

//called when first pages check button is clicked
void manualbolusscreen::confirmDelivery(){
    carbValue = ui->page0InsulinButton->text().toFloat();
    bgValue = ui->page0AddBgButton->text().toFloat();
    unitValue = ui->page0UnitsButton->text().toFloat();
    QTime time = QTime::currentTime();

    //if the cgm sugar value is over the target assign a correction
    if(basal.getSugar() > basal.getTargetSugar()){
        if(confirmation == false){
            switchView(3);
            ui->page3IOBUnitsLabel->setText(QString::number(basal.getInsulin()) + " u");
            ui->page3BGLabel_2->setText(ui->page0AddBgButton->text()+ " mmol/L");

            //correction is the cgm sugar - the target sugar / the profiles correction factor. This is then added to the carb units on the first page
            correction = ((basal.getSugar() - target)/ds.getProfile().activeTimepoint(time).correctionFactor);

            ui->page0UnitsButton->setText(QString::number(unitValue + correction, 'f', 2));
            return;
        }
    }

    //if the cgm sugar value is under the target assign a correction
    if(basal.getSugar() < basal.getTargetSugar()){
        if(confirmation == false){
            switchView(3);
            ui->page3Label1->setText("Your BG is Below Target");
            ui->page3Label2->setText("Reduce Bolus Calculation?");
            ui->page3IOBUnitsLabel->setText(QString::number(basal.getInsulin()) + " u");
            ui->page3BGLabel_2->setText(ui->page0AddBgButton->text()+ " mmol/L");

            //correction is the cgm sugar - the target sugar / the profiles correction factor. This is then added to the carb units on the first page
            correction = ((basal.getSugar() - target)/ds.getProfile().activeTimepoint(time).correctionFactor);

            ui->page0UnitsButton->setText(QString::number(unitValue + correction, 'f', 2));
            return;
        }
    }

    ui->page4CarbsVarLabel->setText(QString::number(carbValue) + " g");
    ui->page4BGVarLabel->setText(QString::number(bgValue) + " mmol/L");
    ui->page4UnitsVarLabel->setText(QString::number(unitValue) + " u");
    ui->page4UnitsVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->page4BGVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->page4CarbsVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    currentPage = 4;
}

void manualbolusscreen::resetView(){
    //Todo Reset Var Fields
}

//confirmation page
void manualbolusscreen::confirmBolus(){
    ui->page5MainLabel->setText("Deliver " + QString::number(unitValue) + " u Bolus?");
    ui->page5MainLabel->setAlignment(Qt::AlignCenter);
    ui->page5CorrectionVarLabel->setText(QString::number(correction, 'f', 2) + " u");
    ui->page5FoodVarLabel->setText(QString::number(unitValue-correction, 'f', 2) + " u");
}

//confirmed the bolus, now see if they wish an extended bolus. If not deliver the quick bolus
void manualbolusscreen::bolusEntered(){

    if(ui->page5ExtendedRadioButton->isChecked()){
        switchView(6);
        ui->page6UnitsButton->setText(QString::number(unitValue-correction, 'f', 2));
        return;
    }

    //deliver the bolus
    switchView(8);
    bolusDelivered();
}

//UI based numpad
void manualbolusscreen::connectNumPad(){
    //qDebug() <<"[DEBUG] ManualBolusNumPadConnected";
    for (int i = 0; i <= 9; ++i) {
        QString name = QString("pushButton_%1").arg(i);

        QPushButton* button = findChild<QPushButton*>(name);

        if (button) {
            connect(button, &QPushButton::released, this, [this, i]() {
                //qDebug() <<"[DEBUG] Button: " << i << " Pressed";
                QString currentText = ui->page9TextEditBox->toPlainText();
                currentText.append(QString::number(i));
                ui->page9TextEditBox->setPlainText(currentText);
            });
        }
    }
    connect(ui->pushButton_Back, &QPushButton::released, this, [this]() {
        QString currentText = ui->page9TextEditBox->toPlainText();
        //qDebug() <<"[DEBUG] Back Button Pressed";
        if (!currentText.isEmpty()) {
            currentText.chop(1);
            ui->page9TextEditBox->setPlainText(currentText);
        }
    });

    connect(ui->pushButton_PlusEquals, &QPushButton::released, this, [this]() {
        QString currentText = ui->page9TextEditBox->toPlainText();
        currentText.append(ui->pushButton_PlusEquals->text());
        ui->page9TextEditBox->setPlainText(currentText);
    });

    connect(ui->page9CheckButton, &QPushButton::released, this, [this]() {
        //qDebug() <<"[DEBUG] Confirm Button Pressed";
        QString currentText = ui->page9TextEditBox->toPlainText();
        numpadValue = currentText.toFloat();
        switchView(currentPage);
        currentButton->setText(currentText);
        ui->page9TextEditBox->setText("");
        if(currentButton == ui->page6DurationButton){
            hoursEntered();
            ui->pushButton_PlusEquals->setText(".");
        }
        if(currentButton == ui->page0InsulinButton){
            ui->page0UnitsButton->setText(QString::number((numpadValue/1), 'f', 2));
        }
    });

    connect(ui->page9CancelButton, &QPushButton::released, this, [this]() {qDebug() <<"[DEBUG] Cancel Button Pressed"; switchView(currentPage); ui->page9TextEditBox->setText("Enter Value");});
}


//Correct Invalid Time Vals
void manualbolusscreen::hoursEntered(){
    QString currentText = ui->page6DurationButton->text();

    if (currentText.length() >= 3) {
        currentText.insert(currentText.length() - 2, ":");
    }

    QTime time = QTime::fromString(currentText, "H:mm");

    if (!time.isValid()) {
        time = QTime(0, 10);
    }

    if (time > QTime(23, 59)) {
        time = QTime(23, 59);
    }
    if (time < QTime(0, 0)) {
        time = QTime(0, 10);
    }

    QString formatted = time.toString("H:mm") + " hrs";
    ui->page6DurationButton->setText(formatted);
}

//ui confirmation of values for extended bolus
void manualbolusscreen::confirmRequest(){
    QString nowValue = ui->page6DeliverLaterButton->text();
    QString laterValue = ui->page6DeliverNowButton->text();
    QString duration = ui->page6DurationButton->text();

    float nowPercent = nowValue.toFloat();
    float laterPercent = laterValue.toFloat();

    if(nowPercent + laterPercent > 100.00){
        ui->page6DeliverNowButton->setText("50");
        ui->page6DeliverLaterButton->setText("50");
        nowValue = "(defaulted) 50";
        laterValue = "(defaulted) 50";
    }


    ui->page7DeliverLaterVarLabel->setText(nowValue + "%");
    ui->page7DeliverNowVarLabel->setText(laterValue + "%");
    ui->page7DurationVarLabel->setText(duration);

    ui->page7DeliverLaterVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->page7DeliverNowVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->page7DurationVarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

//perform final calculations and deliver the bolius #s to the backend
void manualbolusscreen::bolusDelivered(){

    QString units = ui->page0UnitsButton->text(); //correction Already Factored In
    float value = units.toFloat();
    value -= basal.getInsulin(); //current iob

    QString nowValue = ui->page6DeliverNowButton->text();
    QString laterValue = ui->page6DeliverLaterButton->text();

    //calculate the amount to deliver now vs later based on the entered percentages
    float nowPercent = nowValue.toFloat();
    float laterPercent = laterValue.toFloat();

    float nowAmount = value * (nowPercent / 100.0f);
    float laterAmount = value * (laterPercent / 100.0f);

    //if they do or dont want an extended bolus set the labels
    if(ui->page5ExtendedRadioButton->isChecked() == false){
        ui->page8NowLabel->hide();
        ui->page8LaterLabel->hide();

        QString fastValue = QString::number(value, 'f', 2);
        ui->page8PlusLabel->setText(fastValue + " Now");

    }else{

        QString partA = QString::number(nowAmount, 'f', 2);
        QString partB = QString::number(laterAmount, 'f', 2);

        ui->page8NowLabel->setText(partA + " u Now");
        ui->page8LaterLabel->setText(partB + " u Later");
    }

    BolusPump::Settings bolusValues;

    bolusValues.dr = QTime::fromString(ui->page6DurationButton->text(), "hh:mm:ss");

    //if they want a quick bolus just take the full bolus, assign it to the boluspumpsettings and deliver quickly, emit status update to homescreen
    if(ui->page5ExtendedRadioButton->isChecked() == false){
        bolusValues.m = BolusPump::Settings::Mode::QUICK;
        laterAmount = 0.00; //Shouldn't be needed, but ensures no strange behavior
        bolusValues.bolusNow = value;
        emit bolusInitiated(QString::number(value, 'f', 2), 0);
    }else{
        //if they want extended, set the mode and alert homescreen its getting an extended bolus
        bolusValues.m = BolusPump::Settings::Mode::EXTENDED;
        bolusValues.bolusNow = nowAmount;
        emit bolusInitiated(QString::number(nowAmount), 1);
    }

    bolusValues.carbsConsumed = ui->page0InsulinButton->text().toFloat();
    bolusValues.bolusLater  = laterAmount;

    //confirm the settings above and start the bolus
    bolus.confirmSettings(bolusValues);

    bolus.start();

}

void manualbolusscreen::setupBolusConfirm(){
    QString nowValue = ui->page6DeliverNowButton->text();
    QString laterValue = ui->page6DeliverLaterButton->text();
    QString units = ui->page0UnitsButton->text(); //Correction Factored in

    float value = units.toFloat();
    value -= basal.getInsulin();

    float nowPercent = nowValue.toFloat();
    float laterPercent = laterValue.toFloat();

    float nowAmount = value * (nowPercent / 100.0f);
    float laterAmount = value * (laterPercent / 100.0f);


    QString partA = QString::number(nowAmount, 'f', 2);
    QString partB = QString::number(laterAmount, 'f', 2);

    ui->page10DeliverNowVarLabel->setText(partA + " u");
    ui->page10DeliverLaterVarLabel->setText(partB + " u");
    ui->page10DeliverDurationVarLabel->setText(ui->page6DurationButton->text());
    ui->label->setText("Deliver " + QString::number(nowAmount+laterAmount, 'f', 2) + " u Bolus?");
}
