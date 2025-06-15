#ifndef THRESHOLDS_H
#define THRESHOLDS_H

#include<string>
#include "Utils/RangeCompare.h"

namespace Thresholds {
const std::pair<float, float> hyper = {10.0, __FLT_MAX__};
const std::pair<float, float> increase = {8.9, 10.0};
const std::pair<float, float> maintain = {6.25, 8.9};
const std::pair<float, float> decrease = {3.9, 6.25};
const std::pair<float, float> hypo = {0, 3.9};

enum class Range {HYPER, INCREASE, MAINTAIN, DECREASE, HYPO, INVALID};

inline Range getThreshold(float value) {
    if ( RangeCompare::inRange(value, hyper) ) {
        return Range::HYPER;
    } else if ( RangeCompare::inRange(value, increase) ) {
        return Range::INCREASE;
    } else if ( RangeCompare::inRange(value, maintain) ) {
        return Range::MAINTAIN;
    } else if ( RangeCompare::inRange(value, decrease) ) {
        return Range::DECREASE;
    } else if ( RangeCompare::inRange(value, hypo) ) {
        return Range::HYPO;
    } else {
        return Range::HYPO;
    }
}

}

#endif // THRESHOLDS_H
