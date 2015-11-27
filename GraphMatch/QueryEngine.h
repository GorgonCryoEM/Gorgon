// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for performing correspondence queries.

#ifndef QUERY_ENGINE_H
#define QUERY_ENGINE_H

#include <cstdlib>
#include <cstdio>
#include "StandardGraph.h"
#include "WongMatch15Constrained.h"
#include "WongMatch15ConstrainedNoFuture.h"
#include "WongMatch15ConstrainedOnlyA.h"
#include "PDBReader.h"
#include "SEQReader.h"
#include "SkeletonReader.h"
#include "GlobalConstants.h"
#include "GraphGenerator.h"
#include <ctime>
#include <string>

#ifdef DEBUG
	#include <iostream>
#endif

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class QueryEngine {
		public:
			int DoGraphMatching(StandardGraph * sequenceGraph, StandardGraph * skeletonGraph);
			SSECorrespondenceResult GetSolution(int rank);
			void FinishGraphMatching();
			StandardGraph * LoadSequenceGraph();
			StandardGraph * LoadSkeletonGraph();
		private:
			WongMatch15ConstrainedNoFuture * matcherConstrainedNoFuture;
		};


		StandardGraph * QueryEngine::LoadSequenceGraph() {
			#ifdef DEBUG
				cout << "In QueryEngine::LoadSequenceGraph" << endl;
			#endif
			clock_t start, finish;
			StandardGraph * graph;
			string type = SEQUENCE_FILE_TYPE; //easier than doing comparison with a char array
			#ifdef VERBOSE
				printf("Pattern Graph \n");
			#endif
			start = clock();
			if (type == "PDB")
				graph = PDBReader::ReadFile(SEQUENCE_FILE_NAME);
			else if (type == "SEQ")
				graph = SEQReader::ReadFile(SEQUENCE_FILE_NAME);
			else 
				return NULL;
			finish = clock();
			#ifdef VERBOSE
				printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
				graph->PrintGraph();	
			#endif
			return graph;
		}

		StandardGraph * QueryEngine::LoadSkeletonGraph() {
			clock_t start, finish;
			StandardGraph * graph;
			#ifdef VERBOSE
				printf("Base Graph \n");
			#endif
			start = clock();
			graph = SkeletonReader::ReadFile(MRC_FILE_NAME, VRML_HELIX_FILE_NAME, SSE_FILE_NAME, VRML_SHEET_FILE_NAME);
			finish = clock();
			#ifdef VERBOSE
				printf("\tReading Base file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
				graph->PrintGraph();
			#endif
			return graph;
		}


		int QueryEngine::DoGraphMatching(StandardGraph * sequenceGraph, StandardGraph * skeletonGraph) {
			clock_t start;

			PERFORMANCE_COMPARISON_MODE = false;

			// Match Graphs
			// Constrained no future
			if(MISSING_HELIX_COUNT == -1) {
				matcherConstrainedNoFuture = new WongMatch15ConstrainedNoFuture(sequenceGraph, skeletonGraph);
			} else {
				matcherConstrainedNoFuture = new WongMatch15ConstrainedNoFuture(sequenceGraph, skeletonGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
			}
			start = clock();
			int matchCount = matcherConstrainedNoFuture->RunMatching(start);
			matcherConstrainedNoFuture->SaveResults();

			return matchCount;
		}

		SSECorrespondenceResult QueryEngine::GetSolution(int rank) {
			return matcherConstrainedNoFuture->GetResult(rank);
		}

		void QueryEngine::FinishGraphMatching() {
			delete matcherConstrainedNoFuture;
		}
	}
}
#endif
