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

    struct Base {
        unsigned int id;
        bool valid;
    };

    struct Edge : public Base {
        TKey vertexIds[2];
        vector<TKey> faceIds;
        string tag;
    };

    inline ostream& operator<<(ostream& out, const Edge& obj){
        set<unsigned int> faces(obj.faceIds.begin(), obj.faceIds.end());
        return out//<<"\033[34m"
                  <<"\tvertexIds: "<<obj.vertexIds[0]<<"\t"<<obj.vertexIds[1]<<endl
                  <<"faceIds.size(): "<<obj.faceIds.size()
                  <<faces
                  <<endl;
//                  <<"\033[0m";
    }

    struct Face : public Base {
        vector<TKey> edgeIds;
        vector<TKey> vertexIds;
        string tag;
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

    struct TVertex : public Base, public Vertex {
        vector<TKey> edgeIds;
        bool tag;
        TVertex() : Vertex() {}
        TVertex(Vec3F pos) : Vertex(pos) {}

        TKey edge(int i) const {
            return edgeIds[i];
        }

    };

    inline ostream& operator<<(ostream& out, const TVertex& obj){
            return out//<<"\033[34m"
                    <<Vec3F(obj)
                    <<"\tedgeIds.size(): "<<obj.edgeIds.size()
                    <<endl<<obj.edgeIds
                    <<endl;
    //                      <<"\033[0m";
    }

    typedef vector<TVertex > TV;
    typedef vector<Edge >   TE;
    typedef vector<Face >   TF;

    typedef map<TKey, TKey> HashMapType;

    class NonManifoldMesh : public Volume, public Mesh {
        public:
            NonManifoldMesh();
            NonManifoldMesh(const Volume & src);

            void clear();
            int addMarchingVertex(Vec3F location, int hashKey);
            TKey addMarchingFace(Vec3U vertexHash);

            int addVertex(TVertex vertex);
            int addVertex(Vec3F location);
            int addFace(Face face);

            int addEdge(Edge edge);

            Vec3F getVertexNormal(int vertexId);
            Vec3F getFaceNormal(int faceId);
            void draw(bool drawSurfaceBorders, bool drawSurfaces, bool drawLines, bool drawPoints,
                      bool annotateSurfaces, bool annotateLines, bool annotatePoints,
                      bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting,
                      int lineThickness, bool smoothSurfaceNormals);

            bool isEdgePresent(int vertexId1, int vertexId2);
            bool isSurfaceVertex(int ix) const;
            int getEdgeIndex(int vertexId1, int vertexId2) const;
            int getClosestVertexIndex(Vec3F pos);
            void addEdge(int vertexId1, int vertexId2, string tag = "");
            void addQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4,
                         string newEdgeTag = "", string faceTag ="");
            void addTriangle(Vec3U vertexId,
                             string newEdgeTag = "", string faceTag = "");
            void mergeMesh(const NonManifoldMesh & srcMesh);
            vector<TKey> getPath(TKey edge0Ix, TKey edge1Ix);
            vector<TKey> getNeighboringVertexIndices(TKey vertexIx);
            vector<Vec3F> sampleTriangle(int faceId, double discretizationStep);
            Volume toVolume();
            NonManifoldMesh smoothLaplacian(double converganceRate);
            NonManifoldMesh smoothLaplacian(double converganceRate, int iterations);
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
