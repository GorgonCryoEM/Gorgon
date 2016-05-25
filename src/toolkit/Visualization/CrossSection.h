/*
 * CrossSection.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_
#define SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_

#include "DisplayBase.h"

namespace Visualization {

    /*
     *
     */
    class CrossSection : public DisplayBase {
        public:
            CrossSection();
            virtual ~CrossSection();

            void load3DTexture();

            void draw(int subSceneIndex, bool selectEnabled);

        private:
            Vec3F cuttingPlaneCenter;
            Vec3F cuttingPlaneDirection;
            Volume cuttingVolume;
            NonManifoldMesh * cuttingMesh;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_ */
