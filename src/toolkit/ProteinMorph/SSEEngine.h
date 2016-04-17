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

        void GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start);
        int GetPathVertexCount();
        Vec3F GetPathVertex(int index);
        int GetPathEdgeCount();
        int GetEdgeVertexIndex(int index, int side);
        void ClearPathSpace();
        void ClearPathFinder();

    private:
        vector<SSEResult> correspondence;

        // Attributes for path calculation
        NonManifoldMesh * pathMesh;
        NonManifoldMesh * singlePathMesh;
        map<TKey, vector<TKey> > helixStartPoints;
        map<TKey, vector<TKey> > helixEndPoints;
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

    void SSEEngine::GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start) {
        NonManifoldMesh mesh(*pathMesh);

        vector<TKey> queue;
        for(unsigned int i = 0; i < helixStartPoints[helix1Ix].size(); i++) {
            queue.push_back(helixStartPoints[helix1Ix][i]);
        }

        TKey currIx;
        vector<TKey> neighbors;
        vector<TKey> pathVertices;
        pathVertices.clear();
        while(queue.size() > 0){
            currIx = queue[0];
            queue.erase(queue.begin());
            if(mesh.vertices[currIx].tag=="true") {
                mesh.vertices[currIx].tag = "false";
                pathVertices.push_back(currIx);
                neighbors = mesh.getNeighboringVertexIndices(currIx);
                for(unsigned int i = 0; i < neighbors.size(); i++) {
                    if(mesh.vertices[neighbors[i]].tag == "true") {
                        queue.push_back(neighbors[i]);
                    }
                }
            }
        }

        PrunePathMesh(&mesh, pathVertices);

        singlePathMesh = new NonManifoldMesh();
        map<TKey, TKey> vertexMap;
        for(unsigned int i=0; i < mesh.vertices.size(); i++) {
            if(mesh.vertices[i].tag=="false") {
                vertexMap[i] = singlePathMesh->addVertex(mesh.vertices[i]);
            }
        }

        for(unsigned int i=0; i < mesh.edges.size(); i++) {
            if(mesh.vertices[mesh.edges[i].vertexIds[0]].tag =="false" && mesh.vertices[mesh.edges[i].vertexIds[1]].tag=="false") {
                singlePathMesh->addEdge(vertexMap[mesh.edges[i].vertexIds[0]], vertexMap[mesh.edges[i].vertexIds[1]], mesh.edges[i].tag);
            }
        }

        vertexMap.clear();
        pathVertices.clear();
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

    Vec3F SSEEngine::GetPathVertex(int index) {
        return singlePathMesh->vertices[index];
    }

    int SSEEngine::GetPathEdgeCount() {
        return singlePathMesh->edges.size();
    }

    int SSEEngine::GetEdgeVertexIndex(int index, int side) {
        return singlePathMesh->edges[index].vertexIds[side];
    }
}

#endif
