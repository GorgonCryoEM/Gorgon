/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 02/11/2006
*/

#ifndef MATCHER4_H
#define MATCHER4_H

#include "Graph.h"
#include "Node.h"
#include "GlobalConstants.h"

class Matcher4 {
public:
	Graph * patternGraph;
	Graph * baseGraph;
public:
	Matcher4(Graph * patternGraph, Graph * baseGraph);
	~Matcher4();
	void RunMatching();
	void SaveResults();
private:
	void NormalizeGraphs();
	void FindPossiblePaths();
	int GetIndex(int patternHelixNo, int baseHelixNo);
	vector<double> costs;
	vector<Node*>  paths;
};

Matcher4::Matcher4 (Graph * patternGraph, Graph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	for(int i = 0; i < patternGraph->GetNodeCount() * (baseGraph->GetNodeCount()+1); i++) {
		costs.push_back(0.0);
		paths.push_back(0);
	}
}

Matcher4::~Matcher4() {
	for(int i = 0; i < paths.size(); i++) {
		if(paths[i] != 0) {
			delete paths[i];
		}
	}
	paths.clear();
	costs.clear();
}

int Matcher4::GetIndex(int patternHelixNo, int baseHelixNo) {
	return patternHelixNo * patternGraph->GetNodeCount() + baseHelixNo;
}

void Matcher4::RunMatching() {
	NormalizeGraphs();
	FindPossiblePaths();
}

void Matcher4::FindPossiblePaths() {
	for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
		Node * path = new Node();
		path->cost = 0.0;
		path->n1.push_back(0);
		path->n2.push_back(j);
		paths[GetIndex(0, j)] = path; 
	}

	double currentCost;
	double minCost;
	int minNode;
	Node * currentPath;
	bool assigned;
	for(int i = 1; i < patternGraph->GetNodeCount(); i++) {
		for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
			minCost = MAXDOUBLE;
			for(int k = 0; k < baseGraph->GetNodeCount() + 1; k++) {
				// Adding original cost
				currentCost = costs[GetIndex(i-1, k)];
				// Current node is a dummy node
				if(j ==  baseGraph->GetNodeCount()) {
					currentCost += MATCHING2_NOPATH_PENALTY;
				// Both nodes are real nodes.
				} else {
					currentPath = paths[GetIndex(i-1, k)];
					assigned = false;
					for(int m = 0; m < currentPath->n2.size(); m++) {
						if(currentPath->n2[m] == j) {
							assigned = true;
							break;
						}
					}

					// Is node already used in the path.
					if(assigned) {
						currentCost += MATCHING2_NODE_REUSE_PENALTY;
					// Is the edge a wrong type of match? ie. loop with helix or sheet.
					} else if(patternGraph->adjacencyMatrix[i-1][i][0] != baseGraph->adjacencyMatrix[k][j][0]) {
						currentCost += MATCHING2_EDGE_MISMATCH_PENALTY;
					// Good matching, what is the cost difference?
					} else {
						currentCost += fabs(patternGraph->adjacencyMatrix[i-1][i][1] - baseGraph->adjacencyMatrix[k][j][1]);
					}
				}
				if(currentCost < minCost) {
					minCost = currentCost;
					minNode = k;
				}
			}
			costs[GetIndex(i, j)] = minCost;
			Node * path = new Node(paths[GetIndex(i-1, minNode)]);
			path->n1.push_back(i);
			path->n2.push_back(j);
			path->cost = minCost;
			paths[GetIndex(i, j)] = path;		
		}


		printf("\nLevel %0:\n", i);
		for(int j = 0; j < baseGraph->GetNodeCount() + 1; j++) {
			paths[GetIndex(i, j)]->PrintNodeConcise();
		}
	}
}
void Matcher4::SaveResults() {
	printf("\nSolutions:\n");
	for(int i = 0; i < baseGraph->GetNodeCount() + 1; i++) {
		paths[GetIndex(patternGraph->GetNodeCount() - 1, i)]->PrintNodeConcise();
	}
}

void Matcher4::NormalizeGraphs() {

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