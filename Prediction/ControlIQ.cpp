#include "ControlIQ.h"
#include <QDebug>

ControlIQ::ControlIQ()
{
}

float ControlIQ::predict(std::map<QDateTime, float> data, QTime delay) {
    HoltLinear hl;

    std::vector<double> values;
    int steps = QTime(0,0,0).secsTo(delay);
    int i = 0;
    for(auto it = data.rbegin(); it != data.rend() && i < steps; ++it) {
        values.push_back(it->second);
        i++;
    }
    std::reverse(values.begin(), values.end());

    hl.fit(values);
    float e = hl.predict(steps/3);

    return e;
}
