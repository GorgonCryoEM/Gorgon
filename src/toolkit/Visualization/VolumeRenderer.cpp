/*
 * VolumeRenderer.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "VolumeRenderer.h"

namespace Visualization {

    VolumeRenderer::VolumeRenderer()
            : IsoSurface(*this, *this)
    {
        // TODO Auto-generated constructor stub

    }

    VolumeRenderer::~VolumeRenderer() {
        // TODO Auto-generated destructor stub
    }

    void VolumeRenderer::draw(int subSceneIndex, bool selectEnabled) {
        if(subSceneIndex == 0)
            VolumeMesh::draw(true, selectEnabled, _useDisplayRadius, displayRadius, radiusOrigin);
    }

} /* namespace Visualization */
