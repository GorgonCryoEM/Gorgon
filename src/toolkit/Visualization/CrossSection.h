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

            bool calculateDisplay();
            void load3DTexture();

            void draw(int subSceneIndex, bool selectEnabled);

        private:
            Volume cuttingVolume;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_ */
