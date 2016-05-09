/*
 * VolumeRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "IsoSurface.h"

namespace Visualization {

    class Display : public IsoSurface {
        public:
            Display();

        private:
            vector<DisplayType *> displays;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
