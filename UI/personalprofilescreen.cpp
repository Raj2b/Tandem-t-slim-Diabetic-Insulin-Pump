#include "personalprofilescreen.h"
#include "ui_personalprofilescreen.h"
#include <iostream>
#include <QtDebug>
#include <vector>
#include <QTime>

personalprofilescreen::personalprofilescreen(ProfileDB& profileDB, DeviceSettings& deviceSettings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::personalprofilescreen),
    pdb(profileDB),
    ds(deviceSettings)
{
    ui->setupUi(this);
    ui->checkButton->hide();
    ui->checkButton_2->hide();
    ui->label_8->hide();
    ui->stackedWidget->setCurrentIndex(1);
    ui->textEdit->hide();
    ui->deleteButton->hide();
    ui->deleteButton_2->hide();
    ui->deleteLabel_2->hide();
    ui->deleteLabel->hide();
    ui->checkButton_3->hide();
    ui->label_12->hide();

    connect(ui->basalRate, &QTextEdit::textChanged, this, [=]() {
        float value = ui->basalRate->toPlainText().toFloat();
        if (value < 0 ){
            ui->basalRate->blockSignals(true);
            ui->basalRate->setPlainText("0.75");
            ui->basalRate->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->basalRate->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->basalRate->setTextCursor(cursor);
            return;
        }
        if (value > 10) {
            //Cap the value at 1
            ui->basalRate->blockSignals(true);
            ui->basalRate->setPlainText("10");
            ui->basalRate->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->basalRate->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->basalRate->setTextCursor(cursor);
        }
    });

    connect(ui->correctionFactor, &QTextEdit::textChanged, this, [=]() {
        float value = ui->correctionFactor->toPlainText().toFloat();
        if (value < 0 ){
            ui->correctionFactor->blockSignals(true);
            ui->correctionFactor->setPlainText("1");
            ui->correctionFactor->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->correctionFactor->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->correctionFactor->setTextCursor(cursor);
            return;
        }
        if (value > 25) {
            //Cap the value at 25
            ui->correctionFactor->blockSignals(true);
            ui->correctionFactor->setPlainText("25");
            ui->correctionFactor->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->correctionFactor->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->correctionFactor->setTextCursor(cursor);
        }
    });

    connect(ui->carbRatio, &QTextEdit::textChanged, this, [=]() {
        float value = ui->carbRatio->toPlainText().toFloat();
        if (value < 0 ){
            ui->carbRatio->blockSignals(true);
            ui->carbRatio->setPlainText("1");
            ui->carbRatio->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->carbRatio->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->basalRate->setTextCursor(cursor);
            return;
        }
        if (value > 50) {
            //Cap the value at 50
            ui->carbRatio->blockSignals(true);
            ui->carbRatio->setPlainText("50");
            ui->carbRatio->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->carbRatio->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->carbRatio->setTextCursor(cursor);
        }
    });

    connect(ui->targetBG, &QTextEdit::textChanged, this, [=]() {
        float value = ui->targetBG->toPlainText().toFloat();
        if (value < 0 ){
            ui->targetBG->blockSignals(true);
            ui->targetBG->setPlainText("1");
            ui->targetBG->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->targetBG->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->targetBG->setTextCursor(cursor);
            return;
        }
        if (value > 15) {
            //Cap the value at 15
            ui->targetBG->blockSignals(true);
            ui->targetBG->setPlainText("15");
            ui->targetBG->blockSignals(false);

            //Move cursor to end
            QTextCursor cursor = ui->targetBG->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->targetBG->setTextCursor(cursor);
        }
    });

    connect(ui->homeButton_2, &QPushButton::released, this, &personalprofilescreen::createProfile); //Create new Profile
    connect(ui->checkButton, &QPushButton::released, this, &personalprofilescreen::createNewSettings); //Create new Settings
    connect(ui->homeButton, &QPushButton::released, this, [this]() {resetView("NA"); ui->label_9->show(); editing = false; retrieveProfiles(ui->stackedWidget); emit goToPumpSettings();});
    connect(ui->deleteButton, &QPushButton::released, this, [this]() {deleteProfile();}); //Delete Profile Button
    connect(ui->deleteButton_2, &QPushButton::released, this, [this]() {deleteTimeslot(ui->textEdit->toPlainText(), hour, min);}); //Delete Timeslot Button
    connect(ui->addTimeslotButton, &QPushButton::released, this, [this]() {resetView("1"); ui->stackedWidget->setCurrentIndex(2); ui->checkButton_2->show(); ui->label_8->show();}); //Add new timelott
    connect(ui->checkButton_3, &QPushButton::clicked, this, [=]() {
        editTimeslot(ui->textEdit->toPlainText(), hour, min);
    });

    connect(ui->setActiveButton, &QPushButton::released, this, [this]() {activateProfile(ui->textEdit->toPlainText());}); //Extra Bolus Settings
    ui->setActiveButton->hide();
    ui->setActiveLabel->hide();
}

personalprofilescreen::~personalprofilescreen()
{
    delete ui;
}

void personalprofilescreen::createProfile(){
    resetView("NA");
    ui->checkButton->show();
    ui->textEdit->show();
    ui->stackedWidget->show();
    ui->label_9->hide();
    newProfile=true;
    connect(ui->bolusButton_6, &QPushButton::released, this, [this]() {ui->stackedWidget->setCurrentIndex(4);}); //Extra Timed Settings
    connect(ui->bolusButton_5, &QPushButton::released, this, [this]() {ui->stackedWidget->setCurrentIndex(3);}); //Extra Bolus Settings
}

void personalprofilescreen::createNewSettings(){
    ui->label_8->show();
    ui->checkButton_2->show();
    ui->checkButton->hide();
    ui->textEdit->hide();
    ui->stackedWidget->setCurrentIndex(0);

    QString profileName = ui->textEdit->toPlainText();
    QString profileString = "Profile: ";
    QString finalText = profileString + profileName;
    ui->label->setText(finalText);
    ui->label->setAlignment(Qt::AlignCenter);

    //submit button
    connect(ui->checkButton_2, &QPushButton::released, this, [this]() {
        submitProfile();
        if(ui->stackedWidget->currentIndex()==0 && ui->textEdit->toPlainText() == "99Created99"){resetView("NA"); retrieveProfiles(ui->stackedWidget); return;}
        if(ui->stackedWidget->currentIndex()==3 && editing==true){
            resetView("Value"); ui->stackedWidget->setCurrentIndex(0);
            if(ui->carboCheckBox->isChecked()){
                ui->bolusButton_4->setText("ON");
            }else{
                ui->bolusButton_4->setText("OFF");
            }
            int insulinDuration = ui->insulinDuration->value();
            int insulinDuration2 = ui->insulinDuration_2->value();
            QTime insulinTime(insulinDuration, insulinDuration2);
            ui->bolusButton_2->setText(insulinTime.toString("HH:mm") + " hrs");
            return;
        }
        if(ui->stackedWidget->currentIndex()!=0 && editing==true){resetView("Value"); ui->stackedWidget->setCurrentIndex(4); retrieveTimeslots(ui->stackedWidget); return;}
        if(ui->stackedWidget->currentIndex()!=0){
            ui->stackedWidget->setCurrentIndex(0);
            std::cout<<"[Debug] Returning To Main Profile Creation"<<std::endl;
            if(ui->carboCheckBox->isChecked()){
                ui->bolusButton_4->setText("ON");
            }else{
                ui->bolusButton_4->setText("OFF");
            }
            int insulinDuration = ui->insulinDuration->value();
            int insulinDuration2 = ui->insulinDuration_2->value();
            QTime insulinTime(insulinDuration, insulinDuration2);
            ui->bolusButton_2->setText(insulinTime.toString("HH:mm") + " hrs");
            return;
        }
    });
}

//submits the profile information for either creation of updates
void personalprofilescreen::submitProfile(){
    QString profileName = ui->textEdit->toPlainText();

    //if we're not at home, dont submit info
    if(ui->stackedWidget->currentIndex()!=0){
        if(ui->stackedWidget->currentIndex()==3){return;}
        QString basalRate = ui->basalRate->toPlainText();
        QString correctionFactor = ui->correctionFactor->toPlainText();
        QString carbRatio = ui->carbRatio->toPlainText();
        QString targetBG = ui->targetBG->toPlainText();

        //if we're editing a timeslot this will be true
        if(editing==true){
            editTimeslot(profileName,ui->timeHourValue->value(), ui->timeMinuteValue->value());
            return;
        }

        //push all entered values into this vector which is used to create the profile
        timeslotValues.push_back(correctionFactor);
        timeslotValues.push_back(carbRatio);
        timeslotValues.push_back(targetBG);
        timeslotValues.push_back(basalRate);
        timeslotValues.push_back(QString::number(ui->timeHourValue->value()));
        timeslotValues.push_back(QString::number(ui->timeMinuteValue->value()));


        //reset fields
        ui->basalRate->setText("0");
        ui->correctionFactor->setText("0");
        ui->carbRatio->setText("0");
        ui->targetBG->setText("0");
        ui->timeHourValue->setValue(0);
        ui->timeMinuteValue->setValue(0);
        return;
    }

    //this info does not change per timeslot so its done after all timeslot info is recorded
    int insulinDuration = ui->insulinDuration->value();
    int insulinDuration2 = ui->insulinDuration_2->value();
    QTime insulinTime(insulinDuration, insulinDuration2);
    bool carbohydratesEnabled = ui->carboCheckBox->isChecked();

    //create the profile
    Profile newProfile;
    newProfile.name = profileName;
    newProfile.bolus.duration = insulinTime;
    newProfile.bolus.enableCarbs = carbohydratesEnabled;

    //Loop and assign values to the timepoints
    for (size_t i = 0; i < timeslotValues.size(); i+=6) {
        Profile::Timepoint tp;
        tp.correctionFactor = timeslotValues[i].toFloat();
        tp.carbInsulinRatio = timeslotValues[i+1].toFloat();
        tp.targetSugar = timeslotValues[i+2].toFloat();
        tp.basalRate = timeslotValues[i+3].toFloat();

        QTime timeValue(timeslotValues[i+4].toInt(), timeslotValues[i+5].toInt());
        newProfile.timepoints[timeValue] = tp;
    }


    //create the profile through the profile database
    pdb.createProfile(newProfile);
    ui->textEdit->setText("99Created99");
    this->newProfile=false;
}

void personalprofilescreen::retrieveProfiles(QWidget *parentWidget){
     ui->homeButton_2->show();

     //dynamic button creation fields
     int startX = 20;
     int startY = 40;
     int buttonWidth = 440;
     int buttonHeight = 40;
     int spacing = 10;
     QString buttonStyle = "font: 75 20pt \"DejaVu Sans\"; ""background-color: rgb(119, 118, 123);";
     QStringList profileList = pdb.fetchProfiles();
     for (int i = 0; i < profileList.size(); ++i) {
         profileList[i].remove(".json");  //Will remove all ".json" from the names of the profiles
     }

     if(profileList.size()==0){qDebug() << "[Debug] No profiles to retrieve"; return;}

     //for each profile retrieved, create a button and put it on the screen underneath it
     for (int i = 0; i < profileList.size(); ++i) {
         QPushButton *button = new QPushButton(QString("Profile: %1").arg(profileList.at(i)), parentWidget);
         button->setGeometry(startX, startY + (i * (buttonHeight + spacing)), buttonWidth, buttonHeight);
         button->setStyleSheet(buttonStyle);
         button->show();
         button->setObjectName(QString("%1").arg(profileList.at(i)));
         retrievedProfiles.push_back(button);

         //connect each button to the edit profile func. This lambda passes the buttons name to the profile to retrieve from the db
         connect(button, &QPushButton::clicked, this, [=]() {
             QString profileName = button->objectName();
             resetView(profileName);
             editProfile(profileName);
         });
     }
}


//called when one of the dynamically created profiles is clicked
void personalprofilescreen::editProfile(QString profileName){
    ui->homeButton_2->hide();
    std::cout<<"[Debug] Editing Profile"<<std::endl;
    editing=true;

    //read the specified profile from the db
    Profile result = pdb.readProfile(profileName);

    //now we fill in all the fields read from the file
    ui->deleteButton->show();
    ui->deleteLabel->show();

    QString profileString = "Profile: ";
    QString finalText = profileString + profileName;
    ui->label->setText(finalText);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->stackedWidget->show();
    ui->stackedWidget->setCurrentIndex(0);

    ui->textEdit->setText(profileName);

    //creates the createNewSettings state
    createNewSettings();

    if(result.bolus.enableCarbs){ui->bolusButton_4->setText("ON"); ui->carboCheckBox->setChecked(true);}else{ui->bolusButton_4->setText("OFF");}
    QString timeString = result.bolus.duration.toString("H:mm");
    ui->bolusButton_2->setText(timeString + " hrs");
    ui->bolusButton_3->setText("View Settings");
    ui->insulinDuration->setValue(result.bolus.duration.hour());
    ui->insulinDuration_2->setValue(result.bolus.duration.minute());
    ui->setActiveButton->show();
    ui->setActiveLabel->show();

    //connect the newly needed info buttons
    connect(ui->bolusButton_6, &QPushButton::released, this, [this]() {ui->stackedWidget->setCurrentIndex(4); retrieveTimeslots(ui->stackedWidget);}); //Extra Timed Settings
    connect(ui->bolusButton_5, &QPushButton::released, this, [this]() {ui->stackedWidget->setCurrentIndex(3);}); //Extra Bolus Settings
}

//resets the state of this UI
void personalprofilescreen::resetView(QString profileName){
    //qDebug() << "[Debug] Resetting View ";
    ui->checkButton->hide();
    ui->checkButton_2->hide();
    ui->label_8->hide();
    ui->textEdit->hide();
    ui->label_9->hide();
    ui->checkButton->hide();
    ui->textEdit->hide();
    ui->deleteButton->hide();
    ui->deleteLabel->hide();

    //clear any dynamically created profile buttons or retrieved timeslot buttons
    if(retrievedProfiles.size()>0){
        for (QPushButton* profile: retrievedProfiles){profile->deleteLater();}
    }
    if(retrievedTimeslots.size()>0){
        for (QPushButton* timeslot: retrievedTimeslots){timeslot->deleteLater();}
    }
    retrievedProfiles.clear();
    retrievedTimeslots.clear();

    ui->basalRate->setText("0");
    ui->correctionFactor->setText("0");
    ui->carbRatio->setText("0");
    ui->targetBG->setText("0");

    ui->bolusButton_2->setText("0 hrs");
    if(profileName=="NA"){ui->textEdit->setText("Enter Name");} //makes the profile name Enter Name, otherwise leave it
    ui->stackedWidget->setCurrentIndex(1);
    ui->deleteButton_2->hide();
    ui->deleteLabel_2->hide();
    ui->checkButton_3->hide();
    ui->label_12->hide();
    ui->setActiveButton->hide();
    ui->setActiveLabel->hide();
}

//dete the profile when the delete button is clicked
void personalprofilescreen::deleteProfile(){
    QString profileName = ui->textEdit->toPlainText();
    qDebug() << "[Debug] Deleting: "<<profileName;

    pdb.deleteProfile(profileName);

    resetView("NA");
    ui->label_9->show();
    retrieveProfiles(ui->stackedWidget);
}

void personalprofilescreen::retrieveTimeslots(QWidget *parentWidget){
    //dynamic button creation values
    int startX = 20;
    int startY = 40;
    int buttonWidth = 440;
    int buttonHeight = 40;
    int spacing = 10;
    QString buttonStyle = "font: 75 20pt \"DejaVu Sans\"; ""background-color: rgb(119, 118, 123);";

    //Read the profile information and required stuffs
    QString profileName = ui->textEdit->toPlainText();
    QStringList times = pdb.getProfileTimes(profileName);
    Profile tempProfile = pdb.readProfile(profileName);

    qDebug() << "[Debug] Retrieving Timeslot: for "<<profileName;

    //Generate the buttons / timeslots and put them on screen
    for (int i = 0; i < times.size(); ++i) {
        QPushButton *button = new QPushButton(QString("Timeslot: %1").arg(times.at(i)), parentWidget);
        button->setGeometry(startX, startY + (i * (buttonHeight + spacing)), buttonWidth, buttonHeight);
        button->setStyleSheet(buttonStyle);
        button->show();
        button->setObjectName(QString("%1").arg(times.at(i)));
        retrievedTimeslots.push_back(button);

        //when timeslot button is clicked enter the editing state
        connect(button, &QPushButton::clicked, this, [=]() {
            QString timeslot = button->objectName();
            QTime time = QTime::fromString(timeslot, "HH:mm:ss");
            resetView(profileName);
            ui->checkButton_3->show();
            ui->stackedWidget->setCurrentIndex(2);
            ui->checkButton_3->show();
            ui->label_12->show();

                const Profile::Timepoint& tp = tempProfile.timepoints.at(time);
                ui->basalRate->setText(QString::number(tp.basalRate));
                ui->carbRatio->setText(QString::number(tp.carbInsulinRatio));
                ui->correctionFactor->setText(QString::number(tp.correctionFactor));
                ui->targetBG->setText(QString::number(tp.targetSugar));

                ui->timeHourValue->setValue(time.hour());
                hour = time.hour();
                ui->timeMinuteValue->setValue(time.minute());
                min=time.minute();
                ui->deleteButton_2->show();
                ui->deleteLabel_2->show();
            qDebug() << "[Debug] Editing Timeslot: "<<timeslot;
        });
    }
}

//used to edit timeslots
void personalprofilescreen::editTimeslot(QString profileName, int hour, int min){
    Profile temp = pdb.readProfile(profileName);

    //delete the timepoint from the profile so it can be re-written
    QTime timeToDelete(hour, min);
    if (temp.timepoints.find(timeToDelete) != temp.timepoints.end()) {
        temp.timepoints.erase(timeToDelete);
    }

    //get the values, assign it to tp and submit the profile to create the modifitcaions
    QString basalRate = ui->basalRate->toPlainText();
    QString correctionFactor = ui->correctionFactor->toPlainText();
    QString carbRatio = ui->carbRatio->toPlainText();
    QString targetBG = ui->targetBG->toPlainText();
    QString::number(ui->timeHourValue->value());
    QString::number(ui->timeMinuteValue->value());

    Profile::Timepoint tp;
    tp.correctionFactor = correctionFactor.toFloat();
    tp.carbInsulinRatio = carbRatio.toFloat();
    tp.targetSugar = targetBG.toFloat();
    tp.basalRate = basalRate.toFloat();

    int insulinDuration = ui->insulinDuration->value();
    int insulinDuration2 = ui->insulinDuration_2->value();
    QTime insulinTime(insulinDuration, insulinDuration2);
    bool carbohydratesEnabled = ui->carboCheckBox->isChecked();

    temp.bolus.duration = insulinTime;
    temp.bolus.enableCarbs = carbohydratesEnabled;

    QTime newTimeValue(ui->timeHourValue->value(), ui->timeMinuteValue->value());
    temp.timepoints[newTimeValue] = tp;

    //submit the edits
    pdb.updateProfile(temp);

    resetView(profileName);
    retrieveTimeslots(ui->stackedWidget);
}

//delete the timeslot if the button is pressed
void personalprofilescreen::deleteTimeslot(QString profileName, int hour, int min){
    Profile temp = pdb.readProfile(profileName);
    QTime timeToDelete(hour, min);

    if (temp.timepoints.find(timeToDelete) != temp.timepoints.end()) {
        temp.timepoints.erase(timeToDelete);
    }
    pdb.updateProfile(temp);
    resetView(profileName);
    retrieveTimeslots(ui->stackedWidget);
    ui->deleteButton_2->hide();
    ui->deleteLabel_2->hide();
}

//set the profile as active for the devices settings
void personalprofilescreen::activateProfile(QString profileName){
    resetView(profileName);
    qDebug() << "[Debug] Setting: "<< profileName << " as Active";
    Profile p = pdb.readProfile(profileName);

    ds.setProfile(p);
    resetState();
}

//Another reset func
void personalprofilescreen::resetState(){
    resetView("NA");
    ui->insulinDuration->setValue(0);
    ui->insulinDuration_2->setValue(0);
    ui->carboCheckBox->setChecked(0);
    ui->stackedWidget->setCurrentIndex(1);
    retrieveProfiles(ui->stackedWidget);
}
