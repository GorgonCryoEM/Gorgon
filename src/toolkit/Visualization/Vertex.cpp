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

    TKey Vertex::addFaceHash(TKey hash) {
        faceHashes.push_back(hash);
        return hash;
    }

    unsigned int Vertex::sizeFaceHashes() const {
        return faceHashes.size();
    }

    vector<TKey> Vertex::getFaceHashes() const {
        return faceHashes;
    }

    TKey Vertex::edge(int i) const {
        return faceHashes[i];
    }

}

