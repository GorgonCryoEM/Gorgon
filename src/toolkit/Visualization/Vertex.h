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
            Vertex()
                : Vec3F()
                    {}
            Vertex(Vec3F loc)
                : Vec3F(loc)
        {}
            vector<unsigned long long> faces;
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
