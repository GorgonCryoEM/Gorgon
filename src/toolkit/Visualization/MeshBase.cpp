/*
 * MeshBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "MeshBase.h"
#include "GorgonGL.h"

//using namespace std;
////using namespace Foundation;
//using namespace MathTools;
//using namespace Protein_Morph;

namespace Core {
    int MeshBase::addVertex(Vec3F vertex, int id) {
        vertices[id] = Vertex(vertex);
        return id;
    }

    int MeshBase::addFace(IdList face) {
        TKey id = faces.size();
        faces.push_back(face);

        return id;
    }

    TKey MeshBase::addFace(Vec3U vertex) {
        IdList face;
        for (int i = 0; i < 3; ++i) {
            face.addId(vertex[i]);
        }

        TKey id = addFace(face);
        for (int i = 0; i < 3; ++i) {
            vertices[vertex[i]].addId(id);
        }

        return id;
    }

    MUV MeshBase::getVertices() const {
        return vertices;
    }

    void MeshBase::clear() {
        vertices.clear();
        faces.clear();
    }

    Vec3F MeshBase::getVertexNormal(TKey id) {
        Vertex vertex = vertices[id];
        CElem v = vertex.getIds();

        Vec3F normal(0, 0, 0);
        for(CElem::iterator it=v.begin(); it!=v.end(); ++it)
                normal += getFaceNormal(*it);

        normal.normalize();
        return normal;
    }

    Vec3F MeshBase::getFaceNormal(TKey id) {
        IdList face = faces[id];
        CElem v = face.getIds();

        Vec3F normal = (vertices[v[1]] - vertices[v[0]])
                     ^ (vertices[v[2]] - vertices[v[0]]);

        normal.normalize();
        return normal;
    }

    void MeshBase::printVertices() const {
        for(MUV::const_iterator it=vertices.begin(); it!=vertices.end(); ++it)
            cout<<it->second<<endl;
    }

} /* namespace Core */
