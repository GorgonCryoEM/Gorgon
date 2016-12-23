/*
 * MinMax.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "MinMax.h"

#include <algorithm>
#include <limits>

namespace Core {

    MinMax::MinMax()
          : max(std::numeric_limits<float>::min()),
            min(std::numeric_limits<float>::max())
    {}

    MinMax::~MinMax() {
    }

    void MinMax::setMin(float val) {
        min = std::min(min, val);
    }

    void MinMax::setMax(float val) {
        max = std::max(max, val);
    }

    float MinMax::getMin() const {
        return min;
    }

    float MinMax::getMax() const {
        return max;
    }

} /* namespace Core */
