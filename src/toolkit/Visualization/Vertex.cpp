/*
 * Vertex.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Vertex.h"

namespace Protein_Morph {

    Vertex::Vertex()
            : Vec3F(0, 0, 0)
    {}

    Vertex::Vertex(Vec3F pos)
            : Vec3F(pos)
    {}

    unsigned long long Vertex::addFace(unsigned long long hash) {
        faces.push_back(hash);
        return hash;
    }

    unsigned int Vertex::sizeFaces() const {
        return faces.size();
    }

    vector<unsigned long long> Vertex::getFaces() const {
        return faces;
    }

}

