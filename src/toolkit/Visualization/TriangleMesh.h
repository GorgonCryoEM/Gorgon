/*
 * TriangleMesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_
#define SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_

//#include <map>
//#include <vector>
//#include <string>
#include "OpenGLUtils.h"
#include "TriangleMeshFace.h"
#include "TriangleMeshVertex.h"

#include "Mesh.h"

using namespace std;
//using namespace Foundation;
using namespace Core;

namespace Protein_Morph {

    class TriangleMesh : public Mesh {
        public:
            TriangleMesh();
            ~TriangleMesh();

            void Clear();
            unsigned long long AddVertex(TriangleMeshVertex vertex,
                                         unsigned long long hashKey);
            int AddMarchingVertex(Vec3F location, int hashKey);
            unsigned long long AddMarchingFace(unsigned long long vertexHash0,
                                       unsigned long long vertexHash1,
                                       unsigned long long vertexHash2);
            unsigned long long AddFace(TriangleMeshFace face);
            unsigned long long AddFace(unsigned long long vertexHash0,
                                       unsigned long long vertexHash1,
                                       unsigned long long vertexHash2);

            Vec3F GetVertexNormal(unsigned long long vertexHash);
            Vec3F GetFaceNormal(unsigned long long faceHash);
            void Draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);
            void SaveFile(string fileName);

        private:
            typedef map<unsigned long long, TriangleMeshVertex > TriangleMeshVertexType;

            TriangleMeshVertexType vertices;
            vector<TriangleMeshFace> faces;
    };

    int TriangleMesh::AddMarchingVertex(Vec3F location, int hashKey){
        return AddVertex(TriangleMeshVertex(location), hashKey);
    }

    unsigned long long TriangleMesh::AddMarchingFace(unsigned long long vertexHash0,
                                                     unsigned long long vertexHash1,
                                                     unsigned long long vertexHash2)
    {
        return AddFace(vertexHash0, vertexHash1, vertexHash2);
    }

    TriangleMesh::TriangleMesh() {
        Clear();
    }

    TriangleMesh::~TriangleMesh() {
        Clear();
    }

    void TriangleMesh::Clear() {
        vertices.clear();
        faces.clear();
    }

    unsigned long long TriangleMesh::AddVertex(TriangleMeshVertex vertex,
                                               unsigned long long hashKey)
    {
        vertices[hashKey] = vertex;
        return hashKey;
    }

    unsigned long long TriangleMesh::AddFace(TriangleMeshFace face) {
        unsigned long long faceHash = faces.size();
        faces.push_back(face);
        vertices[face.vertexHashes[0]].faceHashes.push_back(faceHash);
        vertices[face.vertexHashes[1]].faceHashes.push_back(faceHash);
        vertices[face.vertexHashes[2]].faceHashes.push_back(faceHash);
        return faceHash;
    }

    unsigned long long TriangleMesh::AddFace(unsigned long long vertexHash0,
                                             unsigned long long vertexHash1,
                                             unsigned long long vertexHash2)
    {
        TriangleMeshFace face;
        face.vertexHashes[0] = vertexHash0;
        face.vertexHashes[1] = vertexHash1;
        face.vertexHashes[2] = vertexHash2;
        return AddFace(face);
    }

    Vec3F TriangleMesh::GetVertexNormal(unsigned long long vertexHash) {
        Vec3F normal = Vec3F(0, 0, 0);
        for(unsigned int i = 0; i < vertices[vertexHash].faceHashes.size();
                i++) {
            normal += GetFaceNormal(vertices[vertexHash].faceHashes[i]);
        }
        normal.normalize();
        return normal;
    }

    Vec3F TriangleMesh::GetFaceNormal(unsigned long long faceHash) {
        TriangleMeshFace face = faces[faceHash];
        Vec3F normal =
                (vertices[face.vertexHashes[1]].position - vertices[face.vertexHashes[0]].position) ^ (vertices[face.vertexHashes[2]].position
                        - vertices[face.vertexHashes[0]].position);

        normal.normalize();
        return normal;
    }

    void TriangleMesh::Draw(bool drawSurfaces,
                            bool annotateSurfaces,
                            bool fadeExtreme, int radius,
                            Vec3F center)
    {
        int k;
        if(drawSurfaces) {
            if(annotateSurfaces) {
                glPushName(0);
                glPushName(0);
            }
            float r, g, b, a;
            OpenGLUtils::GetColor(r, g, b, a);
            for(unsigned int i = 0; i < faces.size(); i++) {
                if(annotateSurfaces) {
                    glLoadName(i);
                }
                glBegin (GL_POLYGON);
                Vec3F normal;
                bool drawTriangle = true;
                if(fadeExtreme) {
                    for(unsigned int j = 0; j < 3; j++) {
                        k = faces[i].vertexHashes[j];
                        drawTriangle =
                                drawTriangle && ( (vertices[k].position - center).length()
                                        <= radius);
                    }
                }
                if(drawTriangle) {
                    for(unsigned int j = 0; j < 3; j++) {
                        k = faces[i].vertexHashes[j];
                        normal = GetVertexNormal(k);
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

    void TriangleMesh::SaveFile(string fileName) {
        FILE * outFile = fopen(fileName.c_str(), "wt");
        fprintf(outFile, "OFF\n");
        fprintf(outFile, "%d %d %d\n", (int)vertices.size(), (int)faces.size(),
                0);

        map<unsigned long long, int> indexedVertices;
        vector < Vec3F > vertexList;

        int index = 0;
        for(TriangleMeshVertexType::iterator i = vertices.begin();
                i != vertices.end(); i++) {
            vertexList.push_back(i->second.position);
            indexedVertices[i->first] = index;
            index++;
        }

        for(int i = 0; i < index; i++) {
            fprintf(outFile, "%f %f %f \n", vertexList[i].X(),
                    vertexList[i].Y(), vertexList[i].Z());
        }

        for(unsigned int i = 0; i < faces.size(); i++) {
            fprintf(outFile, "3 %d %d %d\n",
                    indexedVertices[faces[i].vertexHashes[2]],
                    indexedVertices[faces[i].vertexHashes[1]],
                    indexedVertices[faces[i].vertexHashes[0]]);
        }
        fclose(outFile);
    }
}




#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_ */
