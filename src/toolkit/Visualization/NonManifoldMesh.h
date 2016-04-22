#ifndef TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H
#define TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/BasicDefines.h>
#include <Core/volume.h>
//#include <string>
#include <GorgonGL.h>
#include <Visualization/Rasterizer.h>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#ifdef _WIN32
	#include <hash_map>
	using namespace stdext;
#endif

#include "Mesh.h"

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

    struct NonManifoldMeshBase {
        unsigned int id;
        bool valid;
    };

    struct NonManifoldMeshEdge : public NonManifoldMeshBase {
        unsigned int vertexIds[2];
        vector<unsigned int> faceIds;
        string tag;
    };

    inline ostream& operator<<(ostream& out, const NonManifoldMeshEdge& obj){
        return out<<"\033[34m"
                  <<"faceIds.size(): "<<obj.faceIds.size()
                  <<endl
                  <<"\033[0m";
    }

    struct NonManifoldMeshFace : public NonManifoldMeshBase {
        vector<unsigned int> edgeIds;
        vector<unsigned int> vertexIds;
        string tag;
    };

    inline ostream& operator<<(ostream& out, const NonManifoldMeshFace& obj){
            return out<<"\033[34m"
                      <<"edgeIds.size(): "<<obj.edgeIds.size()
                      <<"vertexIds.size(): "<<obj.vertexIds.size()
                      <<endl
                      <<"\033[0m";
    }

    struct NonManifoldMeshVertex : public NonManifoldMeshBase {
        Vec3F position;
        vector<unsigned int> edgeIds;
        bool tag;
    };

    inline ostream& operator<<(ostream& out, const NonManifoldMeshVertex& obj){
            return out<<"\033[34m"
                      <<"edgeIds.size(): "<<obj.edgeIds.size()
                      <<endl
                      <<"\033[0m";
    }

    typedef vector<NonManifoldMeshVertex > TV;
    typedef vector<NonManifoldMeshEdge >   TE;
    typedef vector<NonManifoldMeshFace >   TF;

    #ifdef _WIN32
        typedef hash_map<int, int> HashMapType;
    #else
        typedef map<int, int> HashMapType;
    #endif

    class NonManifoldMesh : public Volume, public Mesh {
        public:
            NonManifoldMesh();
            NonManifoldMesh(const Volume & src);

            void clear();
            int addMarchingVertex(Vec3F location, int hashKey);
            unsigned long long addMarchingFace(TriangleMeshFace vertexHash);

            int addVertex(NonManifoldMeshVertex vertex);
            int addVertex(Vec3F location);
            int addHashedVertex(Vec3F location, int hashKey);
            int addFace(NonManifoldMeshFace face);

            int addEdge(NonManifoldMeshEdge edge);

            Vec3F getVertexNormal(int vertexId);
            Vec3F getFaceNormal(int faceId);
            void draw(bool drawSurfaceBorders, bool drawSurfaces, bool drawLines, bool drawPoints,
                      bool annotateSurfaces, bool annotateLines, bool annotatePoints,
                      bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting,
                      int lineThickness, bool smoothSurfaceNormals);

            bool isEdgePresent(int vertexId1, int vertexId2);
            bool isSurfaceVertex(int ix) const;
            int getVertexIndex(int vertexId);
            int getFaceIndex(int faceId);
            int getEdgeIndex(int edgeId) const;
            int getEdgeIndex(int vertexId1, int vertexId2) const;
            int getClosestVertexIndex(Vec3F pos);
            void addEdge(int vertexId1, int vertexId2, string tag = "");
            void addQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4,
                         string newEdgeTag = "", string faceTag ="");
            void addTriangle(TriangleMeshFace vertexId,
                             string newEdgeTag = "", string faceTag = "");
            void mergeMesh(const NonManifoldMesh & srcMesh);
            void removeFace(int faceId);
            void removeEdge(int edgeId);
            void removeVertex(int vertexId);
            void removeNullEntries();
            void toOffCells(string fileName);
            void toMathematicaFile(string fileName);
            void setScale(float x, float y, float z);
            void setScale(Dim3D<float> val);
            vector<unsigned int> getPath(unsigned int edge0Ix, unsigned int edge1Ix);
            vector<unsigned int> getNeighboringVertexIndices(unsigned int vertexIx);
            vector<Vec3F> sampleTriangle(int faceId, double discretizationStep);
            Volume toVolume();
            NonManifoldMesh smoothLaplacian(double converganceRate);
            NonManifoldMesh smoothLaplacian(double converganceRate, int iterations);
            static NonManifoldMesh loadOffFile(string fileName);

    public:
        Dim3D<float> scale;
        TV vertices;
        TE edges;
        TF faces;

        HashMapType vertexHashMap;
        bool fromVolume;

        friend ostream& operator<<(ostream& out, const NonManifoldMesh& obj){
            return out<<"\033[34m"
                      <<"vertices.size(): "<<obj.vertices.size()
                      <<"\nedges.size(): "<<obj.edges.size()
                      <<"\nfaces.size(): "<<obj.faces.size()
                      <<endl
                      <<"\033[0m";
        }
    };
}

#endif
