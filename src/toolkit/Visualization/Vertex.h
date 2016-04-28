/*
 * Vertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_VERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_VERTEX_H_

#include "MathTools/Vector3.h"
#include "Core/Output.h"

using namespace GraphMatch;

namespace Core {

    typedef unsigned long long TKey;

    class Vertex : public Vec3F {
        public:
            Vertex();
            Vertex(Vec3F pos);

            TKey addId(TKey id);
            unsigned int sizeIds() const;
            vector<TKey> getIds() const;
            TKey id(int id) const;

        private:
            vector<TKey> ids;

            friend ostream& operator<<(ostream& out, const Vertex& obj);
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
