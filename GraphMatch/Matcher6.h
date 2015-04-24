/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef MATCHER6_H
#define MATCHER6_H

#include "Graph.h"
#include "Node.h"
#include "GlobalConstants.h"
#include "Combinatorics.h"
#include <vector>

struct Matcher6PathHistory {
	unsigned int pathBitMap;
	double cost;
	signed char lastPatternHelix;
	signed char lastBaseNode;
	signed char lastHistoryTable;
	int lastHistoryRow;
};

class Matcher6Junction {
public:
	Matcher6Junction(int patternHelixNo, int baseHelixCount, int missingHelixCount) {
		Matcher6PathHistory * hist;
		for(int i = 0; i <= missingHelixCount; i++) {
			int count = Combinatorics::Combinations(baseHelixCount, patternHelixNo-i);
			elementCount.push_back(count);
			hist = new Matcher6PathHistory[count];
			for(int j = 0; j < count; j++) {
				hist[j].pathBitMap = 0;
				hist[j].cost = 0;
				hist[j].lastBaseNode = -1;
				hist[j].lastPatternHelix = -1;
				hist[j].lastHistoryTable = -1;
				hist[j].lastHistoryRow = -1;
			}
			history.push_back(hist);
		}
	}

	~Matcher6Junction() {
		elementCount.clear();
		for(int i = 0; i < history.size(); i++) {
			delete history[i];
		}
	}
public:
	vector<Matcher6PathHistory*> history;
	vector<int> elementCount;
};

class Matcher6 {
public:
	Graph * patternGraph;
	Graph * baseGraph;
public:
	Matcher6(Graph * patternGraph, Graph * baseGraph);
	Matcher6(Graph * patternGraph, Graph * baseGraph, int missingHelixCount, int missingSheetCount);
	~Matcher6();
	void RunMatching();
	void SaveResults();
private:
	double GetPathCost(Node * node);
	void NormalizeGraphs();
	void FindPossiblePaths();
	int GetIndex(int patternHelixNo, int baseHelixNo);
	unsigned int EncodePath(unsigned int currentPath, int baseNode);
	bool IsVisited(unsigned int pathMap, int baseHelix);
	unsigned int GetBitValue(int bitPosition);
	int GetOtherBaseNode(int baseNode);
	int GetHistoryIndex(unsigned int pathBitmap);
	double GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2);
	Node * FindPath(int patternHelix, int baseNode, int historyTable, int historyRow);
	void PrintJunction(int patternHelix, int baseHelix);
	bool PathExists(int p1, int p2, int p3, int b1, int b2, int b3);
	void GetJunctionStats();
private:
	vector<Matcher6Junction*>  junctions;
	vector<Node*> results;
	int dummyHelixNo;
	int missingHelixCount;
	int missingSheetCount;
};

Matcher6::Matcher6 (Graph * patternGraph, Graph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	dummyHelixNo = baseGraph->GetNodeCount();
	for(int i = 0; i < patternGraph->pdbStructures.size() * baseGraph->GetNodeCount() * 2; i++) {
		junctions.push_back(NULL);
	}

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;
}

Matcher6::Matcher6(Graph * patternGraph, Graph * baseGraph, int missingHelixCount, int missingSheetCount) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	dummyHelixNo = baseGraph->GetNodeCount();
	for(int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount() + 1) * 2; i++) {
		junctions.push_back(NULL);
	}
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
}

Matcher6::~Matcher6() {
	for(int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount()) * 2; i++) {
		if(junctions[i] != NULL) {
			delete junctions[i];
		}
	}
	for(int i = 0; i < results.size(); i++) {
		delete results[i];
	}
	results.clear();
	junctions.clear();
}

bool Matcher6::IsVisited(unsigned int pathMap, int baseHelix) {
	bool isVisited;
	unsigned int bitMask = GetBitValue(baseHelix);
	isVisited = ((pathMap & bitMask) == bitMask);
	return isVisited;
}

unsigned int Matcher6::GetBitValue(int bitPosition) {
	unsigned int bitValue = 1;
	for(int i = 0; i < bitPosition; i++) {
		bitValue *= 2;
	}
	return bitValue;
}

unsigned int Matcher6::EncodePath(unsigned int currentPath, int baseHelix) {
	if(IsVisited(currentPath, baseHelix)) {
		return currentPath;
	} else {
		return currentPath + GetBitValue(baseHelix);
	}
}

int Matcher6::GetIndex(int patternHelixNo, int baseHelixNo) {
	return baseHelixNo * baseGraph->GetNodeCount() + patternHelixNo;
}

void Matcher6::RunMatching() {
	NormalizeGraphs();
	FindPossiblePaths();

}

int Matcher6::GetOtherBaseNode(int baseNode) {
	if((baseNode & 1) == 1) {
		return baseNode - 1;
	} else {
		return baseNode + 1;
	}
}

int Matcher6::GetHistoryIndex(unsigned int pathBitmap) {
	char * X = new char[baseGraph->skeletonHelixes.size() + 1];
	X[0] = 0;
	int count = 0;
	for(int i = 0; i < baseGraph->skeletonHelixes.size(); i++) {
		if(IsVisited(pathBitmap, i)) {
			count++;
			X[count] = i+1;
		}
	}
	int index = 0;
	int n = baseGraph->skeletonHelixes.size();
	int m = count;
	for(int i = 1; i <= m; i++) {
		for(int j = X[i-1]+2; j <= X[i]; j++) {
			index += Combinatorics::Combinations(n-j+1, m-i);
		}
	}
	return index;
}

double Matcher6::GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2) {
	double patternLength = 0.0;
	for(int i  = patternNode1; i < patternNode2; i++) {
		patternLength += patternGraph->adjacencyMatrix[i][i+1][1];
	}

	double weight = 1.0;
	switch((int)(baseGraph->adjacencyMatrix[baseNode1][baseNode2][0] + 0.01)) {
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
	return weight * fabs(patternLength - baseGraph->adjacencyMatrix[baseNode1][baseNode2][1]);
}

bool Matcher6::PathExists(int p1, int p2, int p3, int b1, int b2, int b3) {
	
	return ((patternGraph->adjacencyMatrix[p1][p2][0] == baseGraph->adjacencyMatrix[b1][b2][0]) && 
		(patternGraph->adjacencyMatrix[p2][p3][0] == baseGraph->adjacencyMatrix[b2][b3][0]) &&
		(baseGraph->adjacencyMatrix[b1][b2][1] != MAXINT) && 
		(baseGraph->adjacencyMatrix[b2][b3][1] != MAXINT));
}

void Matcher6::FindPossiblePaths() {
	// Setting up the first paths
	int baseHelixCount = baseGraph->skeletonHelixes.size();
	for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
		junctions[GetIndex(0, j)] = new Matcher6Junction(0, baseHelixCount, missingHelixCount);
		//PrintJunction(0, j);
	}

	Matcher6Junction * currentJunction, * oldJunction;
	Matcher6PathHistory * currentHistoryTable, * oldHistoryTable;
	int currentHistoryCount, oldHistoryCount, oldHistoryTableNum, ob2, obh, b2, bh, hr;
	unsigned int currentPathMap;
	double cost;

	// p-pattern Helix	b-base Node		m-missing count		bh - base helix = b/2	b2 - other end of base node 
	// hr - CurrentHistoryRow;
	// op-oldpattern	ob-oldbase	ohr - oldHistoryRow		obh - Old base helix	ob2 - other end of old base node

	for(int p = 1; p < patternGraph->pdbStructures.size(); p++) {
		for(int b = 0; b < baseGraph->GetNodeCount(); b++) {
			bh = b/2;
			b2 = GetOtherBaseNode(b);
			currentJunction = new Matcher6Junction(p, baseHelixCount, missingHelixCount);
			junctions[GetIndex(p, b)] = currentJunction;
			for(int m = 0; m <= missingHelixCount; m++) {
				currentHistoryTable = (Matcher6PathHistory*)currentJunction->history[m];
				currentHistoryCount = (int)currentJunction->elementCount[m];
				for(int op = max(p-m-1,0); op < p; op++) {
					for(int ob = 0; ob < baseGraph->GetNodeCount(); ob++) {
						obh = ob /2;
						ob2 = GetOtherBaseNode(ob);
						oldJunction = (Matcher6Junction*)junctions[GetIndex(op, ob)];
						oldHistoryTableNum = m-(p-op-1);
						oldHistoryTable = oldJunction->history[oldHistoryTableNum];
						oldHistoryCount = oldJunction->elementCount[oldHistoryTableNum];
						for(int ohr = 0; ohr < oldHistoryCount; ohr++) {						
							if(((oldHistoryTable[ohr].pathBitMap != 0) || ((op <= missingHelixCount)&&(oldHistoryTableNum == op))) &&
							!IsVisited(oldHistoryTable[ohr].pathBitMap, bh) &&			// Current node has not been visited								
							PathExists((p-1)*2+1, p*2, p*2+1, ob2, b, b2)) {			// Same typed paths exists
								currentPathMap = EncodePath(oldHistoryTable[ohr].pathBitMap, obh);
								hr = GetHistoryIndex(currentPathMap);
								// TODO Fix this:
								//Node * n1 = FindPath(op, ob, oldHistoryTableNum, ohr);
								//cost = GetPathCost(n1) + GetCost(op*2+1, p*2, ob2, b) + GetCost(p*2, p*2+1, b, b2);
								cost = oldHistoryTable[ohr].cost+ GetCost(op*2+1, p*2, ob2, b) + GetCost(p*2, p*2+1, b, b2);
								if((currentHistoryTable[hr].lastBaseNode == -1) || (currentHistoryTable[hr].cost > cost)) {
									currentHistoryTable[hr].cost = cost;
									currentHistoryTable[hr].pathBitMap = currentPathMap;
									currentHistoryTable[hr].lastBaseNode = ob;
									currentHistoryTable[hr].lastPatternHelix = op;
									currentHistoryTable[hr].lastHistoryTable = oldHistoryTableNum;
									currentHistoryTable[hr].lastHistoryRow = ohr;
								}	
							}
						}
					}
				}
			}
			//PrintJunction(p, b);
		}
	}
}

Node * Matcher6::FindPath(int patternHelix, int baseNode, int historyTable, int historyRow) {
	Node * path = new Node();
	path->cost = -1;

	Matcher6Junction * junction;
	Matcher6PathHistory * histTable;
	while (patternHelix >= 0) {
		assert((patternHelix >= 0) && (baseNode >= 0) && (historyRow >= 0) && (historyTable >= 0));
		path->n1.insert(path->n1.begin(), patternHelix*2 + 1);
		path->n1.insert(path->n1.begin(), patternHelix*2);
		path->n2.insert(path->n2.begin(), GetOtherBaseNode(baseNode));
		path->n2.insert(path->n2.begin(), baseNode);


		junction = (Matcher6Junction*)junctions[GetIndex(patternHelix, baseNode)];
		histTable = (Matcher6PathHistory*)junction->history[historyTable];
		if(histTable[historyRow].lastPatternHelix != -1) {
			for(int i = patternHelix-2; i >= histTable[historyRow].lastPatternHelix; i--) {
				path->n1.insert(path->n1.begin(), i*2 + 1);
				path->n1.insert(path->n1.begin(), i*2);
				path->n2.insert(path->n2.begin(), baseGraph->GetNodeCount());
				path->n2.insert(path->n2.begin(), baseGraph->GetNodeCount());
			}
		}
		if(path->cost == -1) {
			path->cost = histTable[historyRow].cost;
		}


		if(histTable[historyRow].lastPatternHelix == -1) {
			for(int i = patternHelix-1; i >= 0; i--) {
				path->n1.insert(path->n1.begin(), i*2 + 1);
				path->n1.insert(path->n1.begin(), i*2);
				path->n2.insert(path->n2.begin(), baseGraph->GetNodeCount());
				path->n2.insert(path->n2.begin(), baseGraph->GetNodeCount());
			}
		}

		baseNode = histTable[historyRow].lastBaseNode;
		patternHelix = histTable[historyRow].lastPatternHelix;
		historyTable = histTable[historyRow].lastHistoryTable;
		historyRow = histTable[historyRow].lastHistoryRow;

	}
	return path;
}

void Matcher6::SaveResults() {
	printf("\n\nLegal Paths:\n", RESULT_COUNT);
	Matcher6Junction * junction;
	Matcher6PathHistory * historyTable;
	int elementCount;
	Node * node, * oldNode;
	bool inserted;
	results.clear();
	for(int p = patternGraph->pdbStructures.size()-1 - missingHelixCount; p < patternGraph->pdbStructures.size(); p++) {
		for(int b = 0; b < baseGraph->GetNodeCount(); b++) {
			junction = (Matcher6Junction*)junctions[GetIndex(p, b)];
			for(int m = 0; m <= missingHelixCount - (patternGraph->pdbStructures.size() - 1 - p); m++) {
				historyTable = junction->history[m];
				elementCount = junction->elementCount[m];
				for(int r = 0; r < elementCount; r++) {
					if(historyTable[r].pathBitMap != 0) {
						node = FindPath(p, b, m, r);						
						inserted = false;
						for(int i = 0; i < results.size(); i++) {
							oldNode = (Node*)results[i];
							if(oldNode->cost > node->cost) {
								results.insert(results.begin() + i, node);
								inserted = true;
								break;
							}
						}
						if(!inserted) {
							results.push_back(node);
						}
					}
				}
			}
		}
	}
	
	for(int i = 0; i < results.size(); i++) {
		node = (Node*)results[i];
		for(int j = node->n1.size(); j < patternGraph->nodeCount; j++) {
			node->n1.push_back(j);
			node->n2.push_back(baseGraph->nodeCount);
		}
		node->PrintNodeConcise();
	}



		MatchingList n1, n2, m1, m2;
	for(int i = 0; i < 18; i++) {
		n1.push_back(i);
	}
	
//7 8 4 3 17 17 13 14 1 2 10 9 11 12 5 6 16 15
	//n2.push_back(6); 
	//n2.push_back(7);
	//n2.push_back(3);
	//n2.push_back(2);
	//n2.push_back(16);
	//n2.push_back(16);
	//n2.push_back(12);
	//n2.push_back(13);
	//n2.push_back(0);
	//n2.push_back(1);
	//n2.push_back(9);
	//n2.push_back(8);
	//n2.push_back(10);
	//n2.push_back(11);
	//n2.push_back(4);
	//n2.push_back(5);
	//n2.push_back(15);
	//n2.push_back(14);


	//n2.push_back(0); 
	//n2.push_back(1);
	//n2.push_back(15);
	//n2.push_back(14);
	//n2.push_back(6);
	//n2.push_back(7);
	//n2.push_back(16);
	//n2.push_back(16);
	//n2.push_back(9);
	//n2.push_back(8);
	//n2.push_back(10);
	//n2.push_back(11);
	//n2.push_back(4);
	//n2.push_back(5);
	//n2.push_back(3);
	//n2.push_back(2);
	//n2.push_back(12);
	//n2.push_back(13);

	//
	//node = new Node(n1, n2, MatchingList(), MatchingList(), 0);
	//node->cost = GetPathCost(node);
	//printf("\n\nCorrect Path!!\n");
	//node->PrintNodeConcise();
	//GetJunctionStats();
	//int c = getchar();
}

void Matcher6::PrintJunction(int patternHelix, int baseNode) {
	Matcher6Junction * junc = junctions[GetIndex(patternHelix, baseNode)];
	if(junc != NULL) {
		printf("\nJunction History (%d, %d)\n", patternHelix+1, baseNode+1);
		Matcher6PathHistory * historyTable;
		Matcher6PathHistory historyElement;
		for(int i = 0; i < junc->history.size(); i++) {
			printf("\t%d missing helixes\n", i);
			historyTable = (Matcher6PathHistory*)junc->history[i];
			for(int j = 0; j < (int)junc->elementCount[i]; j++) {
				historyElement = historyTable[j];
				if(historyElement.lastBaseNode != -1) {
					printf("\tBit map : %d | Cost : %f\n", historyElement.pathBitMap, historyElement.cost);
					Node * node = FindPath(patternHelix, baseNode, i, j);
					node->PrintNodeConcise();
				}				
			}
		}
	}
}
void Matcher6::NormalizeGraphs() {

	printf("Normalizing the base graph based on helix length ratio\nNormalized Graph:\n");
	double ratio = 0;

	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
		ratio += (double)baseGraph->skeletonHelixes[i]->length / (double)baseGraph->adjacencyMatrix[i*2][i*2+1][1];
	}
	ratio = ratio / (double)baseGraph->skeletonHelixes.size();

	for(int i = 1; i <= baseGraph->nodeCount; i++) {
		for(int j = 1; j <= baseGraph->nodeCount; j++) {
			if(baseGraph->GetCost(i,j) != MAXINT) {
				baseGraph->SetCost(i,j, baseGraph->GetCost(i,j) * ratio);
			}

		}
	}	
	baseGraph->PrintGraph();
}
double Matcher6::GetPathCost(Node * node) {
	double cost = 0;
	int b1 = -1, b2 = -1, p1, p2;
	for(int i = 0; i < node->n1.size()-1; i++) {
		if(node->n2[i] != baseGraph->GetNodeCount()) {
			b1 = node->n2[i];
			p1 = node->n1[i];
		}

		if(node->n2[i+1] != baseGraph->GetNodeCount()) {
			b2 = node->n2[i+1];
			p2 = node->n1[i+1];
			cost += GetCost(p1, p2, b1, b2);
			//printf("(%d %d) (%d %d)\n", p1+1, p2+1, b1+1, b2+1);
		}	
	}
	return cost;
}
void Matcher6::GetJunctionStats() {
	Matcher6Junction * currentJunction;
	Matcher6PathHistory * pathTable;
	int count;
	int tot;
	for(int p = 0; p < patternGraph->pdbStructures.size(); p++) {
		printf("\n[%d] : ", p + 1);
		for(int b = 0; b < baseGraph->GetNodeCount(); b++) {
			currentJunction = junctions[GetIndex(p, b)];
			count = 0;
			tot = 0;
			for(int m = 0; m <= missingHelixCount; m++) {
				pathTable = currentJunction->history[m];
				//tot =+ (int)currentJunction->elementCount[m];
				for(int r = 0; r < currentJunction->elementCount[m]; r++) {
					tot++;
					if(pathTable[r].lastPatternHelix != -1) {
						count++;
					}
				}
			}
			printf(" (%d : %d / %d) \t", b+1, count, tot);
		}
	}
}
#endif