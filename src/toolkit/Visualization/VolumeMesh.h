/*
 * VolumeMesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_
#define SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_

#include "Core/volume.h"
#include "MeshBase.h"

namespace Core {

    class VolumeMesh : public Volume, public MeshBase {
        public:
            void draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);

            void save(string fileName);
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_ */
