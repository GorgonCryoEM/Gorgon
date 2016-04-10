/*
 * Range.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Range.h"

namespace Core {

    Range::Range()
          : max(MIN_FLOAT),
            min(MAX_FLOAT)
    {}

    Range::~Range() {
    }

    void Range::setMin(float val) {
        min = ::min(min, val);
    }

    void Range::setMax(float val) {
        max = ::max(max, val);
    }

    float Range::getMin() {
        return min;
    }

    float Range::getMax() {
        return max;
    }

} /* namespace Core */
