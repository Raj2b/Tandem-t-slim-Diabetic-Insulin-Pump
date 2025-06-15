#include "Person.h"
#include <QDebug>
Person::Person()
{
    scantimer.callOnTimeout( [this](){ generateFluctuations(); } );
    scantimer.start(1000);
}

void Person::generateFluctuations() {
    //qDebug() << "REAL SUGAR:" << getSugar() << insulin << bolus;

//    correctionFactor = RNG::uniformDistribution(1.5,5.5);

    floatingSugar = RNG::normalDistribution(0.0f, 22.0f, 18.0f/*floatingSugar*/, 2.0f);
    floatingSugar = std::max(0.0f, floatingSugar);

    //reduce potency over time
    insulin = std::max(0.0, insulin - 0.25);
    bolus = std::max(0.0, bolus - 0.75);
}

void Person::injectBasal(float amount) {
    insulin += amount;
}

void Person::injectBolus(float amount) {
    bolus += amount;
}

const QMap<QDateTime, float>& Person::getRealData() const {
    return realData;
}

const QMap<QDateTime, float>& Person::getPredictedData() const {
    return predictedData;
}

