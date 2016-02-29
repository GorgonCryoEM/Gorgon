/*
 * VolumeRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "DisplayBase.h"
#include "IsoSurface.h"
#include "CrossSection.h"
#include "Solid.h"

namespace Visualization {

    class Display : public DisplayBase {
        public:
            Display();

            bool calculateDisplay();

        private:
            vector<DisplayBase *> displays;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
