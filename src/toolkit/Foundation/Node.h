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
#include <iostream>

using std::vector;
using std::ostream;

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

    inline bool operator<(const Node & l, const Node & r) {
        return l.cost < r.cost;
    }
    
    inline ostream& operator<<(ostream &out, const Node& obj) {
        return out
            <<" "<<obj.m1Bitmap
            <<" "<<obj.m2Bitmap
            <<" "<<(int)obj.missingNodesUsed
            <<" "<<(int)obj.missingHelixNodesUsed
            <<" "<<(int)obj.missingSheetNodesUsed
            <<" "<<(int)obj.depth
            <<" "<<obj.cost
            <<" "<<obj.costGStar;
    }

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_FOUNDATION_NODE_H_ */
