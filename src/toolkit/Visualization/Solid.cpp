/*
 * Solid.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Solid.h"

namespace Visualization {

    Solid::Solid() {
        // TODO Auto-generated constructor stub

    }

    Solid::~Solid() {
        // TODO Auto-generated destructor stub
    }

    Solid::Solid(const Volume& vol)
        : DisplayBase(vol)
    {
    }

    bool Solid::calculateDisplay() {
        cuttingMesh->Clear();
        bool redraw = false;
        if(volData != NULL) {
            redraw = true;
            NonManifoldMesh tempMesh;


            Vec3F modelCenter = (minPts + maxPts)/2.0;
            float distance = (minPts - modelCenter).length();

            for(float position = 1.0; position >= -1.0; position -= 0.01) {
                Vec3F center = modelCenter + cuttingPlaneDirection * position * distance;

                if((center.X() >= minPts[0]) && (center.X() <= maxPts[0]) &&
                    (center.Y() >= minPts[1]) && (center.Y() <= maxPts[1]) &&
                    (center.Z() >= minPts[2]) && (center.Z() <= maxPts[2])) {


                    for(int i = 0; i < 2; i++) {
                        for(int j = 0; j < 2; j++) {
                            for(int k = 0; k < 2; k++) {
                                cuttingVolume(i, j, k) = (center - Vec3F(i * getSizeX(), j * getSizeY(), k * getSizeZ()))* cuttingPlaneDirection;
                            }
                        }
                    }
                    tempMesh.Clear();
                    MarchingCube(&cuttingVolume, &tempMesh, 0.0f, 0, 0, 0, 1);
                    cuttingMesh->MergeMesh(&tempMesh);
                }
            }
        }
        return redraw;
    }

    void Solid::load3DTexture() {
    }

} /* namespace Visualization */
