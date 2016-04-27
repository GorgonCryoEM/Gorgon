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

    TKey Vertex::addId(TKey id) {
        ids.push_back(id);
        return id;
    }

    unsigned int Vertex::sizeIds() const {
        return ids.size();
    }

    vector<TKey> Vertex::getFaceHashes() const {
        return ids;
    }

    TKey Vertex::edge(int id) const {
        return ids[id];
    }

    TKey Vertex::sizeEdge() const {
        return sizeIds();
    }

    ostream& operator<<(ostream& out, const Vertex& obj) {
        return out//<<"\033[34m"
                <<Vec3F(obj)
                <<"\tedgeIds.size(): "<<obj.sizeIds()
                <<endl<<obj.getFaceHashes()
                <<endl;
//                      <<"\033[0m";
    }

}

