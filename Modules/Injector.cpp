#include "Injector.h"

Injector::Injector(BasalPump& basalpump, BolusPump& boluspump)
    : basal(basalpump),
      bolus(boluspump)
{
    connect(&basal, &BasalPump::consumeInsulin, this, &Injector::consumeInsulin);
    connect(&bolus, &BolusPump::consumeInsulin, this, &Injector::consumeInsulin);

    connect(&basal, &BasalPump::logMessage, this, &Injector::logMessage);
    connect(&bolus, &BolusPump::logMessage, this, &Injector::logMessage);

    connect(&basal, &BasalPump::recordSession, this, &Injector::recordSession);
    connect(&bolus, &BolusPump::recordSession, this, &Injector::recordSession);
}
