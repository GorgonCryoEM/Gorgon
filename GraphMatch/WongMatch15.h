/*
Graph Matching algorithm based on the following paper:
  Wong et al., An algorithm for graph optimal monomorphism, 
  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636

Author: Sasakthi S. Abeysinghe
Date  : 01/23/2006
*/

#ifndef WONGMATCH15_H
#define WONGMATCH15_H

#include "StandardGraph.h"
#include "StandardNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"

class WongMatch15{
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
public:
	WongMatch15(StandardGraph * patternGraph, StandardGraph * baseGraph);
	WongMatch15(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~WongMatch15();
	void RunMatching();
	void SaveResults();


private:
	StandardNode * currentNode;
	NodeList activeNodes;
	int missingHelixCount;
	int missingSheetCount;
	bool newBaseGraph;
	EdgeMinCostEntry edgeMinCosts[MAX_NODES*MAX_NODES][MAX_NODES*MAX_NODES];
	int expandCount;
private:
	void Init(StandardGraph * patternGraph, StandardGraph * baseGraph);
	void InitializeEdgeMinCosts();
	double GetC(int p, int qp);
	double GetC(int j, int p, int qj, int qp);
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
	unsigned long long EncodeNode(unsigned long long bitmap, int node);

};

WongMatch15::WongMatch15(StandardGraph * patternGraph, StandardGraph * baseGraph) {
	Init(patternGraph, baseGraph);
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	InitializeEdgeMinCosts();
}

WongMatch15::WongMatch15(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	Init(patternGraph, baseGraph);
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	InitializeEdgeMinCosts();

}

WongMatch15::~WongMatch15() {
	for(int i = 0; i < (int)activeNodes.nodes.size(); i++) {
		delete(activeNodes.nodes[i]);
	}
	activeNodes.nodes.clear();

	if(newBaseGraph) {
		baseGraph->skeletonHelixes.clear();
		delete baseGraph;
	}
}
void WongMatch15::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {
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


void WongMatch15::InitializeEdgeMinCosts() {	
	//for(int q = 0; q < MAX_NODES * MAX_NODES; q++) {
	//	for(int j = 0; j < MAX_NODES * MAX_NODES; j++) {
	//		edgeMinCosts[j][q].bitmap = MAXUNSIGNEDINT;
	//		edgeMinCosts[j][q].cost = MAXDOUBLE;
	//	}
	//}

	unsigned long long bitmap;
	int cnt;
	for(int j = 0; j < patternGraph->nodeCount; j++) {	
		for(int i = 0; i < j; i++) {					
			cnt = 0;
			for(int q = 0; q < baseGraph->nodeCount; q++) {
				for(int r = 0; r < baseGraph->nodeCount; r++) {
					if(q != r) {
						bitmap = EncodeNode(0, q+1);
						bitmap = EncodeNode(bitmap, r+1);
						edgeMinCosts[i*MAX_NODES + j][cnt].bitmap = bitmap;
						edgeMinCosts[i*MAX_NODES + j][cnt].cost = GetCPrime(i+1, j+1, q+1, r+1);
						cnt++;
					}
				}
			}
			assert(baseGraph->nodeCount * baseGraph->nodeCount - baseGraph->nodeCount == cnt);
		}		
	}



	int index1;
	int minIndex;
	int totalNodeCount = baseGraph->nodeCount * baseGraph->nodeCount - baseGraph->nodeCount;
	EdgeMinCostEntry temp;
	for(int j = 0; j < patternGraph->nodeCount; j++) {
		for(int i = 0; i < j; i++) {
			for(int k = 0; k < totalNodeCount-1; k++) {
				minIndex = k;
				for(int l = k+1; l < totalNodeCount; l++) {
					if(edgeMinCosts[i*MAX_NODES + j][l].cost < edgeMinCosts[i*MAX_NODES + j][minIndex].cost) {
						minIndex = l;
					}
				}
				temp = edgeMinCosts[i*MAX_NODES + j][k];
				edgeMinCosts[i*MAX_NODES + j][k] = edgeMinCosts[i*MAX_NODES + j][minIndex];
				temp = edgeMinCosts[i*MAX_NODES + j][minIndex] = temp;
			}
		}
	}
}

void WongMatch15::HandleMissingHelixes() {
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

void WongMatch15::RunMatching() {
	if(patternGraph->GetNodeCount() > baseGraph->GetNodeCount()) {
		printf("Unable to perform matching.. Pattern graph contains more nodes than the base graph!!");
	}
	else
	{
		bool continueLoop = true;
		while(continueLoop)
		{
			PopBestNode();
			//PrintCurrentNode();
			if(currentNode->m1Top > 0) {
				ExpandNode();		
			}

			continueLoop = (currentNode->m1Top > 0);
			if(continueLoop) {
				delete currentNode;
			}
		}
	}
}

void WongMatch15::SaveResults(){
	//printf("\t");
	//for(int i = 0; i < currentNode->n1Top; i++) {
	//	printf("%d ", currentNode->n2[i]);
	//}
	//printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
	printf("%d\t\t", expandCount);
	delete currentNode;
}

double WongMatch15::GetC(int p, int qp) {
	return GetC(p, p, qp, qp);
}

double WongMatch15::GetC(int j, int p, int qj, int qp) {

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

double WongMatch15::GetCPrime(int a, int b, int c, int d) {
	// Performance optimization on the symmetry of the graph 
	// return GetC(a, b, c, d) + GetC(b, a, d, c); 
	return 2*GetC(a, b, c, d);
}


//int WongMatch15::GetK(int p, int qp){
//	int cost = GetC(p, qp);
//	for (int j = 0; j < currentNode->n1Top - 1; j++)
//	{
//		// Performance Optimization
//		cost += GetCPrime(currentNode->n1[j], p, currentNode->n2[j], qp);
//		//cost += 2*GetC(currentNode->n1[j], p, currentNode->n2[j], qp);
//	}
//	return cost;
//}
//int WongMatch15::GetKPrime(int i, int q) {
//	int cost = GetC(i, q);
//	for(int j = 0; j < currentNode->n1Top; j++)
//	{
//		// Performance Optimization
//		cost += GetCPrime(i, currentNode->n1[j], q, currentNode->n2[j]);
//		//cost += 2*GetC(i, currentNode->n1[j], q, currentNode->n2[j]);
//	}
//	return cost;
//}
//
//int WongMatch15::GetA() {
//	int cost = 0;
//	int minCost;
//	int iNode;
//	for(int i = 0; i < currentNode->m1Top; i++)
//	{
//		iNode = currentNode->m1[i];
//		minCost = MAXINT;
//		for(int j = 0; j < currentNode->m2Top; j++) {
//			minCost = min(minCost, GetKPrime(iNode, currentNode->m2[j]));
//		}
//		cost += minCost;
//	}
//	return cost;
//}
double WongMatch15::GetK(int p, int qp){
	double cost = GetC(p, qp);
	if(currentNode->n1Top - 2 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 2], p, currentNode->n2[currentNode->n2Top - 2], qp);
	}
	return cost;
}
double WongMatch15::GetKPrime(int i, int q) {
	double cost = GetC(i, q);
	if(currentNode->n1Top - 1 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 1], i, currentNode->n2[currentNode->n2Top - 1], q);
	}
	return cost;
}

double WongMatch15::GetA() {
	double cost = 0;
	double minCost;
	int iNode;
	// Cost for node matching
	for(int i = 0; i < currentNode->m1Top; i++)
	{
		iNode = currentNode->m1[i];
		minCost = MAXINT;
		for(int j = 0; j < currentNode->m2Top; j++) {
			minCost = min(minCost, GetC(iNode, currentNode->m2[j]));
		}
		cost += minCost;
	}

	// Cost for first node matched
	minCost = MAXINT;
	for(int i = 0; i < currentNode->m1Top; i++)
	{
		iNode = currentNode->m1[i];
		for(int j = 0; j < currentNode->m2Top; j++) {
			minCost = min(minCost, GetKPrime(iNode, currentNode->m2[j]));
		}
		
	}
	if(currentNode->m1Top > 0) {
		cost += minCost;
	}

	return cost;
}
double WongMatch15::GetB() {
	double cost = 0;
	double minCost;
	int iNode, jNode, qNode;
	unsigned long long bitmap = 0;
	for(int i = 0; i < currentNode->n2Top; i++) {
		bitmap = EncodeNode(bitmap, currentNode->n2[i]);
	}

	int cnt1, cnt2;
	bool breakoff = false;
	for(int j = 0; j < currentNode->m1Top; j++) {
		jNode = currentNode->m1[j];
		for(int i = currentNode->m1Top-1; i > j; i--) {
			iNode = currentNode->m1[i];
			assert(iNode < jNode);
			cnt1 = (iNode-1)*MAX_NODES + (jNode-1);
			if(patternGraph->EdgeExists(iNode-1, jNode-1)) {
				breakoff = false;
				cnt2 = 0;
				for(int cnt2 = 0; cnt2 < baseGraph->nodeCount * baseGraph->nodeCount && !breakoff; cnt2++) {					
					if((edgeMinCosts[cnt1][cnt2].bitmap & bitmap) == 0) {
						cost += edgeMinCosts[cnt1][cnt2].cost;
						breakoff = true;
					}
				}
			}
		}
	}
	return cost;
}
double WongMatch15::GetF() {
	return currentNode->costGStar + GetA() + GetB();	
}

void WongMatch15::PopBestNode(){
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

void WongMatch15::ExpandNode() {
	StandardNode * temp;
	expandCount++;
	for(int i = 0; i < (int)currentNode->m2Top; i++)
	{
		if((currentNode->n2Top == 0) || (baseGraph->EdgeExists(currentNode->n2[currentNode->n2Top-1]-1, currentNode->m2[i]-1)))
		{
			temp = currentNode;
			currentNode = new StandardNode(currentNode, i);
			currentNode->costGStar = temp->costGStar + GetK(temp->n1Top+1, temp->m2[i]);
			currentNode->cost = GetF();
			activeNodes.Insert(currentNode);
			currentNode = temp;		
		}
	}
}
void WongMatch15::PrintCurrentNode()
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

void WongMatch15::NormalizeGraphs() {

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

unsigned long long WongMatch15::EncodeNode(unsigned long long bitmap, int node) {
	return bitmap | (unsigned long long)(pow(2.0, node-1) + 0.1);
}


#endif