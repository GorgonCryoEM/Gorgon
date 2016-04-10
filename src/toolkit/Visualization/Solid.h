/*
 * Solid.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_SOLID_H_
#define SRC_TOOLKIT_VISUALIZATION_SOLID_H_

#include "DisplayBase.h"

namespace Visualization {

    /*
     *
     */
    class Solid : public DisplayBase {
        public:
            Solid();
            virtual ~Solid();

            void load3DTexture();

            void draw(int subSceneIndex, bool selectEnabled);
        private:
            Vec3F cuttingPlaneDirection;
            Volume cuttingVolume;
            NonManifoldMesh * cuttingMesh;

    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_SOLID_H_ */
