/*
 * Mesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_MESH_H_
#define SRC_TOOLKIT_VISUALIZATION_MESH_H_

#include "MathTools/Vector3.h"

using namespace GraphMatch;

namespace Core {

    typedef Vector3<unsigned long long> Face;

    class Mesh {
        public:
//            Mesh();
//            virtual ~Mesh();
            virtual int addMarchingVertex(Vec3F location, int hashKey) =0;
            virtual unsigned long long addMarchingFace(Face vertexHash) =0;

    };

} /* namespace Core */
#endif /* SRC_TOOLKIT_VISUALIZATION_MESH_H_ */

