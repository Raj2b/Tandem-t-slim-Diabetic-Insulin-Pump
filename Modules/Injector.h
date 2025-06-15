#ifndef INJECTOR_H
#define INJECTOR_H

#include "Enums/Thresholds.h"
#include "Modules/BolusPump.h"
#include "Modules/BasalPump.h"
#include "Modules/Cartridge.h"
#include "Profile.h"
#include "Person.h"

class Injector : public QObject {
    Q_OBJECT
public:
    Injector(BasalPump& basalpump, BolusPump& boluspump);

signals:
    void consumeInsulin(float basal, float bolus);
    void logMessage(QString msg);
    void recordSession(Session& s);

private:
    BasalPump& basal;
    BolusPump& bolus;
};

#endif // INJECTOR_H
