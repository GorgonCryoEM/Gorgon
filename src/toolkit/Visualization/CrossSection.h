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
            CrossSection(const RendererBase & rend);
            virtual ~CrossSection();

            bool calculateDisplay();
            void load3DTexture();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CROSSSECTION_H_ */
