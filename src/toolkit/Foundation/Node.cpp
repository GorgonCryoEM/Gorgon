/*
 * Node.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Node.h"

#include "GraphMatch/Graph.h"

namespace GraphMatch {

    Node::Node() {
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

    Node::Node(Node * olderNode, NodeStub * olderStub,
                           int n2Node, int dummyHelixCount, int dummySheetCount,
                           bool allowRevisit)
    {
        this->n1Node     = olderNode->n1Node + (char)dummyHelixCount + (char)dummySheetCount + 1;
        this->n2Node     = (char)n2Node;
        this->depth      = olderNode->depth + (char)dummyHelixCount + (char)dummySheetCount + 1;
        this->parentNode = olderStub;
        this->m1Bitmap   = olderNode->m1Bitmap;
        this->m2Bitmap   = olderNode->m2Bitmap;
        // remove all recently matched sequence graph nodes from bitmap
        for(int i = olderNode->n1Node + 1; i <= this->n1Node; i++) {
            Node::RemoveNodeFromBitmap(this->m1Bitmap, i);
        }
        if (!allowRevisit) {
            // remove the recently matched pattern graph node from bitmap
            Node::RemoveNodeFromBitmap(this->m2Bitmap, this->n2Node);
        }
        this->missingNodesUsed      = olderNode->missingNodesUsed      + (char)dummyHelixCount + (char)dummySheetCount;
        this->missingHelixNodesUsed = olderNode->missingHelixNodesUsed + (char)dummyHelixCount;
        this->missingSheetNodesUsed = olderNode->missingSheetNodesUsed + (char)dummySheetCount;
        //cout << "missing nodes=" << (int)this->missingNodesUsed << ", missing helices=" << (int)this->missingHelixNodesUsed << ", missing sheets=" << (int)this->missingSheetNodesUsed << endl;
    }



    vector<int> Node::GetNodeCorrespondence() {
        vector<bool> used(MAX_NODES, false);
        vector<int>    n1(MAX_NODES);
        vector<int>    n2(MAX_NODES);

        NodeStub * currentNode = this;
        int top = 0;

        for(;
                currentNode->parentNode != NULL;
                currentNode = currentNode->parentNode, top++)
        {
            n1[top] = currentNode->n1Node;
            n2[top] = currentNode->n2Node;
            used[(int)currentNode->n1Node] = true;
        }

        for(int i = 1; i <= this->depth; i++) {
            if(!used[i]) {
                n1[top] = i;
                n2[top] = -1;
                top++;
            }
        }

        for(int i = 0; i < top - 1; i++) {
            int minIndex = i;
            for(int j = i+1; j < top; j++) {
                if(n1[minIndex] > n1[j]) {
                    minIndex = j;
                }
            }
            swap(n1[minIndex], n1[i]);
            swap(n2[minIndex], n2[i]);
        }

        vector<int> correspondance;

        for(int i = 0; i < top; i++) {
            if(n2[i] >= 0) {
                correspondance.push_back(n2[i]-1);
            } else {
                correspondance.push_back(n2[i]);
            }
        }
        return correspondance;
    }

    double Node::GetCost() {
        return cost;
    }

    void Node::AddNodeToBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap | ((unsigned long long)1 << node);
    }

    void Node::RemoveNodeFromBitmap(unsigned long long & bitmap, int node) {
        bitmap = bitmap - ((unsigned long long)1 << node);
    }

    bool Node::IsNodeInBitmap(unsigned long long bitmap, int node) {
        unsigned long long bitvalue = ((unsigned long long)1 << node);
        return ((bitmap & bitvalue) == bitvalue);
    }

} /* namespace GraphMatch */
