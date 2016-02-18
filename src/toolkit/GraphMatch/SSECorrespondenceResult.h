#ifndef TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H
#define TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H

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
        int GetSkeletonNode(int sequenceNode);
        int NodeToHelix(int nodeId);
    private:
        vector<int> correspondence;
        double cost;
        int helixCount;
    };

    SSECorrespondenceResult::SSECorrespondenceResult()
    					: cost(0), helixCount(0)
    {}

    SSECorrespondenceResult::SSECorrespondenceResult(LinkedNode * node, int nHelix)
    					: correspondence(node->GetNodeCorrespondence()),
						  cost(node->GetCost()),
						  helixCount(nHelix)
    {}

    SSECorrespondenceResult::SSECorrespondenceResult(vector<int> corr, double cst, int nHelix)
						: correspondence(corr),
						  cost(cst),
						  helixCount(nHelix)
    {}

    double SSECorrespondenceResult::GetCost() {
        return cost;
    }

    int SSECorrespondenceResult::GetNodeCount() {
        return correspondence.size();
    }

    int SSECorrespondenceResult::GetHelixCount() {
        return helixCount;
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
