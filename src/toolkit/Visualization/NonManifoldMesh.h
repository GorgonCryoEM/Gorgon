#ifndef TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H
#define TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
//#include <string>
#include <map>
#include <set>
#include <queue>
#include <fstream>

#include "Mesh.h"

#include "MathTools/BasicDefines.h"
#include "Core/volume.h"
#include "GorgonGL.h"
#include "Visualization/Rasterizer.h"

using namespace std;
using namespace MathTools;
using namespace Foundation;
using namespace SkeletonMaker;
using namespace Core;


namespace Protein_Morph {

    template <class T, class U>
    inline ostream & operator<<(ostream & out, const map<T, U> & obj) {
            for(typename map<T, U>::const_iterator it=obj.begin();
                    it!=obj.end();
                    ++it)

                out<<"\t"<<it->first
                <<"\t"<<it->second
                <<endl;

            return out;
    }

    template <class T>
    inline ostream & operator<<(ostream & out, const set<T> & obj) {
            for(typename set<T>::const_iterator it=obj.begin();
                    it!=obj.end();
                    ++it)

                out<<"\t"<<*it
                <<endl;

            return out;
    }

    template <class T>
    inline ostream & operator<<(ostream & out, const vector<T> & obj) {
            for(typename vector<T>::const_iterator it=obj.begin();
                    it!=obj.end();
                    ++it)

                out<<"\t\t\t"<<*it
                <<endl;

            return out;
    }

    typedef set<TKey> CKey;

    struct Edge {
        private:
        vector<TKey> vertexIds;
        vector<TKey> faceIds;

        public:
        Edge() : vertexIds(2) {}

        Edge(TKey v1, TKey v2) : vertexIds(2) {
            vertexIds[0] = v1;
            vertexIds[1] = v2;
        }

        vector<TKey> getVertices() const {
            return vertexIds;
        }

        CKey getVerticesSet() const {
            return CKey(vertexIds.begin(), vertexIds.end());
        }

        int size() const {
            return faceIds.size();
        }

        CKey getFaces() const {
            return CKey(faceIds.begin(), faceIds.end());
        }

        TKey vertex(int i) const {
            return vertexIds[i];
        }

        TKey edge(int i) const {
            return faceIds[i];
        }

        void addEdge(TKey i) {
            faceIds.push_back(i);
        }

        friend bool operator<(const Edge &l, const Edge &r);
        friend ostream& operator<<(ostream& out, const Edge& obj);
    };

    inline bool operator<(const Edge &l, const Edge &r) {
        vector<TKey> ll = l.vertexIds;
        vector<TKey> rr = r.vertexIds;

        if(ll.size() != 2 && rr.size() != 2)
            throw "Edges have inconsistent size!";

        sort(ll.begin(), ll.end());
        sort(rr.begin(), rr.end());

        return ll[0]<rr[0] && ll[1]<rr[1];
    }

    inline ostream& operator<<(ostream& out, const Edge& obj){
        set<unsigned int> faces(obj.faceIds.begin(), obj.faceIds.end());
        return out//<<"\033[34m"
                  <<"\tvertexIds: "<<obj.vertexIds[0]<<"\t"<<obj.vertexIds[1]<<endl
                  <<"faceIds.size(): "<<obj.faceIds.size()
                  <<faces
                  <<endl;
//                  <<"\033[0m";
    }

    typedef set<Edge> CEdge;
    struct Face {
        vector<TKey> edgeIds;
        vector<TKey> vertexIds;
        CEdge edges;

        void insert(Edge edge) {
            edges.insert(edge);
        }

        CKey getVertices() const {
            CKey result;
            for(CEdge::const_iterator it=edges.begin(); it!=edges.end(); ++it) {
                CKey vertices = it->getVerticesSet();
                result.insert(vertices.begin(), vertices.end());
            }

            return result;
        }

        int size() const {
            return edgeIds.size();
        }
    };

    inline ostream& operator<<(ostream& out, const Face& obj){
        set<unsigned int> vertices(obj.vertexIds.begin(), obj.vertexIds.end());
        set<unsigned int> edges(obj.edgeIds.begin(), obj.edgeIds.end());
            return out//<<"\033[34m"
                      <<"\nedgeIds.size(): "<<obj.edgeIds.size()
                      <<endl<<edges
                      <<"\n\nvertexIds.size(): "<<obj.vertexIds.size()
                      <<endl<<vertices
                      <<endl<<endl;
//                      <<"\033[0m";
    }

    typedef vector<Vertex > TV;
    typedef vector<Edge >   TE;
    typedef vector<Face >   TF;

    typedef map<TKey, TKey> HashMapType;

    class NonManifoldMesh : public Volume, public Mesh {
        public:
            NonManifoldMesh();
            NonManifoldMesh(const Volume & src);
            void clear();
            void draw(bool drawSurfaceBorders, bool drawSurfaces, bool drawLines, bool drawPoints,
                      bool annotateSurfaces, bool annotateLines, bool annotatePoints,
                      bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting,
                      int lineThickness, bool smoothSurfaceNormals);

            int addMarchingVertex(Vec3F location, int hashKey);
            TKey addMarchingFace(Vec3U vertexHash);

            int addVertex(Vec3F location);
            int addFace(Face face);
            int addEdge(Edge edge);
            Edge addEdge(int vertexId1, int vertexId2);
            int getEdgeIndex(int vertexId1, int vertexId2) const;

            Vec3F getVertexNormal(int vertexId);
            Vec3F getFaceNormal(int faceId);

            TKey addTriangle(Vec3U vertexId);
            void addQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4);
            void mergeMesh(const NonManifoldMesh & srcMesh);
            bool isEdgePresent(int vertexId1, int vertexId2);
            bool isSurfaceVertex(int ix) const;
            int getClosestVertexIndex(Vec3F pos);
            vector<TKey> getPath(TKey edge0Ix, TKey edge1Ix);
            vector<TKey> getNeighboringVertexIndices(TKey vertexIx);
            vector<Vec3F> sampleTriangle(int faceId, double discretizationStep);

            Volume toVolume();
            static NonManifoldMesh loadOffFile(string fileName);

    public:
        TV vertices;
        TE edges;
        TF faces;

        HashMapType vertexHashMap;
        bool fromVolume;

        friend ostream& operator<<(ostream& out, const NonManifoldMesh& obj){
            return out//<<"\033[34m"
                      <<"vertices.size(): "<<obj.vertices.size()
                      <<"\nedges.size(): "<<obj.edges.size()
                      <<"\nfaces.size(): "<<obj.faces.size()
                      <<endl;
//                      <<"\033[0m";
        }
    };
}

#endif
