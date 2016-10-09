#ifndef TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H
#define TOOLKIT_GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H

#include "Foundation/Node.h"
//#include <vector>
//#include <cstdio>
//#include "Core/Output.h"

using namespace std;
//using namespace Core;

namespace GraphMatch {

//    template <class T>
//    inline ostream & operator<<(ostream & out, const vector<T> & obj) {
//            for(typename vector<T>::const_iterator it=obj.begin();
//                    it!=obj.end();
//                    ++it)
//
//                out<<"\t\t\t"<<*it
//                <<endl;
//
//            return out;
//    }

    class SSEResult {
    public:
        SSEResult();
        SSEResult(Node * node, int helixCount);
        SSEResult(vector<int> correspondence, double cost, int helixCount);

        double getCost();
        int getNodeCount();
        int getHelixCount();
        int getSkeletonNode(int sequenceNode);
        int nodeToHelix(int nodeId);
    private:
        vector<int> correspondence;
        double cost;
        int helixCount;

        friend ostream & operator<<(ostream & out, const SSEResult & obj) {
            return out
//                    <<"corespondences: "<<obj.correspondence
                    <<endl
                    <<"  cost: "<<obj.cost
                    <<"  helixCount: "<<obj.helixCount
                    <<endl;
        }
    };

    inline SSEResult::SSEResult()
    					: cost(0), helixCount(0)
    {}

    inline SSEResult::SSEResult(Node * node, int nHelix)
    					: correspondence(node->GetNodeCorrespondence()),
						  cost(node->GetCost()),
						  helixCount(nHelix)
    {}

    inline SSEResult::SSEResult(vector<int> corr, double cst, int nHelix)
						: correspondence(corr),
						  cost(cst),
						  helixCount(nHelix)
    {}

    inline double SSEResult::getCost() {
        return cost;
    }

    inline int SSEResult::getNodeCount() {
        return correspondence.size();
    }

    inline int SSEResult::getHelixCount() {
        return helixCount;
    }

    inline int SSEResult::getSkeletonNode(int sequenceNode) {
        return correspondence[sequenceNode];
    }

    inline int SSEResult::nodeToHelix(int nodeId) {
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
