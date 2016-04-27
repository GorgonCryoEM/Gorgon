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

    int Mesh::addMarchingVertex(Vec3F location) {
        return addVertex(location);
    }

    TKey Mesh::addMarchingFace(Vec3U face) {
        TKey faceHash = faceHashes.size();
        faceHashes.push_back(face);
        for (int i = 0; i < 3; ++i) {
            vertices[face[i]].addFaceHash(faceHash);
        }

        return faceHash;
    }

    void Mesh::clear() {
        vertices.clear();
        faceHashes.clear();
    }

    TKey Mesh::addVertex(Vec3F vertex) {
        TKey hashKey = vertices.size();
        vertices[hashKey] = Vertex(vertex);
        return hashKey;
    }

    Vec3F Mesh::getVertexNormal(TKey vertexHash) {
        Vertex vertex(vertices[vertexHash]);
        vector<TKey> hashes(vertex.getFaceHashes());

        Vec3F normal = Vec3F(0, 0, 0);

        for(unsigned int i = 0; i < hashes.size(); i++) {
            normal += getFaceNormal(hashes[i]);
        }
        normal.normalize();
        return normal;
    }

    Vec3F Mesh::getFaceNormal(TKey faceHash) {
        Vec3U face = faceHashes[faceHash];
        Vec3F normal =
                (vertices[face[1]] - vertices[face[0]]) ^ (vertices[face[2]] - vertices[face[0]]);

        normal.normalize();
        return normal;
    }

    void Mesh::draw(bool drawSurfaces,
                    bool annotateSurfaces,
                    bool fadeExtreme, int radius,
                    Vec3F center)
    {
        if(drawSurfaces) {
            if(annotateSurfaces) {
                glPushName(0);
                glPushName(0);
            }
            for(unsigned int i = 0; i < faceHashes.size(); i++) {
                if(annotateSurfaces) {
                    glLoadName(i);
                }
                glBegin (GL_POLYGON);
                bool drawTriangle = true;
                if(fadeExtreme) {
                    for(unsigned int j = 0; j < 3; j++) {
                        int k = faceHashes[i][j];
                        drawTriangle = drawTriangle && (vertices[k] - center).length() <= radius;
                    }
                }
                if(drawTriangle) {
                    for(unsigned int j = 0; j < 3; j++) {
                        int k = faceHashes[i][j];
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

    void Mesh::save(string fileName) {
        FILE * outFile = fopen(fileName.c_str(), "wt");
        fprintf(outFile, "OFF\n");
        fprintf(outFile, "%d %d %d\n", (int)vertices.size(), (int)faceHashes.size(), 0);

        map<TKey, int> indexedVertices;
        vector<Vec3F> vertexList;

        int index = 0;
        for(MUV::iterator i = vertices.begin(); i != vertices.end(); ++i) {
            vertexList.push_back(i->second);
            indexedVertices[i->first] = index;
            index++;
        }

        for(int i = 0; i < index; i++) {
            fprintf(outFile, "%f %f %f \n", vertexList[i].X(), vertexList[i].Y(), vertexList[i].Z());
        }

        for(unsigned int i = 0; i < faceHashes.size(); i++) {
            fprintf(outFile, "3 %d %d %d\n",
                    indexedVertices[faceHashes[i][2]],
                    indexedVertices[faceHashes[i][1]],
                    indexedVertices[faceHashes[i][0]]);
        }
        fclose(outFile);
    }

} /* namespace Core */
