#ifndef QUERY_ENGINE_H
#define QUERY_ENGINE_H

#include <stdlib.h>
#include <stdio.h>
#include "StandardGraph.h"
#include "WongMatch15Constrained.h"
#include "WongMatch15ConstrainedNoFuture.h"
#include "WongMatch15ConstrainedOnlyA.h"
#include "PDBReader.h"
#include "SkeletonReader.h"
#include "GlobalConstants.h"
#include "GraphGenerator.h"
#include <time.h>

using namespace std;

public class QueryEngine {
public:
	int DoGraphMatching();
	LinkedNode * GetSolution(int rank);
	void FinishGraphMatching();
private:
	WongMatch15ConstrainedNoFuture * matcherConstrainedNoFuture;
};

int QueryEngine::DoGraphMatching() {
	clock_t start, finish;
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;

	PERFORMANCE_COMPARISON_MODE = false;

	#ifdef VERBOSE
		printf("Pattern Graph \n");
	#endif
	start = clock();
	patternGraph = PDBReader::ReadFile(PDB_FILE_NAME);
	finish = clock();
	#ifdef VERBOSE
		printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
		patternGraph->PrintGraph();

		printf("\n\nBase Graph \n");		
	#endif
	start = clock();
	baseGraph = SkeletonReader::ReadFile(MRC_FILE_NAME, VRML_HELIX_FILE_NAME, SSE_FILE_NAME, VRML_SHEET_FILE_NAME);
	finish = clock();
	#ifdef VERBOSE
		printf("\tReading Base file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
		baseGraph->PrintGraph();
	#endif


	// Match Graphs
	// Constrained no future
	if(MISSING_HELIX_COUNT == -1) {
		matcherConstrainedNoFuture = new WongMatch15ConstrainedNoFuture(patternGraph, baseGraph);
	} else {
		matcherConstrainedNoFuture = new WongMatch15ConstrainedNoFuture(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	}
	start = clock();
	int matchCount = matcherConstrainedNoFuture->RunMatching(start);
	matcherConstrainedNoFuture->SaveResults();

	delete(baseGraph);
	delete(patternGraph);

	return matchCount;
}

LinkedNode * QueryEngine::GetSolution(int rank) {
	return matcherConstrainedNoFuture->GetResult(rank);
}

void QueryEngine::FinishGraphMatching() {
	delete matcherConstrainedNoFuture;
}
#endif