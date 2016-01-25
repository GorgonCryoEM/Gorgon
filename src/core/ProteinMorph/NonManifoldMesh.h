#ifndef CORE_PROTEINMORPH_NON_MANIFOLD_MESH_H
#define CORE_PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/BasicDefines.h>
#include <SkeletonMaker/volume.h>
//#include <string>
//#include <GorgonGL.h>
#include <Foundation/Rasterizer.h>
#include <map>
#include <queue>
#ifdef _WIN32
	#include <hash_map>
	using namespace stdext;
#endif

using namespace std;
using namespace MathTools;
using namespace Foundation;
using namespace SkeletonMaker;


namespace Protein_Morph {
    struct NonManifoldMeshEdge {
        unsigned int id;
        unsigned int vertexIds[2];
        vector<unsigned int> faceIds;
        unsigned char tag;
        bool valid;
    };

    struct NonManifoldMeshFace {
        unsigned int id;
        vector<unsigned int> edgeIds;
        vector<unsigned int> vertexIds;
        unsigned char tag;
        bool valid;
    };

    struct NonManifoldMeshVertex {
        unsigned int id;
        Vector3DFloat position;
        vector<unsigned int> edgeIds;
        bool valid;
        bool tag;
    };


    #ifdef _WIN32
        typedef hash_map<int, int> HashMapType;
    #else
        typedef map<int, int> HashMapType;
    #endif

    class NonManifoldMesh : public Volume {
    public:
        NonManifoldMesh();
        NonManifoldMesh(NonManifoldMesh * srcMesh);
        NonManifoldMesh(Volume * sourceVol);
        bool IsEdgePresent(int vertexId1, int vertexId2);
        bool IsSurfaceVertex(int ix);
        int AddVertex(NonManifoldMeshVertex vertex);
        int AddVertex(Vector3DFloat location, bool tag = NULL);
        int AddHashedVertex(Vector3DFloat location, int hashKey, bool tag = NULL);
        int AddEdge(NonManifoldMeshEdge edge);
        int AddFace(NonManifoldMeshFace face);
        int GetVertexIndex(int vertexId);
        int GetFaceIndex(int faceId);
        int GetEdgeIndex(int edgeId);
        int GetEdgeIndex(int vertexId1, int vertexId2);
        int GetClosestVertexIndex(Vector3DFloat pos);
        void AddEdge(int vertexId1, int vertexId2, unsigned char tag = NULL);
        void AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, unsigned char newEdgeTag = NULL, unsigned char faceTag = NULL);
        void AddTriangle(int vertexId1, int vertexId2, int vertexId3, unsigned char newEdgeTag = NULL, unsigned char faceTag = NULL);
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
        void TranslateVertex(int vertexIx, Vector3DFloat translateVector);
        vector<unsigned int> GetPath(unsigned int edge0Ix, unsigned int edge1Ix);
        vector<unsigned int> GetNeighboringVertexIndices(unsigned int vertexIx);
        vector<Vector3DFloat> SampleTriangle(int faceId, double discretizationStep);
        Volume * ToVolume();
        Vector3DFloat GetVertexNormal(int vertexId);
        Vector3DFloat GetFaceNormal(int faceId);
        NonManifoldMesh * SmoothLaplacian(double converganceRate);
        NonManifoldMesh * SmoothLaplacian(double converganceRate, int iterations);
        static NonManifoldMesh * LoadOffFile(string fileName);


    public:
        Dim3D<float> scale;
        vector< NonManifoldMeshVertex > vertices;
        vector< NonManifoldMeshEdge > edges;
        vector< NonManifoldMeshFace > faces;
        int edgeCount;
        int vertexCount;
        int faceCount;
        HashMapType vertexHashMap;
        bool fromVolume;
        int volSizeX, volSizeY, volSizeZ;
    };



    NonManifoldMesh::NonManifoldMesh() {
        fromVolume = false;
        setOrigin(0,0,0);
        setScale(1,1,1);

    }

    NonManifoldMesh::NonManifoldMesh(NonManifoldMesh * srcMesh) {
        fromVolume = false;
        for(unsigned int i = 0; i < srcMesh->vertices.size(); i++) {
            vertices.push_back(srcMesh->vertices[i]);
        }
        for(unsigned int i = 0; i < srcMesh->edges.size(); i++) {
            edges.push_back(srcMesh->edges[i]);
        }
        for(unsigned int i = 0; i < srcMesh->faces.size(); i++) {
            faces.push_back(srcMesh->faces[i]);
        }
        setOrigin(srcMesh->origin.X(),srcMesh->origin.Y(),srcMesh->origin.Z());
        setScale(srcMesh->scale);
    }

    NonManifoldMesh::NonManifoldMesh(Volume * sourceVol) {
        int x, y, z, i, j, index, index2;
        int * vertexLocations = new int[sourceVol->getSizeX() * sourceVol->getSizeY() * sourceVol->getSizeZ()];
        int value;
        fromVolume = true;
        volSizeX = sourceVol->getSizeX();
        volSizeY = sourceVol->getSizeY();
        volSizeZ = sourceVol->getSizeZ();
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
                        tempVertex.position = Vector3DFloat(x, y, z);
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

    int NonManifoldMesh::AddVertex(Vector3DFloat location, bool tag) {
        NonManifoldMeshVertex v;
        v.position = location;
        v.tag = tag;
        return AddVertex(v);
    }

    int NonManifoldMesh::AddHashedVertex(Vector3DFloat location, int hashKey, bool tag) {
        HashMapType::const_iterator pos = vertexHashMap.find(hashKey);
        int vertexId;
        if(pos == vertexHashMap.end()) {
            vertexId = AddVertex(location, tag);
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

    void NonManifoldMesh::AddEdge(int vertexId1, int vertexId2, unsigned char tag){
        NonManifoldMeshEdge edge;
        edge.tag = tag;
        edge.faceIds.clear();
        edge.vertexIds[0] = vertexId1;
        edge.vertexIds[1] = vertexId2;
        int edgeId = AddEdge(edge);
        vertices[GetVertexIndex(vertexId1)].edgeIds.push_back(edgeId);
        vertices[GetVertexIndex(vertexId2)].edgeIds.push_back(edgeId);
    }

    void NonManifoldMesh::AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, unsigned char newEdgeTag, unsigned char faceTag) {
        AddTriangle(vertexId1, vertexId2, vertexId3, newEdgeTag, faceTag);
        AddTriangle(vertexId1, vertexId3, vertexId4, newEdgeTag, faceTag);
    }

    void NonManifoldMesh::AddTriangle(int vertexId1, int vertexId2, int vertexId3, unsigned char newEdgeTag, unsigned char faceTag) {
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
        FILE * outFile = fopen(fileName.c_str(), "wt");
        fprintf(outFile, "OFF\n");
        fprintf(outFile, "%i %li %i\n", (int)vertices.size(), faces.size() + edges.size(), 0);
        int i,j;
        for(i = 0; i < (int)vertices.size(); i++) {
            fprintf(outFile, "%lf %lf %lf\n", origin.X() + scale.X() * vertices[i].position.X(), origin.Y() + scale.Y() * vertices[i].position.Y(), origin.Z() + scale.Z() * vertices[i].position.Z());
        }
        int lastVertex;
        for(i = 0; i < (int)faces.size(); i++) {
            fprintf(outFile, "%li ", faces[i].edgeIds.size());
            lastVertex = -1;

            for(j =0; j < (int)faces[i].vertexIds.size(); j++) {
                fprintf(outFile, "%i ", GetVertexIndex(faces[i].vertexIds[j]));
            }

            fprintf(outFile, "\n");
        }

        for(i = 0; i < (int)edges.size(); i++) {
            fprintf(outFile, "4 %i %i %i %i \n", edges[i].vertexIds[0], edges[i].vertexIds[0], edges[i].vertexIds[1], edges[i].vertexIds[1]);
        }
        fclose(outFile);
    }


    void NonManifoldMesh::ToMathematicaFile(string fileName) {
        RemoveNullEntries();
        FILE * outF = fopen(fileName.c_str(), "wt");
        // Vertices
        fprintf(outF, "{\n");
        fprintf(outF, "{");
        for(unsigned int i = 0; i < vertices.size(); i++) {
            fprintf(outF, "{%lf, %lf, %lf}", vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
            if(i != (int)vertices.size()-1) {
                fprintf(outF, ", ");
            }
        }
        fprintf(outF, "},\n");

        // Edges
        fprintf(outF, "{");
        for(unsigned int i = 0; i < edges.size(); i++) {
            fprintf(outF, "{%li, %li}", edges[i].vertexIds[0]+1, edges[i].vertexIds[1]+1);
            if(i != (int)edges.size()-1) {
                fprintf(outF, ", ");
            }
        }
        fprintf(outF, "},\n");

        // Faces
        fprintf(outF, "{");
        int lastVertex;
        for(unsigned int i = 0; i < faces.size(); i++) {
            lastVertex = -1;
            fprintf(outF, "{");
            for(int j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
                if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) ||
                    (edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];
                } else {
                    lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
                }
                fprintf(outF, "%li", lastVertex+1);
                if(j != 0) {
                    fprintf(outF, ", ");
                }
            }
            fprintf(outF, "}");

            if(i != (int)faces.size()-1) {
                fprintf(outF, ", ");
            }
        }
        fprintf(outF, "}\n");

        fprintf(outF, "}");

        fclose(outF);
    }
    Volume * NonManifoldMesh::ToVolume() {
        double minPos[3] = {MAX_DOUBLE,MAX_DOUBLE,MAX_DOUBLE};
        double maxPos[3] = {MIN_DOUBLE, MIN_DOUBLE, MIN_DOUBLE};
        if(fromVolume) {
            minPos[0] = 0;
            minPos[1] = 0;
            minPos[2] = 0;
            maxPos[0] = volSizeX-1;
            maxPos[1] = volSizeY-1;
            maxPos[2] = volSizeZ-1;
        }
        for(unsigned int i = 0; i < vertices.size(); i++) {
            for(unsigned int j = 0; j < 3; j++) {
                minPos[j] = min(minPos[j], (double)vertices[i].position.values[j]);
                maxPos[j] = max(maxPos[j], (double)vertices[i].position.values[j]);
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
                    pos[k] = positions[j].values[k] - minPosInt[k];
                }
                vol->setDataAt(pos[0], pos[1], pos[2], 1.0);
            }
        }
        vol->setOrigin(origin);
        vol->setSpacing(scale);
        return vol;
    }


    Vector3DFloat NonManifoldMesh::GetVertexNormal(int vertexId) {
        int index = GetVertexIndex(vertexId);
        int edgeIndex;
        Vector3DFloat normal = Vector3DFloat(0,0,0);
        for(unsigned int i = 0; i < vertices[index].edgeIds.size(); i++) {
            edgeIndex = GetEdgeIndex(vertices[index].edgeIds[i]);
            for(unsigned int j = 0; j < edges[edgeIndex].faceIds.size(); j++) {
                normal += GetFaceNormal(edges[edgeIndex].faceIds[j]);
            }
        }
        normal.Normalize();
        return normal;
    }

    Vector3DFloat NonManifoldMesh::GetFaceNormal(int faceId) {

        Vector3DFloat normal = Vector3DFloat(1,0,0);

        NonManifoldMeshFace face = faces[GetFaceIndex(faceId)];

        if(face.vertexIds.size() >= 3) {
            normal = (vertices[GetVertexIndex(face.vertexIds[1])].position - vertices[GetVertexIndex(face.vertexIds[0])].position) ^
                            (vertices[GetVertexIndex(face.vertexIds[2])].position - vertices[GetVertexIndex(face.vertexIds[0])].position);
            normal.Normalize();
        }
        return normal;
    }
    NonManifoldMesh * NonManifoldMesh::SmoothLaplacian(double converganceRate) {
        NonManifoldMesh * smoothedMesh = new NonManifoldMesh(this);
        int i, j, vertexIndex;
        Vector3DFloat newPosition;
        NonManifoldMeshVertex vertex;
        for(i = 0; i < (int)vertices.size(); i++) {
            vertex = vertices[i];
            if(vertex.valid) {
                if(vertex.edgeIds.size() > 0) {
                    newPosition = Vector3DFloat(0,0,0);
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
                smoothedMesh->vertices[i].position = smoothedMesh->vertices[i].position * (1.0 - converganceRate)+ newPosition * converganceRate;
            }
        }
        return smoothedMesh;
    }


    NonManifoldMesh * NonManifoldMesh::SmoothLaplacian(double converganceRate, int iterations) {
        NonManifoldMesh * newMesh;
        NonManifoldMesh * oldMesh = new NonManifoldMesh(this);

        for(int i = 0; i < iterations; i++) {
            newMesh = oldMesh->SmoothLaplacian(converganceRate);
            delete oldMesh;
            oldMesh = newMesh;
        }

        return oldMesh;
    }
    NonManifoldMesh * NonManifoldMesh::LoadOffFile(string fileName) {
        NonManifoldMesh * mesh = new NonManifoldMesh();
        FILE * inFile = fopen(fileName.c_str(), "rt");
        char strTemp[255];
        int nVertices, nEdges, nFaces;
        int lVertices, lFaces;
        lVertices = 0;
        lFaces = 0;

        fscanf(inFile, "%s\n", strTemp);
        //printf("[%s]\n", strTemp);
        fscanf(inFile, "%d %d %d\n", &nVertices, &nFaces, &nEdges);
        //printf("[%d] [%d] [%d]\n", nVertices, nFaces, nEdges);

        float xPos, yPos, zPos;
        lVertices = 0;
        for(int i=0; i < nVertices; i++) {
            lVertices++;
            fscanf(inFile, "%f %f %f", &xPos, &yPos, &zPos);
            //printf("[%f] [%f] [%f]\n", xPos, yPos, zPos);
            mesh->AddVertex(Vector3DFloat(xPos, yPos, zPos));
            fgets(strTemp, 255, inFile);
        }


        int faceNodes[100], nFaceNodes;
        lFaces = 0;
        for(int i=0; i < nFaces; i++) {
            fscanf(inFile, "%d", &nFaceNodes);
            //printf("[%d]\n", nFaceNodes);
            switch(nFaceNodes) {
                case 1:
                case 2:
                    printf("Cannot load polygon... unsupported polygon size: %d\n", nFaceNodes);
                    break;
                case 4:
                    lFaces++;
                    for(int i = 0; i < nFaceNodes; i++) {
                        fscanf(inFile, "");
                        fscanf(inFile, "%d", &faceNodes[i]);
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
                        fscanf(inFile, "");
                        fscanf(inFile, "%d", &faceNodes[i]);
                    }
                    for(int i = 2; i < nFaceNodes; i++) {
                        mesh->AddTriangle(faceNodes[0], faceNodes[i-1], faceNodes[i]);
                    }
                    break;

            }
            fgets(strTemp, 255, inFile);
        }

        //printf(" Vertices %d of %d loaded.  Faces %d of %d loaded", lVertices, nVertices, lFaces, nFaces);

        //int v1, v2;
        //for(int i=0; i < nEdges; i++) {
        //	fscanf(inFile, "%d %d\n", &v1, &v2);
        //	if(!mesh->IsEdgePresent(v1, v2)) {
        //		mesh->AddEdge(v1, v2, false);
        //	}
        //}

        fclose(inFile);
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
    vector<Vector3DFloat> NonManifoldMesh::SampleTriangle(int faceId, double discretizationStep) {
        int faceIndex = GetFaceIndex(faceId);
        NonManifoldMeshFace face = faces[faceIndex];

        vector<Vector3DFloat> points;
        if(face.vertexIds.size() != 3) {
            printf("ERROR: Sampling a polygon NOT a triangle!\n");
            return points;
        } else {
            NonManifoldMeshVertex p = vertices[GetVertexIndex(face.vertexIds[0])];
            NonManifoldMeshVertex q = vertices[GetVertexIndex(face.vertexIds[1])];
            NonManifoldMeshVertex r = vertices[GetVertexIndex(face.vertexIds[2])];
            Vector3DFloat v1 = q.position - p.position;
            Vector3DFloat v2 = r.position - p.position;
            double v1Length = v1.Length();
            double v2Length = v2.Length();
            v1.Normalize();
            v2.Normalize();

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

    void NonManifoldMesh::TranslateVertex(int vertexIx, Vector3DFloat translateVector) {
        vertices[vertexIx].position = vertices[vertexIx].position + translateVector;
    }

    int NonManifoldMesh::GetClosestVertexIndex(Vector3DFloat pos) {
        if(vertices.size() == 0) {
            return -1;
        }

        double distance, minDistance = (pos - vertices[0].position).Length();
        int minIx = 0;
        for(unsigned int i = 0; i < vertices.size(); i++) {
            distance = (pos - vertices[i].position).Length();
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
