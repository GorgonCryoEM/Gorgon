/*
 * VolumeRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_

#include "RendererBase.h"
#include "VolumeMesh.h"

namespace Visualization {

    class VolumeRenderer : public RendererBase {
        public:
            VolumeRenderer();
            virtual ~VolumeRenderer();

            void draw(int subSceneIndex, bool selectEnabled);

        private:
            VolumeMesh surfaceMesh;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_ */
