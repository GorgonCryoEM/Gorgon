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

namespace Core {

    typedef unsigned long long TKey;

    class Vertex : public Vec3F {
        public:
            Vertex();
            Vertex(Vec3F pos);

            TKey addFaceHash(TKey hash);
            unsigned int sizeFaceHashes() const;
            vector<TKey> getFaceHashes() const;

        private:
            vector<TKey> faceHashes;
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
