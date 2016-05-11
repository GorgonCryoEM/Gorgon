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
            : IsoSurface((*this), surfaceMesh)
    {}

    float RendererBase::getMinPos(int i) const {
        return 0.0;
    }

    float RendererBase::getMaxPos(int i) const {
        float result;
        switch(i) {
            case 0:
                result = float(getSizeX()-1);
                break;
            case 1:
                result = float(getSizeY()-1);
                break;
            case 2:
                result = float(getSizeZ()-1);
                break;
        }

        return result;
    }
}
