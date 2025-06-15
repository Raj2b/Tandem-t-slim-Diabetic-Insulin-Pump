#ifndef RANGECOMPARE_H
#define RANGECOMPARE_H

#include <algorithm>

namespace RangeCompare {

template<typename T>
bool inRange(const T& value, const T& min, const T& max) {
    return (value >= std::min(min, max) && value <= std::max(min, max));
}

template<typename T>
bool inRange(const T& value, const std::pair<T,T>& bounds) {
    return inRange(value, bounds.first, bounds.second);
}

template<typename T>
T clamp(const T& value, const T& min, const T& max) {
    T temp = std::max(min, value);
    temp = std::min(temp, max);
    return temp;
}

template<typename T>
T clamp(const T& value, const std::pair<T,T>& bounds) {
    return clamp(value, bounds.first, bounds.second);
}

}

#endif // RANGECOMPARE_H
