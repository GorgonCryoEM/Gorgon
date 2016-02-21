#ifndef TOOLKIT_SSE_CORRESPONDENCE_ENGINE_H
#define TOOLKIT_SSE_CORRESPONDENCE_ENGINE_H

//#include <GraphMatch/SSEResult.h>
#include <GraphMatch/IBackEnd.h>
//#include <GraphMatch/Shape.h>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
//#include <GorgonGL.h>
//#include <Foundation/StringUtils.h>

//using namespace GraphMatch;
//using namespace Foundation;
//using namespace std;

namespace Visualization {
    class SSEEngine : public IBackEnd {
    public:
        SSEEngine();

        int ExecuteQuery();
        int GetResultCount();
        int load(string fileName);
        SSEResult GetResult(int rank);
        void save(string fileName);
        Shape * GetSkeletonSSE(int sseId);
        SecStruct * GetSequenceSSE(int sseId);
        int GetSkeletonSSECount();
        int GetSequenceSSECount();

        void InitializePathFinder(NonManifoldMesh * mesh);
        void InitializePathHelix(int helixIndex, Vector3Float p1, Vector3Float p2, float radius);
        void PrunePathMesh(NonManifoldMesh * mesh, vector<unsigned int> pathVertices);
        void GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start);
        int GetPathVertexCount();
        Vector3Float GetPathVertex(int index);
        int GetPathEdgeCount();
        int GetEdgeVertexIndex(int index, int side);
        void ClearPathSpace();
        void ClearPathFinder();

    private:
        vector<SSEResult> correspondence;

        // Attributes for path calculation
        NonManifoldMesh * pathMesh;
        NonManifoldMesh * singlePathMesh;
        map<unsigned int, vector<unsigned int> > helixStartPoints;
        map<unsigned int, vector<unsigned int> > helixEndPoints;
        int pathCount;

    };


    SSEEngine::SSEEngine()
    						: pathCount(0)
    {}

    int SSEEngine::ExecuteQuery() {
        if(skeleton != NULL && sequence != NULL) {
            int resultCount = matcher->match(sequence, skeleton);
            correspondence.clear();
            for(int i = 1; i <= resultCount; i++) {
                correspondence.push_back(matcher->GetSolution(i));
            }
            return resultCount;
        } else {
            return 0;
        }
    }

    int SSEEngine::GetResultCount() {
        return correspondence.size();
    }

    int SSEEngine::load(string fileName) {

        ifstream fin(fileName.c_str());
        if (!fin)
        {
            cout<<"Error opening input file "<<fileName<<".\n";
            exit(0) ;
        }

        correspondence.clear();

        int correspondenceCount = 0, nodeCount, skeletonNode;
        vector<int> nodes;
        double cost;
        fin>>correspondenceCount;

        for(int i = 0; i < correspondenceCount; i++) {
            nodes.clear();
            fin>>nodeCount;
            for(int j = 0; j < nodeCount; j++) {
                fin>>skeletonNode;
                nodes.push_back(skeletonNode);
            }
            fin>>cost;
            // TODO: Fix! 0 not acceptable!
            correspondence.push_back(SSEResult(nodes, cost, 0));
        }

        fin.close();

        return correspondenceCount;
    }

    SSEResult SSEEngine::GetResult(int rank) {
        // TODO: Fix!
        //if(rank <= (int)correspondence.size() && (rank >= 1)) {
            return correspondence[rank-1];
        //} else {
        //	return NULL;
        //}
    }

    void SSEEngine::save(string fileName) {
        ofstream fout(fileName.c_str());
        if (!fout)
        {
            cout<<"Error opening output file "<<fileName<<".\n";
            exit(0) ;
        }

        fout<<correspondence.size()<<endl;
        for(unsigned int i = 0; i < correspondence.size(); i++) {
            fout<<correspondence[i].GetNodeCount()<<" ";
            for(int j = 0; j < correspondence[i].GetNodeCount(); j++) {
                fout<<correspondence[i].GetSkeletonNode(j)<<" ";
            }
            fout<<fixed<<setprecision(6)<<correspondence[i].GetCost()<<endl;
        }

        fout.close();
    }

    Shape * SSEEngine::GetSkeletonSSE(int sseId) {
        if((skeleton != NULL) && (sseId < (int)skeleton->skeletonHelixes.size())) {
            return skeleton->skeletonHelixes[sseId];
        } else {
            return NULL;
        }
    }

    SecStruct * SSEEngine::GetSequenceSSE(int sseId) {
        if((sequence != NULL) && (sseId < (int)sequence->pdbStructures.size())) {
            return sequence->pdbStructures[sseId];
        } else {
            return NULL;
        }
    }


    int SSEEngine::GetSkeletonSSECount() {
        return skeleton->skeletonHelixes.size();
    }

    int SSEEngine::GetSequenceSSECount() {
        return sequence->pdbStructures.size();
    }


    void SSEEngine::InitializePathFinder(NonManifoldMesh * mesh) {
        pathMesh = new NonManifoldMesh(mesh);
        for(unsigned int i = 0; i < pathMesh->vertices.size(); i++) {
            pathMesh->vertices[i].tag = true;
        }
        helixEndPoints.clear();
        pathCount++;
    }

    void SSEEngine::InitializePathHelix(int helixIndex, Vector3Float p1, Vector3Float p2, float radius) {
        Shape * helix = Shape::CreateHelix(p1, p2, radius);
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
                dist1 = (p1 - pathMesh->vertices[*i].position).length();
                dist2 = (p2 - pathMesh->vertices[*i].position).length();
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
            printf("Error <SSEEngine, InitializePathHelix>: No helix start points found for helix %d\n", helixIndex);
        }
        if(helixEndPoints[helixIndex].size() == 0) {
            printf("Error <SSEEngine, InitializePathHelix>: No helix end points found for helix %d\n", helixIndex);
        }

        delete helix;

    }

    void SSEEngine::PrunePathMesh(NonManifoldMesh * mesh, vector<unsigned int> pathVertices) {
        for(unsigned int i = 0; i < mesh->vertices.size(); i++) {
            mesh->vertices[i].tag = true;
        }
        for(unsigned int i = 0; i < pathVertices.size(); i++) {
            mesh->vertices[pathVertices[i]].tag = false;
        }
    }

    void SSEEngine::GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start) {
        NonManifoldMesh * mesh = new NonManifoldMesh(pathMesh);

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

        PrunePathMesh(mesh, pathVertices);

        singlePathMesh = new NonManifoldMesh();
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

        vertexMap.clear();
        pathVertices.clear();
        delete mesh;
    }

    void SSEEngine::ClearPathSpace() {
        delete singlePathMesh;
        singlePathMesh = NULL;
    }

    void SSEEngine::ClearPathFinder() {
        helixStartPoints.clear();
        helixEndPoints.clear();
        delete pathMesh;
    }

    int SSEEngine::GetPathVertexCount() {
        return singlePathMesh->vertices.size();
    }

    Vector3Float SSEEngine::GetPathVertex(int index) {
        return singlePathMesh->vertices[index].position;
    }

    int SSEEngine::GetPathEdgeCount() {
        return singlePathMesh->edges.size();
    }

    int SSEEngine::GetEdgeVertexIndex(int index, int side) {
        return singlePathMesh->edges[index].vertexIds[side];
    }
}

#endif