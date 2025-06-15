#ifndef SESSION_H
#define SESSION_H

#include <QDateTime>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Profile.h"
#include "Utils/Duration.h"

struct Session {
public:
    struct Datapoint {
        float sugar;
        float insulin;
        float bolus;

        QJsonObject toJSON();
        static Datapoint fromJSON(QJsonObject json);
    };

    Duration getDuration();
    QDateTime getStartDT() {
        return datapoints.empty() ? QDateTime::currentDateTime() : datapoints.begin()->first;
    }
    QDateTime getEndDT() {
        return datapoints.empty() ? QDateTime::currentDateTime() : datapoints.rbegin()->first;
    }

    std::map<QDateTime, float> getSugar();
    std::map<QDateTime, float> getInsulin();
    std::map<QDateTime, float> getBolus();

    QJsonObject toJSON();
    static Session fromJSON(QJsonObject json);

    Profile profile; //to get all the profile timepoints
    std::map<QDateTime, Datapoint> datapoints /*= {
        {QDateTime::currentDateTime(), Datapoint()},
        {QDateTime::currentDateTime().addSecs(1), Datapoint()}
    }*/;
};

#endif // SESSION_H
