/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef LEDAMATCH_H
#define LEDAMATCH_H

#include "StandardGraph.h"
#include "GlobalConstants.h"
#include <LEDA/graph/graph_morphism.h>
#include <LEDA/graph/graph_morphism_algorithm.h>
#include <LEDA/graph/graph.h>
#include <vector>


//using namespace leda;
using namespace std;

class EdgeAttribute {
public:
	double weight;
	int type;
public:
	EdgeAttribute(double weight, int type) {
		this->weight = weight;
		this->type = type;
	}
};

class cmp_edges: public leda::leda_cmp_base<leda::edge> {
public:
	int operator()(const leda::edge& e, const leda::edge& f) const
	{
		int a = 1;
		return 1;
	}
};

class cmp_nodes: public leda::leda_cmp_base<leda::node> {
public:
	int operator()(const leda::node& n, const leda::node& m) const
	{
		int a = 1;
		return 0;
	}
};



class LedaMatch {
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
	leda::GRAPH<int, EdgeAttribute*> g1;
	leda::GRAPH<int, EdgeAttribute*> g2;
public:
	LedaMatch(StandardGraph * patternGraph, StandardGraph * baseGraph);
	LedaMatch(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~LedaMatch();
	void RunMatching();
	void SaveResults();
private:
	void NormalizeGraphs();
	void FindPossiblePaths();
	void MakeLedaGraphs();
private:
	int missingHelixCount;
	int missingSheetCount;
};

LedaMatch::LedaMatch (StandardGraph * patternGraph, StandardGraph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;
}

LedaMatch::LedaMatch(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
}

LedaMatch::~LedaMatch() {
}


void LedaMatch::RunMatching() {
	//NormalizeGraphs();
	MakeLedaGraphs();
	FindPossiblePaths();
}


void LedaMatch::FindPossiblePaths() {
	leda::graph_morphism_algorithm<>* alg = new leda::graph_morphism<leda::graph, leda::vf2<leda::graph> >();
	leda::graph_morphism_algorithm<>::morphism_list morphisms;
	leda::graph_morphism_algorithm<>::node_compat cn = cmp_nodes();
	leda::graph_morphism_algorithm<>::edge_compat ce = cmp_edges();
	//alg->find_all_sub(g2, g1, morphisms, cn, ce);	
	leda::graph_morphism_algorithm<>::node_morphism * nmorphism = new leda::graph_morphism_algorithm<>::node_morphism();
	leda::graph_morphism_algorithm<>::edge_morphism * emorphism = new leda::graph_morphism_algorithm<>::edge_morphism();
	bool isomorphic = leda::graph_morphism<>().find_sub(g2, g1, nmorphism, emorphism, cn, ce);
	if(isomorphic) {
		printf("Isomorphic\n");
	} else {
		printf("Not isomorphic\n");
	}
	printf("morphism count :%d\n", nmorphism->size());	
}


void LedaMatch::SaveResults() {
}

void LedaMatch::NormalizeGraphs() {

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
				//baseGraph->euclidianMatrix[i-1][j-1] *= ratio; 
			}

		}
	}	
	baseGraph->PrintGraph();
}


void LedaMatch::MakeLedaGraphs() {
	vector<leda::graph::node> nodes;
	leda::graph::node n;
	for(int i = 0; i < patternGraph->nodeCount; i++) {
		n = g1.new_node(0);
		nodes.push_back(n);
	}

	for(int i = 0; i < patternGraph->nodeCount; i++) {
		for(int j = 0; j < patternGraph->nodeCount; j++) {
			if(patternGraph->adjacencyMatrix[i][j][1] != MAXINT) {
				g1.new_edge(nodes[i], nodes[j], new EdgeAttribute(patternGraph->adjacencyMatrix[i][j][1], patternGraph->adjacencyMatrix[i][j][0]));
			}
		}
	}

	nodes.clear();
	for(int i = 0; i < baseGraph->nodeCount; i++) {
		n = g2.new_node();
		nodes.push_back(n);
	}

	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j < baseGraph->nodeCount; j++) {
			if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT) {
				g2.new_edge(nodes[i], nodes[j], new EdgeAttribute(patternGraph->adjacencyMatrix[i][j][1], patternGraph->adjacencyMatrix[i][j][0]));
			}
		}
	}
	nodes.clear();


}
#endif


//#include <LEDA/graph/graph.h>#include <LEDA/graph/graph_morphism.h>
//// declare the input graphs.
//graph g1, g2;
//// In order to use node compatibility, declare associated node maps for the
//// attributes and a corresponding node compatibility function
//// (exemplary, see above for the definition of identity_compatibility).
//node_map<int> nm1(g1), nm2(g2);
//identity_compatibility<int> ic(nm1, nm2);
//// do something useful to build up the graphs and the attributes
//// instantiate the algorithm object
//graph_morphism<graph, conauto<graph> > alg;
//// declare the node and edge mapping arrays
//node_array<node> node_mapping(g2);
//edge_array<edge> edge_mapping(g2);
//// prepare a graph morphism data structure for the first graph.
//graph_morphism_algorithm<>::prep_graph pg1 = alg.prepare_graph(g1, ic);
//// find the graph isomorphism.
//bool isomorphic = alg.find_iso(pg1, g2, &node_mapping, &edge_mapping, ic);
//// delete the prepared graph data structure again.
//alg.delete_prepared_graph(pg1);