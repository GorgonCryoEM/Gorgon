/*
 * TriangleMesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "TriangleMesh.h"


#include <map>
//#include <vector>
//#include <string>
//#include "OpenGLUtils.h"
#include "GorgonGL.h"

#include "Mesh.h"

using namespace std;
//using namespace Foundation;
using namespace Core;
using namespace MathTools;
using namespace Protein_Morph;



int TriangleMesh::addMarchingVertex(Vec3F location, int hashKey){
    return addVertex(location, hashKey);
}

unsigned long long TriangleMesh::addMarchingFace(Face face)
{
    unsigned long long faceHash = faces.size();
    faces.push_back(face);
    vertices[face[0]].addFaceHash(faceHash);
    vertices[face[1]].addFaceHash(faceHash);
    vertices[face[2]].addFaceHash(faceHash);
    return faceHash;

}

void TriangleMesh::clear() {
    vertices.clear();
    faces.clear();
}

unsigned long long TriangleMesh::addVertex(Vec3F vertex,
                                           unsigned long long hashKey)
{
    vertices[hashKey] = Vertex(vertex);
    return hashKey;
}

Vec3F TriangleMesh::getVertexNormal(unsigned long long vertexHash) {
    Vertex vertex(vertices[vertexHash]);
    vector<unsigned long long> hashes(vertex.getFaceHashes());

    Vec3F normal = Vec3F(0, 0, 0);

    for(unsigned int i = 0; i < hashes.size(); i++) {
        normal += getFaceNormal(hashes[i]);
    }
    normal.normalize();
    return normal;
}

Vec3F TriangleMesh::getFaceNormal(unsigned long long faceHash) {
    Face face = faces[faceHash];
    Vec3F normal =
            (vertices[face[1]] - vertices[face[0]]) ^ (vertices[face[2]] - vertices[face[0]]);

    normal.normalize();
    return normal;
}

void TriangleMesh::draw(bool drawSurfaces,
                        bool annotateSurfaces,
                        bool fadeExtreme, int radius,
                        Vec3F center)
{
    if(drawSurfaces) {
        if(annotateSurfaces) {
            glPushName(0);
            glPushName(0);
        }
        for(unsigned int i = 0; i < faces.size(); i++) {
            if(annotateSurfaces) {
                glLoadName(i);
            }
            glBegin (GL_POLYGON);
            bool drawTriangle = true;
            if(fadeExtreme) {
                for(unsigned int j = 0; j < 3; j++) {
                    int k = faces[i][j];
                    drawTriangle = drawTriangle && (vertices[k] - center).length() <= radius;
                }
            }
            if(drawTriangle) {
                for(unsigned int j = 0; j < 3; j++) {
                    int k = faces[i][j];
                    Vec3F normal = getVertexNormal(k);
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    glVertex3fv(vertices[k].getValues());
                }
            }
            glEnd();
        }
        if(annotateSurfaces) {
            glPopName();
            glPopName();
        }
    }
    glFlush();
}

void TriangleMesh::save(string fileName) {
    FILE * outFile = fopen(fileName.c_str(), "wt");
    fprintf(outFile, "OFF\n");
    fprintf(outFile, "%d %d %d\n", (int)vertices.size(), (int)faces.size(), 0);

    map<unsigned long long, int> indexedVertices;
    vector<Vec3F> vertexList;

    int index = 0;
    for(TVertex::iterator i = vertices.begin(); i != vertices.end(); ++i) {
        vertexList.push_back(i->second);
        indexedVertices[i->first] = index;
        index++;
    }

    for(int i = 0; i < index; i++) {
        fprintf(outFile, "%f %f %f \n", vertexList[i].X(), vertexList[i].Y(), vertexList[i].Z());
    }

    for(unsigned int i = 0; i < faces.size(); i++) {
        fprintf(outFile, "3 %d %d %d\n",
                indexedVertices[faces[i][2]],
                indexedVertices[faces[i][1]],
                indexedVertices[faces[i][0]]);
    }
    fclose(outFile);
}
