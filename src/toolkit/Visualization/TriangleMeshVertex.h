/*
 * TriangleMeshVertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_

#include "MathTools/Vector3.h"

using namespace GraphMatch;


using namespace std;

namespace Protein_Morph {
    class TriangleMeshVertex {
        public:
            TriangleMeshVertex();
            TriangleMeshVertex(Vec3F position);
        public:
            Vec3F position;
            vector<unsigned long long> faceHashes;
            bool tag;
    };

    inline TriangleMeshVertex::TriangleMeshVertex() {
        position = Vec3F(0, 0, 0);
    }

    inline TriangleMeshVertex::TriangleMeshVertex(Vec3F position) {
        this->position = position;
    }
}


#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_ */
