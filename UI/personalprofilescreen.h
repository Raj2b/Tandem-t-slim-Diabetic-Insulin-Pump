#ifndef PERSONALPROFILESCREEN_H
#define PERSONALPROFILESCREEN_H

#include <QWidget>
#include <QPushButton>
#include <vector>

#include "Profile.h"
#include "Modules/ProfileDB.h"
#include "Modules/Injector.h"

namespace Ui {
class personalprofilescreen;
}

class personalprofilescreen : public QWidget
{
    Q_OBJECT

public:
    explicit personalprofilescreen(ProfileDB& profileDB, DeviceSettings& deviceSettings, QWidget *parent = nullptr);
    ~personalprofilescreen();

private:
    Ui::personalprofilescreen *ui;
    void createProfile();
    void createNewSettings();
    void retrieveProfiles(QWidget *parentWidget);
    void editProfile(QString profileName);
    void resetView(QString profileName);
    void submitProfile();
    void deleteProfile();
    void retrieveTimeslots(QWidget *parentWidget);
    void editTimeslot(QString profileName, int hour, int min);
    void deleteTimeslot(QString profileName, int hour, int min);

    void activateProfile(QString);

    std::vector<QPushButton*> retrievedProfiles;
    std::vector<QPushButton*> retrievedTimeslots;
    std::vector<QString> timeslotValues;

    ProfileDB& pdb;
    DeviceSettings& ds;

    int hour, min;
    bool editing=false;
    bool newProfile=false;
signals:
    void clickHome();
    void goToPumpSettings();

public slots:
    void resetState();

};

#endif // PERSONALPROFILESCREEN_H
