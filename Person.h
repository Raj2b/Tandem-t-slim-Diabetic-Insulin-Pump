#ifndef PERSON_H
#define PERSON_H

#include <QTimer>
#include <vector>
#include "Utils/RNG.h"
#include <QMap>
#include <QDateTime>

/*
 * predicted sugar calculated using:
 *      RNG that generates a number representing floating sugar (without insulin)
 *      subtract insulin reduction effect to get true sugar
*/
class Person : public QObject {
    Q_OBJECT

public:
    Person();
    float getSugar() { return floatingSugar - reductionEffect(); }
    void injectBasal(float amount);
    void injectBolus(float amount);
    const QMap<QDateTime, float>& getRealData() const;
    const QMap<QDateTime, float>& getPredictedData() const;

private:
    void generateFluctuations();
    float reductionEffect() { return (insulin + bolus) * correctionFactor; }

    float floatingSugar = 19;//RNG::uniformDistribution(18,20);
    float insulin = 0;
    float bolus = 0;
    float correctionFactor = 3;//RNG::uniformDistribution(2.5,4.5); //intentionally might not match active profile

    QTimer scantimer;
    QMap<QDateTime, float> realData;
    QMap<QDateTime, float> predictedData;
};

#endif // PERSON_H
