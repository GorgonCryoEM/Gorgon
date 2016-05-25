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
			Matcher(Graph & sequenceGraph, Graph & skeletonGraph);
			int run();
			SSEResult getResult(int rank);
			void destruct();

		protected:
			WongMatch * matcher;
	        Graph & skeleton;
	        Graph & sequence;
		};

		inline Matcher::Matcher(Graph & sequenceGraph, Graph & skeletonGraph)
		            : sequence(sequenceGraph), skeleton(skeletonGraph)
		{}

		inline int Matcher::run() {
			clock_t start;

			PERFORMANCE_COMPARISON_MODE = false;

			// Match Graphs
			// Constrained no future
			if(MISSING_HELIX_COUNT == -1) {
				matcher = new WongMatch(sequence, skeleton);
			} else {
				matcher = new WongMatch(sequence, skeleton, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
			}
			start = clock();
			int matchCount = matcher->run(start);
			matcher->saveResults();

			return matchCount;
		}

		inline SSEResult Matcher::getResult(int rank) {
			return matcher->getResult(rank);
		}

		inline void Matcher::destruct() {
			delete matcher;
		}
	}
#endif
