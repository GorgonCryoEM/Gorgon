// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Graph Matching algorithm based on the following paper:
//				  Wong et al., An algorithm for graph optimal monomorphism, 
//				  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636
//				  Modified to support missing helixes and constrained nodes
//				  Contains no future function

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.15.2.3  2009/06/29 21:45:29  schuhs
//   Helix and sheet matching works. Jump edge code needs some work.
//
//   Revision 1.15.2.2  2009/06/19 17:46:01  schuhs
//   Continuing to modify to work with sheets.
//
//   Revision 1.15.2.1  2009/06/18 20:43:55  schuhs
//   Starting to modify to work with sheets.
//
//   Revision 1.15  2008/11/20 20:49:09  ssa1
//   Fixing bug with loading in more VRML helices than there are in the SEQ... Also using scale directly from volume instead of a user-parameter
//
//   Revision 1.14  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.13  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//


#ifndef WONGMATCH15CONSTRAINEDNOFUTURE_H
#define WONGMATCH15CONSTRAINEDNOFUTURE_H

#include "StandardGraph.h"
#include "LinkedNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"
#include "PathGenerator.h"
#include "Structures.h"
#include <ctime>
#include <SkeletonMaker/PriorityQueue.h>
#include "SSECorrespondenceResult.h"

namespace wustl_mm {
	namespace GraphMatch {

		class WongMatch15ConstrainedNoFuture{
		public:
			StandardGraph * patternGraph;
			StandardGraph * baseGraph;
		public:
			WongMatch15ConstrainedNoFuture(StandardGraph * patternGraph, StandardGraph * baseGraph);
			WongMatch15ConstrainedNoFuture(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
			~WongMatch15ConstrainedNoFuture();
			int RunMatching(clock_t startTime);
			SSECorrespondenceResult GetResult(int rank);
			void SaveResults();


		private:
		#ifdef VERBOSE
			clock_t timeInGetA;
			clock_t timeInGetB;
			clock_t timeInQueue;
		#endif
			LinkedNode * currentNode;
			PriorityQueue<LinkedNode, double> * queue;
			vector<LinkedNodeStub*> usedNodes;
			vector<SSECorrespondenceResult> solutions;
			int missingHelixCount;
			int missingSheetCount;
			int expandCount;
			int foundCount;
			int longestMatch;
			PathGenerator * pathGenerator;

		private:
			void Init(StandardGraph * patternGraph, StandardGraph * baseGraph);
			double GetC(int p, int qp);
			double GetC(int j, int p, int qj, int qp);
			double GetC(int p, int qp, LinkedNodeStub * currentNode);
			double GetCost(int d, int m, int qj, int qp);
			double GetCPrime(int a, int b, int c, int d);
			double GetK(int p, int qp);
			double GetKPrime(int i, int q);
			double GetF();
			void PopBestNode(); // Gets the best (first) node from the active nodes list.
			bool ExpandNode(LinkedNodeStub * currentStub);  // Expands all the children of the current node.
			void NormalizeGraphs();
			unsigned long long EncodeNode(unsigned long long bitmap, int node);

		};

		WongMatch15ConstrainedNoFuture::WongMatch15ConstrainedNoFuture(StandardGraph * patternGraph, StandardGraph * baseGraph) {
			Init(patternGraph, baseGraph);
		}

		WongMatch15ConstrainedNoFuture::WongMatch15ConstrainedNoFuture(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
			Init(patternGraph, baseGraph);
			this->missingHelixCount = missingHelixCount;
			this->missingSheetCount = missingSheetCount;
		}

		WongMatch15ConstrainedNoFuture::~WongMatch15ConstrainedNoFuture() {
			for(unsigned int i = 0; i < usedNodes.size(); i++) {
				delete usedNodes[i];
			}
			usedNodes.clear();
			solutions.clear();
			int queueSize = queue->getLength();
			double tempKey;
			LinkedNode * tempNode;
			for(int i = 0; i < queueSize; i++) {		
				queue->remove(tempNode, tempKey);
				delete tempNode;
			}

			delete queue;
			
			delete pathGenerator;
		}
		void WongMatch15ConstrainedNoFuture::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {	
			cout << "WongMatch15ConstrainedNoFuture::Init" << endl;
			usedNodes.clear();
			cout << "WongMatch15ConstrainedNoFuture::Init 1" << endl;
			queue = new PriorityQueue<LinkedNode, double> (PRIORITYQUEUESIZE);
			cout << "WongMatch15ConstrainedNoFuture::Init 1.1" << endl;
			this->patternGraph = patternGraph;
			cout << "WongMatch15ConstrainedNoFuture::Init 1.2" << endl;
			this->baseGraph = baseGraph;
			cout << "WongMatch15ConstrainedNoFuture::Init 1.3" << endl;
			expandCount = 0;

			// TODO: Make this more accurate
			// old method of counting missing sheets and helices
			
			cout << "WongMatch15ConstrainedNoFuture::Init 2" << endl;
			missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
			if(missingHelixCount < 0)  {
				missingHelixCount = 0;
			}
			if(missingSheetCount < 0)  {
				missingSheetCount = 0;
			}
			

			// new method of counting missing sheets and helices
			
			cout << "WongMatch15ConstrainedNoFuture::Init 3" << endl;
			// count helix nodes in base graph
			int baseHelixNodes = 0;
			int baseSheetNodes = 0;
			int patternHelixNodes = 0;
			int patternSheetNodes = 0;

			cout << "calculating the number of missing sheets and helices" << endl;

			for (int i = 0; i < baseGraph->GetNodeCount(); i++) {
				cout << "base graph node " << i << " has type " << (int)(baseGraph->adjacencyMatrix[i][i][0]) << endl;
				switch((int)(baseGraph->adjacencyMatrix[i][i][0] + 0.01)) {
					case(GRAPHNODE_HELIX) : 
						baseHelixNodes++;
						break;
					case(GRAPHNODE_SHEET):
						baseSheetNodes++;
						break;
					default:
						break;
				}
			}
			cout << "base graph has " << baseHelixNodes << " helix nodes and " << baseSheetNodes << " sheet nodes." << endl;

			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				cout << "pattern graph node " << i << " has type " << (int)(patternGraph->adjacencyMatrix[i][i][0]) << endl;
				switch((int)(patternGraph->adjacencyMatrix[i][i][0] + 0.01)) {
					case(GRAPHNODE_HELIX) : 
						patternHelixNodes++;
						break;
					case(GRAPHNODE_SHEET):
						patternSheetNodes++;
						break;
					default:
						break;
				}
			}
			cout << "pattern graph has " << patternHelixNodes << " helix nodes and " << patternSheetNodes << " sheet nodes." << endl;


			missingHelixCount = (patternHelixNodes - baseHelixNodes) / 2;
			//missingSheetCount = patternSheetNodes - baseSheetNodes;
			missingSheetCount = patternSheetNodes;

			cout << "missing helix count is " << missingHelixCount << ", missing sheet count is " << missingSheetCount << endl;

			cout << "missing helix penalty is " << MISSING_HELIX_PENALTY << ", missing sheet penalty is " << MISSING_SHEET_PENALTY << endl;

			//if(!PERFORMANCE_COMPARISON_MODE) {
			//	NormalizeGraphs();
			//}
			foundCount = 0;
			longestMatch = 0;
		#ifdef VERBOSE
			timeInGetA = 0;
			timeInGetB = 0;
			timeInQueue = 0;
		#endif

			currentNode = new LinkedNode();
			for(int j = 1; j <= patternGraph->nodeCount; j++) {
				LinkedNode::AddNodeToBitmap(currentNode->m1Bitmap, j);
			}
			for(int j = 1; j <= baseGraph->nodeCount; j++) {
				LinkedNode::AddNodeToBitmap(currentNode->m2Bitmap, j);
			}
			queue->add(currentNode, currentNode->cost);
			pathGenerator = new PathGenerator(baseGraph);
		}




		int WongMatch15ConstrainedNoFuture::RunMatching(clock_t startTime) {
			cout << "Start WongMatch15ConstrainedNoFuture::RunMatching: " << endl;
			bool continueLoop = true;
			clock_t finishTime;
			while(continueLoop)
			{
				PopBestNode();		
				if(currentNode == NULL) {
					break;
				}
				//currentNode->PrintNodeConcise(foundCount, false);
				//printf("\n");
				//cout << " current node has depth " << (int)currentNode->depth << ", max depth is " << patternGraph->nodeCount << endl;
				if(currentNode->depth == patternGraph->nodeCount) {
					//cout << " current node at max depth (" << (int)currentNode->depth << ")" << endl;
					finishTime = clock();
					foundCount++;
					currentNode->PrintNodeConcise(foundCount, false);
					printf(": (%d expanded) (%f seconds) (%fkB Memory) (%d queue size) (%d parent size)\n", expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0, queue->getLength(), (int)usedNodes.size());
					solutions.push_back(SSECorrespondenceResult(currentNode));

					#ifdef MAKE_FINAL_MRC
						char fileName[80];
						sprintf(fileName, "Solution%d.mrc", foundCount);
						pathGenerator->GenerateGraph(currentNode, fileName);
					#endif
				} else {
					//cout << " current node not at max depth" << endl;
					LinkedNodeStub * currentStub = new LinkedNodeStub(currentNode);
					if(ExpandNode(currentStub)) {
						usedNodes.push_back(currentStub);
					} else {
						delete currentStub;
					}
					delete currentNode;
				}		
				continueLoop = (foundCount < RESULT_COUNT);
			}

			//Cleaning up memory
			for(unsigned int i = 0; i < usedNodes.size(); i++) {
				delete usedNodes[i];
			}
			usedNodes.clear();

			int queueSize = queue->getLength();
			double tempKey;
			LinkedNode * tempNode;
			for(int i = 0; i < queueSize; i++) {		
				queue->remove(tempNode, tempKey);
				delete tempNode;
			}

			cout << "at end of WongMatch15ConstrainedNoFuture::RunMatching. results found = " << foundCount << endl;

			return foundCount;
		}

		SSECorrespondenceResult WongMatch15ConstrainedNoFuture::GetResult(int rank) {
			if(rank <= (int)solutions.size() && (rank >= 1)) {
				return solutions[rank-1];
			} else {
				return NULL;
			}
		}
		void WongMatch15ConstrainedNoFuture::SaveResults(){
			//printf("\t");
			//for(int i = 0; i < currentNode->n1Top; i++) {
			//	printf("%d ", currentNode->n2[i]);
			//}
			//printf(" - %f : (%d expanded)\n", currentNode->cost, expandCount);
			//printf("%d\t\t", expandCount);	
			#ifdef VERBOSE
				printf("Time taken in GetA %f\n", timeInGetA / (double)CLOCKS_PER_SEC);
				printf("Time taken in GetB %f\n", timeInGetB / (double)CLOCKS_PER_SEC);
				printf("Time taken in Queue %f\n", timeInQueue / (double)CLOCKS_PER_SEC);
			#endif

		}

		// returns the cost of matching node p in the pattern graph to node qp in the base graph
		double WongMatch15ConstrainedNoFuture::GetC(int p, int qp) {
			return GetC(p, p, qp, qp);
		}

		// returns the cost of matching node p in the pattern graph to node qp in the base graph
		double WongMatch15ConstrainedNoFuture::GetC(int p, int qp, LinkedNodeStub * currentNode) {
			double cost = GetC(p, p, qp, qp);
			
			// if sheet-to-strand match,
			if( (int)(patternGraph->adjacencyMatrix[p-1][p-1][0] + 0.01) == GRAPHNODE_SHEET && 
				(int)(baseGraph->adjacencyMatrix[qp-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET ) {
				cost = 0;
				//cout << " ... original cost = " << cost << endl;

				double capacity = baseGraph->nodeWeights[qp-1];
				//cout << " ... original capacity = " << capacity << endl;
				bool continueLoop = true;
				// walk up tree finding nodes where this sheet was matched to another strand, decreasing
				// sheet capacity each time a match is found.
				while(continueLoop) {
					if(currentNode->parentNode == NULL) {
						 break;
					}
					if (qp == currentNode->n2Node) {
						int matchedNode = currentNode->n1Node;
						capacity -= patternGraph->nodeWeights[matchedNode-1];
						//cout << " ... one match at node qp = " << qp << ", p = " << matchedNode << " ... new capacity = " << capacity << endl;
					}
					currentNode = currentNode->parentNode;		
				}
				//cout << " ... unused capacity = " << capacity << endl;

				// if capacity has all been used, penalize this match
				if (capacity < 0) {
					cost -= capacity;
					//cout << " ... negative capacity. cost should be nonzero now." << endl;
				}
				if (cost > 0) {
					cout << " ... match cost for base graph node " << qp << " = " << cost << endl;
				}
			}

			return cost;

		}

		// GetC(currentNode->n1Node, currentNode->n2Node)

		// returns the cost of matching edge j,p in the pattern graph to edge qj,qp in the base graph.
		// when j=p and qj=qp, returns the cost of matching node j to node qj.
		// three cases:
		//   j == p and qj == qp -- node match cost
		//   j != p and qj == qp -- edge match cost, special case where same sheet revisited by two consecutive nodes
		//   j != p and qj != qp -- edge match cost
		double WongMatch15ConstrainedNoFuture::GetC(int j, int p, int qj, int qp) {

			double jpCost;
			double qjqpCost;
			double typeCost = 0;

			// if no edge exists between j and p
			if(patternGraph->adjacencyMatrix[j-1][p-1][1] == MAXINT) {
				jpCost = 1000;
			} else {
				// if edge exists or if j == p
				jpCost = patternGraph->adjacencyMatrix[j-1][p-1][1];
			}

			// if no edge exists between qj and qp
			if(baseGraph->adjacencyMatrix[qj-1][qp-1][1] == MAXINT) {
				qjqpCost = 1000;
			} else {
				// if edge exists between qj and qp or if qj == qp
				qjqpCost = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
			}

			// if edge types or node types do not match. cost decided here may be recomputed in next block.
			if(patternGraph->adjacencyMatrix[j-1][p-1][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) {
				typeCost = 1000;
			}

			// case where a sheet is revisited by two consecutive strands
			if( (int)(patternGraph->adjacencyMatrix[j-1][p-1][0] + 0.01) == GRAPHEDGE_LOOP && 
				(int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET) {
				//cout << "----> revisiting sheet " << qj-1 << ". type cost 0, total cost " << fabs(jpCost - qjqpCost) << endl;
				typeCost = 0;
			}
			
			// add extra cost for node to node match. maybe this should be somewhere else?
			/*
			if ( (qj==qp) ) {
				//if ( ((int)(patternGraph->adjacencyMatrix[j-1][j-1][0]) == GRAPHNODE_SHEET) && ((int)(baseGraph->adjacencyMatrix[qj-1][qj-1][0]) == GRAPHNODE_SHEET) ) {
				if ((int)(baseGraph->adjacencyMatrix[qj-1][qj-1][0]) == GRAPHNODE_SHEET) {
					//typeCost += 0;
					qjqpCost = SHEET_SELF_LOOP_LENGTH;
				}
			}
			*/

			/*
			if ( (j==p) && (qj == qp) ) {
				if ( ((int)(patternGraph->adjacencyMatrix[j-1][j-1][0]) == GRAPHNODE_SHEET) && ((int)(baseGraph->adjacencyMatrix[qj-1][qj-1][0]) == GRAPHNODE_SHEET) ) {
					//typeCost += 0;
					qjqpCost = 0;
					jpCost = 0;
				}
			}
			*/

			// TODO: Think I need to add an extra case here for the type check done by the simpler GetC(i, j) method above

			//cout << "cost of pattern " << patternGraph->adjacencyMatrix[j-1][p-1][0] << " and base " << baseGraph->adjacencyMatrix[qj-1][qp-1][0] << " is " << fabs(jpCost - qjqpCost) + typeCost << endl;
			return fabs(jpCost - qjqpCost) + typeCost;
		}

		// returns the cost of matching a loop from the pattern graph (with missing helices) to a loop in the base graph.
		// d is the start match in the pattern graph
		// m is the number of missing helices or sheets in the pattern graph
		// qj is the start node in the base graph
		// qp is the end node in the base graph
		double WongMatch15ConstrainedNoFuture::GetCost(int d, int m, int qj, int qp) {
			// TODO: Fix patthernLength and baseLength for sheet-to-sheet case.
			double patternLength = 0;
			double baseLength;

			// Adding the length of the skipped helixes
			for(int i = 1; i < m; i++) {
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
			}

			// Adding the length of the edges
			// Check that first and last edges are both loops, a requirement for all edges in the pattern graph
			bool firstIsLoop = false;
			bool lastIsLoop = false;
			for(int i = 0; i < m; i++) {
				//lastIsLoop = (((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP) || ((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP_EUCLIDEAN));
				lastIsLoop = (((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP) || ((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP_EUCLIDEAN) || ((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHNODE_SHEET));
				if(i==0) {
					firstIsLoop = lastIsLoop;
				}
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i][1];		
			}

			bool euclideanEstimate = false;
			double weight = 1.0;

			bool sheetSheet = false; // for debugging

			if(qj == -1) { // special handling for missing helixes at the ends
				baseLength = 0;
				switch((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01)) {
					case(GRAPHEDGE_HELIX) : 
						weight = HELIX_WEIGHT_COEFFICIENT;
						break;
					case(GRAPHEDGE_LOOP):
						weight = LOOP_WEIGHT_COEFFICIENT;
						break;
					case(GRAPHEDGE_SHEET):
						weight = SHEET_WEIGHT_COEFFICIENT;
						break;
				}
			}
			else {
				assert(baseGraph->EdgeExists(qj-1,qp-1));		
				baseLength = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
				euclideanEstimate = ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01) == GRAPHEDGE_LOOP_EUCLIDEAN);
				switch((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01)) {
					case(GRAPHEDGE_HELIX) : 
						weight = HELIX_WEIGHT_COEFFICIENT;
						break;
					case(GRAPHEDGE_LOOP):
						weight = LOOP_WEIGHT_COEFFICIENT;
						break;
					case(GRAPHEDGE_SHEET):
						weight = SHEET_WEIGHT_COEFFICIENT;
						break;
					case(GRAPHNODE_SHEET): // two strands in a row match to the same sheet
						//cout << "---> sheet to sheet case. parameters: " << d << "," << m << "," << qj << "," << qp << endl;
						sheetSheet = true;
						weight = LOOP_WEIGHT_COEFFICIENT;
						//weight = 0;
						break;
				}
				weight = euclideanEstimate? weight * EUCLIDEAN_LOOP_PENALTY: weight;
			}

			if(m == 1) {		
				if((qj!= -1) && ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) != (int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01)) &&
					//!(((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP) && ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHEDGE_LOOP_EUCLIDEAN))) 	{
					!(((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP) && ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHEDGE_LOOP_EUCLIDEAN)) &&
					!(((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP) && ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHNODE_SHEET))) 	{
					if (sheetSheet) {
						//cout << "first case returning -1" << endl;
					}
					return -1;
				}		
				//// TODO: Figure out why I had put in the following code!!!!
				//if((qj != -1) && (baseGraph->euclideanMatrix[qj-1][qp-1] > patternGraph->adjacencyMatrix[d-1][d][1])){
				////	printf("%lf %lf %lf %lf \n", baseGraph->euclideanMatrix[qj-1][qp-1], patternGraph->adjacencyMatrix[d-1][d][1], patternGraph->adjacencyMatrix[d-1][d][1], EUCLIDEAN_VOXEL_TO_PDB_RATIO);
				//	return -1;
				//}

			} else {
				if(!firstIsLoop || !lastIsLoop) {
					if (sheetSheet) {
						//cout << "second case returning -1 (first and last are not loops)" << endl;
					}	
					return -1;
				}

			}


			switch(COST_FUNCTION)
			{
			case(1):
				if (sheetSheet) {
					//cout << "first case (absolute cost) returning weight of " << weight * fabs(patternLength - baseLength) << endl;
				}
				return weight * fabs(patternLength - baseLength);
				break;
			case(2):
				if (sheetSheet) {
					//cout << "second case (normalized cost) returning weight of " << weight * fabs(patternLength - baseLength) / (patternLength + baseLength) << endl;
				}
				return weight * fabs(patternLength - baseLength) / (patternLength + baseLength);
				break;
			case(3):
				if (sheetSheet) {
					//cout << "third case (squared cost) returning weight of " << weight * pow((patternLength - baseLength),2) << endl;
				}
				return weight * pow((patternLength - baseLength),2);
				break;
			}
			if (sheetSheet) {
				//cout << "returning zero (impossible case)" << endl;
			}
			return 0;
		}

		double WongMatch15ConstrainedNoFuture::GetF() {
			return currentNode->costGStar;	
		}

		void WongMatch15ConstrainedNoFuture::PopBestNode(){
		#ifdef VERBOSE
			clock_t start = clock();
		#endif
			double cost;
			queue->remove(currentNode, cost);
		#ifdef VERBOSE
			timeInQueue += clock() - start;
		#endif
		}

		// expand a node.
		// checks the base graph for edges between this node and every other.
		// if an edge is found, match the pattern graph to that edge and add the match to the queue.
		// also match edges that include skip edges in the pattern graph
		// costs of matches are determined by the GetC method
		bool WongMatch15ConstrainedNoFuture::ExpandNode(LinkedNodeStub * currentStub) {
			bool expanded = false;
			expandCount++;

			LinkedNode * temp;
			double edgeCost;
		#ifdef VERBOSE
			if(longestMatch < currentNode->depth) {
				longestMatch = currentNode->depth;
				printf(" %d elements matched! (%f kB Memory Used)\n", longestMatch, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0);
				cout << "WongMatch15ConstrainedNoFuture::ExpandNode: " << longestMatch << " elements expanded (" << ((queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0) << " kB memory used)" << endl;
			}
		#endif //VERBOSE
			
			int currentM1Top = patternGraph->nodeCount - currentNode->depth;
			bool notConstrained;
			//currentNode->PrintNodeConcise(0, true, true);

			// Expanding nodes with a real terminal node
			// for every node i in baseGraph
			for(int i = 1; i <= baseGraph->nodeCount; i++) {
				// if: 
				//   currentNode is at level 0 of tree
				//   or 
				//   i is in the currentNode bitmap, and there is an edge in baseGraph between currentNode and node i
				if((currentNode->depth == 0) || 
					(LinkedNode::IsNodeInBitmap(currentNode->m2Bitmap, i) && (baseGraph->EdgeExists(currentNode->n2Node-1, i-1)))) {						
					// TODO: the following line used to assume that every other SSE was a helix
					//       j is number of helices to jump 
					//       think i've fixed it. need to test.
					//for(int j = 0; j <= min(missingHelixCount * 2 + missingSheetCount - currentNode->missingNodesUsed + 1, currentM1Top); j += 1) {  // Stepping by one for sheets; helix code adds an extra step at end
					int skippedHelixNodes = 0;
					int skippedSheetNodes = 0;
					for (int j = 0; (j <= currentM1Top) && (skippedHelixNodes <= missingHelixCount * 2) && (skippedSheetNodes <= missingSheetCount); ) {
					//for(int j = 0; j <= min(missingHelixCount * 2 - currentNode->missingNodesUsed + 1, currentM1Top); j += 2) {  // Stepping by two since we jump every 2 loops

						// i is the node from baseGraph being matched to currentNode
						// j is the number of missing helices or sheets from patternGraph to be skipped for this match

						notConstrained = true;

						for(int k = currentNode->n1Node + 1; k <= currentNode->n1Node + j; k++) {
							notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
						}
						notConstrained = notConstrained && IsNodeAssignmentAllowed(currentNode->n1Node + j + 1, i);

						if(notConstrained) {	
							// store the current node as temp
							temp = currentNode; 
							// create new current node. i is the index of the new node(?), j is the number of skipped nodes.


							// check whether i is a revisitable node (a sheet)
							bool revisitable = ( (int)(baseGraph->adjacencyMatrix[i-1][i-1][0] + 0.01) == GRAPHNODE_SHEET );
							/*
							if (revisitable) {
								currentNode->PrintNodeConcise(0, true, true);
								cout << "searching from node " << (int)(currentNode->n2Node) << " to    revisitable node i=" << i << ", type=" << (int)(baseGraph->adjacencyMatrix[i-1][i-1][0] + 0.01) << ", j=" << j << endl;
							} else {
								currentNode->PrintNodeConcise(0, true, true);
								cout << "searching from node " << (int)(currentNode->n2Node) << " to nonrevisitable node i=" << i << ", type=" << (int)(baseGraph->adjacencyMatrix[i-1][i-1][0] + 0.01) << ", j=" << j << endl;
							}
							*/

							// generate a current node, marking it as revisitable or not depending on result from test
							// the constructor marches forward along the sequence, skipping j nodes
							currentNode = new LinkedNode(currentNode, currentStub, i, skippedHelixNodes, skippedSheetNodes, revisitable);
							//currentNode = new LinkedNode(currentNode, currentStub, i, j, revisitable);
							//currentNode = new LinkedNode(currentNode, currentStub, i, j);

							//if (currentNode->n2Node == temp->n2Node) {
							//	cout << "============== progress! picked two nodes that are the same! node is " << (int)(currentNode->n2Node+0.01) << ", cost is " << currentNode->cost << endl;
							//}

							currentNode->costGStar = 0;

							// if previous node was at top of tree and it was skipped
							// TODO: make up for this later by incrementing j by 2?
							if(((temp->depth == 0) && (j > 0)) || 
								((patternGraph->nodeCount - currentNode->depth == 0) && (currentNode->n2Node == -1))) {
								currentNode->costGStar += START_END_MISSING_HELIX_PENALTY;
							}	

							// if previous node was at top of tree
							if(temp->depth == 0) {
								edgeCost = 0;
							} else {								
								edgeCost = GetCost(temp->n1Node, j+1, temp->n2Node, currentNode->n2Node);
								//printf("%i %i %i %i %lf\n", temp->n1Node, j+1, temp->n2Node, currentNode->n2Node, edgeCost);
								//if (edgeCost < 0) {cout << "edge cost (" << (int)temp->n1Node << "," << (int)j+1 << "," << (int)temp->n2Node << "," << (int)currentNode->n2Node << ") is " << edgeCost << endl;}
							}
							
							// TODO: Look below here for edge and node costs. 
							// TODO: Fix assumption that all nodes are helices!
							// if this is an allowed match:
							if(edgeCost >= 0) {
								/*
								if (currentNode->n2Node == temp->n2Node) {
									cout << "============== progress! starting to match a node to itself! node is " << (int)(currentNode->n2Node+0.01) << endl;
									cout << "               BEFORE:" << endl;
									cout << "               edge cost is " << edgeCost << endl;
									cout << "               current n1 node type is " << (int)(baseGraph->adjacencyMatrix[currentNode->n1Node-1][currentNode->n1Node-1][0] + 0.01) << endl;
									cout << "               temp n1 node type is " << (int)(baseGraph->adjacencyMatrix[temp->n1Node-1][temp->n1Node-1][0] + 0.01) << endl;
									cout << "               current n2 node type is " << (int)(baseGraph->adjacencyMatrix[currentNode->n2Node-1][currentNode->n2Node-1][0] + 0.01) << endl;
									cout << "               temp n2 node type is " << (int)(baseGraph->adjacencyMatrix[temp->n2Node-1][temp->n2Node-1][0] + 0.01) << endl;
									cout << "               current n1 is " << (int)currentNode->n1Node << endl;
									cout << "               temp n1 is " << (int)temp->n1Node << endl;
									cout << "               current n2 is " << (int)currentNode->n2Node << endl;
									cout << "               temp n2 is " << (int)temp->n2Node << endl;
									cout << "               costGStar is " << currentNode->costGStar << endl;

								}
								*/
								//currentNode->costGStar += temp->costGStar + edgeCost +	MISSING_HELIX_PENALTY * (j/2.0) + GetC(currentNode->n1Node, currentNode->n2Node);
								//currentNode->costGStar += temp->costGStar + edgeCost +	MISSING_HELIX_PENALTY * (skippedHelixNodes/2.0) + MISSING_SHEET_PENALTY * (skippedSheetNodes) + GetC(currentNode->n1Node, currentNode->n2Node);
								currentNode->costGStar += temp->costGStar + edgeCost +	MISSING_HELIX_PENALTY * (skippedHelixNodes/2.0) + MISSING_SHEET_PENALTY * (skippedSheetNodes) + GetC(currentNode->n1Node, currentNode->n2Node, currentNode);
								currentNode->cost = GetF();			
								//currentNode->PrintNodeConcise(-1, true, true);
								queue->add(currentNode, currentNode->cost);
								expanded = true;
								/*
								if (currentNode->n2Node == temp->n2Node) {
									cout << "               AFTER:" << endl;
									cout << "               GetC returns " << GetC(currentNode->n1Node, currentNode->n2Node) << endl;
									cout << "               edge cost is " << edgeCost << endl;
									cout << "               costGStar is " << currentNode->costGStar << endl;
									cout << "               cost is " << currentNode->cost << endl;
									cout << "============== " << endl;
								}
								*/
							} else { // not an allowed match
								delete currentNode;
							}
							// set currentNode pointer back to the current node
							currentNode = temp;	
						}
						// if this node is a helix, increment j by one more to prepare for the next iteration
						// TODO: Fix the indexing of the test here. Not right.
						//if ( (int)(baseGraph->adjacencyMatrix[j][j][0] + 0.01) == GRAPHNODE_HELIX ) {
						//if ( (int)(baseGraph->adjacencyMatrix[currentNode->n1Node + j + 1][currentNode->n1Node + j + 1][0] + 0.01) == GRAPHNODE_HELIX ) {
						switch ( (int)(baseGraph->adjacencyMatrix[currentNode->n1Node + j + 1][currentNode->n1Node + j + 2][0] + 0.01)) {
							case GRAPHEDGE_HELIX:
								skippedHelixNodes += 2;
								j+=2;
								break;
							default:
								skippedSheetNodes += 1;
								j++;
								break;
						}
						//if ( (int)(baseGraph->adjacencyMatrix[currentNode->n1Node + j + 1][currentNode->n1Node + j + 2][0] + 0.01) == GRAPHEDGE_HELIX ) {
						//	j++;
						//}
					}
				}
			}


			// Expanding nodes with a dummy terminal node
			if(2*missingHelixCount - currentNode->missingNodesUsed >= currentM1Top) {
				notConstrained = true;
				for(int k = currentNode->n1Node + 1; k <= patternGraph->nodeCount; k++) {
					notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
				}

				if(notConstrained) {
					temp = currentNode;
					currentNode = new LinkedNode(temp);
					currentNode->depth = (char)patternGraph->nodeCount;
					currentNode->costGStar = temp->costGStar + MISSING_HELIX_PENALTY * (patternGraph->nodeCount - temp->depth) / 2.0 + START_END_MISSING_HELIX_PENALTY;
					currentNode->cost = currentNode->costGStar;
					queue->add(currentNode, currentNode->cost);
					currentNode = temp;
				}
			}
			return expanded;
		}

		void WongMatch15ConstrainedNoFuture::NormalizeGraphs() {
			printf("Normalizing Graphs\n");

		#ifdef VERBOSE
			printf("\tNormalizing the base graph based on helix length ratio\nNormalized Graph:\n");
		#endif
			double ratio = 0;

			for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
				ratio += (double)baseGraph->skeletonHelixes[i]->length  / (double)baseGraph->adjacencyMatrix[i*2][i*2+1][1];
			}
			ratio = ratio / (double)baseGraph->skeletonHelixes.size();

			for(int i = 0; i < baseGraph->nodeCount; i++) {
				for(int j = 0; j < baseGraph->nodeCount; j++) {
					if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT) {
						baseGraph->SetCost(i+1,j+1, baseGraph->adjacencyMatrix[i][j][1] * ratio);
					}
				}
			}	
		#ifdef VERBOSE
			baseGraph->PrintGraph();
		#endif
		}

		unsigned long long WongMatch15ConstrainedNoFuture::EncodeNode(unsigned long long bitmap, int node) {
			if(node == -1)
				return bitmap;

			return (bitmap | ((unsigned long long)1 << node));
		}
	}
}
#endif