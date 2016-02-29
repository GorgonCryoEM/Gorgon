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
            CrossSection(const DisplayBase & rend);
            virtual ~CrossSection();

            bool calculateDisplay();
            void load3DTexture();

//            void draw(int subSceneIndex, bool selectEnabled);
            void setSampleInterval(const int size);
            void setSurfaceValue(const float value);
//            void setMaxSurfaceValue(const float value);
//            bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_ */
