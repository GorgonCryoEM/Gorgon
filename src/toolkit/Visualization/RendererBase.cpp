/*
 * RendererBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "RendererBase.h"
#include "Foundation/StringUtils.h"

namespace Visualization {
    
    void RendererBase::setDisplayStyle(int style) {
        displayStyle = style;
    }

    float RendererBase::getMinPos(int i) const {
        return minmaxPts[i].getMin();
    }

    float RendererBase::getMaxPos(int i) const {
        return minmaxPts[i].getMax();
    }

    float RendererBase::getSpacingX() const {
        return 1;
    }

    float RendererBase::getSpacingY() const {
        return 1;
    }

    float RendererBase::getSpacingZ() const {
        return 1;
    }

    float RendererBase::getOriginX() const {
        return 0;
    }

    float RendererBase::getOriginY() const {
        return 0;
    }

    float RendererBase::getOriginZ() const {
        return 0;
    }
    
    void RendererBase::setMinMax(Vec3F p) {
        for(int i=0; i<3; ++i) {
            minmaxPts[i].setMin(p[i]);
            minmaxPts[i].setMax(p[i]);
        }
    }

}
