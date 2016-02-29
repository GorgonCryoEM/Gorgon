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
            IsoSurface(const RendererBase & rend);
            virtual ~IsoSurface();

            bool calculateDisplay();
            void load3DTexture();
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_ */
