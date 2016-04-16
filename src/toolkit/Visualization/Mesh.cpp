/*
 * Mesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Mesh.h"
#include "GorgonGL.h"

//using namespace std;
////using namespace Foundation;
//using namespace MathTools;
//using namespace Protein_Morph;

namespace Core {

    Mesh::Mesh() {
    }

    Mesh::~Mesh() {
    }

    int Mesh::addMarchingVertex(Vec3F location, int hashKey) {
    }

    TKey Mesh::addMarchingFace(Vec3U face) {
    }

    void Mesh::clear() {
    }

    TKey Mesh::addVertex(Vec3F vertex, TKey hashKey) {
    }

    Vec3F Mesh::getVertexNormal(TKey vertexHash) {
    }

    Vec3F Mesh::getFaceNormal(TKey faceHash) {
    }

    void Mesh::draw(bool drawSurfaces,
                    bool annotateSurfaces,
                    bool fadeExtreme, int radius,
                    Vec3F center)
    {
    }

    void Mesh::save(string fileName) {
    }

} /* namespace Core */
