/*
 * TriangleMeshFace.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHFACE_H_
#define SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHFACE_H_

using namespace std;

namespace Protein_Morph {
    class TriangleMeshFace {
        public:
            unsigned long long vertexHashes[3];
    };
}

#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESHFACE_H_ */
