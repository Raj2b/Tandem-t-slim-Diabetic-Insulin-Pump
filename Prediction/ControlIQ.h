#ifndef CONTROLIQ_H
#define CONTROLIQ_H

#include <QTime>
#include <map>

#include "HoltLinear.h"

class ControlIQ {
public:
    ControlIQ();
    float predict(std::map<QDateTime, float> data, QTime delay);

private:

};

#endif // CONTROLIQ_H
