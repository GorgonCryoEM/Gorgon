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
            Solid(const DisplayBase & rend);
            virtual ~Solid();

            bool calculateDisplay();
            void load3DTexture();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_SOLID_H_ */
