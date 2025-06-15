#include "CgmDataManager.h"
#include <QtGlobal>
#include <QRandomGenerator>

CgmDataManager::CgmDataManager(QObject* parent)
    : QObject(parent), lastValue(5.5f) // Start at 5.5 mmol/L
{
  //  connect(&timer, &QTimer::timeout, this, &CgmDataManager::generatePoint);
    //timer.start(3000); // Generate new point every 3 seconds
}

const QVector<GlucosePoint>& CgmDataManager::getHistory() const {
    return history;
}

void CgmDataManager::generatePoint() {
   // qDebug() << "[CGM] new point:" << lastValue;
    float delta = -0.2f + static_cast<float>(QRandomGenerator::global()->bounded(1000)) / 1000.0f * 0.4f;
    lastValue = qBound(3.0f, lastValue + delta, 15.0f);

    GlucosePoint p { QDateTime::currentDateTime(), lastValue };
    history.append(p);
    emit newCgmPoint(p);
}
