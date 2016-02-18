/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 02/11/2006
*/

#ifndef MATCHER2_H
#define MATCHER2_H

#include "Graph.h"
#include "GlobalConstants.h"
#include "Matching2WeightList.h"

class Matcher2 {
public:
	Graph * patternGraph;
	Graph * baseGraph;
public:
	Matcher2(Graph * patternGraph, Graph * baseGraph);
	~Matcher2();
	void RunMatching();
	void SaveResults();
	void CalculateWeightMatrix();
	void FindPossiblePaths();
	void AddEdgeWeights();
	void AddEdgeWeights2();
	Node * AddEdgeWeightsToNode(Node * node);
	void NormalizeGraphs();
private:
	Matching2WeightList * weightList;
	NodeList * pathList;
	vector<Matcher2Helix> patternHelixes;
	vector<Matcher2Helix> baseHelixes;
};

Matcher2::Matcher2 (Graph * patternGraph, Graph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
}

Matcher2::~Matcher2() {
}

void Matcher2::RunMatching() {
	NormalizeGraphs();
	CalculateWeightMatrix();
	FindPossiblePaths();
	AddEdgeWeights();
}

void Matcher2::CalculateWeightMatrix() {
	patternHelixes = patternGraph->GetHelixLengths();
	baseHelixes = baseGraph->GetHelixLengths();

	vector<double> patternScores;
	vector<double> baseScores;

	double score;

	int patternHelixCount = (int)patternHelixes.size();
	int baseHelixCount = (int)baseHelixes.size();
    
	// No need to normalize lengths anymore
	//for(int i = 0; i < patternHelixCount; i++) {
	//	score = 0;
	//	for(int j = 0; j < patternHelixCount; j++) {
	//		score += (double)patternHelixes[i].length / (double)patternHelixes[j].length;
	//	}
	//	patternScores.push_back(score / (double)patternHelixCount);
	//}

	//for(int i = 0; i < baseHelixCount; i++) {
	//	score = 0;
	//	for(int j = 0; j < baseHelixCount; j++) {
	//		score += (double)baseHelixes[i].length / (double)baseHelixes[j].length;
	//	}
	//	baseScores.push_back(score / (double)baseHelixCount);
	//}
	// Magic Node for matches where there is no helix in the base graph

	for(int i = 0; i < patternHelixCount; i++) {
		patternScores.push_back(patternHelixes[i].length);
	}
	for(int i = 0; i < baseHelixCount; i++) {
		baseScores.push_back(baseHelixes[i].length);
	}
	// Magic helix for missing helixes in base graph
	baseScores.push_back(0);
	baseHelixCount++;

	weightList = new Matching2WeightList(baseHelixCount - 1);

	printf("\n Weight Matrix :\n");

	double weight;
	double largerScore;
	for(int i = 0; i < baseHelixCount; i++) {
		printf("\t");
		for(int j = 0; j < patternHelixCount; j++) {

			if(baseScores[i] > patternScores[j]) {
				largerScore = baseScores[i];
			} else {
				largerScore = patternScores[j];
			}

			// larger errors for smaller mismatches
			weight = HELIX_WEIGHT_COEFFICIENT * (fabs(baseScores[i] - patternScores[j]) + 1) / largerScore;
			
			// same error for mismatches everywhere
			//weight = HELIX_WEIGHT_COEFFICIENT * (baseScores[i] - patternScores[j]) ;

			weightList->AddWeightSorted(weight, j, i);
			printf(" %f \t", weight);
		}
		printf(" \n");
	}
}


void Matcher2::FindPossiblePaths() {
	pathList = new NodeList();
	NodeList * pathList2 = new NodeList();
	NodeList * tempList;

	pathList->InsertSorted(new Node());

	Node * currentNode;

	for(int i = 0; i < (int)patternHelixes.size(); i++) {
		for(int k = 0; k < (int)pathList->nodes.size(); k++) {
			weightList->ClearUsed();
			for(int j = 0; j < NODE_EXPANSIONS_PER_LEVEL; j++) {
				currentNode = new Node(pathList->nodes[k]);
				if(weightList->AddBestWeightToNode(currentNode)) {
					pathList2->InsertSorted(currentNode);
				}
			}
		}
		tempList = pathList2;
		pathList2 = pathList;
		pathList = tempList;

		for(int j = 0; j < (int)pathList2->nodes.size(); j++) {
			delete pathList2->nodes[j];
		}
		pathList2->nodes.clear();
	}

	for(int i = 0; i < pathList->nodes.size(); i++) {
		pathList->nodes[i]->SortOnPattern();
	}
	pathList->RemoveDuplicates();

	printf("\nPotential Paths: \n");
	for(int i = 0; i < pathList->nodes.size(); i++) {
		pathList->nodes[i]->PrintNode();
	}
}

void Matcher2::AddEdgeWeights2() {
	NodeList * pathList2 = new NodeList();
	for(int i = 0; i < (int)pathList->nodes.size(); i++) {
		pathList2->InsertSorted(AddEdgeWeightsToNode(pathList->nodes[i]));
	}
}

Node * Matcher2::AddEdgeWeightsToNode(Node * node) {
	Node * finalNode = new Node();
	for(int i = 0; i < node->n1.size(); i++) {
	}
	return finalNode;
}

void Matcher2::AddEdgeWeights() {
	Node * currentNode;
	NodeList * pathList2 = new NodeList();
	int magicHelix = baseHelixes.size();
	int nb11, nb12, nb21, nb22, np11, np12, np21, np22, baseCost, patternCost;
	for(int i = 0; i < (int)pathList->nodes.size(); i++) {
		currentNode = pathList->nodes[i];
		for(int j = 0; j < (int)currentNode->n2.size() - 1; j++) {

			int n1 = currentNode->n2[j];
			int n2 = currentNode->n2[j+1];
			if((currentNode->n2[j] == magicHelix) || (currentNode->n2[j+1] == magicHelix)) {
				currentNode->cost += MATCHING2_NOHELIXORPATH_PENALTY;
			} else {
				nb11 = baseHelixes[currentNode->n2[j]].node1;
				nb12 = baseHelixes[currentNode->n2[j]].node2;
				nb21 = baseHelixes[currentNode->n2[j+1]].node1;
				nb22 = baseHelixes[currentNode->n2[j+1]].node2;
				baseCost = min(min(baseGraph->adjacencyMatrix[nb11][nb21][1], baseGraph->adjacencyMatrix[nb11][nb22][1]), min(baseGraph->adjacencyMatrix[nb12][nb21][1], baseGraph->adjacencyMatrix[nb12][nb22][1]));
				np11 = patternHelixes[currentNode->n1[j]].node1;
				np12 = patternHelixes[currentNode->n1[j]].node2;
				np21 = patternHelixes[currentNode->n1[j+1]].node1;
				np22 = patternHelixes[currentNode->n1[j+1]].node2;
				patternCost = min(min(patternGraph->adjacencyMatrix[np11][np21][1], patternGraph->adjacencyMatrix[np11][np22][1]), min(patternGraph->adjacencyMatrix[np12][np21][1], patternGraph->adjacencyMatrix[np12][np22][1]));
			}

			if(baseCost == MAXINT) {
				currentNode->cost += MATCHING2_NOHELIXORPATH_PENALTY;
			} else {
				currentNode->cost += abs(baseCost - patternCost);
			}
		}		
		pathList2->InsertSorted(currentNode);
	}
	pathList->nodes.clear();
	pathList = pathList2;

	printf("\nPaths with edge weights: \n");
	for(int i = 0; i < pathList->nodes.size(); i++) {
		pathList->nodes[i]->PrintNode();
	}
}
void Matcher2::SaveResults() {
}

void Matcher2::NormalizeGraphs() {

	printf("Normalizing the base graph based on helix length ratio\nNormalized Graph:\n");
	double ratio = 0;

	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
		ratio += (double)baseGraph->skeletonHelixes[i]->length / (double)baseGraph->adjacencyMatrix[i*2][i*2+1][1];
	}
	ratio = ratio / (double)baseGraph->skeletonHelixes.size();

	for(int i = 1; i <= baseGraph->nodeCount; i++) {
		for(int j = 1; j <= baseGraph->nodeCount; j++) {
			if(baseGraph->GetCost(i,j) != MAXINT) {
				baseGraph->SetCost(i,j, (int)((baseGraph->GetCost(i,j) * ratio) + 0.5));
			}

		}
	}	
	baseGraph->PrintGraph();
}
#endif