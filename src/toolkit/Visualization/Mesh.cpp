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

    CElem Face::getVertices() const {
        return CElem(ids.begin(), ids.end());
    }

    void Face::addVertex(TKey i) {
        ids.push_back(i);
    }

    ostream& operator<<(ostream& out, const Face& obj) {
        set<unsigned int> vertices(obj.ids.begin(), obj.ids.end());
//        set<unsigned int> edges(obj.edgeIds.begin(), obj.edgeIds.end());
        return out //<<"\033[34m"
//                    << "\nedgeIds.size(): " << obj.edgeIds.size() << endl
//                    << edges
                    << "\n\nvertexIds.size(): " << obj.ids.size()
                    << endl
                    << vertices << endl << endl;
//                    <<"\033[0m";
    }

    Mesh::Mesh() {
    }

    Mesh::~Mesh() {
    }

    int Mesh::addMarchingVertex(Vec3F loc, int id) {
        return addVertex(loc, id);
    }

    TKey Mesh::addFace(Vec3U face) {
        TKey faceHash = faces.size();
        faces.push_back(face);
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
        Vec3U face = faces[id];
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
                    indexedVertices[faces[i][2]],
                    indexedVertices[faces[i][1]],
                    indexedVertices[faces[i][0]]);
        }
        fclose(outFile);
    }

} /* namespace Core */
