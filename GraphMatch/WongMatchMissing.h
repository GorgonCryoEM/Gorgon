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

class WongMatchMissing{
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
public:
	WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph);
	WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~WongMatchMissing();
	void RunMatching();
	void SaveResults();


private:
	StandardNode * currentNode;
	NodeList activeNodes;
	int missingHelixCount;
	int missingSheetCount;
	bool newBaseGraph;
	int expandCount;
	int foundCount;
private:
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
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		//HandleMissingHelixes();
	}
	foundCount = 0;
}

WongMatchMissing::WongMatchMissing(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	Init(patternGraph, baseGraph);
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	foundCount = 0;

}

WongMatchMissing::~WongMatchMissing() {
	for(int i = 0; i < (int)activeNodes.nodes.size(); i++) {
		delete(activeNodes.nodes[i]);
	}
	activeNodes.nodes.clear();

	if(newBaseGraph) {
		baseGraph->skeletonHelixes.clear();
		delete baseGraph;
	}
}
void WongMatchMissing::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	expandCount = 0;
	newBaseGraph = false;
	
	currentNode = new StandardNode();
	currentNode->cost = 0;
	for(int i = 0; i < patternGraph->nodeCount; i++) {
		currentNode->m1[patternGraph->nodeCount-i-1] = i+1;		
	}
	currentNode->m1Top = patternGraph->nodeCount;
	for(int i = 0; i < baseGraph->nodeCount; i++) {

		currentNode->m2[baseGraph->nodeCount-i-1] = i+1;
	}
	currentNode->m2Top = baseGraph->nodeCount;	
	activeNodes.Insert(currentNode);

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;
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

	for(int i = 0; i < baseGraph->skeletonHelixes.size(); i++) {
		newBase->skeletonHelixes.push_back(baseGraph->skeletonHelixes[i]);
	}

	assert(nodeCount == baseGraph->nodeCount + loopCount * missingHelixCount * 2);

	baseGraph = newBase;
	baseGraph->PrintGraph();
	newBaseGraph = true;
	
}

void WongMatchMissing::RunMatching() {
	bool continueLoop = true;
	while(continueLoop)
	{
		PopBestNode();
		//PrintCurrentNode();
		if(currentNode->m1Top > 0) {
			ExpandNode();					
		} else {
			printf("\t");
			for(int i = 0; i < currentNode->n1Top; i++) {
				printf("%d ", currentNode->n2[i]);
			}
			printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
			foundCount++;
		}

		continueLoop = (foundCount < RESULT_COUNT);
		if(continueLoop) {
			delete currentNode;
		}
	}
}

void WongMatchMissing::SaveResults(){
	printf("\t");
	for(int i = 0; i < currentNode->n1Top; i++) {
		printf("%d ", currentNode->n2[i]);
	}
	printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
	//printf("%d\t\t", expandCount);
	delete currentNode;
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
	double jpCost = 0;
	double qjqpCost;
	double typeCost = 0;

	// Adding the length of the skipped helixes
	for(int i = 1; i < m; i++) {
		jpCost += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
	}

	// Adding the length of the edges
	bool firstIsLoop;
	bool lastIsLoop;
	for(int i = 0; i < m; i++) {
		lastIsLoop = ((patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP) || (patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP_EUCLIDEAN));
		if(i==0) {
			firstIsLoop = lastIsLoop;
		}
		jpCost += patternGraph->adjacencyMatrix[d+i-1][d+i][1];		
	}

	if(!baseGraph->EdgeExists(qj-1,qp-1)) {
		qjqpCost = 1000;
	} else {
		qjqpCost = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
	}

	if(m == 1) {
		if(patternGraph->adjacencyMatrix[d-1][d][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) {
			typeCost = 1000;
		}
	} else {
		if(!firstIsLoop || !lastIsLoop) {
			typeCost = 1000;
		}
	}

	return fabs(jpCost - qjqpCost) + typeCost;
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
	double cost = 0;
	double minCost;
	int iNode;
	// Cost for node matching
	for(int i = 0; i < currentNode->m1Top; i++)
	{
		iNode = currentNode->m1[i];
		minCost = MISSING_HELIX_PENALTY;
		for(int j = 0; j < currentNode->m2Top; j++) {
			minCost = min(minCost, GetC(iNode, currentNode->m2[j]));
		}
		cost += minCost;
	}

	return cost;
}
double WongMatchMissing::GetB() {
	
	int kNode, iNode, jNode;
	double minCost;
	double cost = 0;

	for(int k = 0; k < currentNode->m1Top; k++) {
		kNode = currentNode->m1[k];
		if(kNode != patternGraph->nodeCount) {
			minCost = MAXDOUBLE;

			for(int m = 1; m <= missingHelixCount * 2 + 1; m++) {
				for(int l = 0; l <= m - 1; l++) {				
					if((kNode - l >= 1) && (kNode - l + m <= patternGraph->nodeCount)) {
						

						for(int i = 0; i < currentNode->m2Top-1; i++) {
							iNode = currentNode->m2[i];
							for(int j = i+1; j < currentNode->m2Top; j++) {
								jNode = currentNode->m2[j];
								if(baseGraph->EdgeExists(iNode-1, jNode-1)) {
									minCost = min(minCost, GetCost(kNode-l, m, iNode, jNode)/ (double)m);
								}
							}
						}
					}
				}
			}
			cost += minCost;
		}
	}
	
	return cost;
}

double WongMatchMissing::GetF() {
	return currentNode->costGStar + GetA() + GetB();	
}

void WongMatchMissing::PopBestNode(){
	int index;
	double minCost = MAXINT;
	for(int i = 0; i < activeNodes.nodes.size(); i++) { 
		if(activeNodes.nodes[i]->cost < minCost) {
			minCost = activeNodes.nodes[i]->cost;
			index = i;
		}		
	}
	currentNode = activeNodes.nodes[index];
	activeNodes.nodes.erase(activeNodes.nodes.begin() + index);
}

void WongMatchMissing::ExpandNode() {
	expandCount++;
	StandardNode * temp;
	for(int i = 0; i < (int)currentNode->m2Top; i++)
	{
		if((currentNode->n2Top == 0) || (baseGraph->EdgeExists(currentNode->n2[currentNode->n2Top-1]-1, currentNode->m2[i]-1)))
		{
			for(int j = 0; j < missingHelixCount * 2 - currentNode->missingNodesUsed + 1; j++) {
				temp = currentNode;
				currentNode = new StandardNode(currentNode, i, j);
				if(temp->n1Top == 0) {
					currentNode->costGStar = temp->costGStar + MISSING_HELIX_PENALTY * j + GetC(currentNode->n1[currentNode->n1Top-1], currentNode->n2[currentNode->n2Top-1]);
				} else {
					currentNode->costGStar = temp->costGStar + 
						GetCost(currentNode->n1[currentNode->n1Top-j-2], j+1, currentNode->n2[currentNode->n2Top-j-2], currentNode->n2[currentNode->n2Top-1]) +
						MISSING_HELIX_PENALTY * j +
						GetC(currentNode->n1[currentNode->n1Top-1], currentNode->n2[currentNode->n2Top-1]);
				}
				currentNode->cost = GetF();			

				printf("\t");
				for(int i = 0; i < currentNode->n1Top; i++) {
					printf("%d ", currentNode->n2[i]);
				}
				printf(" - %f\t%f\t%f\n", currentNode->costGStar, currentNode->cost - currentNode->costGStar, currentNode->cost);

				activeNodes.Insert(currentNode);
				currentNode = temp;		
			}
		}
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
	for(int i = 0; i < (int)currentNode->m1Top; i++) {
		printf(" %ld ", currentNode->m1[i]);
	}	
	printf("\n M2 : ");
	for(int i = 0; i < (int)currentNode->m2Top; i++) {
		printf(" %ld ", currentNode->m2[i]);
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