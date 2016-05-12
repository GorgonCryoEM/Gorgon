/*
 * RendererBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "RendererBase.h"
#include "Foundation/StringUtils.h"

namespace Visualization {

    RendererBase::RendererBase()
    {}

    float RendererBase::getMinPos(int i) const {
        return 0.0;
    }

    float RendererBase::getMaxPos(int i) const {
        float result;
        switch(i) {
            case 0:
                result = 100.;
                break;
            case 1:
                result = 100.;
                break;
            case 2:
                result = 100.;
                break;
        }

        return result;
    }
}
