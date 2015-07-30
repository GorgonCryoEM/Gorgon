/*
Graph Matching algorithm based on the following paper:
  Wong et al., An algorithm for graph optimal monomorphism, 
  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636

  Modified for better performance

Author: Sasakthi S. Abeysinghe
Date  : 07/18/2006
*/

#ifndef WONGMATCH2_H
#define WONGMATCH2_H

#include "StandardGraph.h"
#include "StandardNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"

struct EdgeMinCostEntry {
	unsigned long long bitmap;
	double cost;
	int noOfEdges;
};

struct HashEntry {
	unsigned long long bitmap;
	double gStar;		// will be -1 if the entry is not used
	double h;
};

class WongMatch2{
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
public:
	WongMatch2(StandardGraph * patternGraph, StandardGraph * baseGraph);
	WongMatch2(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~WongMatch2();
	void RunMatching();
	void SaveResults();
private:
	StandardNode * currentNode;
	NodeList activeNodes;
	int missingHelixCount;
	int missingSheetCount;
	bool newBaseGraph;
	EdgeMinCostEntry edgeMinCosts[MAX_NODES*MAX_NODES][MAX_NODES*MAX_NODES];
	HashEntry hashTable[WONG_HASH_TABLE_SIZE][MAX_NODES];
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
	void StoreInHash(unsigned long long bitmap, int lastNode, double gStar, double h);
	bool GetHashValue(unsigned long long bitmap, int lastNode, double & gStar, double & h);

};

WongMatch2::WongMatch2(StandardGraph * patternGraph, StandardGraph * baseGraph) {
	Init(patternGraph, baseGraph);
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	InitializeEdgeMinCosts();
}

WongMatch2::WongMatch2(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	Init(patternGraph, baseGraph);
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	InitializeEdgeMinCosts();

}

WongMatch2::~WongMatch2() {
	for(int i = 0; i < (int)activeNodes.nodes.size(); i++) {
		delete(activeNodes.nodes[i]);
	}
	activeNodes.nodes.clear();

	if(newBaseGraph) {
		baseGraph->skeletonHelixes.clear();
		delete baseGraph;
	}
}
void WongMatch2::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {
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

	// Hash Table initialization
	for(int i = 0; i < WONG_HASH_TABLE_SIZE; i++) {
		for(int j = 0; j < MAX_NODES; j++) {
			hashTable[i][j].gStar = -1;
		}
	}
}


void WongMatch2::InitializeEdgeMinCosts() {	
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

void WongMatch2::HandleMissingHelixes() {
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

void WongMatch2::RunMatching() {
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

void WongMatch2::SaveResults(){
	//printf("\t");
	//for(int i = 0; i < currentNode->n1Top; i++) {
	//	printf("%d ", currentNode->n2[i]);
	//}
	//printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
	printf("%d\t\t", expandCount);
	delete currentNode;
}


double WongMatch2::GetC(int p, int qp) {
	return GetC(p, p, qp, qp);
}

double WongMatch2::GetC(int j, int p, int qj, int qp) {

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

double WongMatch2::GetCPrime(int a, int b, int c, int d) {
	// Performance optimization on the symmetry of the graph 
	return GetC(a, b, c, d) + GetC(b, a, d, c); 
	//return 2*GetC(a, b, c, d);
}


double WongMatch2::GetK(int p, int qp){
	double cost = GetC(p, qp);
	if(currentNode->n1Top - 2 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 2], p, currentNode->n2[currentNode->n2Top - 2], qp);
	}
	return cost;
}
double WongMatch2::GetKPrime(int i, int q) {
	double cost = GetC(i, q);
	if(currentNode->n1Top - 1 >= 0) {
		cost += GetCPrime(currentNode->n1[currentNode->n1Top - 1], i, currentNode->n2[currentNode->n2Top - 1], q);
	}
	return cost;
}

double WongMatch2::GetA() {
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
double WongMatch2::GetB() {
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
//int WongMatch2::GetB() {
//	int cost = 0;
//	int minCost;
//	int iNode, jNode, qNode;
//	for(int j = 0; j < currentNode->m1Top; j++) {
//		jNode = currentNode->m1[j];
//		for(int i = currentNode->m1Top-1; i > j; i--) {
//			iNode = currentNode->m1[i];
//			if(patternGraph->EdgeExists(iNode-1, jNode-1)) {
//				minCost = MAXINT;
//				for(int q = 0; q < currentNode->m2Top; q++) {
//					qNode = currentNode->m2[q];
//					for(int r = 0; r < currentNode->m2Top; r++) {
//						if(q != r) {
//							// Performance Optimization
//							minCost = min(minCost, GetCPrime(iNode, jNode, qNode, currentNode->m2[r]));
//							//minCost = min(minCost, 2*GetC(iNode, jNode, qNode, currentNode->m2[r]));
//						}
//					}
//				}
//				cost += minCost;
//			}
//		}
//	}
//	return cost;
//}

double WongMatch2::GetF() {
	return currentNode->costGStar + GetA() + GetB();	
}

void WongMatch2::PopBestNode(){
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

void WongMatch2::ExpandNode() {
	StandardNode * temp;
	expandCount++;
	
	//printf("Ex\t");
	//for(int i = 0; i < currentNode->n1Top; i++) {
	//	printf("%d ", currentNode->n2[i]);
	//}
	//printf(" - %f\t%f\t%f\n", currentNode->costGStar, currentNode->cost - currentNode->costGStar, currentNode->cost);




	unsigned long long bitmap = 0;
	for(int i = 0; i < currentNode->n2Top; i++) {
		bitmap = EncodeNode(bitmap, currentNode->n2[i]);
	}

	double gStar, gStarHashed, hHashed;

	for(int i = 0; i < (int)currentNode->m2Top; i++)
	{
		int lastNode = currentNode->m2[i];
		if((currentNode->n2Top == 0) || (baseGraph->EdgeExists(currentNode->n2[currentNode->n2Top-1]-1, lastNode-1)))
		{
			temp = currentNode;
			
			currentNode = new StandardNode(currentNode, i);
			
			gStar = temp->costGStar + GetK(temp->n1Top+1, lastNode);
			currentNode->costGStar = gStar;
			if(GetHashValue(bitmap, lastNode, gStarHashed, hHashed)) {
				if(gStar < gStarHashed) {
					currentNode->cost = gStar + hHashed;

					//printf("\t");
					//for(int i = 0; i < currentNode->n1Top; i++) {
					//	printf("%d ", currentNode->n2[i]);
					//}
					//printf(" - %f\t%f\t%f\n", currentNode->costGStar, currentNode->cost - currentNode->costGStar, currentNode->cost);


					activeNodes.Insert(currentNode);
					StoreInHash(bitmap, lastNode, gStar, hHashed);
				}
			} else {
				currentNode->cost = gStar + GetA() + GetB();

				//printf("\t");
				//for(int i = 0; i < currentNode->n1Top; i++) {
				//	printf("%d ", currentNode->n2[i]);
				//}
				//printf(" - %f\t%f\t%f\n", currentNode->costGStar, currentNode->cost - currentNode->costGStar, currentNode->cost);


				activeNodes.Insert(currentNode);
				StoreInHash(bitmap, lastNode, gStar, currentNode->cost - gStar);
			}

			currentNode = temp;		
		}
	}
}
void WongMatch2::PrintCurrentNode()
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

void WongMatch2::NormalizeGraphs() {

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


unsigned long long WongMatch2::EncodeNode(unsigned long long bitmap, int node) {
	return bitmap | (unsigned long long)(pow(2.0, node-1) + 0.1);
}

void WongMatch2::StoreInHash(unsigned long long bitmap, int lastNode, double gStar, double h) {
	unsigned long long index = bitmap % WONG_HASH_TABLE_SIZE; 
	hashTable[index][lastNode-1].bitmap = bitmap;
	hashTable[index][lastNode-1].gStar = gStar;
	hashTable[index][lastNode-1].h = h;
}

bool WongMatch2::GetHashValue(unsigned long long bitmap, int lastNode, double & gStar, double & h) {
	unsigned long long index = bitmap % WONG_HASH_TABLE_SIZE; 
	if((hashTable[index][lastNode-1].bitmap == bitmap) && (hashTable[index][lastNode-1].gStar >= 0)) {
		gStar = hashTable[index][lastNode-1].gStar;
		h = hashTable[index][lastNode-1].h;
		return true;
	}
	else {
		return false;
	}
}

#endif