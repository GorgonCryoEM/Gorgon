#ifndef TOOLKIT_GRAPHMATCH_LINKEDNODE_H
#define TOOLKIT_GRAPHMATCH_LINKEDNODE_H

//#include <vector>
//#include "Core/GlobalConstants.h"
#include "LinkedNodeStub.h"
//#include <string>
//#include <cassert>
#include "GraphMatch/StandardGraph.h"

using namespace std;

namespace GraphMatch {
    class LinkedNode : public LinkedNodeStub {
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
        LinkedNode(LinkedNode * olderNode);
        LinkedNode();
        LinkedNode(LinkedNode * olderNode, LinkedNodeStub * olderStub, int n2Node, int dummyHelixCount, int dummySheetCount, bool allowRevisit);

        ~LinkedNode();
        vector<int> GetNodeCorrespondence();
        double GetCost();
        bool IsUserSpecifiedSolution();
        static void AddNodeToBitmap(unsigned long long & bitmap, int node);
        static void RemoveNodeFromBitmap(unsigned long long & bitmap, int node);
        static bool IsNodeInBitmap(unsigned long long bitmap, int node);
        static int RemoveSmallestNode(unsigned long long & bitmap);
    };

    LinkedNode::~LinkedNode() {
    }

    LinkedNode::LinkedNode() {
        cost = 0;
        n1Node = 0;
        n2Node = 0;
        parentNode = NULL;
        costGStar = 0;
        missingNodesUsed = 0;
        missingHelixNodesUsed = 0;
        missingSheetNodesUsed = 0;
        m1Bitmap = 0;
        m2Bitmap = 0;
        depth = 0;
    }

    LinkedNode::LinkedNode(LinkedNode * olderNode) {
        n1Node = olderNode->n1Node;
        n2Node = olderNode->n2Node;
        parentNode = olderNode->parentNode;
        m1Bitmap = olderNode->m1Bitmap;
        m2Bitmap = olderNode->m2Bitmap;
        cost = olderNode->cost;
        costGStar = olderNode->costGStar;
        missingNodesUsed = olderNode->missingNodesUsed;
        missingHelixNodesUsed = olderNode->missingHelixNodesUsed;
        missingSheetNodesUsed = olderNode->missingSheetNodesUsed;
        depth = olderNode->depth;
    }



    LinkedNode::LinkedNode(LinkedNode * olderNode, LinkedNodeStub * olderStub, int n2Node, int dummyHelixCount, int dummySheetCount, bool allowRevisit) {
        this->n1Node = olderNode->n1Node + (char)dummyHelixCount + (char)dummySheetCount + 1;
        this->n2Node = (char)n2Node;
        this->depth = olderNode->depth + (char)dummyHelixCount + (char)dummySheetCount + 1;
        this->parentNode = olderStub;
        this->m1Bitmap = olderNode->m1Bitmap;
        this->m2Bitmap = olderNode->m2Bitmap;
        // remove all recently matched sequence graph nodes from bitmap
        for(int i = olderNode->n1Node + 1; i <= this->n1Node; i++) {
            LinkedNode::RemoveNodeFromBitmap(this->m1Bitmap, i);
        }
        if (!allowRevisit) {
            // remove the recently matched pattern graph node from bitmap
            LinkedNode::RemoveNodeFromBitmap(this->m2Bitmap, this->n2Node);
        }
        this->missingNodesUsed = olderNode->missingNodesUsed + (char)dummyHelixCount + (char)dummySheetCount;
        this->missingHelixNodesUsed = olderNode->missingHelixNodesUsed + (char)dummyHelixCount;
        this->missingSheetNodesUsed = olderNode->missingSheetNodesUsed + (char)dummySheetCount;
        //cout << "missing nodes=" << (int)this->missingNodesUsed << ", missing helices=" << (int)this->missingHelixNodesUsed << ", missing sheets=" << (int)this->missingSheetNodesUsed << endl;
    }



    vector<int> LinkedNode::GetNodeCorrespondence() {
        bool used[MAX_NODES];
        int n1[MAX_NODES];
        int n2[MAX_NODES];
        int top = 0;
        for(int i = 0; i < MAX_NODES; i++) {
            used[i] = false;
        }

        LinkedNodeStub * currentNode = this;
        bool continueLoop = true;
        while(continueLoop) {
            if(currentNode->parentNode == NULL) {
                 break;
            }
            n1[top] = currentNode->n1Node;
            n2[top] = currentNode->n2Node;
            used[(int)currentNode->n1Node] = true;
            top++;
            currentNode = currentNode->parentNode;
        }

        for(int i = 1; i <= this->depth; i++) {
            if(!used[i]) {
                n1[top] = i;
                n2[top] = -1;
                top++;
            }
        }

        int minIndex;
        int temp;
        for(int i = 0; i < top - 1; i++) {
            minIndex = i;
            for(int j = i+1; j < top; j++) {
                if(n1[minIndex] > n1[j]) {
                    minIndex = j;
                }
            }
            temp = n1[minIndex];
            n1[minIndex] = n1[i];
            n1[i] = temp;

            temp = n2[minIndex];
            n2[minIndex] = n2[i];
            n2[i] = temp;
        }

        vector<int> correspondance;
        correspondance.clear();

        for(int i = 0; i < top; i++) {
            if(n2[i] >= 0) {
                correspondance.push_back(n2[i]-1);
            } else {
                correspondance.push_back(n2[i]);
            }
        }
        return correspondance;
    }

    double LinkedNode::GetCost() {
        return cost;
    }

    bool LinkedNode::IsUserSpecifiedSolution() {
        bool used[MAX_NODES];
        int n1[MAX_NODES];
        int n2[MAX_NODES];
        int top = 0;
        for(int i = 0; i < MAX_NODES; i++) {
            used[i] = false;
        }

        LinkedNodeStub * currentNode = this;
        bool continueLoop = true;
        while(continueLoop) {
            if(currentNode->parentNode == NULL) {
                 break;
            }
            n1[top] = currentNode->n1Node;
            n2[top] = currentNode->n2Node;
            used[(int)currentNode->n1Node] = true;
            top++;
            currentNode = currentNode->parentNode;
        }

        for(int i = 1; i <= this->depth; i++) {
            if(!used[i]) {
                n1[top] = i;
                n2[top] = -1;
                top++;
            }
        }

        int minIndex;
        int temp;
        for(int i = 0; i < top - 1; i++) {
            minIndex = i;
            for(int j = i+1; j < top; j++) {
                if(n1[minIndex] > n1[j]) {
                    minIndex = j;
                }
            }
            temp = n1[minIndex];
            n1[minIndex] = n1[i];
            n1[i] = temp;

            temp = n2[minIndex];
            n2[minIndex] = n2[i];
            n2[i] = temp;
        }

        bool isSolution = true;
        for(int i = 0; i < top; i++) {
            isSolution = (isSolution && (n2[i] == SOLUTION[i]));
        }
        return isSolution;
    }

    void LinkedNode::AddNodeToBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap | ((unsigned long long)1 << node);
    }

    void LinkedNode::RemoveNodeFromBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap - ((unsigned long long)1 << node);
    }

    bool LinkedNode::IsNodeInBitmap(unsigned long long bitmap, int node) {
        unsigned long long bitvalue = ((unsigned long long)1 << node);
        return ((bitmap & bitvalue) == bitvalue);
    }


    int LinkedNode::RemoveSmallestNode(unsigned long long & bitmap) {
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
