/*
 * Mesh.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_MESH_H_
#define SRC_TOOLKIT_VISUALIZATION_MESH_H_

#include <map>
#include <set>
#include <vector>
//#include <string>
//#include "OpenGLUtils.h"
#include "MathTools/Vector3.h"
#include "Vertex.h"

using namespace std;
//using namespace Foundation;
using namespace GraphMatch;

namespace Core {

    template <class T, class U>
    inline ostream & operator<<(ostream & out, const map<T, U> & obj) {
        for(typename map<T, U>::const_iterator it = obj.begin();
                it != obj.end(); ++it)

                out<<"\t"<<it->first
                    <<"\t"<<it->second
                    <<endl;

        return out;
    }

    template <class T>
    inline ostream & operator<<(ostream & out, const set<T> & obj) {
        for(typename set<T>::const_iterator it = obj.begin(); it != obj.end();
                ++it)

                out<<"\t"<<*it
                    <<endl;

        return out;
    }

    typedef vector<TKey> CElem;

    class Face {
        private:
            CElem ids;

        public:
            CElem getVertices() const;
            void addVertex(TKey i);

            friend ostream& operator<<(ostream& out, const Face& obj);
    };

    typedef map<TKey, Face >   TF;
    typedef Vector3<TKey> Vec3U;

    class Mesh {
        public:
            Mesh();
            virtual ~Mesh();

            virtual int addMarchingVertex(Vec3F loc, int id);
            virtual TKey addFace(Vec3U face);

            void clear();
            TKey addVertex(Vec3F vertex, TKey id);
            Vec3F getVertexNormal(TKey id);
            Vec3F getFaceNormal(TKey faceHash);
            void draw(bool drawSurfaces, bool annotateSurfaces,
                      bool fadeExtreme, int radius, Vec3F center);

            void save(string fileName);

        private:
            typedef map<TKey, Vertex> MUV;

            MUV vertices;
            vector<Vec3U> faces;

            friend ostream & operator<<(ostream & out, const Mesh & obj) {
                return out
                        <<"Mesh:\n"
                        <<"vertices.size(): "<<obj.vertices.size()<<endl
                        <<"faces.size(): "<<obj.faces.size()
                        <<endl;
            }

    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_VISUALIZATION_MESH_H_ */
