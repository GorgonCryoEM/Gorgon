#ifndef TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H
#define TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H

#include "Foundation/LinkedNode.h"
//#include <vector>
//#include <cstdio>

using namespace std;

namespace GraphMatch {

    class SSEResult {
    public:
        SSEResult();
        SSEResult(LinkedNode * node, int helixCount);
        SSEResult(vector<int> correspondence, double cost, int helixCount);

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

    SSEResult::SSEResult()
    					: cost(0), helixCount(0)
    {}

    SSEResult::SSEResult(LinkedNode * node, int nHelix)
    					: correspondence(node->GetNodeCorrespondence()),
						  cost(node->GetCost()),
						  helixCount(nHelix)
    {}

    SSEResult::SSEResult(vector<int> corr, double cst, int nHelix)
						: correspondence(corr),
						  cost(cst),
						  helixCount(nHelix)
    {}

    double SSEResult::GetCost() {
        return cost;
    }

    int SSEResult::GetNodeCount() {
        return correspondence.size();
    }

    int SSEResult::GetHelixCount() {
        return helixCount;
    }

    int SSEResult::GetSkeletonNode(int sequenceNode) {
        return correspondence[sequenceNode];
    }

    int SSEResult::NodeToHelix(int nodeId) {
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
