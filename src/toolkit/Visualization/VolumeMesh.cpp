/*
 * VolumeMesh.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "VolumeMesh.h"
#include "GorgonGL.h"

namespace Core {

    void VolumeMesh::draw(bool drawSurfaces,
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

    void VolumeMesh::save(string fileName) {
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
