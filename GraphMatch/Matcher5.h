/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef MATCHER5_H
#define MATCHER5_H

#include "Graph.h"
#include "Node.h"
#include "GlobalConstants.h"
#include <vector>

struct Matcher5PathHistory {
	unsigned int pathBitMap;
	double	cost;
	signed char lastNode;
	int lastHistory;
};

class Matcher5Junction {
public:
	vector<Matcher5PathHistory*> history;
	Matcher5Junction() {}
	~Matcher5Junction() {
		for(int i = 0; i < history.size(); i++) {
			delete history[i];
		}
		history.clear();
	}

};

class Matcher5 {
public:
	Graph * patternGraph;
	Graph * baseGraph;
public:
	Matcher5(Graph * patternGraph, Graph * baseGraph);
	~Matcher5();
	void RunMatching();
	void SaveResults();
private:
	void NormalizeGraphs();
	void FindPossiblePaths();
	int GetIndex(int patternHelixNo, int baseHelixNo);
	unsigned int EncodePath(unsigned int currentPath, int baseNode);
	bool IsVisited(unsigned int pathMap, int baseNode);
	unsigned int GetBitValue(int bitPosition);
	int GetOtherBaseNode(int baseNode);
	double GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2);
	Node * FindPath(int patternNode, int baseNode, unsigned int historyElement);
	void PrintJunction(int patternHelix, int baseHelix);
	bool PathExists(int p1, int p2, int p3, int & b1, int b2, int b3, int historyElement);
private:
	vector<Matcher5Junction*>  junctions;
	vector<Node*> results;
	int dummyHelixNo;
};

Matcher5::Matcher5 (Graph * patternGraph, Graph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	dummyHelixNo = baseGraph->GetNodeCount();
	for(int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount() + 1) * 2; i++) {
		junctions.push_back(NULL);
	}
}

Matcher5::~Matcher5() {
	for(int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount() + 1) * 2; i++) {
		delete junctions[i];
	}
	for(int i = 0; i < results.size(); i++) {
		delete results[i];
	}
	results.clear();
	junctions.clear();
}

bool Matcher5::IsVisited(unsigned int pathMap, int baseNode) {
	bool isVisited;
	if(baseNode == dummyHelixNo) {
		isVisited = false;
	} else {
		unsigned int bitMask = GetBitValue(baseNode);
		isVisited = ((pathMap & bitMask) == bitMask);
	}
	return isVisited;
}

unsigned int Matcher5::GetBitValue(int bitPosition) {
	unsigned int bitValue = 1;
	for(int i = 0; i < bitPosition; i++) {
		bitValue *= 2;
	}
	return bitValue;
}

unsigned int Matcher5::EncodePath(unsigned int currentPath, int baseNode) {
	if(IsVisited(currentPath, baseNode)) {
		return currentPath;
	} else {
		return currentPath + GetBitValue(baseNode);
	}
}

int Matcher5::GetIndex(int patternHelixNo, int baseHelixNo) {
	return baseHelixNo * (baseGraph->GetNodeCount() + 1) + patternHelixNo;
}

void Matcher5::RunMatching() {
	NormalizeGraphs();
	FindPossiblePaths();
}

int Matcher5::GetOtherBaseNode(int baseNode) {
	if(baseNode == dummyHelixNo) {
		return baseNode;
	} else if((baseNode & 1) == 1) {
		return baseNode - 1;
	} else {
		return baseNode + 1;
	}
}

double Matcher5::GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2) {
	if((baseNode1 == dummyHelixNo) || (baseNode2 == dummyHelixNo)) {
		return MISSING_HELIX_PENALTY;
	} else {
		return fabs(patternGraph->adjacencyMatrix[patternNode1][patternNode2][1] - baseGraph->adjacencyMatrix[baseNode1][baseNode2][1]);
	}
}

bool Matcher5::PathExists(int p1, int p2, int p3, int & b1, int b2, int b3, int historyElement) {
	if((b2 == dummyHelixNo) || (b3 == dummyHelixNo))
		return true;
	
	bool found = false;
	if(b1 == dummyHelixNo) {
		Node * path = FindPath(p1/2, b1, historyElement);		
		int pos = path->n2.size() - 1;
		while ((!found) && (pos >= 0)) {
			if(path->n2[pos] != dummyHelixNo) {
				b1 = path->n2[pos];
				found = true;
			}
			pos--;
		}
	} else {
		found = true;
	}
	
	if(!found) {
		return true;
	} else {
		return ((patternGraph->adjacencyMatrix[p1][p2][0] == baseGraph->adjacencyMatrix[b1][b2][0]) && 
			(patternGraph->adjacencyMatrix[p2][p3][0] == baseGraph->adjacencyMatrix[b2][b3][0]) &&
			(baseGraph->adjacencyMatrix[b1][b2][1] != MAXINT) && 
			(baseGraph->adjacencyMatrix[b2][b3][1] != MAXINT));
	}
}

void Matcher5::FindPossiblePaths() {
	Matcher5Junction * currentJunction;
	Matcher5Junction * olderJunction;
	Matcher5PathHistory * currentHistory;
	Matcher5PathHistory * newHistory;
	int otherNode, k2;
	// Setting up the first paths
	for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
		currentJunction = new Matcher5Junction();
		junctions[GetIndex(0, j)] = currentJunction;
		currentHistory = new Matcher5PathHistory();
		currentHistory->lastHistory = -1;
		currentHistory->lastNode = -1;
		otherNode = GetOtherBaseNode(j);
		currentHistory->pathBitMap = EncodePath(EncodePath(0, j), otherNode);
		currentHistory->cost = GetCost(0, 1, j, otherNode);
		currentJunction->history.push_back(currentHistory);
		//PrintJunction(0, j);
	}

	// Setting up the other connected paths
	for(int i = 1; i < patternGraph->pdbStructures.size(); i++) {
		for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
			otherNode = GetOtherBaseNode(j);
			currentJunction = new Matcher5Junction();
			junctions[GetIndex(i, j)] = currentJunction;
			for(int k = 0; k < baseGraph->GetNodeCount() + 1; k++) {
				k2 = GetOtherBaseNode(k);
				olderJunction = (Matcher5Junction*)junctions[GetIndex(i-1, k)];

				for(int m = 0; m < olderJunction->history.size(); m++) {
					currentHistory = (Matcher5PathHistory*)olderJunction->history[m];
					// Neither nodes have been visited... 
					if(!IsVisited(currentHistory->pathBitMap, j) && !IsVisited(currentHistory->pathBitMap, otherNode)) {
						// Same typed paths exists
						int k3 = k2;
						if(PathExists((i-1)*2+1, i*2, i*2+1, k3, j, otherNode, m)) {
							newHistory = new Matcher5PathHistory();
							newHistory->lastNode = k;
							newHistory->lastHistory = m;
							newHistory->pathBitMap = EncodePath(EncodePath(currentHistory->pathBitMap, j), otherNode);
							newHistory->cost = currentHistory->cost + GetCost((i-1)*2+1, i*2, k3, j) + GetCost(i*2, i*2+1, j, otherNode);
							currentJunction->history.push_back(newHistory);
						}
					}
				}
			}
			//PrintJunction(i, j);
		}
	}
}

Node * Matcher5::FindPath(int patternNode, int baseNode, unsigned int historyElement) {
	Node * path = new Node();
	path->cost = -1;
	Matcher5Junction * junction;
	Matcher5PathHistory * history;
	while (patternNode >= 0) {
		assert((patternNode >= 0) && (baseNode >= 0) && (historyElement >= 0));
		path->n1.insert(path->n1.begin(), patternNode*2 + 1);
		path->n1.insert(path->n1.begin(), patternNode*2);
		path->n2.insert(path->n2.begin(), GetOtherBaseNode(baseNode));
		path->n2.insert(path->n2.begin(), baseNode);

		junction = (Matcher5Junction*)junctions[GetIndex(patternNode, baseNode)];
		history = (Matcher5PathHistory*)junction->history[historyElement];
		baseNode = history->lastNode;
		historyElement = history->lastHistory;
		patternNode--;
		if(path->cost == -1) {
			path->cost = history->cost;
		}
	}
	return path;
}

void Matcher5::SaveResults() {
	printf("\n\nTop %d Legal Paths:\n", RESULT_COUNT);
	Matcher5Junction * junction;
	Matcher5PathHistory * history;
	Node * node;
	bool inserted;
	for(int j = 0; j < baseGraph->GetNodeCount()+1; j++) {
		junction = (Matcher5Junction*)junctions[GetIndex(patternGraph->pdbStructures.size()-1, j)];
		for(int k = 0; k < junction->history.size(); k++) {
			history = (Matcher5PathHistory*) junction->history[k];
			inserted = false;
			for(int l = 0; l < results.size(); l++) {
				if(history->cost < ((Node*)results[l])->cost) {
					node = FindPath(patternGraph->pdbStructures.size()-1, j, k);
					results.insert(results.begin() + l, node);
					inserted = true;
					break;
				}
			}

			if((!inserted )&& (results.size() < RESULT_COUNT)) {
				node = FindPath(patternGraph->pdbStructures.size()-1, j, k);
				results.push_back(node);
			}

			if(results.size() >= RESULT_COUNT) {
				delete results[RESULT_COUNT-1];
				results.erase(results.end() - 1);
			}
		}
	}

	for(int i = 0; i < results.size(); i++) {
		((Node*)results[i])->PrintNodeConcise();
	}
}

void Matcher5::PrintJunction(int patternHelix, int baseHelix) {
	Matcher5Junction * junc = junctions[GetIndex(patternHelix, baseHelix)];
	if(junc->history.size() > 0) {
		printf("\nJunction History (%d, %d)\n", patternHelix+1, baseHelix+1);
		for(int i = 0; i < junc->history.size(); i++) {
			printf("\tBit map : %d |", ((Matcher5PathHistory*)junc->history[i])->pathBitMap, ((Matcher5PathHistory*)junc->history[i])->cost);
			Node * node = FindPath(patternHelix, baseHelix, i);
			node->PrintNodeConcise();
		}
	}
}
void Matcher5::NormalizeGraphs() {

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
#endif