#ifndef TOOLKIT_GRAPHMATCH_NODE_H
#define TOOLKIT_GRAPHMATCH_NODE_H

#include <vector>
#include <algorithm>
//#include "Core/GlobalConstants.h"

using namespace std;

namespace GraphMatch {

    class GraphNode {
    public:
        vector<int> n1; // Contains the currently matched vertices of the pattern graph
        vector<int> n2; // Contains the currently matched vertices of the base graph
        unsigned long long m1Bitmap;
        unsigned long long m2Bitmap;
        int n1Top, n2Top;
        int missingNodesUsed;

        double cost;
        double costGStar;
    public:
        GraphNode(GraphNode * olderNode);
        GraphNode(GraphNode * olderNode, int insertingNode, int dummyHelixCount);
        GraphNode();
        ~GraphNode();
        void print();
        void PrintNodeConcise();
        void PrintNodeConcise(int rank);
        void SortOnPattern();
        bool operator==(GraphNode &other);
        static void AddNodeToBitmap(unsigned long long & bitmap, int node);
        static void RemoveNodeFromBitmap(unsigned long long & bitmap, int node);
        static bool IsNodeInBitmap(unsigned long long bitmap, int node);
        static int RemoveSmallestNode(unsigned long long & bitmap);
    };

    inline GraphNode::~GraphNode() {
    }

    inline GraphNode::GraphNode()
        : n1(MAX_NODES),
          n2(MAX_NODES)
    {
        cost = 0;
        n1Top = 0;
        n2Top = 0;
        costGStar = 0;
        missingNodesUsed = 0;
        m1Bitmap = 0;
        m2Bitmap = 0;
    }

    inline GraphNode::GraphNode(GraphNode * olderNode)
        : n1(MAX_NODES),
          n2(MAX_NODES)
    {
        for(int i = 0; i < olderNode->n1Top; i++) {
            n1[i] = olderNode->n1[i];
            n2[i] = olderNode->n2[i];
        }
        m1Bitmap = olderNode->m1Bitmap;
        m2Bitmap = olderNode->m2Bitmap;
        cost = olderNode->cost;
        n1Top = olderNode->n1Top;
        n2Top = olderNode->n2Top;
        costGStar = olderNode->costGStar;
        missingNodesUsed = olderNode->missingNodesUsed;
    }

    inline GraphNode::GraphNode(GraphNode * olderNode, int insertingNode, int dummyHelixCount)
        : n1(MAX_NODES),
          n2(MAX_NODES)
    {

        m1Bitmap = olderNode->m1Bitmap;
        m2Bitmap = olderNode->m2Bitmap;

        // Copy the N1
        for(int i = 0; i < olderNode->n1Top; i++) {
            n1[i] = olderNode->n1[i];
        }
        n1Top = olderNode->n1Top;


        // Add dummy N1 nodes + one extra for the node being inserted
        for(int i = 0; i < dummyHelixCount + 1; i++) {
            n1[n1Top] = RemoveSmallestNode(m1Bitmap);
            n1Top++;
        }

        // Copy the N2
        for(int i = 0; i < olderNode->n2Top; i++) {
            n2[i] = olderNode->n2[i];
        }
        n2Top = olderNode->n2Top;

        // Add dummy N2 nodes
        for(int i = 0; i < dummyHelixCount; i++) {
            n2[n2Top] = -1;
            n2Top++;
        }
        // Add the inserted node to N2
        n2[n2Top] = insertingNode;
        n2Top++;

        RemoveNodeFromBitmap(m2Bitmap, insertingNode);

        //Costs
        costGStar = olderNode->costGStar; // Not Accurate
        cost = olderNode->cost;	// Not Accurate
        missingNodesUsed = olderNode->missingNodesUsed + dummyHelixCount;
    }

    inline void GraphNode::print() {
        printf("\t");
        for(int i = 0; i < n1Top; i++) {
            printf("(%d, %d) ", n1[i]+1, n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    inline void GraphNode::PrintNodeConcise() {
        printf("\t");
        for(int i = 0; i < n1Top; i++) {
            printf("%d ", n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    inline void GraphNode::PrintNodeConcise(int rank) {
        printf(" %d)\t", rank);
        for(int i = 0; i < n1Top; i++) {
            printf("%d ", n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    inline void GraphNode::SortOnPattern() {
        sort(n1.begin(), n1.end());
        sort(n2.begin(), n2.end());
    }

    inline bool GraphNode::operator==(GraphNode &other) {
        return (n1Top == other.n1Top) && (n1 == other.n1) && (n2 == other.n2);
    }

    inline void GraphNode::AddNodeToBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap | ((unsigned long long)1 << node);
    }

    inline void GraphNode::RemoveNodeFromBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap - ((unsigned long long)1 << node);
    }

    inline bool GraphNode::IsNodeInBitmap(unsigned long long bitmap, int node) {
        unsigned long long bitvalue = ((unsigned long long)1 << node);
        return ((bitmap & bitvalue) == bitvalue);
    }


    inline int GraphNode::RemoveSmallestNode(unsigned long long & bitmap) {
        for(int i = 1; i <= MAX_NODES; i++) {
            if (IsNodeInBitmap(bitmap, i)) {
                RemoveNodeFromBitmap(bitmap, i);
                return i;
            }
        }
        assert(false);
        return 0;
    }
}
#endif
