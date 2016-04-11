/*
 * MinMax.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "MinMax.h"

#include "MathTools/BasicDefines.h"

#include <algorithm>

using namespace MathTools;

namespace Core {

    MinMax::MinMax()
          : max(MIN_FLOAT),
            min(MAX_FLOAT)
    {}

    MinMax::~MinMax() {
    }

    void MinMax::setMin(float val) {
        min = std::min(min, val);
    }

    void MinMax::setMax(float val) {
        max = std::max(max, val);
    }

    float MinMax::getMin() {
        return min;
    }

    float MinMax::getMax() {
        return max;
    }

} /* namespace Core */
