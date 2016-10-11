/*
 * Vertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_VERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_VERTEX_H_

#include "MathTools/Vector3.h"
#include "Core/IdList.h"

using namespace GraphMatch;

namespace Core {

    typedef unsigned long long TKey;

    class Vertex : public Vec3F, public IdList {
        public:
            Vertex();
            Vertex(Vec3F pos);

            friend ostream& operator<<(ostream& out, const Vertex& obj);
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
