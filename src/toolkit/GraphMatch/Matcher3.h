/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 02/11/2006
*/

#ifndef MATCHER3_H
#define MATCHER3_H

#include "Graph.h"
#include "GlobalConstants.h"
#include "PathGenerator.h"

class Matcher3 {
public:
	Graph * patternGraph;
	Graph * baseGraph;
public:
	Matcher3(Graph * patternGraph, Graph * baseGraph);
	~Matcher3();
	void RunMatching();
	void SaveResults();
	double GetCost(int & baseNode1, int & baseNode2, int baseNode3, Node * node, double patternCost, bool doLeft);
	void FindPossiblePaths();
	void NormalizeGraphs();
private:
	NodeList * ExpandNode(Node *  node);
	vector<Matcher2Helix> patternHelixes;
	vector<Matcher2Helix> baseHelixes;
};

Matcher3::Matcher3 (Graph * patternGraph, Graph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
}

Matcher3::~Matcher3() {
}

void Matcher3::RunMatching() {
	NormalizeGraphs();
	FindPossiblePaths();
}

double Matcher3::GetCost(int & baseNode1, int & baseNode2, int baseNode3, Node * node, double patternCost, bool doLeft) {
	// Found a Dummy Helix
	if((baseNode1 >= baseGraph->nodeCount) || (baseNode2 >= baseGraph->nodeCount)) {
		return 0;
	}
	
	// Earlier step chose a Dummy Helix
	if(baseNode3 >= baseGraph->nodeCount) {
		if(node->n2[0] == baseNode3) {
			baseNode3 = node->n2[2];
		} else {
			baseNode3 = node->n2[node->n2.size() - 3];
		}
	}

	// No matching helix in base graph
	if(baseGraph->adjacencyMatrix[baseNode1][baseNode2][0] != GRAPHEDGE_HELIX) {
		return MISSING_HELIX_PENALTY;
	}

	// No matching edge in base graph
	if((baseGraph->adjacencyMatrix[baseNode2][baseNode3][0] != GRAPHEDGE_LOOP) && 
		(baseGraph->adjacencyMatrix[baseNode1][baseNode3][0] != GRAPHEDGE_LOOP)){
		return MATCHING2_NOPATH_PENALTY;
	}


	double cost = baseGraph->adjacencyMatrix[baseNode1][baseNode2][1];
	double cost2 = baseGraph->adjacencyMatrix[baseNode2][baseNode3][1];
	double cost3 = baseGraph->adjacencyMatrix[baseNode1][baseNode3][1];
	if(doLeft) {
		if(cost3 < cost2) {
			swap(baseNode1, baseNode2);   
			cost += cost3;
		} else {
			cost += cost2;
		}
	}
	else {
		if(cost2 < cost3) {
			swap(baseNode1, baseNode2);
			cost += cost2;
		} else {
			cost += cost3;
		}
	}
	return fabs(cost - patternCost);
}

NodeList * Matcher3::ExpandNode(Node * node) {
	NodeList * nodes = new NodeList();
	Node * currentNode;

	int leftPatternNode = node->n1[0];
	int rightPatternNode = node->n1[node->n1.size()-1];
	int leftBaseNode = node->n2[0];
	int rightBaseNode = node->n2[node->n2.size()-1];

	bool doLeft = leftPatternNode > 0;
	bool doRight = rightPatternNode < patternGraph->nodeCount-1;

	int patternNode1;
	int patternNode2;
	int baseNode1;
	int baseNode2;
	vector<bool> baseUsed;
	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size()+1; i++) {
		baseUsed.push_back(false);
	}
	for(int i = 0; i < (int)node->n2.size(); i++) {
		baseUsed[node->n2[i]/2] = true;
	}

	double patternCost;
	double cost;
	double tempCost;
	int tempBaseNode1;
	int tempBaseNode2;
	int helixCount;

	if(doLeft) {
		patternNode1 = leftPatternNode - 2;
		patternNode2 = leftPatternNode - 1;
		for(int j = 0; j < NODE_EXPANSIONS_PER_LEVEL; j++) {
			baseNode1 = -1;
			cost = MAXINT;
			patternCost = patternGraph->adjacencyMatrix[patternNode1][patternNode2][1] + patternGraph->adjacencyMatrix[patternNode2][leftPatternNode][1];
			helixCount = (int)baseGraph->skeletonHelixes.size();
			// To avoid placing dummy helixes at the start unless absolutely necessary
			if((patternGraph->nodeCount > baseGraph->nodeCount) || (patternNode1 != 0)) {
				helixCount++;
			}
			for(int i = 0; i < helixCount; i++) {
				if(!baseUsed[i]) {
					tempBaseNode1 = i*2;
					tempBaseNode2 = i*2+1;
					tempCost = GetCost(tempBaseNode1, tempBaseNode2, leftBaseNode, node, patternCost, true);
					if(tempCost < cost) {
						cost = tempCost;
						baseNode1 = tempBaseNode1;
						baseNode2 = tempBaseNode2;
					}
				}
			}

			if(baseNode1 >= 0) {
				baseUsed[baseNode1/2] = true;
				currentNode = new Node(node);
				currentNode->n1.insert(currentNode->n1.begin(), patternNode2);
				currentNode->n1.insert(currentNode->n1.begin(), patternNode1);
				currentNode->n2.insert(currentNode->n2.begin(), baseNode2);
				currentNode->n2.insert(currentNode->n2.begin(), baseNode1);
				currentNode->cost += cost;
				nodes->nodes.push_back(currentNode);
			}
		}

	} else if (doRight) {
		patternNode1 = rightPatternNode + 1;
		patternNode2 = rightPatternNode + 2;
		for(int j = 0; j < NODE_EXPANSIONS_PER_LEVEL; j++) {
			baseNode1 = -1;
			cost = MAXINT;
			patternCost = patternGraph->adjacencyMatrix[patternNode1][patternNode2][1] + patternGraph->adjacencyMatrix[rightPatternNode][patternNode1][1];
			helixCount = (int)baseGraph->skeletonHelixes.size();
			// To avoid placing dummy helixes at the end unless absolutely necessary
			if((patternGraph->nodeCount > baseGraph->nodeCount) || (patternNode2 != patternGraph->nodeCount - 1)) {
				helixCount++;
			}
			for(int i = 0; i < helixCount; i++) {
				if(!baseUsed[i]) {
					tempBaseNode1 = i*2;
					tempBaseNode2 = i*2+1;
					tempCost = GetCost(tempBaseNode1, tempBaseNode2, rightBaseNode, node, patternCost, false);
					if(tempCost < cost) {
						cost = tempCost;
						baseNode1 = tempBaseNode1;
						baseNode2 = tempBaseNode2;
					}
				}
			}

			if(baseNode1 >= 0) {
				baseUsed[baseNode1/2] = true;
				currentNode = new Node(node);
				currentNode->n1.push_back(patternNode1);
				currentNode->n1.push_back(patternNode2);
				currentNode->n2.push_back(baseNode1);
				currentNode->n2.push_back(baseNode2);
				currentNode->cost += cost;
				nodes->nodes.push_back(currentNode);
			}
		}
	}

	baseUsed.clear();
    
	return nodes;
}

void Matcher3::FindPossiblePaths() {
	NodeList * pathList = new NodeList();
	double patternLength = -1;
	double baseLength = -1;
	int maxHelix;
	int helixLength;
	for(int i = 0; i < patternGraph->pdbStructures.size(); i++) {
		helixLength = patternGraph->pdbStructures[i]->GetLength();
		if(helixLength > patternLength) {
			patternLength = helixLength;
			maxHelix = i;
		}
	}
	int patternNode1 = maxHelix * 2;
	int patternNode2 = patternNode1 + 1;
	int baseNode1;
	int baseNode2;
	double cost;
	Node * currentNode;

	vector<bool> used;
	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
		used.push_back(false);
	}

	for(int j = 0; j < NODE_EXPANSIONS_PER_LEVEL; j++) {
		baseLength = -1;
		for(int i = 0; i < baseGraph->skeletonHelixes.size(); i++) {
			helixLength = baseGraph->skeletonHelixes[i]->length;
			if((helixLength > baseLength) && !used[i]) {
				baseLength = helixLength;
				maxHelix = i;
			}
		}
		if(baseLength >= 0) {
			used[maxHelix] = true;
			baseNode1 = maxHelix*2;
			baseNode2 = baseNode1 + 1;
			currentNode = new Node();
			currentNode->n1.push_back(patternNode1);
			currentNode->n1.push_back(patternNode2);
			currentNode->n2.push_back(baseNode1);
			currentNode->n2.push_back(baseNode2);
			currentNode->cost = HELIX_WEIGHT_COEFFICIENT * (fabs(patternLength - baseLength));
			pathList->InsertSorted(currentNode);

			currentNode = new Node();
			currentNode->n1.push_back(patternNode1);
			currentNode->n1.push_back(patternNode2);
			currentNode->n2.push_back(baseNode2);
			currentNode->n2.push_back(baseNode1);
			currentNode->cost = HELIX_WEIGHT_COEFFICIENT * (fabs(patternLength - baseLength));
			pathList->InsertSorted(currentNode);
		}
	}

	NodeList * pathList2 = new NodeList();
	NodeList * tempList;
	NodeList * solution = new NodeList();

	bool added = true;
	while (added) {
		added = false;
		for(int j = 0; j < (int)pathList->nodes.size(); j++) {
			currentNode = pathList->nodes[j];
			//currentNode->PrintNodeConcise();
			tempList = ExpandNode(currentNode);
			added = added || (tempList->nodes.size() > 0);
			for(int k = 0; k < (int)tempList->nodes.size(); k++) {
				if(tempList->nodes[k]->n1.size() == patternGraph->nodeCount) {
					solution->InsertSorted(tempList->nodes[k]);
				} else {
					pathList2->InsertSorted(tempList->nodes[k]);
				}
			}
			
			tempList->nodes.clear();
			delete tempList;
			delete currentNode;
		}

		tempList = pathList;
		pathList = pathList2;
		pathList2 = tempList;
		pathList2->nodes.clear();
	}

	solution->RemoveDuplicates();

	printf("\nSolutions: \n");
	for(int i = 0; i < solution->nodes.size(); i++) {
		//solution->nodes[i]->PrintNodeConcise();
	}

	PathGenerator * pathGen = new PathGenerator(baseGraph);
	solution->nodes[0]->n2[0] = 1;
	solution->nodes[0]->n2[1] = 0;
	solution->nodes[0]->n2[4] = 6;
	solution->nodes[0]->n2[5] = 7;
	pathGen->GenerateGraph(solution->nodes[0], "Solution1.mrc");
	delete pathGen;
}

void Matcher3::SaveResults() {
}

void Matcher3::NormalizeGraphs() {

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