/*
Graph Matching algorithm based on the following paper:
  Wong et al., An algorithm for graph optimal monomorphism, 
  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636

  Modified to support missing helixes.

Author: Sasakthi S. Abeysinghe
Date  : 08/14/2006
*/

#ifndef WONGMATCHMISSING_H
#define WONGMATCHMISSING_H

#include "StandardGraph.h"
#include "StandardNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"
#include <time.h>
#include "../SkeletonMaker/PriorityQueue.h"

class WongMatchMissing{
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
public:
	WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph);
	WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~WongMatchMissing();
	void RunMatching(clock_t startTime);
	void SaveResults();


private:
	StandardNode * currentNode;
	PriorityQueue<StandardNode, double> * queue;
	int missingHelixCount;
	int missingSheetCount;
	bool newBaseGraph;
	int expandCount;
	int foundCount;
	int longestMatch;
private:
	clock_t timeInGetA;
	clock_t timeInGetB;
	clock_t timeInQueue;
	void Init(StandardGraph * patternGraph, StandardGraph * baseGraph);
	double GetC(int p, int qp);
	double GetC(int j, int p, int qj, int qp);
	double GetCost(int d, int m, int qj, int qp);
	double GetCPrime(int a, int b, int c, int d);
	double GetK(int p, int qp);
	double GetKPrime(int i, int q);
	double GetA();
	double GetB();
	double GetF();
	void PrintCurrentNode();
	void PopBestNode(); // Gets the best (first) node from the active nodes list.
	void ExpandNode();  // Expands all the children of the current node.
	void NormalizeGraphs();
	void HandleMissingHelixes();

};

WongMatchMissing::WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph) {
	Init(patternGraph, baseGraph);
}

WongMatchMissing::WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	Init(patternGraph, baseGraph);
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
}

WongMatchMissing::~WongMatchMissing() {
	delete queue;

	if(newBaseGraph) {
		baseGraph->skeletonHelixes.clear();
		delete baseGraph;
	}
}
void WongMatchMissing::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {
	queue = new PriorityQueue<StandardNode, double> (PRIORITYQUEUESIZE);
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	expandCount = 0;
	newBaseGraph = false;
	
	currentNode = new StandardNode();
	currentNode->cost = 0;
	for(int i = 0; i < patternGraph->nodeCount; i++) {
		StandardNode::AddNodeToBitmap(currentNode->m1Bitmap, i+1);
	}
	for(int i = 0; i < baseGraph->nodeCount; i++) {
		StandardNode::AddNodeToBitmap(currentNode->m2Bitmap, i+1);
	}
	queue->add(currentNode, currentNode->cost);

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;

	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
	}
	foundCount = 0;
	longestMatch = 0;
	timeInGetA = 0;
	timeInGetB = 0;
	timeInQueue = 0;
}


void WongMatchMissing::HandleMissingHelixes() {
	if(missingHelixCount == 0) 
		return;
	printf("Modifying the graph to handle missing helixes\n");

	// Get a count on how many new nodes are needed
	int loopCount = 0;
	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j < i; j++) {
			if((baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP) || (baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP_EUCLIDEAN)){
				loopCount++;
			}			
		}
	}
	
	// Creating the new graph
	StandardGraph * newBase = new StandardGraph(baseGraph->nodeCount + loopCount * missingHelixCount * 2);
	int lastNode;
	int nodeCount = baseGraph->nodeCount;

	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j <= i; j++) {
			newBase->adjacencyMatrix[i][j][0] = baseGraph->adjacencyMatrix[i][j][0];
			newBase->adjacencyMatrix[i][j][1] = baseGraph->adjacencyMatrix[i][j][1];
			newBase->adjacencyMatrix[j][i][0] = baseGraph->adjacencyMatrix[j][i][0];
			newBase->adjacencyMatrix[j][i][1] = baseGraph->adjacencyMatrix[j][i][1];

			if((baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP) || (baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP_EUCLIDEAN)) {
				lastNode = i;
				for(int k = 0; k < missingHelixCount; k++) {
					// First Loop
					newBase->adjacencyMatrix[lastNode][nodeCount][1] = 0;
					newBase->adjacencyMatrix[nodeCount][lastNode][1] = 0;
					newBase->adjacencyMatrix[lastNode][nodeCount][0] = GRAPHEDGE_LOOP;
					newBase->adjacencyMatrix[nodeCount][lastNode][0] = GRAPHEDGE_LOOP;
					// Missing Helix
					nodeCount++;
					newBase->adjacencyMatrix[nodeCount-1][nodeCount][1] = 0;
					newBase->adjacencyMatrix[nodeCount][nodeCount-1][1] = 0;
					newBase->adjacencyMatrix[nodeCount-1][nodeCount][0] = GRAPHEDGE_HELIX;
					newBase->adjacencyMatrix[nodeCount][nodeCount-1][0] = GRAPHEDGE_HELIX;
					newBase->adjacencyMatrix[nodeCount-1][nodeCount-1][1] = 0;
					newBase->adjacencyMatrix[nodeCount][nodeCount][1] = 0;
					// Loop Back
					newBase->adjacencyMatrix[nodeCount][j][1] = baseGraph->adjacencyMatrix[i][j][1];
					newBase->adjacencyMatrix[j][nodeCount][1] = baseGraph->adjacencyMatrix[i][j][1];
					newBase->adjacencyMatrix[nodeCount][j][0] = GRAPHEDGE_LOOP;
					newBase->adjacencyMatrix[j][nodeCount][0] = GRAPHEDGE_LOOP;
					lastNode = nodeCount;
					nodeCount++;
				}				
			}
		}
	}

	for(unsigned int i = 0; i < baseGraph->skeletonHelixes.size(); i++) {
		newBase->skeletonHelixes.push_back(baseGraph->skeletonHelixes[i]);
	}

	assert(nodeCount == baseGraph->nodeCount + loopCount * missingHelixCount * 2);

	baseGraph = newBase;
	baseGraph->PrintGraph();
	newBaseGraph = true;
	
}

void WongMatchMissing::RunMatching(clock_t startTime) {
	bool continueLoop = true;
	clock_t finishTime;
	while(continueLoop)
	{
		PopBestNode();
		//PrintCurrentNode();
		if(currentNode->n1Top == patternGraph->nodeCount) {						
			finishTime = clock();

			printf("\t");
			for(int i = 0; i < currentNode->n1Top; i++) {
				printf("%d ", currentNode->n2[i]);
			}
			printf(" - %f : (%d expanded) (%f seconds)\n", currentNode->cost, expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC);
			foundCount++;
		} else {
			ExpandNode();
		}

		continueLoop = (foundCount < RESULT_COUNT);
		if(continueLoop) {
			delete currentNode;
		}
	}
}

void WongMatchMissing::SaveResults(){
	//printf("\t");
	//for(int i = 0; i < currentNode->n1Top; i++) {
	//	printf("%d ", currentNode->n2[i]);
	//}
	//printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
	//printf("%d\t\t", expandCount);
	delete currentNode;	
	printf("Time taken in GetA %f\n", timeInGetA / (double)CLOCKS_PER_SEC);
	printf("Time taken in GetB %f\n", timeInGetB / (double)CLOCKS_PER_SEC);
	printf("Time taken in Queue %f\n", timeInQueue / (double)CLOCKS_PER_SEC);

}

double WongMatchMissing::GetC(int p, int qp) {
	return GetC(p, p, qp, qp);
}

double WongMatchMissing::GetC(int j, int p, int qj, int qp) {

	double jpCost;
	double qjqpCost;
	double typeCost = 0;

	if(patternGraph->adjacencyMatrix[j-1][p-1][1] == MAXINT) {
		jpCost = 1000;
	} else {
		jpCost = patternGraph->adjacencyMatrix[j-1][p-1][1];
	}

	if(baseGraph->adjacencyMatrix[qj-1][qp-1][1] == MAXINT) {
		qjqpCost = 1000;
	} else {
		qjqpCost = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
	}

	if(patternGraph->adjacencyMatrix[j-1][p-1][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) {
		typeCost = 1000;
	}

	return fabs(jpCost - qjqpCost) + typeCost;
}

double WongMatchMissing::GetCost(int d, int m, int qj, int qp) {
	double patternLength = 0;
	double baseLength;
	double additionalCosts = 0;

	// Adding the length of the skipped helixes
	for(int i = 1; i < m; i++) {
		patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
	}

	// Adding the length of the edges
	bool firstIsLoop;
	bool lastIsLoop;
	for(int i = 0; i < m; i++) {
		lastIsLoop = ((patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP) || (patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP_EUCLIDEAN));
		if(i==0) {
			firstIsLoop = lastIsLoop;
		}
		patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i][1];		
	}


	bool euclideanEstimate = false;
	double weight = 1.0;

	if(qj == -1) { // special handling for missing helixes at the ends
		baseLength = 0;
		switch((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01)) {
			case(GRAPHEDGE_HELIX) : 
				weight = HELIX_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_LOOP):
				weight = LOOP_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_SHEET):
				weight = SHEET_WEIGHT_COEFFICIENT;
				break;
		}
	}
	else {
		assert(baseGraph->EdgeExists(qj-1,qp-1));		
		baseLength = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
		euclideanEstimate = (baseGraph->adjacencyMatrix[qj-1][qp-1][0] == GRAPHEDGE_LOOP_EUCLIDEAN);
		switch((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01)) {
			case(GRAPHEDGE_HELIX) : 
				weight = HELIX_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_LOOP):
				weight = LOOP_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_SHEET):
				weight = SHEET_WEIGHT_COEFFICIENT;
				break;
		}
		weight = euclideanEstimate? weight * EUCLIDEAN_LOOP_PENALTY: weight;
	}


	if(m == 1) {		
		if((qj!= -1) && (patternGraph->adjacencyMatrix[d-1][d][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) &&
			!((patternGraph->adjacencyMatrix[d-1][d][0] == GRAPHEDGE_LOOP) && (baseGraph->adjacencyMatrix[qj-1][qp-1][0] == GRAPHEDGE_LOOP_EUCLIDEAN))) 	{
			additionalCosts += 1000;
		}
	} else {
		if(!firstIsLoop || !lastIsLoop) {
			additionalCosts += 1000;
		}
	}


	switch(COST_FUNCTION)
	{
	case(1):
		return weight * fabs(patternLength - baseLength) + additionalCosts;
		break;
	case(2):
		return weight * fabs(patternLength - baseLength) / (patternLength + baseLength) + additionalCosts;
		break;
	case(3):
		return weight * pow((patternLength - baseLength),2) + additionalCosts;
		break;
	}
	return 0;
}

double WongMatchMissing::GetCPrime(int a, int b, int c, int d) {
	// Performance optimization on the symmetry of the graph 
	// return GetC(a, b, c, d) + GetC(b, a, d, c); 
	return 2*GetC(a, b, c, d);
}



double WongMatchMissing::GetK(int p, int qp){
	double cost = GetC(p, qp);
	if(currentNode->n1Top - 2 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 2], p, currentNode->n2[currentNode->n2Top - 2], qp);
	}
	return cost;
}
double WongMatchMissing::GetKPrime(int i, int q) {
	double cost = GetC(i, q);
	if(currentNode->n1Top - 1 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 1], i, currentNode->n2[currentNode->n2Top - 1], q);
	}
	return cost;
}

double WongMatchMissing::GetA() {
	clock_t startTime = clock();
	double cost = 0;
	double minCost;
	// Cost for node matching
	for(int i = 1; i <= patternGraph->nodeCount; i++)
	{
		if(StandardNode::IsNodeInBitmap(currentNode->m1Bitmap, i)) {			
			minCost = MISSING_HELIX_PENALTY;
			for(int j = 1; j <= baseGraph->nodeCount; j++) {
				if(StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, j)) {
					minCost = min(minCost, GetC(i, j));
				}
			}
			cost += minCost;
		}
	}

	// Cost for the first edge matched
	if(currentNode->n1Top != patternGraph->nodeCount) {
		int lastPatternNode = currentNode->n1[currentNode->n1Top-1];
		int lastBaseNode = currentNode->n2[currentNode->n2Top-1];
		int usableEdgeCount = missingHelixCount * 2 - currentNode->missingNodesUsed + 1;
		minCost = (usableEdgeCount > 1) ? MISSING_HELIX_PENALTY : MAXINT;
		for(int i = 1; i <= baseGraph->nodeCount; i++) {
			if((StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, i)) && (baseGraph->EdgeExists(lastBaseNode - 1, i - 1))) {
				for(int j = 1; j <= min(patternGraph->nodeCount - currentNode->n1Top, usableEdgeCount); j++) {
					minCost = min(minCost, GetCost(lastPatternNode, j, lastBaseNode, i) / (double)j);
				}
			}
		}
		cost += minCost;
	}
	timeInGetA += clock() - startTime;
	return cost;
}
double WongMatchMissing::GetB() {
	clock_t startTime = clock();
	
	double minCost;
	double cost = 0;

	for(int k = 1; k < patternGraph->nodeCount; k++) {
		if(StandardNode::IsNodeInBitmap(currentNode->m1Bitmap, k)) {
			minCost = MISSING_HELIX_PENALTY;

			for(int m = 1; m <= missingHelixCount * 2 - currentNode->missingNodesUsed + 1; m++) {
				for(int l = 0; l <= m - 1; l++) {				
					if((k - l >= 1) && (k - l + m <= patternGraph->nodeCount)) {
						for(int i = 1; i < baseGraph->nodeCount; i++) {
							if(StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, i)) {
								for(int j = i+1; j <= baseGraph->nodeCount; j++) {
									if(StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, i)) {
										if(baseGraph->EdgeExists(i-1, j-1)) {
											minCost = min(minCost, GetCost(k-l, m, i, j)/ (double)m);
										}
									}
								}
							}
						}
					}
				}
			}
			cost += minCost;
		}
	}
	timeInGetB += clock() - startTime;
	return cost;
}

double WongMatchMissing::GetF() {
	return currentNode->costGStar + GetA() + GetB();	
}

void WongMatchMissing::PopBestNode(){
	clock_t start = clock();
	double cost;
	queue->remove(currentNode, cost);
	timeInQueue += clock() - start;
}

void WongMatchMissing::ExpandNode() {
	expandCount++;
	StandardNode * temp;
	if(longestMatch < currentNode->n1Top) {
		longestMatch = currentNode->n1Top;
		printf(" %d elements matched!\n", longestMatch);
	}
	
	int currentM1Top = patternGraph->nodeCount - currentNode->n1Top;
	int currentM2Top = baseGraph->nodeCount - currentNode->n2Top + currentNode->missingNodesUsed;

	// Expanding nodes with a real terminal node
	for(int i = 1; i <= baseGraph->nodeCount; i++) {		
		if(StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, i) &&
			((currentNode->n2Top == 0) || (baseGraph->EdgeExists(currentNode->n2[currentNode->n2Top-1]-1, i-1)))) {
				for(int j = 0; j < min(missingHelixCount * 2 - currentNode->missingNodesUsed + 1, currentM2Top); j += 2) {  // Stepping by two since we jump every 2 loops
				temp = currentNode;
				currentNode = new StandardNode(currentNode, i, j);
				currentNode->costGStar = 0;

				if(((temp->n1Top == 0) && (currentNode->n2[0] == -1)) || 
					((patternGraph->nodeCount - currentNode->n1Top == 0) && (currentNode->n2[currentNode->n2Top-1] == -1))) {
					currentNode->costGStar += START_END_MISSING_HELIX_PENALTY;
				}


				if(temp->n1Top == 0) {
					currentNode->costGStar += temp->costGStar + MISSING_HELIX_PENALTY * (j/2.0) + GetC(currentNode->n1[currentNode->n1Top-1], currentNode->n2[currentNode->n2Top-1]);
				} else {
					currentNode->costGStar += temp->costGStar + 
						GetCost(currentNode->n1[currentNode->n1Top-j-2], j+1, currentNode->n2[currentNode->n2Top-j-2], currentNode->n2[currentNode->n2Top-1]) +
						MISSING_HELIX_PENALTY * (j/2.0) +
						GetC(currentNode->n1[currentNode->n1Top-1], currentNode->n2[currentNode->n2Top-1]);
				}
				currentNode->cost = GetF();			

				//printf("\t");
				//for(int i = 0; i < currentNode->n1Top; i++) {
				//	printf("%d ", currentNode->n2[i]);
				//}
				//printf(" - %f\t%f\t%f\t%f\n", currentNode->costGStar, GetA(), GetB(), currentNode->cost);

				queue->add(currentNode, currentNode->cost);
				currentNode = temp;		
			}
		}
	}


	// Expanding nodes with a dummy terminal node
	if((currentM2Top == 0) && (2*missingHelixCount - currentNode->missingNodesUsed >= currentM1Top)) {
		temp = currentNode;
		currentNode = new StandardNode(currentNode);
		currentNode->costGStar = temp->costGStar + START_END_MISSING_HELIX_PENALTY;
		while(currentNode->m1Bitmap != 0) {
			currentNode->costGStar += (MISSING_HELIX_PENALTY / 2.0) ;//+ GetCost(currentNode->n1[currentNode->n1Top-1], 1, -1, -1);
			currentNode->n1[currentNode->n1Top] = StandardNode::RemoveSmallestNode(currentNode->m1Bitmap);
			currentNode->n1Top++;
			currentNode->n2[currentNode->n2Top] = -1;
			currentNode->n2Top++;			
		}
		currentNode->cost = currentNode->costGStar;

		//printf("\t");
		//for(int i = 0; i < currentNode->n1Top; i++) {
		//	printf("%d ", currentNode->n2[i]);
		//}
		//printf(" - %f\t%f\t%f\n", currentNode->costGStar, currentNode->cost - currentNode->costGStar, currentNode->cost);

		queue->add(currentNode, currentNode->cost);
		currentNode = temp;
	}
}
void WongMatchMissing::PrintCurrentNode()
{
	printf(" N1 : ");
	for(int i = 0; i < (int)currentNode->n1Top; i++) {
		printf(" %ld ", currentNode->n1[i]);
	}
	printf("\n N2 : ");
	for(int i = 0; i < (int)currentNode->n2Top; i++) {
		printf(" %ld ", currentNode->n2[i]);
	}	
	printf("\n M1 : ");
	for(int i = 1; i <= patternGraph->nodeCount; i++) {
		if(StandardNode::IsNodeInBitmap(currentNode->m1Bitmap, i)) {
			printf(" %ld ", i);
		}
	}	
	printf("\n M2 : ");
	for(int i = 1; i <= baseGraph->nodeCount; i++) {
		if(StandardNode::IsNodeInBitmap(currentNode->m2Bitmap, i)) {
			printf(" %ld ", i);
		}
	}	
	printf("\n Cost : %ld\n", currentNode->cost);
}

void WongMatchMissing::NormalizeGraphs() {

	printf("Normalizing the base graph based on helix length ratio\nNormalized Graph:\n");
	double ratio = 0;

	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
		ratio += (double)baseGraph->skeletonHelixes[i]->length / (double)baseGraph->adjacencyMatrix[i*2][i*2+1][1];
	}
	ratio = ratio / (double)baseGraph->skeletonHelixes.size();

	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j < baseGraph->nodeCount; j++) {
			if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT) {
				baseGraph->SetCost(i+1,j+1, baseGraph->adjacencyMatrix[i][j][1] * ratio);
			}

		}
	}	
	baseGraph->PrintGraph();
}

#endif