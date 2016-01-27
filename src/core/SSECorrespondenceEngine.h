#ifndef CORE_SSE_CORRESPONDENCE_ENGINE_H
#define CORE_SSE_CORRESPONDENCE_ENGINE_H

//#include <GraphMatch/SSECorrespondenceResult.h>
#include <GraphMatch/BackEndInterface.h>
//#include <GraphMatch/GeometricShape.h>
#include <vector>
#include <map>
#include <set>
//#include <GorgonGL.h>
//#include <Foundation/StringUtils.h>

//using namespace GraphMatch;
//using namespace Foundation;
//using namespace std;

namespace Visualization {
    class SSECorrespondenceEngine : public BackEndInterface {
    public:
        SSECorrespondenceEngine();

        int ExecuteQuery();
        int GetResultCount();
        int LoadCorrespondenceFromFile(string fileName);
        SSECorrespondenceResult GetResult(int rank);
        void SaveCorrespondenceToFile(string fileName);
        GeometricShape * GetSkeletonSSE(int sseId);
        SecondaryStructure * GetSequenceSSE(int sseId);
        int GetSkeletonSSECount();
        int GetSequenceSSECount();
        void SetVisibleCorrespondence(int correspondenceIndex);

        void InitializePathFinder(NonManifoldMesh_Annotated * mesh);
        void InitializePathHelix(int helixIndex, Vector3DFloat p1, Vector3DFloat p2, float radius);
        void PrunePathMesh(NonManifoldMesh_Annotated * mesh, vector<unsigned int> pathVertices, set<unsigned int> preserve);
        void GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start);
        int GetPathVertexCount();
        Vector3DFloat GetPathVertex(int index);
        int GetPathEdgeCount();
        int GetEdgeVertexIndex(int index, int side);
        void ClearPathSpace();
        void ClearPathFinder();

    private:
        vector<SSECorrespondenceResult> correspondence;
        int correspondenceIndex;

        // Attributes for path calculation
        NonManifoldMesh_Annotated * pathMesh;
        NonManifoldMesh_Annotated * singlePathMesh;
        map<unsigned int, vector<unsigned int> > helixStartPoints;
        map<unsigned int, vector<unsigned int> > helixEndPoints;
        int pathCount;

    };


    SSECorrespondenceEngine::SSECorrespondenceEngine() {
        correspondence.clear();
        correspondenceIndex = -1;
        pathCount = 0;
    }

    int SSECorrespondenceEngine::ExecuteQuery() {
        if(skeleton != NULL && sequence != NULL) {
            int resultCount = queryEngine->DoGraphMatching(sequence, skeleton);
            correspondence.clear();
            for(int i = 1; i <= resultCount; i++) {
                correspondence.push_back(queryEngine->GetSolution(i));
            }
            return resultCount;
        } else {
            return 0;
        }
    }

    int SSECorrespondenceEngine::GetResultCount() {
        return correspondence.size();
    }

    int SSECorrespondenceEngine::LoadCorrespondenceFromFile(string fileName) {

        FILE* fin = fopen((char*)fileName.c_str(), "rt");
        if (fin == NULL)
        {
            printf("Error opening input file %s.\n", fileName.c_str()) ;
            exit(0) ;
        }

        correspondence.clear();

        int correspondenceCount = 0, nodeCount, skeletonNode;
        vector<int> nodes;
        double cost;
        fscanf(fin, "%d\n", &correspondenceCount);

        for(int i = 0; i < correspondenceCount; i++) {
            nodes.clear();
            fscanf(fin, "%d ", &nodeCount);
            for(int j = 0; j < nodeCount; j++) {
                fscanf(fin, "%d ", &skeletonNode);
                nodes.push_back(skeletonNode);
            }
            fscanf(fin, "%lf\n", &cost);
            // TODO: Fix! 0 not acceptable!
            correspondence.push_back(SSECorrespondenceResult(nodes, cost, 0));
        }

        fclose(fin);

        return correspondenceCount;
    }

    SSECorrespondenceResult SSECorrespondenceEngine::GetResult(int rank) {
        // TODO: Fix!
        //if(rank <= (int)correspondence.size() && (rank >= 1)) {
            return correspondence[rank-1];
        //} else {
        //	return NULL;
        //}
    }

    void SSECorrespondenceEngine::SaveCorrespondenceToFile(string fileName) {
        FILE* fout = fopen((char*)fileName.c_str(), "wt");
        if (fout == NULL)
        {
            printf("Error opening output file %s.\n", fileName.c_str()) ;
            exit(0) ;
        }

        fprintf(fout, "%ld\n", correspondence.size());
        for(unsigned int i = 0; i < correspondence.size(); i++) {
            fprintf(fout, "%d ", correspondence[i].GetNodeCount());
            for(int j = 0; j < correspondence[i].GetNodeCount(); j++) {
                fprintf(fout, "%d ", correspondence[i].GetSkeletonNode(j));
            }
            fprintf(fout, "%lf\n", correspondence[i].GetCost());
        }

        fclose(fout);
    }

    GeometricShape * SSECorrespondenceEngine::GetSkeletonSSE(int sseId) {
        if((skeleton != NULL) && (sseId < (int)skeleton->skeletonHelixes.size())) {
            return skeleton->skeletonHelixes[sseId];
        } else {
            return NULL;
        }
    }

    SecondaryStructure * SSECorrespondenceEngine::GetSequenceSSE(int sseId) {
        if((sequence != NULL) && (sseId < (int)sequence->pdbStructures.size())) {
            return sequence->pdbStructures[sseId];
        } else {
            return NULL;
        }
    }


    int SSECorrespondenceEngine::GetSkeletonSSECount() {
        return skeleton->skeletonHelixes.size();
    }

    int SSECorrespondenceEngine::GetSequenceSSECount() {
        return sequence->pdbStructures.size();
    }

    void SSECorrespondenceEngine::SetVisibleCorrespondence(int correspondenceIndex) {
        this->correspondenceIndex = correspondenceIndex;
    }


    void SSECorrespondenceEngine::InitializePathFinder(NonManifoldMesh_Annotated * mesh) {
        pathMesh = new NonManifoldMesh_Annotated(mesh);
        for(unsigned int i = 0; i < pathMesh->vertices.size(); i++) {
            pathMesh->vertices[i].tag = true;
        }
        helixEndPoints.clear();
        pathCount++;
    }

    void SSECorrespondenceEngine::InitializePathHelix(int helixIndex, Vector3DFloat p1, Vector3DFloat p2, float radius) {
        GeometricShape * helix = GeometricShape::CreateHelix(p1, p2, radius);
        set<unsigned int> internalVertices;
        internalVertices.clear();
        vector<unsigned int> startPoints;
        vector<unsigned int> endPoints;
        helixStartPoints[helixIndex] = startPoints;
        helixEndPoints[helixIndex] = endPoints;

        for(unsigned int i = 0; i < pathMesh->vertices.size(); i++) {
            if(helix->IsInsideShape(pathMesh->vertices[i].position)) {
                internalVertices.insert(i);
                pathMesh->vertices[i].tag = false;
            }
        }
        vector<unsigned int> neighbors;
        bool isEnd = false, isStart;
        float dist1, dist2;
        for(set<unsigned int>::iterator i = internalVertices.begin(); i != internalVertices.end(); i++) {
            neighbors = pathMesh->GetNeighboringVertexIndices(*i);
            for(unsigned int j = 0; j < neighbors.size(); j++) {
                isEnd = isEnd || (internalVertices.find(neighbors[j]) == internalVertices.end());
            }
            if(isEnd) {
                dist1 = (p1 - pathMesh->vertices[*i].position).Length();
                dist2 = (p2 - pathMesh->vertices[*i].position).Length();
                isStart = (dist1 <= dist2);
                if(isStart && (dist1 <= radius)) {
                    helixStartPoints[helixIndex].push_back(*i);
                    pathMesh->vertices[*i].tag = true;
                } else if (!isStart && (dist2 <= radius)) {
                    helixEndPoints[helixIndex].push_back(*i);
                    pathMesh->vertices[*i].tag = true;
                }
            }
        }
        if(helixStartPoints[helixIndex].size() == 0) {
            printf("Error <SSECorrespondenceEngine, InitializePathHelix>: No helix start points found for helix %d\n", helixIndex);
        }
        if(helixEndPoints[helixIndex].size() == 0) {
            printf("Error <SSECorrespondenceEngine, InitializePathHelix>: No helix end points found for helix %d\n", helixIndex);
        }

        delete helix;

    }

    void SSECorrespondenceEngine::PrunePathMesh(NonManifoldMesh_Annotated * mesh, vector<unsigned int> pathVertices, set<unsigned int> preserve) {
        for(unsigned int i = 0; i < mesh->vertices.size(); i++) {
            mesh->vertices[i].tag = true;
        }
        for(unsigned int i = 0; i < pathVertices.size(); i++) {
            mesh->vertices[pathVertices[i]].tag = false;
        }
    }

    void SSECorrespondenceEngine::GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start) {
        NonManifoldMesh_Annotated * mesh = new NonManifoldMesh_Annotated(pathMesh);

        vector<unsigned int> queue;
        for(unsigned int i = 0; i < helixStartPoints[helix1Ix].size(); i++) {
            queue.push_back(helixStartPoints[helix1Ix][i]);
        }

        unsigned int currIx;
        vector<unsigned int> neighbors;
        vector<unsigned int> pathVertices;
        pathVertices.clear();
        while(queue.size() > 0){
            currIx = queue[0];
            queue.erase(queue.begin());
            if(mesh->vertices[currIx].tag) {
                mesh->vertices[currIx].tag = false;
                pathVertices.push_back(currIx);
                neighbors = mesh->GetNeighboringVertexIndices(currIx);
                for(unsigned int i = 0; i < neighbors.size(); i++) {
                    if(mesh->vertices[neighbors[i]].tag) {
                        queue.push_back(neighbors[i]);
                    }
                }
            }
        }

        // Preserving start and end terminus, while pruning away the single directional branches.
        set<unsigned int> preserve;
        if(helix1Start) {
            for(unsigned int i = 0; i < helixStartPoints[helix1Ix].size(); i++) {
                preserve.insert(helixStartPoints[helix1Ix][i]);
            }
        } else {
            for(unsigned int i = 0; i < helixEndPoints[helix1Ix].size(); i++) {
                preserve.insert(helixEndPoints[helix1Ix][i]);
            }
        }

        if(helix2Start) {
            for(unsigned int i = 0; i < helixStartPoints[helix2Ix].size(); i++) {
                preserve.insert(helixStartPoints[helix2Ix][i]);
            }
        } else {
            for(unsigned int i = 0; i < helixEndPoints[helix2Ix].size(); i++) {
                preserve.insert(helixEndPoints[helix2Ix][i]);
            }
        }

        //printf("Preserving:");
        //for(set<unsigned int>::iterator i = preserve.begin(); i != preserve.end(); i++) {
        //	printf("%d ", *i);
        //}
        //printf("\n");

        PrunePathMesh(mesh, pathVertices, preserve);

        singlePathMesh = new NonManifoldMesh_Annotated();
        map<unsigned int, unsigned int> vertexMap;
        for(unsigned int i=0; i < mesh->vertices.size(); i++) {
            if(!mesh->vertices[i].tag) {
                vertexMap[i] = singlePathMesh->AddVertex(mesh->vertices[i]);
            }
        }

        for(unsigned int i=0; i < mesh->edges.size(); i++) {
            if(!mesh->vertices[mesh->edges[i].vertexIds[0]].tag && !mesh->vertices[mesh->edges[i].vertexIds[1]].tag) {
                singlePathMesh->AddEdge(vertexMap[mesh->edges[i].vertexIds[0]], vertexMap[mesh->edges[i].vertexIds[1]], mesh->edges[i].tag);
            }
        }


        //char filename[100];
        //sprintf(filename, "C:\\path_%d_%d.off", pathCount, helix1Ix);
        //printf("vertex count: %d, edgeCount: %d, faceCount: %d\n", singlePathMesh->vertices.size(), singlePathMesh->edges.size(), singlePathMesh->faces.size()); flushall();
        //singlePathMesh->ToOffCells(filename);

        vertexMap.clear();
        pathVertices.clear();
        delete mesh;
    }

    void SSECorrespondenceEngine::ClearPathSpace() {
        delete singlePathMesh;
        singlePathMesh = NULL;
    }

    void SSECorrespondenceEngine::ClearPathFinder() {
        helixStartPoints.clear();
        helixEndPoints.clear();
        delete pathMesh;
    }

    int SSECorrespondenceEngine::GetPathVertexCount() {
        return singlePathMesh->vertices.size();
    }

    Vector3DFloat SSECorrespondenceEngine::GetPathVertex(int index) {
        return singlePathMesh->vertices[index].position;
    }

    int SSECorrespondenceEngine::GetPathEdgeCount() {
        return singlePathMesh->edges.size();
    }

    int SSECorrespondenceEngine::GetEdgeVertexIndex(int index, int side) {
        return singlePathMesh->edges[index].vertexIds[side];
    }
}

#endif
