/*
 * Node.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_NODE_H_
#define SRC_TOOLKIT_FOUNDATION_NODE_H_

#include "NodeStub.h"

#include <vector>

using std::vector;

namespace GraphMatch {

    class Node : public NodeStub {
        public:
            unsigned long long m1Bitmap;
            unsigned long long m2Bitmap;
            char missingNodesUsed;
            char missingHelixNodesUsed;
            char missingSheetNodesUsed;
            char depth;
            double cost;
            double costGStar;
        public:
            Node();
            Node(Node * olderNode, NodeStub * olderStub,
                 int n2Node, int dummyHelixCount, int dummySheetCount,
                 bool allowRevisit);

            vector<int> GetNodeCorrespondence();
            double GetCost();
            static void AddNodeToBitmap(unsigned long long & bitmap, int node);
            static void RemoveNodeFromBitmap(unsigned long long & bitmap, int node);
            static bool IsNodeInBitmap(unsigned long long bitmap, int node);
    };

    bool operator<(const Node & l, const Node & r) {
        return l.cost < r.cost;
    }

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_FOUNDATION_NODE_H_ */
