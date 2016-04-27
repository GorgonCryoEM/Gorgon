/*
 * Mesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_MESH_H_
#define SRC_TOOLKIT_VISUALIZATION_MESH_H_

#include <map>
//#include <vector>
//#include <string>
//#include "OpenGLUtils.h"
#include "MathTools/Vector3.h"
#include "Vertex.h"

using namespace std;
//using namespace Foundation;
using namespace GraphMatch;

namespace Core {

    typedef Vector3<TKey> Vec3U;

    class Mesh {
        public:
            Mesh();
            virtual ~Mesh();

            virtual int addMarchingVertex(Vec3F location, int hashKey);
            virtual TKey addMarchingFace(Vec3U face);

            void clear();
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

            friend ostream & operator<<(ostream & out, const Mesh & obj) {
                return out
                        <<"Mesh:\n"
                        <<"vertices.size(): "<<obj.vertices.size()<<endl
                        <<"faces.size(): "<<obj.faceHashes.size()
                        <<endl;
            }

    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_VISUALIZATION_MESH_H_ */
