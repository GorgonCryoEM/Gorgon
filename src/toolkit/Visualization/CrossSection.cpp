/*
 * CrossSection.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "CrossSection.h"

namespace Visualization {

    CrossSection::CrossSection() {
        // TODO Auto-generated constructor stub

    }

    CrossSection::~CrossSection() {
        // TODO Auto-generated destructor stub
    }

    CrossSection::CrossSection(const Volume& vol)
            : DisplayBase(vol)
    {
    }

    bool CrossSection::calculateDisplay() {
        cuttingMesh->Clear();

        bool redraw = false;

        if(volData != NULL) {
            redraw = true;

            if((cuttingPlaneCenter.X() >= minPts[0]) && (cuttingPlaneCenter.X() <= maxPts[0]) &&
                (cuttingPlaneCenter.Y() >= minPts[1]) && (cuttingPlaneCenter.Y() <= maxPts[1]) &&
                (cuttingPlaneCenter.Z() >= minPts[2]) && (cuttingPlaneCenter.Z() <= maxPts[2])) {

                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < 2; j++) {
                        for(int k = 0; k < 2; k++) {
                            cuttingVolume(i, j, k) = (cuttingPlaneCenter - Vec3F(i * getSizeX(), j * getSizeY(), k * getSizeZ()))* cuttingPlaneDirection;
                        }
                    }
                }
                MarchingCube(&cuttingVolume, cuttingMesh, 0.0f, 0, 0, 0, 1);
            }
        }
        return redraw;
    }

} /* namespace Visualization */
