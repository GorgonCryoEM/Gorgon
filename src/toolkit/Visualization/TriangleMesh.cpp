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
    return addVertex(Vertex(location), hashKey);
}

unsigned long long TriangleMesh::addMarchingFace(TriangleMeshFace face)
{
    unsigned long long faceHash = faces.size();
    faces.push_back(face);
    vertices[face[0]].faceHashes.push_back(faceHash);
    vertices[face[1]].faceHashes.push_back(faceHash);
    vertices[face[2]].faceHashes.push_back(faceHash);
    return faceHash;

}

void TriangleMesh::clear() {
    vertices.clear();
    faces.clear();
}

unsigned long long TriangleMesh::addVertex(Vertex vertex,
                                           unsigned long long hashKey)
{
    vertices[hashKey] = vertex;
    return hashKey;
}

Vec3F TriangleMesh::getVertexNormal(unsigned long long vertexHash) {
    Vec3F normal = Vec3F(0, 0, 0);
    for(unsigned int i = 0; i < vertices[vertexHash].faceHashes.size(); i++) {
        normal += getFaceNormal(vertices[vertexHash].faceHashes[i]);
    }
    normal.normalize();
    return normal;
}

Vec3F TriangleMesh::getFaceNormal(unsigned long long faceHash) {
    TriangleMeshFace face = faces[faceHash];
    Vec3F normal =
            (vertices[face[1]].position - vertices[face[0]].position) ^ (vertices[face[2]].position - vertices[face[0]].position);

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
                    drawTriangle = drawTriangle && (vertices[k].position - center).length() <= radius;
                }
            }
            if(drawTriangle) {
                for(unsigned int j = 0; j < 3; j++) {
                    int k = faces[i][j];
                    Vec3F normal = getVertexNormal(k);
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    glVertex3fv(vertices[k].position.getValues());
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
    for(TriangleMeshVertexType::iterator i = vertices.begin(); i != vertices.end(); ++i) {
        vertexList.push_back(i->second.position);
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
