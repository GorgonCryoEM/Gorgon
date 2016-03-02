/*
 * IsoSurface.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_
#define SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_

#include "DisplayBase.h"

namespace Visualization {

    /*
     *
     */
    class IsoSurface : public DisplayBase {
        public:
            IsoSurface();
            virtual ~IsoSurface();

            bool calculateDisplay();

            void draw(int subSceneIndex, bool selectEnabled);
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_ */
