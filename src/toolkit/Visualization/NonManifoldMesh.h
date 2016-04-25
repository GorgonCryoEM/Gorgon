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

    class Edge {
        private:
        vector<TKey> vertexIds;
        vector<TKey> faceIds;

        public:
        Edge();
        Edge(TKey v1, TKey v2);
        vector<TKey> getVertices() const;
        CKey getVerticesSet() const;
        int size() const;
        CKey getFaces() const;
        TKey vertex(int i) const;
        TKey edge(int i) const;
        void addEdge(TKey i);

        friend bool operator<(const Edge &l, const Edge &r);
        friend ostream& operator<<(ostream& out, const Edge& obj);
    };

    typedef set<Edge> CEdge;
    class Face {
        private:
        vector<TKey> edgeIds;
        vector<TKey> vertexIds;

        public:
        CEdge edges;

        void insert(Edge edge);
        CKey getVertices() const;
        int size() const;
        vector<TKey> getVerticesVec() const;
        TKey vertex(int i) const;
        TKey edge(int i) const;
        void addVertex(TKey i);
        void addEdge(TKey i);

        friend ostream& operator<<(ostream& out, const Face& obj);
    };

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

        friend ostream& operator<<(ostream& out, const NonManifoldMesh& obj);
    };
}

#endif
