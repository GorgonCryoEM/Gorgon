/*
 * VolumeRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "DisplayBase.h"

namespace Visualization {

    class Display : public DisplayBase {
        public:
            Display();

            bool calculateDisplay();
            void setViewingType(const int type);

//            void draw(int subSceneIndex, bool selectEnabled);
//            void setSampleInterval(const int size);
//            void setSurfaceValue(const float value);
//            void setMaxSurfaceValue(const float value);
//            bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);
        private:
            vector<DisplayBase *> displays;
            DisplayBase * cur;

            DisplayBase & renderer;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
