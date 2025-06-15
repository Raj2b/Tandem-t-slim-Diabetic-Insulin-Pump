#ifndef RNG_H
#define RNG_H

#include <random>

#include "RangeCompare.h"

namespace RNG {

template<typename T>
float normalDistribution(T floor, T ceiling, T mean, T deviation) {
    floor = std::min(floor, ceiling);
    ceiling = std::max(floor, ceiling);

    std::random_device rd;  //seed
    std::mt19937 gen( rd() ); //seeded mersenne_twister_engine

    std::normal_distribution<T> distribution( mean, deviation );

    return RangeCompare::clamp(distribution(gen) , floor, ceiling);
}

template<typename T>
float uniformDistribution(T floor, T ceiling) {
    floor = std::min(floor, ceiling);
    ceiling = std::max(floor, ceiling);

    std::random_device rd;
    std::mt19937 gen( rd() );

    std::uniform_real_distribution<> distribution(floor, ceiling);

    return distribution(gen);
}

};

#endif // RNG_H
