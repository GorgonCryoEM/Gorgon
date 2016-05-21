#ifndef TOOLKIT_GRAPHMATCH_STANDARDNODE_H
#define TOOLKIT_GRAPHMATCH_STANDARDNODE_H

#include <vector>
//#include "Core/GlobalConstants.h"

using namespace std;

namespace GraphMatch {

    typedef int MatchingList[MAX_NODES];

    class GraphNode {
    public:
        MatchingList n1; // Contains the currently matched vertices of the pattern graph
        MatchingList n2; // Contains the currently matched vertices of the base graph
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
        void PrintNode();
        void PrintNodeConcise();
        void PrintNodeConcise(int rank);
        void SortOnPattern();
        bool operator==(GraphNode &other);
        static void AddNodeToBitmap(unsigned long long & bitmap, int node);
        static void RemoveNodeFromBitmap(unsigned long long & bitmap, int node);
        static bool IsNodeInBitmap(unsigned long long bitmap, int node);
        static int RemoveSmallestNode(unsigned long long & bitmap);
    };

    GraphNode::~GraphNode() {
    }

    GraphNode::GraphNode() {
        cost = 0;
        n1Top = 0;
        n2Top = 0;
        costGStar = 0;
        missingNodesUsed = 0;
        m1Bitmap = 0;
        m2Bitmap = 0;
    }

    GraphNode::GraphNode(GraphNode * olderNode) {
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

    GraphNode::GraphNode(GraphNode * olderNode, int insertingNode, int dummyHelixCount) {

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

    void GraphNode::PrintNode() {
        printf("\t");
        for(int i = 0; i < n1Top; i++) {
            printf("(%d, %d) ", n1[i]+1, n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    void GraphNode::PrintNodeConcise() {
        printf("\t");
        for(int i = 0; i < n1Top; i++) {
            printf("%d ", n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    void GraphNode::PrintNodeConcise(int rank) {
        printf(" %d)\t", rank);
        for(int i = 0; i < n1Top; i++) {
            printf("%d ", n2[i]+1);
        }
        printf(" - %f\n", cost);
    }

    void GraphNode::SortOnPattern() {
        int temp;
        int minIndex;
        for(int i = 0; i < n1Top -1; i++) {
            minIndex = i;
            for(int j = i+1; j < n1Top; j++) {
                if(n1[j] < n1[minIndex]) {
                    minIndex = j;
                }
            }

            temp = n1[i];	n1[i] = n1[minIndex];	n1[minIndex] = temp;
            temp = n2[i];	n2[i] = n2[minIndex];	n2[minIndex] = temp;
        }
    }

    bool GraphNode::operator==(GraphNode &other) {
        if(n1Top != other.n1Top) {
            return false;
        }

        bool returnValue = true;
        for(int i = 0; i < n1Top; i++) {
            returnValue = returnValue && (n1[i] == other.n1[i]) && (n2[i] == other.n2[i]);
        }
        return returnValue;
    }

    void GraphNode::AddNodeToBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap | ((unsigned long long)1 << node);
    }

    void GraphNode::RemoveNodeFromBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap - ((unsigned long long)1 << node);
    }

    bool GraphNode::IsNodeInBitmap(unsigned long long bitmap, int node) {
        unsigned long long bitvalue = ((unsigned long long)1 << node);
        return ((bitmap & bitvalue) == bitvalue);
    }


    int GraphNode::RemoveSmallestNode(unsigned long long & bitmap) {
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
