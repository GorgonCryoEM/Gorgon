/*
 * RendererBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_

#include "Core/volume.h"
#include "Visualization/MeshBase.h"

using namespace Core;

namespace Visualization {

    class RendererBase : public Volume {
        public:
            RendererBase();

            virtual float getMinPos(int i) const;
            virtual float getMaxPos(int i) const;

        protected:
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
