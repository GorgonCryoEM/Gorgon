#ifndef TOOLKIT_BASIC_DEFINES_H
#define TOOLKIT_BASIC_DEFINES_H


namespace MathTools {

    const double MIN_DOUBLE = 1.7E-308;
    const double MAX_DOUBLE = 1.7E+308;
    const float MIN_FLOAT = 3.4E-38f;
    const float MAX_FLOAT = 3.4E+38f;
    const int MAX_INT = 2147483647;

    inline bool isZero(double value, double varience = 1e-16) {
        return (value < varience && value > -varience)? true : false;
    }

    inline bool isEqual(double value1, double value2, double varience = 1e-16) {
        return isZero(value1-value2, varience);
    }
}

#endif
