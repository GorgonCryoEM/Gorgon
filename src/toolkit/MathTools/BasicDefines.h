#ifndef TOOLKIT_BASIC_DEFINES_H
#define TOOLKIT_BASIC_DEFINES_H

#include <limits>

namespace MathTools {

    const double MIN_DOUBLE = std::numeric_limits<double>::min();
    const double MAX_DOUBLE = std::numeric_limits<double>::max();
    const  float MIN_FLOAT  = std::numeric_limits<float>::min();
    const  float MAX_FLOAT  = std::numeric_limits<float>::max();
    const    int MAX_INT    = std::numeric_limits<int>::max();

    inline bool isZero(double value, double varience = 1e-16) {
        return value < varience && value > -varience ? true : false;
    }

    inline bool isEqual(double value1, double value2, double varience = 1e-16) {
        return isZero(value1-value2, varience);
    }
}

#endif
