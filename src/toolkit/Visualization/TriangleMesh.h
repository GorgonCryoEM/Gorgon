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
#include "Vertex.h"
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
            TKey addMarchingFace(Vec3U vertexHash);
            TKey addVertex(Vec3F vertex, TKey hashKey);
            Vec3F getVertexNormal(TKey vertexHash);
            Vec3F getFaceNormal(TKey faceHash);
            void draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);

            void save(string fileName);

        private:
            typedef map<TKey, Vertex> MUV;

            MUV vertices;
            vector<Vec3U> faceHashes;

            friend ostream & operator<<(ostream & out, const TriangleMesh & obj) {
                return out
                        <<"TriangleMesh:\n"
                        <<"vertices.size(): "<<obj.vertices.size()<<endl
                        <<"faces.size(): "<<obj.faceHashes.size()
                        <<endl;
            }
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_TRIANGLEMESH_H_ */
