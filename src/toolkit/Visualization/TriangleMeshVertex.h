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
    template <class TVertex>
    class TriangleMeshVertex {
        public:
            TriangleMeshVertex();
            TriangleMeshVertex(Vector3Float position);
        public:
            Vector3Float position;
            vector<unsigned long long> faceHashes;
            TVertex tag;
    };

    template<class TVertex>
    TriangleMeshVertex<TVertex>::TriangleMeshVertex() {
        position = Vector3Float(0, 0, 0);
    }

    template<class TVertex>
    TriangleMeshVertex<TVertex>::TriangleMeshVertex(Vector3Float position) {
        this->position = position;
    }
}


#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHVERTEX_H_ */
