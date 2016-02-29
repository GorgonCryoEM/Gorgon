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

        private:
            vector<DisplayBase *> displays;
            DisplayBase * cur;

            RendererBase & renderer;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
