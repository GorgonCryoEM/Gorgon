/*
 * Vertex.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Vertex.h"

namespace Protein_Morph {

    Vertex::Vertex() {
        position = Vec3F(0, 0, 0);
    }

    Vertex::Vertex(Vec3F pos) {
        position = pos;
    }

}

