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
#include "MathTools/Vector3.h"
#include "TriangleMeshVertex.h"
//
#include "Mesh.h"

using namespace std;
//using namespace Foundation;
using namespace Core;
using namespace GraphMatch;

namespace Protein_Morph {

    class TriangleMesh : public Mesh {
        public:
            void clear();
            int addMarchingVertex(Vec3F location, int hashKey);
            unsigned long long addMarchingFace(TriangleMeshFace vertexHash);

            unsigned long long addVertex(TriangleMeshVertex vertex,
                                         unsigned long long hashKey);

            Vec3F getVertexNormal(unsigned long long vertexHash);
            Vec3F getFaceNormal(unsigned long long faceHash);
            void draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);

            void save(string fileName);

        private:
            typedef map<unsigned long long, TriangleMeshVertex> TriangleMeshVertexType;

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
