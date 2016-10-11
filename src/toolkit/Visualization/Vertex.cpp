/*
 * Vertex.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Vertex.h"

namespace Core {

    Vertex::Vertex()
            : Vec3F(0, 0, 0)
    {}

    Vertex::Vertex(Vec3F pos)
            : Vec3F(pos)
    {}

    ostream& operator<<(ostream& out, const Vertex& obj) {
        return out//<<"\033[34m"
                <<Vec3F(obj);
//                <<IdList(obj)
//                <<endl;
//                      <<"\033[0m";
    }

}

