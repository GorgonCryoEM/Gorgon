/*
 * VolumeMesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_
#define SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_

#include "Mesh.h"

namespace Core {

    class VolumeMesh : public Volume, public MeshBase {
        public:
            VolumeMesh();
            virtual ~VolumeMesh();
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_VISUALIZATION_VOLUMEMESH_H_ */
