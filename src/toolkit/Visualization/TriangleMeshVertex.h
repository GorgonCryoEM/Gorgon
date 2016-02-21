/*
 * TriangleMeshVertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_


using namespace std;

namespace Protein_Morph {
    class TriangleMeshVertex {
        public:
            TriangleMeshVertex();
            TriangleMeshVertex(Vector3Float position);
        public:
            Vector3Float position;
            vector<unsigned long long> faceHashes;
            bool tag;
    };

    TriangleMeshVertex::TriangleMeshVertex() {
        position = Vector3Float(0, 0, 0);
    }

    TriangleMeshVertex::TriangleMeshVertex(Vector3Float position) {
        this->position = position;
    }
}


#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_ */
