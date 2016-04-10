/*
 * Range.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Range.h"
#include "MathTools/BasicDefines.h"

#include <algorithm>

using namespace MathTools;

namespace Core {

    Range::Range()
          : max(MIN_FLOAT),
            min(MAX_FLOAT)
    {}

    Range::~Range() {
    }

    void Range::setMin(float val) {
        min = std::min(min, val);
    }

    void Range::setMax(float val) {
        max = std::max(max, val);
    }

    float Range::getMin() {
        return min;
    }

    float Range::getMax() {
        return max;
    }

} /* namespace Core */
