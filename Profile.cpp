#include "Profile.h"

Profile::Timepoint Profile::activeTimepoint(QTime time)  {
    for (auto it = timepoints.begin(); it != timepoints.end(); ++it) {
        if (time < it->first) {
            return (it--)->second;
        }
    }
    return timepoints.rbegin()->second;
}

QJsonObject Profile::toJSON() {
    QJsonObject profileJson;

    profileJson["name"] = name;

    { //timepoints
        QJsonObject obj;
        for (auto &tpEntry : timepoints) {
            QString timekey = tpEntry.first.toString("HH:mm:ss");
            obj.insert(timekey, tpEntry.second.toJSON());
        }
        profileJson.insert("timepoints", obj);
    }

    profileJson.insert("bolus", bolus.toJSON());

    return profileJson;
}

Profile Profile::fromJSON(QJsonObject json) {
    Profile p;
    p.name = json["name"].toString();

    { //timepoints
        QJsonObject obj = json["timepoints"].toObject();

        for (const QString &timekey : obj.keys()) {
            QTime time = QTime::fromString(timekey, "HH:mm:ss");
            if (!time.isValid()) {
                throw std::runtime_error("Timepoint invalid");
            }

            p.timepoints[time] = Timepoint::fromJSON( obj[timekey].toObject() );
        }
    }

    p.bolus = Bolus::fromJSON(json["bolus"].toObject());

    return p;
}

//------------------------------------
QJsonObject Profile::Timepoint::toJSON() {
    QJsonObject json;
    json["basalRate"] = basalRate;
    json["correctionFactor"] = correctionFactor;
    json["targetSugar"] = targetSugar;
    json["carbRatio"] = carbInsulinRatio;
    return json;
}

Profile::Timepoint Profile::Timepoint::fromJSON(QJsonObject json) {
    Profile::Timepoint tp;
    tp.basalRate = json["basalRate"].toDouble();
    tp.correctionFactor = json["correctionFactor"].toDouble();
    tp.targetSugar = json["targetSugar"].toDouble();
    tp.carbInsulinRatio = json["carbRatio"].toDouble();
    return tp;
}

//------------------------------------
QJsonObject Profile::Bolus::toJSON() {
    QJsonObject json;
    json["duration"] = duration.toString("HH:mm:ss");
    json["enableCarbs"] = enableCarbs;
    return json;
}

Profile::Bolus Profile::Bolus::fromJSON(QJsonObject json) {
    Profile::Bolus bol;

    QTime time = QTime::fromString(json["duration"].toString());
    if (!time.isValid()) {
        throw std::runtime_error("Bolus invalid");
    }

    bol.duration = time;
    bol.enableCarbs = json["enableCarbs"].toBool();
    return bol;
}
