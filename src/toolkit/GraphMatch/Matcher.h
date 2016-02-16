#ifndef TOOLKIT_GRAPHMATCH_QUERY_ENGINE_H
#define TOOLKIT_GRAPHMATCH_QUERY_ENGINE_H

//#include <cstdlib>
//#include <cstdio>
//#include "Graph.h"
//#include "WongMatch15Constrained.h"
#include "WongMatch.h"
//#include "WongMatch15ConstrainedOnlyA.h"
#include "PDBReader.h"
#include "Readers/SEQReader.h"
#include "Readers/SkeletonReader.h"
//#include "Core/GlobalConstants.h"
//#include "GraphGenerator.h"
//#include <ctime>
//#include <string>
#include "ProteinMorph/SSEResult.h"

#ifdef DEBUG
	#include <iostream>
#endif

using namespace std;

	namespace GraphMatch {

		class Matcher {
		public:
			int match(Graph * sequenceGraph, Graph * skeletonGraph);
			SSEResult GetSolution(int rank);
			void destruct();
			Graph * loadSequence();
			Graph * loadSkeleton();
		private:
			WongMatch * matcher;
		};


		Graph * Matcher::loadSequence() {
			#ifdef GORGON_DEBUG
				cout << "In QueryEngine::LoadSequenceGraph" << endl;
			#endif
			clock_t start, finish;
			Graph * graph;
			string type = SEQUENCE_FILE_TYPE; //easier than doing comparison with a char array
			#ifdef VERBOSE
				printf("Pattern Graph \n");
			#endif
			start = clock();
			if (type == "PDB")
				graph = PDBReader::ReadFile(SEQUENCE_FILE_NAME.c_str());
			else if (type == "SEQ")
				graph = SEQReader::ReadFile(SEQUENCE_FILE_NAME.c_str());
			else
				return NULL;
			finish = clock();
			#ifdef VERBOSE
				printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
				graph->PrintGraph();
			#endif
			return graph;
		}

		Graph * Matcher::loadSkeleton() {
			clock_t start, finish;
			Graph * graph;
			#ifdef VERBOSE
				printf("Base Graph \n");
			#endif
			start = clock();
			graph = SkeletonReader::ReadFile(MRC_FILE_NAME, VRML_HELIX_FILE_NAME, SSE_FILE_NAME, VRML_SHEET_FILE_NAME);
			finish = clock();
			#ifdef VERBOSE
				printf("\033[32m\tReading Base file Took %f seconds.\n\033[0m", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
				graph->PrintGraph();
			#endif
			return graph;
		}


		int Matcher::match(Graph * sequenceGraph, Graph * skeletonGraph) {
			clock_t start;

			PERFORMANCE_COMPARISON_MODE = false;

			// Match Graphs
			// Constrained no future
			if(MISSING_HELIX_COUNT == -1) {
				matcher = new WongMatch(sequenceGraph, skeletonGraph);
			} else {
				matcher = new WongMatch(sequenceGraph, skeletonGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
			}
			start = clock();
			int matchCount = matcher->RunMatching(start);
			matcher->SaveResults();

			return matchCount;
		}

		SSEResult Matcher::GetSolution(int rank) {
			return matcher->GetResult(rank);
		}

		void Matcher::destruct() {
			delete matcher;
		}
	}
#endif
