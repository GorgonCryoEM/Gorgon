/*
 * RendererBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_

#include "IsoSurface.h"
#include "Core/volume.h"
#include "Visualization/Mesh.h"

using namespace Core;

namespace Visualization {

    class RendererBase : public Volume, public IsoSurface {
        public:
            RendererBase();

            virtual void draw(int subSceneIndex, bool selectEnabled);

            virtual float getMinPos(int i) const;
            virtual float getMaxPos(int i) const;

        protected:
            Mesh surfaceMesh;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
