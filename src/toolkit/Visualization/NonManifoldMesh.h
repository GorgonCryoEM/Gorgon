#ifndef TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H
#define TOOLKIT_PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/BasicDefines.h>
#include <Core/volume.h>
//#include <string>
//#include <GorgonGL.h>
#include <Visualization/Rasterizer.h>
#include <map>
#include <queue>
#include <fstream>
#ifdef _WIN32
	#include <hash_map>
	using namespace stdext;
#endif

using namespace std;
using namespace MathTools;
using namespace Foundation;
using namespace SkeletonMaker;


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
    ostream& operator<<(ostream& out, const NonManifoldMeshFace& obj){
            return out<<"\033[34m"
                      <<"edgeIds.size(): "<<obj.edgeIds.size()
                      <<"vertexIds.size(): "<<obj.vertexIds.size()
                      <<endl
                      <<"\033[0m";
    }

    struct NonManifoldMeshVertex : public NonManifoldMeshBase {
        Vector3Float position;
        vector<unsigned int> edgeIds;
        bool tag;
    };
    ostream& operator<<(ostream& out, const NonManifoldMeshVertex& obj){
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

    class NonManifoldMesh : public Volume {
    public:
        NonManifoldMesh();
        NonManifoldMesh(Volume * sourceVol);
        bool IsEdgePresent(int vertexId1, int vertexId2);
        bool IsSurfaceVertex(int ix);
        int AddVertex(NonManifoldMeshVertex vertex);
        int AddVertex(Vector3Float location);
        int AddHashedVertex(Vector3Float location, int hashKey);
        int AddEdge(NonManifoldMeshEdge edge);
        int AddFace(NonManifoldMeshFace face);
        int GetVertexIndex(int vertexId);
        int GetFaceIndex(int faceId);
        int GetEdgeIndex(int edgeId);
        int GetEdgeIndex(int vertexId1, int vertexId2);
        int GetClosestVertexIndex(Vector3Float pos);
        void AddEdge(int vertexId1, int vertexId2, string tag = "");
        void AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, string newEdgeTag = "", string faceTag = "");
        void AddTriangle(int vertexId1, int vertexId2, int vertexId3, string newEdgeTag = "", string faceTag = "");
        void MarkFixedVertices();
        void MergeMesh(NonManifoldMesh * srcMesh);
        void RemoveFace(int faceId);
        void RemoveEdge(int edgeId);
        void RemoveVertex(int vertexId);
        void RemoveNullEntries();
        void ToOffCells(string fileName);
        void ToMathematicaFile(string fileName);
        void setScale(float x, float y, float z);
        void setScale(Dim3D<float> val);
        void TranslateVertex(int vertexIx, Vector3Float translateVector);
        vector<unsigned int> GetPath(unsigned int edge0Ix, unsigned int edge1Ix);
        vector<unsigned int> GetNeighboringVertexIndices(unsigned int vertexIx);
        vector<Vector3Float> SampleTriangle(int faceId, double discretizationStep);
        Volume * ToVolume();
        Vector3Float GetVertexNormal(int vertexId);
        Vector3Float GetFaceNormal(int faceId);
        NonManifoldMesh SmoothLaplacian(double converganceRate);
        NonManifoldMesh SmoothLaplacian(double converganceRate, int iterations);
        static NonManifoldMesh * LoadOffFile(string fileName);


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



    NonManifoldMesh::NonManifoldMesh()
        : scale(1,1,1),
          fromVolume(false)
    {
      setOrigin(0,0,0);
    }

    NonManifoldMesh::NonManifoldMesh(Volume * sourceVol) {
      #ifdef GORGON_DEBUG
            cout<<"\033[33mDEBUG: File:   NonManifoldMesh.h"<<endl;
            cout<<"DEBUG: Method: NonManifoldMesh::NonManifoldMesh\033[0m"<<endl;
            cout<<"DEBUG: Args: Volume*\033[0m"<<endl;
            cout<<"sourceVol->getSize(): "<<sourceVol->getSize()<<endl;
      #endif

        int x, y, z, i, j, index, index2;
        int * vertexLocations = new int[sourceVol->getMaxIndex()];
        int value;
        fromVolume = true;
        size = sourceVol->getSizeObj();
        setOrigin(sourceVol->getOriginX(), sourceVol->getOriginY(), sourceVol->getOriginZ());
        setScale(sourceVol->getSpacingX(), sourceVol->getSpacingY(), sourceVol->getSpacingZ());

        // Adding vertices
        NonManifoldMeshVertex tempVertex;
        tempVertex.edgeIds.clear();
        for(x = 0; x < sourceVol->getSizeX(); x++) {
            for(y = 0; y < sourceVol->getSizeY(); y++) {
                for(z = 0; z < sourceVol->getSizeZ(); z++) {
                    index = sourceVol->getIndex(x, y, z);
                    vertexLocations[index] = -1;
                    value = (int)round(sourceVol->getDataAt(index));
                    if(value > 0) {
                        tempVertex.position = Vector3Float(x, y, z);
                        vertexLocations[index] = AddVertex(tempVertex);
                    }
                }
            }
        }

        //Adding edges
        int edgeNeighbors[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
        for(x = 0; x < sourceVol->getSizeX()-1; x++) {
            for(y = 0; y < sourceVol->getSizeY()-1; y++) {
                for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
                    index = sourceVol->getIndex(x, y, z);
                    for(i = 0; i < 3; i++) {
                        index2 = sourceVol->getIndex(x+edgeNeighbors[i][0], y+edgeNeighbors[i][1], z+edgeNeighbors[i][2]);
                        if((vertexLocations[index] >= 0) && (vertexLocations[index2] >= 0)) {
                            AddEdge(vertexLocations[index], vertexLocations[index2]);
                        }
                    }
                }
            }
        }

        //Adding Faces
        int faceNeighbors[3][3][3] = {  {{1,0,0}, {1,0,1}, {0,0,1}},
                                        {{1,0,0}, {1,1,0}, {0,1,0}},
                                        {{0,1,0}, {0,1,1}, {0,0,1}}};
        int indices[4];
        bool faceFound;
        for(x = 0; x < sourceVol->getSizeX()-1; x++) {
            for(y = 0; y < sourceVol->getSizeY()-1; y++) {
                for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
                    index = sourceVol->getIndex(x, y, z);
                    if(vertexLocations[index] >= 0) {
                        for(i = 0; i < 3; i++) {
                            faceFound = true;
                            indices[0] = vertexLocations[index];
                            for(j = 0; j < 3; j++) {
                                index2 = sourceVol->getIndex(x+faceNeighbors[i][j][0], y+faceNeighbors[i][j][1], z+faceNeighbors[i][j][2]);
                                indices[j+1] = vertexLocations[index2];
                                faceFound = faceFound && vertexLocations[index2] >= 0;
                            }
                            if(faceFound) {
                                AddQuad(indices[0], indices[1], indices[2], indices[3]);
                            }
                        }
                    }
                }
            }
        }
        delete [] vertexLocations;
        MarkFixedVertices();
#ifdef GORGON_DEBUG
      cout<<"\033[33mDEBUG: END"<<endl;
      cout<<"DEBUG: Method: NonManifoldMesh::NonManifoldMesh\n\033[0m"<<endl;
#endif

    }

    bool NonManifoldMesh::IsEdgePresent(int vertexId1, int vertexId2) {
        bool isPresent = false;
        int v1Index = GetVertexIndex(vertexId1);
        int v2Index = GetVertexIndex(vertexId2);
        for(unsigned int i = 0; (i < vertices[v1Index].edgeIds.size()) && !isPresent; i++) {
            isPresent = ((int)edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[0] == v2Index) || ((int)edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[1] == v2Index);
        }
        return isPresent;

    }
    int NonManifoldMesh::AddVertex(NonManifoldMeshVertex vertex) {
        vertex.id = vertices.size();
        vertex.valid = true;
        vertices.push_back(vertex);
        vertexCount++;
        return vertex.id;
    }

    int NonManifoldMesh::AddVertex(Vector3Float location) {
        NonManifoldMeshVertex v;
        v.position = location;
//        v.tag = tag;
        return AddVertex(v);
    }

    int NonManifoldMesh::AddHashedVertex(Vector3Float location, int hashKey) {
        HashMapType::const_iterator pos = vertexHashMap.find(hashKey);
        int vertexId;
        if(pos == vertexHashMap.end()) {
            vertexId = AddVertex(location);
            vertexHashMap[hashKey] = vertexId;
        } else {
            vertexId = pos->second;
        }
        return vertexId;
    }

    int NonManifoldMesh::AddEdge(NonManifoldMeshEdge edge) {
        edge.id = edges.size();
        edge.valid = true;
        edges.push_back(edge);
        edgeCount++;
        return edge.id;
    }

    int NonManifoldMesh::AddFace(NonManifoldMeshFace face) {
        face.id = faces.size();
        face.valid = true;
        faces.push_back(face);
        faceCount++;
        return face.id;
    }

    int NonManifoldMesh::GetVertexIndex(int vertexId) {
        return vertexId;
    }

    int NonManifoldMesh::GetFaceIndex(int faceId) {
        return faceId;
    }

    int NonManifoldMesh::GetEdgeIndex(int edgeId) {
        return edgeId;
    }

    int NonManifoldMesh::GetEdgeIndex(int vertexId1, int vertexId2) {
        int edgeId = -1;
        for(int i = 0; i < vertices[vertexId1].edgeIds.size(); i++) {
            if((edges[vertices[vertexId1].edgeIds[i]].vertexIds[0] == vertexId2) ||
                (edges[vertices[vertexId1].edgeIds[i]].vertexIds[1] == vertexId2)) {
                edgeId = vertices[vertexId1].edgeIds[i];
            }
        }
        return edgeId;
    }

    void NonManifoldMesh::AddEdge(int vertexId1, int vertexId2, string tag){
        NonManifoldMeshEdge edge;
        edge.tag = tag;
        edge.faceIds.clear();
        edge.vertexIds[0] = vertexId1;
        edge.vertexIds[1] = vertexId2;
        int edgeId = AddEdge(edge);
        vertices[GetVertexIndex(vertexId1)].edgeIds.push_back(edgeId);
        vertices[GetVertexIndex(vertexId2)].edgeIds.push_back(edgeId);
    }

    void NonManifoldMesh::AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, string newEdgeTag, string faceTag) {
        AddTriangle(vertexId1, vertexId2, vertexId3, newEdgeTag, faceTag);
        AddTriangle(vertexId1, vertexId3, vertexId4, newEdgeTag, faceTag);
    }

    void NonManifoldMesh::AddTriangle(int vertexId1, int vertexId2, int vertexId3, string newEdgeTag, string faceTag) {
        if(!IsEdgePresent(vertexId1, vertexId2)) {
                AddEdge(vertexId1, vertexId2, newEdgeTag);
        }
        if(!IsEdgePresent(vertexId2, vertexId3)) {
                AddEdge(vertexId2, vertexId3, newEdgeTag);
        }
        if(!IsEdgePresent(vertexId3, vertexId1)) {
                AddEdge(vertexId3, vertexId1, newEdgeTag);
        }

        NonManifoldMeshFace face;
        face.tag = faceTag;
        face.vertexIds.clear();
        face.vertexIds.push_back(vertexId1);
        face.vertexIds.push_back(vertexId2);
        face.vertexIds.push_back(vertexId3);
        face.edgeIds.clear();

        int vertexIds[4] = {vertexId1, vertexId2, vertexId3, vertexId1};
        int i,j, edgeIndex, vertexIndex;

        for(i = 0; i < 3; i++) {
            vertexIndex = GetVertexIndex(vertexIds[i]);
            for (j = 0; j < (int)vertices[vertexIndex].edgeIds.size(); j++) {
                edgeIndex = GetEdgeIndex(vertices[vertexIndex].edgeIds[j]);

                if( (((int)edges[edgeIndex].vertexIds[0] == vertexIds[i])   && ((int)edges[edgeIndex].vertexIds[1] == vertexIds[i+1])) ||
                    (((int)edges[edgeIndex].vertexIds[0] == vertexIds[i+1]) && ((int)edges[edgeIndex].vertexIds[1] == vertexIds[i]))) {
                    face.edgeIds.push_back(vertices[vertexIndex].edgeIds[j]);
                }
            }
        }
        int faceId = AddFace(face);
        for(i = 0; i < (int)face.edgeIds.size(); i++) {
            edges[GetEdgeIndex(face.edgeIds[i])].faceIds.push_back(faceId);
        }
    }

    void NonManifoldMesh::MarkFixedVertices() {
        bool sheetFound;
        bool edgeFound;

        for(unsigned int i = 0; i < vertices.size(); i++) {
            if(vertices[i].valid) {
                sheetFound = false;
                edgeFound = false;
                for(unsigned int j = 0; j < vertices[i].edgeIds.size(); j++) {
                    sheetFound = sheetFound || (edges[GetEdgeIndex(vertices[i].edgeIds[j])].faceIds.size() > 0);
                    edgeFound = edgeFound || (edges[GetEdgeIndex(vertices[i].edgeIds[j])].faceIds.size() == 0);
                }
            }
        }
    }

    void NonManifoldMesh::MergeMesh(NonManifoldMesh * srcMesh) {
        vector<int> indices;
        indices.clear();
        for(unsigned int i = 0; i < srcMesh->vertices.size(); i++) {
            indices.push_back(AddVertex(srcMesh->vertices[i]));
        }
        for(unsigned int i = 0; i < srcMesh->edges.size(); i++) {
            AddEdge(indices[srcMesh->edges[i].vertexIds[0]], indices[srcMesh->edges[i].vertexIds[1]], srcMesh->edges[i].tag);
        }
        for(unsigned int i = 0; i < srcMesh->faces.size(); i++) {
            if(srcMesh->faces[i].vertexIds.size() == 3) {
                AddTriangle(indices[srcMesh->faces[i].vertexIds[0]], indices[srcMesh->faces[i].vertexIds[1]], indices[srcMesh->faces[i].vertexIds[2]], NULL, srcMesh->faces[i].tag);
            } else if(srcMesh->faces[i].vertexIds.size() == 3) {
                AddQuad(indices[srcMesh->faces[i].vertexIds[0]], indices[srcMesh->faces[i].vertexIds[1]], indices[srcMesh->faces[i].vertexIds[2]], indices[srcMesh->faces[i].vertexIds[3]], NULL, srcMesh->faces[i].tag);
            }
        }
    }

    void NonManifoldMesh::RemoveFace(int faceId) {
        int faceIndex = GetFaceIndex(faceId);
        int edgeIndex;
        faces[faceIndex].valid = false;
        faceCount--;
        for(unsigned int i = 0; i < faces[faceIndex].edgeIds.size(); i++) {
            edgeIndex = GetEdgeIndex(faces[faceIndex].edgeIds[i]);
            for(int j = (int)edges[edgeIndex].faceIds.size()-1; j >= 0; j--) {
                if(edges[edgeIndex].faceIds[j] == faceId) {
                    edges[edgeIndex].faceIds.erase(edges[edgeIndex].faceIds.begin() + j);
                }
            }
        }
    }

    void NonManifoldMesh::RemoveEdge(int edgeId) {
        int edgeIndex = GetEdgeIndex(edgeId);
        int vertexIndex;
        if(edges[edgeIndex].faceIds.size() > 0) {
            printf("Cannot remove edgeId %i as it has face associations\n", edgeId);
            exit(0);
        }
        edges[edgeIndex].valid = false;
        edgeCount--;

        for(int i = 0; i < 2; i++) {
            vertexIndex = GetVertexIndex(edges[edgeIndex].vertexIds[i]);
            for(int j = (int)vertices[vertexIndex].edgeIds.size()-1; j >= 0; j--) {
                if((int)vertices[vertexIndex].edgeIds[j] == edgeId) {
                    vertices[vertexIndex].edgeIds.erase(vertices[vertexIndex].edgeIds.begin() + j);
                }
            }
        }
    }

    void NonManifoldMesh::RemoveVertex(int vertexId) {
        int vertexIndex = GetVertexIndex(vertexId);
        vertices[vertexIndex].valid = false;
        vertexCount--;

        if(vertices[vertexIndex].edgeIds.size() > 0) {
            printf("Cannot remove vertexId %i as it has edge associations\n", vertexId);
            exit(0);
        }
    }

    void NonManifoldMesh::RemoveNullEntries() {
        for(int i = (int)vertices.size()-1; i >= 0; i--) {
            if(!vertices[i].valid) {
                vertices.erase(vertices.begin() + i);
            }
        }

        int oldId, newId;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            if(vertices[i].id != i) {
                oldId = vertices[i].id;
                newId = i;
                vertices[i].id = newId;
                for(unsigned int j = 0; j < vertices[i].edgeIds.size(); j++) {
                    for(int k = 0; k < 2; k++) {
                        if((int)edges[GetEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] == oldId) {
                            edges[GetEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] = newId;
                        }
                    }
                }
            }
        }

        for(int i = (int)edges.size()-1; i >= 0; i--) {
            if(!edges[i].valid) {
                edges.erase(edges.begin() + i);
            }
        }

        for(unsigned int i = 0; i < edges.size(); i++) {
            if(edges[i].id != i) {
                oldId = edges[i].id;
                newId = i;
                edges[i].id = newId;

                for(int j = 0; j < 2; j++)  {
                    for(unsigned int k = 0; k < vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds.size(); k++) {
                        if((int)vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds[k] == oldId) {
                            vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds[k] = newId;
                        }
                    }
                }

                for(unsigned int j = 0; j < edges[i].faceIds.size(); j++)  {
                    for(unsigned int k = 0; k < faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds.size(); k++) {
                        if((int)faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds[k] == oldId) {
                            faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds[k] = newId;
                        }
                    }
                }
            }
        }

        for(int i = (int)faces.size()-1; i >= 0; i--) {
            if(!faces[i].valid) {
                faces.erase(faces.begin() + i);
            }
        }

        for(unsigned int i = 0; i < faces.size(); i++) {
            if(faces[i].id != i) {
                oldId = faces[i].id;
                newId = i;
                faces[i].id = newId;
                for(unsigned int j = 0; j < faces[i].edgeIds.size(); j++) {
                    for(unsigned int k = 0; k < edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds.size(); k++) {
                        if((int)edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds[k] == oldId) {
                            edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds[k] = newId;
                        }
                    }
                }
            }
        }
    }


    void NonManifoldMesh::ToOffCells(string fileName) {
        RemoveNullEntries();
        ofstream outFile(fileName.c_str());
        outFile<<"OFF\n";
        outFile<<vertices.size()
               <<" "<<faces.size() + edges.size()
               <<" 0"
               <<endl;
        int i,j;
        for(i = 0; i < (int)vertices.size(); i++) {
            outFile<<" "<<origin.X() + scale.X() * vertices[i].position.X()
                   <<" "<<origin.Y() + scale.Y() * vertices[i].position.Y()
                   <<" "<<origin.Z() + scale.Z() * vertices[i].position.Z()
                   <<endl;
        }
        int lastVertex;
        for(i = 0; i < (int)faces.size(); i++) {
            outFile<<faces[i].edgeIds.size();
            lastVertex = -1;

            for(j =0; j < (int)faces[i].vertexIds.size(); j++) {
                outFile<<GetVertexIndex(faces[i].vertexIds[j]);
            }

            outFile<<endl;
        }

        for(i = 0; i < (int)edges.size(); i++) {
            outFile<<"4"
                   <<" "<<edges[i].vertexIds[0]
                   <<" "<<edges[i].vertexIds[0]
                   <<" "<<edges[i].vertexIds[1]
                   <<" "<<edges[i].vertexIds[1]
                   <<endl;
        }
        outFile.close();
    }


    void NonManifoldMesh::ToMathematicaFile(string fileName) {
        RemoveNullEntries();
        ofstream outF(fileName.c_str());
        // Vertices
        outF<<"{\n";
        outF<<"{";
        for(unsigned int i = 0; i < vertices.size(); i++) {
            outF<<"{"
                <<vertices[i].position.X()
                <<" "<<vertices[i].position.Y()
                <<" "<<vertices[i].position.Z()
                <<"}";
            if(i != (int)vertices.size()-1) {
                outF<<", ";
            }
        }
        outF<<"},\n";

        // Edges
        outF<<"{";
        for(unsigned int i = 0; i < edges.size(); i++) {
            outF<<"{"
                <<edges[i].vertexIds[0]+1
                <<" "<<edges[i].vertexIds[1]+1
                <<"}";
            if(i != (int)edges.size()-1) {
                outF<<", ";
            }
        }
        outF<<"},\n";

        // Faces
        outF<<"{";
        int lastVertex;
        for(unsigned int i = 0; i < faces.size(); i++) {
            lastVertex = -1;
            outF<<"{";
            for(int j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
                if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) ||
                    (edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];
                } else {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
                }
                outF<<lastVertex+1;
                if(j != 0) {
                    outF<<", ";
                }
            }
            outF<<"}";

            if(i != (int)faces.size()-1) {
                outF<<", ";
            }
        }
        outF<<"}\n";

        outF<<"}";

        outF.close();
    }
    Volume * NonManifoldMesh::ToVolume() {
        double minPos[3] = {MAX_DOUBLE,MAX_DOUBLE,MAX_DOUBLE};
        double maxPos[3] = {MIN_DOUBLE, MIN_DOUBLE, MIN_DOUBLE};
        if(fromVolume) {
            minPos[0] = 0;
            minPos[1] = 0;
            minPos[2] = 0;
            maxPos[0] = getSizeX()-1;
            maxPos[1] = getSizeY()-1;
            maxPos[2] = getSizeZ()-1;
        }
        for(unsigned int i = 0; i < vertices.size(); i++) {
            for(unsigned int j = 0; j < 3; j++) {
                minPos[j] = min(minPos[j], (double)vertices[i].position[j]);
                maxPos[j] = max(maxPos[j], (double)vertices[i].position[j]);
            }
        }

        int minPosInt[3];
        int maxPosInt[3];

        for(unsigned int j = 0; j < 3; j++) {
            minPosInt[j] = (int)floor(minPos[j]);
            maxPosInt[j] = (int)ceil(maxPos[j]);
        }
        Volume * vol = new Volume(maxPosInt[0] - minPosInt[0]+1, maxPosInt[1] - minPosInt[1]+1, maxPosInt[2] - minPosInt[2]+1);

        NonManifoldMeshVertex v1,v2;
        int pos[3];

        for(unsigned int i = 0;  i < edges.size(); i++) {
            v1 = vertices[GetVertexIndex(edges[i].vertexIds[0])];
            v2 = vertices[GetVertexIndex(edges[i].vertexIds[1])];
            vector<Vector3DInt> positions = Rasterizer::ScanConvertLineC8(v1.position.XInt(), v1.position.YInt(), v1.position.ZInt(), v2.position.XInt(), v2.position.YInt(), v2.position.ZInt());
            for(unsigned int j = 0; j < positions.size(); j++) {
                for(unsigned int k = 0; k < 3; k++) {
                    pos[k] = positions[j][k] - minPosInt[k];
                }
                vol->setDataAt(pos[0], pos[1], pos[2], 1.0);
            }
        }
        vol->setOrigin(origin);
        vol->setSpacing(scale);
        return vol;
    }


    Vector3Float NonManifoldMesh::GetVertexNormal(int vertexId) {
        int index = GetVertexIndex(vertexId);
        int edgeIndex;
        Vector3Float normal = Vector3Float(0,0,0);
        for(unsigned int i = 0; i < vertices[index].edgeIds.size(); i++) {
            edgeIndex = GetEdgeIndex(vertices[index].edgeIds[i]);
            for(unsigned int j = 0; j < edges[edgeIndex].faceIds.size(); j++) {
                normal += GetFaceNormal(edges[edgeIndex].faceIds[j]);
            }
        }
        normal.normalize();
        return normal;
    }

    Vector3Float NonManifoldMesh::GetFaceNormal(int faceId) {

        Vector3Float normal = Vector3Float(1,0,0);

        NonManifoldMeshFace face = faces[GetFaceIndex(faceId)];

        if(face.vertexIds.size() >= 3) {
            normal = (vertices[GetVertexIndex(face.vertexIds[1])].position - vertices[GetVertexIndex(face.vertexIds[0])].position) ^
                            (vertices[GetVertexIndex(face.vertexIds[2])].position - vertices[GetVertexIndex(face.vertexIds[0])].position);
            normal.normalize();
        }
        return normal;
    }
    NonManifoldMesh NonManifoldMesh::SmoothLaplacian(double converganceRate) {
        NonManifoldMesh smoothedMesh = NonManifoldMesh(this);
        int i, j, vertexIndex;
        Vector3Float newPosition;
        NonManifoldMeshVertex vertex;
        for(i = 0; i < (int)vertices.size(); i++) {
            vertex = vertices[i];
            if(vertex.valid) {
                if(vertex.edgeIds.size() > 0) {
                    newPosition = Vector3Float(0,0,0);
                    for(j = 0; j < (int)vertex.edgeIds.size(); j++) {
                        if((int)edges[GetEdgeIndex(vertex.edgeIds[j])].vertexIds[0] == i) {
                            vertexIndex = 1;
                        } else {
                            vertexIndex = 0;
                        }

                        newPosition = newPosition + vertices[GetVertexIndex(edges[GetEdgeIndex(vertex.edgeIds[j])].vertexIds[vertexIndex])].position;
                    }
                    newPosition = newPosition * (1.0/vertex.edgeIds.size());
                } else {
                    newPosition = vertex.position;
                }
                smoothedMesh.vertices[i].position = smoothedMesh.vertices[i].position * (1.0 - converganceRate)+ newPosition * converganceRate;
            }
        }
        return smoothedMesh;
    }


    NonManifoldMesh NonManifoldMesh::SmoothLaplacian(double converganceRate, int iterations) {
        NonManifoldMesh newMesh;
        NonManifoldMesh oldMesh = NonManifoldMesh(this);

        for(int i = 0; i < iterations; i++) {
            oldMesh = oldMesh.SmoothLaplacian(converganceRate);
        }

        return oldMesh;
    }
    NonManifoldMesh * NonManifoldMesh::LoadOffFile(string fileName) {
        NonManifoldMesh * mesh = new NonManifoldMesh();
        ifstream inFile(fileName.c_str());
        string strTemp;
        int nVertices, nEdges, nFaces;
        int lVertices, lFaces;
        lVertices = 0;
        lFaces = 0;

        inFile>>strTemp;
        //printf("[%s]\n", strTemp);
        inFile>>nVertices>>nFaces>>nEdges;
        //printf("[%d] [%d] [%d]\n", nVertices, nFaces, nEdges);

        float xPos, yPos, zPos;
        lVertices = 0;
        for(int i=0; i < nVertices; i++) {
            lVertices++;
            inFile>>xPos>>yPos>>zPos;
            //printf("[%f] [%f] [%f]\n", xPos, yPos, zPos);
            mesh->AddVertex(Vector3Float(xPos, yPos, zPos));
            inFile>>strTemp;
        }


        int faceNodes[100], nFaceNodes;
        lFaces = 0;
        for(int i=0; i < nFaces; i++) {
            inFile>>nFaceNodes;
            //printf("[%d]\n", nFaceNodes);
            switch(nFaceNodes) {
                case 1:
                case 2:
                    printf("Cannot load polygon... unsupported polygon size: %d\n", nFaceNodes);
                    break;
                case 4:
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
//                        fscanf(inFile, "");
                        inFile>>faceNodes[i];
                    }

                    if((faceNodes[0] != faceNodes[1]) && (faceNodes[0] != faceNodes[2]) && (faceNodes[0] != faceNodes[3])
                        && (faceNodes[1] != faceNodes[2]) && (faceNodes[1] != faceNodes[3]) && (faceNodes[2] != faceNodes[3])) {
                        mesh->AddQuad(faceNodes[0], faceNodes[1], faceNodes[2], faceNodes[3]);
                    } else {
                        mesh->AddEdge(faceNodes[0], faceNodes[2]);
                    }
                    break;
                default :
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
//                        fscanf(inFile, "");
                        inFile>>faceNodes[i];
                    }
                    for(int i = 2; i < nFaceNodes; i++) {
                        mesh->AddTriangle(faceNodes[0], faceNodes[i-1], faceNodes[i]);
                    }
                    break;

            }
            inFile>>strTemp;
        }

        //printf(" Vertices %d of %d loaded.  Faces %d of %d loaded", lVertices, nVertices, lFaces, nFaces);

        //int v1, v2;
        //for(int i=0; i < nEdges; i++) {
        //	fscanf(inFile, "%d %d\n", &v1, &v2);
        //	if(!mesh->IsEdgePresent(v1, v2)) {
        //		mesh->AddEdge(v1, v2, false);
        //	}
        //}

        inFile.close();
        return mesh;
    }
    vector<unsigned int> NonManifoldMesh::GetPath(unsigned int edge0Ix, unsigned int edge1Ix) {
        vector<unsigned int> path;
        map<unsigned int,  unsigned int> source;

        queue<unsigned int> edgeList;
        edgeList.push(edge0Ix);

        bool found = false;
        unsigned int currentEdge, vertexIx, edgeIx;

        while((edgeList.size() > 0) && !found) {
            currentEdge = edgeList.front();
            edgeList.pop();
            found = currentEdge == edge1Ix;
            if(!found) {
                for(unsigned int v = 0; v < 2; v++) {
                    vertexIx = GetVertexIndex(edges[currentEdge].vertexIds[v]);
                    for(unsigned int e = 0; e < vertices[vertexIx].edgeIds.size(); e++) {
                        edgeIx = GetEdgeIndex(vertices[vertexIx].edgeIds[e]);
                        if(source.find(edgeIx) == source.end()) {
                            source[edgeIx] = currentEdge;
                            edgeList.push(edgeIx);
                        }
                    }
                }
            }
        }

        if(found) {
            currentEdge = edge1Ix;
            path.push_back(currentEdge);
            while(currentEdge != edge0Ix) {
                currentEdge = source[currentEdge];
                path.push_back(currentEdge);
            }
        }

        return path;
    }
    vector<Vector3Float> NonManifoldMesh::SampleTriangle(int faceId, double discretizationStep) {
        int faceIndex = GetFaceIndex(faceId);
        NonManifoldMeshFace face = faces[faceIndex];

        vector<Vector3Float> points;
        if(face.vertexIds.size() != 3) {
            printf("ERROR: Sampling a polygon NOT a triangle!\n");
            return points;
        } else {
            NonManifoldMeshVertex p = vertices[GetVertexIndex(face.vertexIds[0])];
            NonManifoldMeshVertex q = vertices[GetVertexIndex(face.vertexIds[1])];
            NonManifoldMeshVertex r = vertices[GetVertexIndex(face.vertexIds[2])];
            Vector3Float v1 = q.position - p.position;
            Vector3Float v2 = r.position - p.position;
            double v1Length = v1.length();
            double v2Length = v2.length();
            v1.normalize();
            v2.normalize();

            for(double a1 = 0; a1 <= v1Length; a1 += discretizationStep) {
                for(double a2 = 0; a2 <= v2Length; a2 += discretizationStep) {
                    if(a1/v1Length + a2/v2Length <= 1) {
                        points.push_back(p.position + v1 * a1 + v2 * a2);
                    }
                }
            }
            return points;
        }
    }

    void NonManifoldMesh::setScale(float x, float y, float z){
        scale = Dim3D<float>(x, y, z);
    }

    void NonManifoldMesh::setScale(Dim3D<float> val){
        scale = val;
    }

    void NonManifoldMesh::TranslateVertex(int vertexIx, Vector3Float translateVector) {
        vertices[vertexIx].position = vertices[vertexIx].position + translateVector;
    }

    int NonManifoldMesh::GetClosestVertexIndex(Vector3Float pos) {
        if(vertices.size() == 0) {
            return -1;
        }

        double distance, minDistance = (pos - vertices[0].position).length();
        int minIx = 0;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            distance = (pos - vertices[i].position).length();
            if(distance < minDistance) {
                minDistance = distance;
                minIx = i;
            }

        }
        return minIx;
    }

    bool NonManifoldMesh::IsSurfaceVertex(int ix) {
        bool isSurface = false;
        NonManifoldMeshEdge edge;

        for(unsigned int i = 0; i < vertices[ix].edgeIds.size(); i++) {
            edge = edges[GetEdgeIndex(vertices[ix].edgeIds[i])];
            isSurface = isSurface || (edge.faceIds.size() > 0);
        }
        return isSurface;
    }

    vector<unsigned int> NonManifoldMesh::GetNeighboringVertexIndices(unsigned int vertexIx) {
        vector<unsigned int> neighbors;
        for(unsigned int i = 0; i < vertices[vertexIx].edgeIds.size(); i++) {
            if(edges[vertices[vertexIx].edgeIds[i]].vertexIds[0] == vertexIx) {
                neighbors.push_back(edges[vertices[vertexIx].edgeIds[i]].vertexIds[1]);
            } else {
                neighbors.push_back(edges[vertices[vertexIx].edgeIds[i]].vertexIds[0]);
            }
        }
        return neighbors;
    }

}

#endif
