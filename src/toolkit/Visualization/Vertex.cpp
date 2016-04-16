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

    unsigned long long Vertex::addFaceHash(unsigned long long hash) {
        faceHashes.push_back(hash);
        return hash;
    }

    unsigned int Vertex::sizeFaceHashes() const {
        return faceHashes.size();
    }

    vector<unsigned long long> Vertex::getFaceHashes() const {
        return faceHashes;
    }

}

