#ifndef CORE_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H
#define CORE_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H

#include "LinkedNode.h"
//#include <vector>
//#include <cstdio>

using namespace std;

namespace GraphMatch {

    class SSECorrespondenceResult {
    public:
        SSECorrespondenceResult();
        SSECorrespondenceResult(LinkedNode * node, int helixCount);
        SSECorrespondenceResult(vector<int> correspondence, double cost, int helixCount);

        double GetCost();
        int GetNodeCount();
        int GetHelixCount();
        int GetSheetCount();
        int GetSkeletonNode(int sequenceNode);
        int NodeToHelix(int nodeId);
    private:
        vector<int> correspondence;
        double cost;
        int helixCount;
        int sheetCount;
    };

    SSECorrespondenceResult::SSECorrespondenceResult()
    					: cost(0), helixCount(0), sheetCount(0)
    {}

    SSECorrespondenceResult::SSECorrespondenceResult(LinkedNode * node, int helixCount){
        correspondence = node->GetNodeCorrespondence();
        cost = node->GetCost();
        this->helixCount = helixCount;
    }

    SSECorrespondenceResult::SSECorrespondenceResult(vector<int> correspondence, double cost, int helixCount){
        this->correspondence = correspondence;
        this->cost = cost;
        this->helixCount = helixCount;
    }

    double SSECorrespondenceResult::GetCost() {
        return cost;
    }

    int SSECorrespondenceResult::GetNodeCount() {
        return correspondence.size();
    }

    int SSECorrespondenceResult::GetHelixCount() {
        return helixCount;
    }

    int SSECorrespondenceResult::GetSheetCount() {
        // TODO: fix
        return -1;
    }

    int SSECorrespondenceResult::GetSkeletonNode(int sequenceNode) {
        return correspondence[sequenceNode];
    }

    int SSECorrespondenceResult::NodeToHelix(int nodeId) {
        if(nodeId >= 0) {
            if (nodeId < 2 * helixCount) {
                return (int)(nodeId / 2);
            } else {
                return (int)(nodeId - helixCount);
            }
        } else {
            return nodeId;
        }
    }

}

#endif
