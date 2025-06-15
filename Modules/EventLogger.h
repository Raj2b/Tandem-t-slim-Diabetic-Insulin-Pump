#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include <QString>

class EventLogger {
public:
    EventLogger() = default;
    virtual void log(QString msg) = 0;
};

#endif // EVENTLOGGER_H
