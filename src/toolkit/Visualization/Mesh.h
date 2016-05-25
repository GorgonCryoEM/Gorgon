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

    typedef unsigned long long TKey;
    typedef Vector3<TKey> Vec3U;

    class Mesh {
        public:
//            Mesh();
//            virtual ~Mesh();
            virtual int addMarchingVertex(Vec3F location, int hashKey) =0;
            virtual TKey addMarchingFace(Vec3U vertexHash) =0;

    };

} /* namespace Core */
#endif /* SRC_TOOLKIT_VISUALIZATION_MESH_H_ */

