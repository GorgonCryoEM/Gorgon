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
    class Vertex {
        public:
            Vertex();
            Vertex(Vec3F pos);
        public:
            Vec3F position;
            vector<unsigned long long> faces;
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
