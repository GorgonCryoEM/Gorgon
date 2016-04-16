/*
 * Vertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_VERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_VERTEX_H_

#include "MathTools/Vector3.h"

using namespace GraphMatch;


using namespace std;

namespace Protein_Morph {
    class Vertex : public Vec3F {
        public:
            Vertex();
            Vertex(Vec3F pos);

            unsigned long long addFaceHash(unsigned long long hash);
            unsigned int sizeFaceHashes() const;
            vector<unsigned long long> getFaceHashes() const;

        private:
            vector<unsigned long long> faceHashes;
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
