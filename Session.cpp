#include "Session.h"

Duration Session::getDuration() {
    qint64 secs = getStartDT().secsTo(getEndDT());

    Duration duration = {
        .d = secs / (24 * 60 * 60),
        .h = (secs % (24 * 60 * 60)) / (60*60),
        .m = (secs % 60 * 60) / 60,
        .s = secs % 60
    };

    return duration;
}

std::map<QDateTime, float> Session::getSugar() {
    std::map<QDateTime, float> temp;
    for (auto &dpEntry : datapoints) {
        temp[dpEntry.first] = dpEntry.second.sugar;
    }
    return temp;
}
std::map<QDateTime, float> Session::getInsulin() {
    std::map<QDateTime, float> temp;
    for (auto &dpEntry : datapoints) {
        temp[dpEntry.first] = dpEntry.second.insulin;
    }
    return temp;
}
std::map<QDateTime, float> Session::getBolus() {
    std::map<QDateTime, float> temp;
    for (auto &dpEntry : datapoints) {
        temp[dpEntry.first] = dpEntry.second.bolus;
    }
    return temp;
}

//--------------------------------
QJsonObject Session::toJSON() {
    QJsonObject json;

    json.insert("profile", profile.toJSON());

    { //datapoints
        QJsonObject obj;
        for (auto &dpEntry : datapoints) {
            QString timekey = dpEntry.first.toString("yyyy-MM-dd HH:mm:ss");
            obj.insert(timekey, dpEntry.second.toJSON());
        }
        json.insert("datapoints", obj);
    }

    return json;
}

Session Session::fromJSON(QJsonObject json) {
    try {
        Session s;
        s.profile = Profile::fromJSON( json["profile"].toObject() ); //profile

        { //datapoints
            QJsonObject obj = json["datapoints"].toObject();

            for (const QString &timekey : obj.keys()) {
                QDateTime dt = QDateTime::fromString(timekey, "yyyy-MM-dd HH:mm:ss");
                if (!dt.isValid()) {
                    throw std::runtime_error("Datapoint invalid");
                }

                s.datapoints[dt] = Datapoint::fromJSON( obj[timekey].toObject() );
            }
        }

        return s;

    } catch (std::exception& e) {
        throw std::runtime_error("JSON document invalid");
    }
}

QJsonObject Session::Datapoint::toJSON() {
    QJsonObject json;
    json["sugar"] = sugar;
    json["insulin"] = insulin;
    json["bolus"] = bolus;
    return json;
}

Session::Datapoint Session::Datapoint::fromJSON(QJsonObject json) {
    Datapoint dp;
    dp.bolus = json["bolus"].toDouble();
    dp.insulin = json["insulin"].toDouble();
    dp.sugar = json["sugar"].toDouble();
    return dp;
}
