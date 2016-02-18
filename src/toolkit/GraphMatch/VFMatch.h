/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef VFMATCH_H
#define VFMATCH_H

#include "StandardGraph.h"
#include "GlobalConstants.h"
#include "argraph.h"
#include "argedit.h"
#include "ull_sub_state.h"
#include "vf_mono_state.h"
#include "vf2_mono_state.h"
#include "match.h"
#include <vector>


using namespace std;


class EdgeAttribute {
public:
	double weight;
	double type;
public:
	EdgeAttribute(double weight, double type) {
		this->weight = weight;
		this->type = type;
	}
};

class EdgeDestroyer : public AttrDestroyer {
public:
	virtual void destroy(void * p) {
		delete p;
	}
};

class EdgeComparator : public AttrComparator {
private:
	double threshold;
public:
	EdgeComparator::EdgeComparator(double threshold) {
		this->threshold = threshold;
	}

	virtual bool compatible(void *ea, void *eb) {
		EdgeAttribute * a = (EdgeAttribute *)ea;
		EdgeAttribute * b = (EdgeAttribute *)eb;
		return ((fabs(a->weight - b->weight) <= threshold) && (fabs(a->type - b->type) <= 0.1));
	}
};

class VFMatch {
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
	ARGraph<void, EdgeAttribute> * g1;
	ARGraph<void, EdgeAttribute> * g2;
public:
	VFMatch(StandardGraph * patternGraph, StandardGraph * baseGraph);
	VFMatch(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);	
	~VFMatch();
	void RunMatchingUllmann();
	void RunMatchingVF();
	void RunMatchingVF2();
private:
	void HandleMissingHelixes();
	void NormalizeGraphs();
	void MakeVFGraphs();
	void MakeLedaGraphs();
	void PerformMatching( State * state );
private:
	int missingHelixCount;
	int missingSheetCount;
};

VFMatch::VFMatch (StandardGraph * patternGraph, StandardGraph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	MakeVFGraphs();
}

VFMatch::VFMatch(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
		HandleMissingHelixes();
	}
	MakeVFGraphs();
}

VFMatch::~VFMatch() {
	delete g1;
	delete g2;
}


void VFMatch::RunMatchingUllmann() {
	UllSubState * state = new UllSubState(g1, g2); 
	PerformMatching(state);
	delete state;
}
void VFMatch::RunMatchingVF() {
	VFMonoState * state = new VFMonoState(g1, g2); 
	PerformMatching(state);
	delete state;
}
void VFMatch::RunMatchingVF2() {
	VF2MonoState * state = new VF2MonoState(g1, g2); 
	PerformMatching(state);
	delete state;
}

void VFMatch::MakeVFGraphs() {

	ARGEdit ed1;
	for(int i = 0; i < patternGraph->nodeCount; i++) {
		ed1.InsertNode(NULL);
	}
	for(int i = 0; i < patternGraph->nodeCount; i++) {
		for(int j = 0; j < patternGraph->nodeCount; j++) {
			if((i != j) && (fabs(patternGraph->adjacencyMatrix[i][j][1] - MAXINT) > 0.1)) {
				ed1.InsertEdge(i, j, new EdgeAttribute(patternGraph->adjacencyMatrix[i][j][1], patternGraph->adjacencyMatrix[i][j][0]));
			}
		}
	}
	g1 = new ARGraph<void, EdgeAttribute>(&ed1);
	g1->SetEdgeDestroyer(new EdgeDestroyer);
	g1->SetEdgeComparator(new EdgeComparator(100));


	ARGEdit ed2;
	for(int i = 0; i < baseGraph->nodeCount; i++) {
		ed2.InsertNode(NULL);
	}
	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j < baseGraph->nodeCount; j++) {
			if((i != j) && (fabs(baseGraph->adjacencyMatrix[i][j][1] - MAXINT) > 0.1)) {
				ed2.InsertEdge(i, j, new EdgeAttribute(baseGraph->adjacencyMatrix[i][j][1], baseGraph->adjacencyMatrix[i][j][0]));
			}
		}
	}
	g2 = new ARGraph<void, EdgeAttribute>(&ed2);
	g2->SetEdgeDestroyer(new EdgeDestroyer);
	g2->SetEdgeComparator(new EdgeComparator(100));
}


void VFMatch::PerformMatching( State * state ) {
	node_id ni1[MAX_NODES], ni2[MAX_NODES];
	int n;
	if(PERFORMANCE_COMPARISON_MODE) {
		match(state, &n, ni1, ni2);
	} else {
		if(!match(state, &n, ni1, ni2)) {
			printf("No Matching Found!\n");
		} else {
			printf("Found a matching with %d nodes:\n", n);
			for(int i = 0 ; i < n; i++) {
				printf("%d ", ni2[i] + 1);
			}
		}
	}
}
void VFMatch::HandleMissingHelixes() {
	printf("Modifying the graph to handle missing helixes\n");
	if(missingHelixCount == 0) 
		return;

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
	
}

void VFMatch::NormalizeGraphs() {

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

