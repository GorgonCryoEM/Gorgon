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

    /*
     *
     */
    class Mesh {
        public:
//            Mesh();
//            virtual ~Mesh();
            virtual int AddMarchingVertex(Vec3F location, int hashKey) =0;
            virtual unsigned long long AddMarchingFace(unsigned long long vertexHash0,
                                                       unsigned long long vertexHash1,
                                                       unsigned long long vertexHash2) =0;

    };

} /* namespace Core */
#endif /* SRC_TOOLKIT_VISUALIZATION_MESH_H_ */

