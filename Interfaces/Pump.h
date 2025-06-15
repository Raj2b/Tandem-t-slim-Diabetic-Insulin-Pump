#ifndef PUMP_H
#define PUMP_H

#include <QObject>
#include "Session.h"

class Pump {
public:
    virtual bool isActive() = 0;
    virtual bool isRunning() = 0;
    virtual bool timelimit() = 0;

signals:
    virtual void consumeInsulin(float basal, float bolus) = 0;
    virtual void logMessage(QString msg) = 0;
    virtual void recordSession(Session& s) = 0;

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void resume() = 0;
    virtual void abort() = 0;
    virtual void close() = 0;
};


#endif // PUMP_H
