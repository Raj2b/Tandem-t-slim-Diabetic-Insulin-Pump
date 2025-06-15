#ifndef PROFILE_H
#define PROFILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTime>
#include <string>
#include <map>

struct Profile {
public:
    struct Timepoint {
        float basalRate = 1.0;
        float correctionFactor = 3.0;
        float targetSugar = 7.0;
        float carbInsulinRatio = 10.0;

        QJsonObject toJSON();
        static Timepoint fromJSON(QJsonObject json);
    };
    struct Bolus {
        QTime duration = QTime(0,0,0);
        bool enableCarbs = false;

        QJsonObject toJSON();
        static Bolus fromJSON(QJsonObject json);
    };

    QString name = "";

    std::map<QTime, Timepoint> timepoints = { std::make_pair(QTime(0,0,0), Timepoint()) };
    Bolus bolus;

//    QTime duration = QTime(0,0,0);
//    bool useCarbs = false;

    //--------------------------------
    Timepoint activeTimepoint(QTime time);
    QJsonObject toJSON();
    static Profile fromJSON(QJsonObject json);
};

#endif // PROFILE_H
