#ifndef TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H
#define TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/BasicDefines.h>
#include <Core/volume.h>
//#include <string>
#include <GorgonGL.h>
#include <Visualization/Rasterizer.h>
#include <map>
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
    struct NonManifoldMeshBase {
        unsigned int id;
        bool valid;
    };

    struct NonManifoldMeshEdge : public NonManifoldMeshBase {
        unsigned int vertexIds[2];
        vector<unsigned int> faceIds;
        string tag;
    };

    ostream& operator<<(ostream& out, const NonManifoldMeshEdge& obj){
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
            NonManifoldMesh(Volume * src);
            bool IsEdgePresent(int vertexId1, int vertexId2);
            bool IsSurfaceVertex(int ix) const;
            int AddVertex(NonManifoldMeshVertex vertex);
            int AddVertex(Vec3F location);
            int AddHashedVertex(Vec3F location, int hashKey);
            int AddMarchingVertex(Vec3F location, int hashKey);
            unsigned long long AddMarchingFace(unsigned long long vertexHash0,
                                       unsigned long long vertexHash1,
                                       unsigned long long vertexHash2);
            int AddEdge(NonManifoldMeshEdge edge);
            int AddFace(NonManifoldMeshFace face);
            int GetVertexIndex(int vertexId);
            int GetFaceIndex(int faceId);
            int GetEdgeIndex(int edgeId) const;
            int GetEdgeIndex(int vertexId1, int vertexId2) const;
            int GetClosestVertexIndex(Vec3F pos);
            void AddEdge(int vertexId1, int vertexId2, string tag = "");
            void AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4,
                         string newEdgeTag = "", string faceTag ="");
            void AddTriangle(int vertexId1, int vertexId2, int vertexId3,
                             string newEdgeTag = "", string faceTag = "");
            void MarkFixedVertices();
            void MergeMesh(NonManifoldMesh * srcMesh);
            void draw(bool drawSurfaceBorders, bool drawSurfaces, bool drawLines, bool drawPoints,
                      bool annotateSurfaces, bool annotateLines, bool annotatePoints,
                      bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting,
                      int lineThickness, bool smoothSurfaceNormals);
            void RemoveFace(int faceId);
            void RemoveEdge(int edgeId);
            void RemoveVertex(int vertexId);
            void RemoveNullEntries();
            void ToOffCells(string fileName);
            void ToMathematicaFile(string fileName);
            void setScale(float x, float y, float z);
            void setScale(Dim3D<float> val);
            void TranslateVertex(int vertexIx, Vec3F translateVector);
            vector<unsigned int> GetPath(unsigned int edge0Ix, unsigned int edge1Ix);
            vector<unsigned int> GetNeighboringVertexIndices(unsigned int vertexIx);
            vector<Vec3F> SampleTriangle(int faceId, double discretizationStep);
            Volume * ToVolume();
            Vec3F GetVertexNormal(int vertexId);
            Vec3F GetFaceNormal(int faceId);
            NonManifoldMesh SmoothLaplacian(double converganceRate);
            NonManifoldMesh SmoothLaplacian(double converganceRate, int iterations);
            static NonManifoldMesh * LoadOffFile(string fileName);
            void Clear();

    public:
        Dim3D<float> scale;
        TV vertices;
        TE edges;
        TF faces;
        int edgeCount;
        int vertexCount;
        int faceCount;
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
