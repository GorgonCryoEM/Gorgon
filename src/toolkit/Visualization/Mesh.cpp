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

    int Mesh::addMarchingVertex(Vec3F loc, int id) {
        return addVertex(loc, id);
    }

    int Mesh::addFace(IdList face) {
        TKey id = faces.size();
        faces.push_back(face);

        return id;
    }

    TKey Mesh::addFace(Vec3U face) {
        TKey faceHash = addFace(face);
        for (int i = 0; i < 3; ++i) {
            vertices[face[i]].addId(faceHash);
        }

        return faceHash;
    }

    void Mesh::clear() {
        vertices.clear();
        faces.clear();
    }

    TKey Mesh::addVertex(Vec3F vertex, TKey id) {
        vertices[id] = Vertex(vertex);
        return id;
    }

    Vec3F Mesh::getVertexNormal(TKey id) {
        Vertex vertex(vertices[id]);
        vector<TKey> hashes(vertex.getIds());

        Vec3F normal = Vec3F(0, 0, 0);

        for(unsigned int i = 0; i < hashes.size(); i++) {
            normal += getFaceNormal(hashes[i]);
        }
        normal.normalize();
        return normal;
    }

    Vec3F Mesh::getFaceNormal(TKey id) {
        IdList face = faces[id];
        CElem v = face.getIds();

        Vec3F normal =
                (vertices[v[1]] - vertices[v[0]]) ^ (vertices[v[2]] - vertices[v[0]]);

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
            for(TF::iterator it=faces.begin(); it!=faces.end(); ++it) {
                int i = distance(faces.begin(), it);
                if(annotateSurfaces) {
                    glLoadName(i);
                }
                glBegin (GL_POLYGON);
                bool drawTriangle = true;
                CElem v = it->getIds();
                if(fadeExtreme) {
                    for(CElem::iterator jt=v.begin(); jt!=v.end(); ++jt) {
                        int k = (*jt);
                        drawTriangle = drawTriangle && (vertices[k] - center).length() <= radius;
                    }
                }
                if(drawTriangle) {
                    for(CElem::iterator jt=v.begin(); jt!=v.end(); ++jt) {
                        int k = (*jt);
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
        fprintf(outFile, "%d %d %d\n", (int)vertices.size(), (int)faces.size(), 0);

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

        for(unsigned int i = 0; i < faces.size(); i++) {
            fprintf(outFile, "3 %d %d %d\n",
                    indexedVertices[faces[i].id(2)],
                    indexedVertices[faces[i].id(1)],
                    indexedVertices[faces[i].id(0)]);
        }
        fclose(outFile);
    }

} /* namespace Core */
