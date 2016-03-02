/*
 * TriangleMesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_
#define SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_

#include <map>
//#include <vector>
//#include <string>
//#include "OpenGLUtils.h"
#include "TriangleMeshFace.h"
#include "TriangleMeshVertex.h"
//
#include "Mesh.h"

using namespace std;
//using namespace Foundation;
using namespace Core;

namespace Protein_Morph {

    class TriangleMesh : public Mesh {
        public:
            TriangleMesh();
            ~TriangleMesh();

            void clear();
            int addMarchingVertex(Vec3F location, int hashKey);
            unsigned long long addMarchingFace(unsigned long long vertexHash0,
                                       unsigned long long vertexHash1,
                                       unsigned long long vertexHash2);

            unsigned long long addVertex(TriangleMeshVertex vertex,
                                         unsigned long long hashKey);
            unsigned long long addFace(TriangleMeshFace face);
            unsigned long long addFace(unsigned long long vertexHash0,
                                       unsigned long long vertexHash1,
                                       unsigned long long vertexHash2);

            Vec3F getVertexNormal(unsigned long long vertexHash);
            Vec3F getFaceNormal(unsigned long long faceHash);
            void draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);

            void save(string fileName);

        private:
            typedef map<unsigned long long, TriangleMeshVertex > TriangleMeshVertexType;

            TriangleMeshVertexType vertices;
            vector<TriangleMeshFace> faces;

            friend ostream & operator<<(ostream & out, const TriangleMesh & obj) {
                return out
                        <<"TriangleMesh:\n"
                        <<"vertices.size(): "<<obj.vertices.size()<<endl
                        <<"faces.size(): "<<obj.faces.size()
                        <<endl;
            }
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_ */
