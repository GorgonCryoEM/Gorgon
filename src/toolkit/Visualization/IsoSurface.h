/*
 * IsoSurface.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_
#define SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_

#include "Renderer.h"

namespace Visualization {

    /*
     *
     */
    class IsoSurface : public Renderer {
        public:
            IsoSurface();
            virtual ~IsoSurface();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_ */
