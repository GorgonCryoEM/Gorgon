/*
Storage structure for a node in the graph traversal.

Author: Sasakthi S. Abeysinghe
Date  : 01/23/2006
*/

#ifndef STANDARDNODE_H
#define STANDARDNODE_H

#include <vector>
#include "GlobalConstants.h"

using namespace std;

typedef int MatchingList[MAX_NODES];

class StandardNode {
public:
	MatchingList n1; // Contains the currently matched vertices of the pattern graph
	MatchingList n2; // Contains the currently matched vertices of the base graph
	MatchingList m1; // Contains the currently unmatched vertices of the pattern graph
	MatchingList m2; // Contains the currently unmatched vertices of the base graph
	int n1Top, n2Top, m1Top, m2Top;
	int missingNodesUsed;

	double cost;
	double costGStar;
public:
	StandardNode(StandardNode * olderNode);
	StandardNode(StandardNode * olderNode, int insertIndex);
	StandardNode(StandardNode * olderNode, int insertIndex, int dummyHelixCount);
	StandardNode();
	~StandardNode();
	void PrintNode();
	void PrintNodeConcise();
	void PrintNodeConcise(int rank);
	void SortOnPattern();
	bool operator==(StandardNode &other);
};

StandardNode::~StandardNode() {
}

StandardNode::StandardNode() {
	cost = 0;
	n1Top = 0;
	n2Top = 0;
	m1Top = 0;
	m2Top = 0;
	costGStar = 0;
	missingNodesUsed = 0;
}

StandardNode::StandardNode(StandardNode * olderNode) {
	for(int i = 0; i < olderNode->n1Top; i++) {
		n1[i] = olderNode->n1[i];
		n2[i] = olderNode->n2[i];
	}
	for(int i = 0; i < olderNode->m1Top; i++) {
		m1[i] = olderNode->m1[i];
	}
	for(int i = 0; i < olderNode->m2Top; i++) {
		m2[i] = olderNode->m2[i];
	}
	cost = olderNode->cost;
	n1Top = olderNode->n1Top;
	n2Top = olderNode->n2Top;
	m1Top = olderNode->m1Top;
	m2Top = olderNode->m2Top;
	costGStar = olderNode->costGStar;
	missingNodesUsed = olderNode->missingNodesUsed;
}


StandardNode::StandardNode(StandardNode * olderNode, int insertIndex) {
	for(int i = 0; i < olderNode->n1Top; i++) {
		n1[i] = olderNode->n1[i];
	}

	n1[olderNode->n1Top] = olderNode->m1[olderNode->m1Top-1];			
	n1Top = olderNode->n1Top+1;

	for(int i = 0; i < olderNode->m1Top-1; i++) {
		m1[i] = olderNode->m1[i];
	}
	m1Top = olderNode->m1Top-1;

	for(int i = 0; i < olderNode->n1Top; i++) {
		n2[i] = olderNode->n2[i];
	}
	n2[olderNode->n2Top] = olderNode->m2[insertIndex];
	n2Top = olderNode->n2Top+1;

	m2Top = 0;
	for(int i = 0; i < olderNode->m2Top; i++) {
		if(i != insertIndex) {
			m2[m2Top] = olderNode->m2[i];
			m2Top++;
		}
	}
	costGStar = olderNode->costGStar; // Not Accurate
	cost = olderNode->cost;	// Not Accurate
	missingNodesUsed = olderNode->missingNodesUsed;
}

StandardNode::StandardNode(StandardNode * olderNode, int insertIndex, int dummyHelixCount) {
	// Copy the N1
	for(int i = 0; i < olderNode->n1Top; i++) {
		n1[i] = olderNode->n1[i];
	}
	n1Top = olderNode->n1Top;

	// Add dummy N1 nodes + one extra for the node being inserted
	for(int i = 0; i < dummyHelixCount + 1; i++) {
		n1[n1Top] = olderNode->m1[olderNode->m1Top - i - 1];
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
	n2[n2Top] = olderNode->m2[insertIndex];
	n2Top++;

	// Copy the M1 Nodes
	m1Top = 0;
	for(int i = 0; i < olderNode->m1Top - dummyHelixCount - 1; i++) {
		m1[m1Top] = olderNode->m1[i];
		m1Top++;
	}

	// Copy the M2 Nodes
	m2Top = 0;
	for(int i = 0; i < olderNode->m2Top; i++) {
		if(i != insertIndex) {
			m2[m2Top] = olderNode->m2[i];
			m2Top++;
		}
	}

	//Costs
	costGStar = olderNode->costGStar; // Not Accurate
	cost = olderNode->cost;	// Not Accurate
	missingNodesUsed = olderNode->missingNodesUsed + dummyHelixCount;
}

void StandardNode::PrintNode() {
	printf("\t");
	for(int i = 0; i < n1Top; i++) {
		printf("(%d, %d) ", n1[i]+1, n2[i]+1);
	}
	printf(" - %f\n", cost);
}

void StandardNode::PrintNodeConcise() {
	printf("\t");
	for(int i = 0; i < n1Top; i++) {
		printf("%d ", n2[i]+1);
	}
	printf(" - %f\n", cost);
}

void StandardNode::PrintNodeConcise(int rank) {
	printf(" %d)\t", rank);
	for(int i = 0; i < n1Top; i++) {
		printf("%d ", n2[i]+1);
	}
	printf(" - %f\n", cost);
}

void StandardNode::SortOnPattern() {	
	int temp;
	int minIndex;
	for(int i = 0; i < n1Top -1; i++) {
		minIndex = i+1;
		for(int j = i; j < n1Top; j++) {
			if(n1[j] < n1[minIndex]) {
				minIndex = j;
			}
		}
		
		temp = n1[i];	n1[i] = n1[minIndex];	n1[minIndex] = temp;
		temp = n2[i];	n2[i] = n2[minIndex];	n2[minIndex] = temp;
	}
}

bool StandardNode::operator==(StandardNode &other) {
	if(n1Top != other.n1Top) {
		return false; 
	}

	int returnValue = true;
	for(int i = 0; i < n1Top; i++) {
		returnValue = returnValue && (n1[i] == other.n1[i]) && (n2[i] == other.n2[i]);
	}
	return returnValue;
}

#endif