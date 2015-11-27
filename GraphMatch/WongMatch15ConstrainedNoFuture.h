// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Graph Matching algorithm based on the following paper:
//				  Wong et al., An algorithm for graph optimal monomorphism, 
//				  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636
//				  Modified to support missing helixes and constrained nodes
//				  Contains no future function



#ifndef WONGMATCH15CONSTRAINEDNOFUTURE_H
#define WONGMATCH15CONSTRAINEDNOFUTURE_H

#include "StandardGraph.h"
#include "LinkedNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"
#include "PathGenerator.h"
#include "Structures.h"
#include <ctime>
//#include <SkeletonMaker/PriorityQueue.h>
#include <Foundation/GorgonPriorityQueue.h>
#include "SSECorrespondenceResult.h"

using namespace wustl_mm::Foundation;
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
			//PriorityQueue<LinkedNode, double> * queue;
			GorgonPriorityQueue<double, LinkedNode *> * queue;
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
			double GetCost(int d, int m, int qj, int qp, bool debugMsg);
			double GetPenaltyCost(int d, int m, bool debugMsg);
			double GetCPrime(int a, int b, int c, int d);
			double GetK(int p, int qp);
			double GetKPrime(int i, int q);
			double GetF();
			void PopBestNode(); // Gets the best (first) node from the active nodes list.
			bool ExpandNode(LinkedNodeStub * currentStub);  // Expands all the children of the current node.
			void ComputeSolutionCost(int solution[], bool extraMessages);
			void AnalyzeResults(int results[][MAX_NODES], int groundTruth[]);
			void NormalizeGraphs();
			void NormalizeSheets();
			unsigned long long EncodeNode(unsigned long long bitmap, int node);
			void PrintNodeConcise(LinkedNode * node, int rank, bool endOfLine, bool printCostBreakdown);
			int bestMatches[RESULT_COUNT][MAX_NODES];

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
			//int queueSize = queue->getLength();
			//double tempKey;
			//LinkedNode * tempNode;
			//for(int i = 0; i < queueSize; i++) {		
			//	queue->remove(tempNode, tempKey);
			//	delete tempNode;
			//}
			LinkedNode * tempNode;
			while(!queue->IsEmpty()) {
				tempNode = queue->PopFirst();
				delete tempNode;
			}

			delete queue;
			
			delete pathGenerator;
		}
		void WongMatch15ConstrainedNoFuture::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {	
#ifdef VERBOSE
			cout << "Initializing search" << endl;
			cout << "Base graph has " << baseGraph->GetHelixCount() << " helices and " << baseGraph->GetSheetCount() << " sheets." << endl;
			cout << "Pattern graph has " << patternGraph->GetHelixCount() << " helices and " << patternGraph->GetSheetCount() << " sheets." << endl;
#endif // VERBOSE
			usedNodes.clear();
#ifdef VERBOSE
			cout << "Creating priority queue" << endl;
#endif // VERBOSE
			//queue = new PriorityQueue<LinkedNode, double> (PRIORITYQUEUESIZE);
			queue = new GorgonPriorityQueue<double, LinkedNode *>(false);
#ifdef VERBOSE
			cout << "Loading pattern graph" << endl;
#endif // VERBOSE
			this->patternGraph = patternGraph;
#ifdef VERBOSE
			cout << "Loading base graph" << endl;
#endif // VERBOSE
			this->baseGraph = baseGraph;
			expandCount = 0;

#ifdef VERBOSE
			cout << "Finding the number of missing helices and sheets" << endl;
#endif // VERBOSE
			missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
			if(missingHelixCount < 0)  {
				missingHelixCount = 0;
			}
			if(missingSheetCount < 0)  {
				missingSheetCount = 0;
			}
			
			// TEMPORARY CODE!
			//EUCLIDEAN_VOXEL_TO_PDB_RATIO *= 1.5;
			//EUCLIDEAN_VOXEL_TO_PDB_RATIO = 3.0;
			//EUCLIDEAN_VOXEL_TO_PDB_RATIO = 1.5 * LOOP_C_ALPHA_TO_ANGSTROMS; // this is what i think it should be.
			// END TEMPORARY CODE

			// new method of counting missing sheets and helices
			// count helix nodes in base graph
			int baseHelixNodes = 0;
			int baseSheetNodes = 0;
			int patternHelixNodes = 0;
			int patternSheetNodes = 0;

			for (int i = 0; i < baseGraph->GetNodeCount(); i++) {
				//cout << "base graph node " << i << " has type " << (int)(baseGraph->adjacencyMatrix[i][i][0]) << endl;
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
#ifdef VERBOSE
			cout << "base graph has " << baseHelixNodes << " helix nodes and " << baseSheetNodes << " sheet nodes." << endl;
#endif // VERBOSE

			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				//cout << "pattern graph node " << i << " has type " << (int)(patternGraph->adjacencyMatrix[i][i][0]) << endl;
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
#ifdef VERBOSE
			cout << "pattern graph has " << patternHelixNodes << " helix nodes and " << patternSheetNodes << " sheet nodes." << endl;
#endif // VERBOSE


			missingHelixCount = (patternHelixNodes - baseHelixNodes) / 2;

			// allow all strands to be missing
			//missingSheetCount = patternSheetNodes - baseSheetNodes;
			missingSheetCount = patternSheetNodes;

#ifdef VERBOSE
			cout << "missing helix count is " << missingHelixCount << ", missing sheet count is " << missingSheetCount << endl;
			cout << "missing helix penalty is " << MISSING_HELIX_PENALTY << ", missing sheet penalty is " << MISSING_SHEET_PENALTY << endl;
#endif // VERBOSE

			if(!PERFORMANCE_COMPARISON_MODE) {
				NormalizeGraphs();
				NormalizeSheets();
			}
			foundCount = 0;
			longestMatch = 0;
#ifdef VERBOSE
			timeInGetA = 0;
			timeInGetB = 0;
			timeInQueue = 0;
#endif

			// create and set up a new node to start the search
			currentNode = new LinkedNode();
			for(int j = 1; j <= patternGraph->nodeCount; j++) {
				LinkedNode::AddNodeToBitmap(currentNode->m1Bitmap, j);
			}
			for(int j = 1; j <= baseGraph->nodeCount; j++) {
				LinkedNode::AddNodeToBitmap(currentNode->m2Bitmap, j);
			}
			//queue->add(currentNode, currentNode->cost);
			queue->Add(currentNode->cost, currentNode);
			pathGenerator = new PathGenerator(baseGraph);
		}



		// searches for correspondences between the pattern graph and base graph.
		int WongMatch15ConstrainedNoFuture::RunMatching(clock_t startTime) {
#ifdef VERBOSE
			cout << "Starting to search for correspondences." << endl;
			DisplayConstants();
#endif // VERBOSE
			bool continueLoop = true;
			clock_t finishTime;
			// repeat the following loop until all results are found
			while(continueLoop)
			{
				PopBestNode();		
				if(currentNode == NULL) {
					break;
				}

				// if currentNode contains a complete sequence match, add it to the solutions list
				if(currentNode->depth == patternGraph->nodeCount) {
					finishTime = clock();
					foundCount++;
					//currentNode->PrintNodeConcise(foundCount, false);
					//printf("\n");
					PrintNodeConcise(currentNode,foundCount, false, false);
					//printf(": (%d expanded) (%f seconds) (%fkB Memory) (%d queue size) (%d parent size)\n", expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0, queue->getLength(), (int)usedNodes.size());
					printf(": (%d expanded) (%f seconds) (%d parent size)\n", expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC, (int)usedNodes.size());
#ifdef _WIN32
					flushall();
#endif
					int numHelices = baseGraph->GetHelixCount();
					solutions.push_back(SSECorrespondenceResult(currentNode, numHelices));

#ifdef MAKE_FINAL_MRC
					char fileName[80];
					sprintf(fileName, "Solution%d.mrc", foundCount);
					pathGenerator->GenerateGraph(currentNode, fileName);
#endif
				// otherwise, expand currentNode and adds its children to usedNodes
				} else {
					LinkedNodeStub * currentStub = new LinkedNodeStub(currentNode);
					if(ExpandNode(currentStub)) {
						usedNodes.push_back(currentStub);
					} else {
						delete currentStub;
					}
					delete currentNode;
				}		
				// continue until desired number of results are found
				continueLoop = (foundCount < RESULT_COUNT) && (!queue->IsEmpty());
			}

			//Cleaning up memory
			for(unsigned int i = 0; i < usedNodes.size(); i++) {
				delete usedNodes[i];
			}
			usedNodes.clear();

			//int queueSize = queue->getLength();
			//double tempKey;
			//LinkedNode * tempNode;
			//for(int i = 0; i < queueSize; i++) {		
			//	queue->remove(tempNode, tempKey);
			//	delete tempNode;
			//}
			LinkedNode * tempNode;
			while(!queue->IsEmpty()) {
				tempNode = queue->PopFirst();
				delete tempNode;
			}

#ifdef VERBOSE
			cout << "Finished the correspondence search. Found " << foundCount << " results." << endl;
#endif // VERBOSE
			/*vector<int> groundTruth;
			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				groundTruth[i]=SOLUTION[i];
			}
			ComputeSolutionCost(groundTruth);*/
			/*
			cout << "The ground truth solution is" << endl;
			cout << "**      ";
			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				cout.width(2);
				cout << SOLUTION[i] << " ";
			}
			//ComputeSolutionCost(SOLUTION, false);
			ComputeSolutionCost(SOLUTION, false);
			AnalyzeResults(bestMatches, SOLUTION);
			cout << endl;
			*/
			return foundCount;
		}

		// returns one of the results of a correspondence search
		SSECorrespondenceResult WongMatch15ConstrainedNoFuture::GetResult(int rank) {
			//if(rank <= (int)solutions.size() && (rank >= 1)) {
				return solutions[rank-1];
			//} else {
			//	return NULL;
			//}
		}

		// prints correspondence search results
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
		// this method does not include any cost for matching strands to sheets.
		double WongMatch15ConstrainedNoFuture::GetC(int p, int qp) {
			//return GetC(p, p, qp, qp);
			double cost = GetC(p, p, qp, qp);
			
			// if sheet-to-strand match, compute the cost of the match based on the unused sheet capacity and the strand length
			if( (int)(patternGraph->adjacencyMatrix[p-1][p-1][0] + 0.01) == GRAPHNODE_SHEET && 
				(int)(baseGraph->adjacencyMatrix[qp-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET ) {
				cost = 0;
				//cout << " ... original cost = " << cost << endl;
				cost = abs(patternGraph->nodeWeights[p-1] - baseGraph->nodeWeights[qp-1]);
				//cout << " ... cost to match strand " << p << " to sheet " << qp << " is " << cost << endl;
			}
			return cost;


		}

		// returns the cost of matching node p in the pattern graph to node qp in the base graph
		// this method includes the cost of matching strands to sheets.
		double WongMatch15ConstrainedNoFuture::GetC(int p, int qp, LinkedNodeStub * currentNode) {
			double cost = GetC(p, p, qp, qp);
			
			// if sheet-to-strand match, compute the cost of the match based on the unused sheet capacity and the strand length
			if( (int)(patternGraph->adjacencyMatrix[p-1][p-1][0] + 0.01) == GRAPHNODE_SHEET && 
				(int)(baseGraph->adjacencyMatrix[qp-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET ) {
				cost = 0;
				//cout << " ... original cost = " << cost << endl;
				cost = abs(patternGraph->nodeWeights[p-1] - baseGraph->nodeWeights[qp-1]);
				//cout << " ... cost to match strand " << p << " to sheet " << qp << " is " << cost << endl;
			}
			return cost;

		}

		// returns the cost of matching edge j,p in the pattern graph to edge qj,qp in the base graph.
		// when j=p and qj=qp, returns the cost of matching node j to node qj.
		// three possible cases:
		//   j == p and qj == qp -- node match cost
		//   j != p and qj == qp -- edge match cost, special case where same sheet revisited by two consecutive nodes
		//   j != p and qj != qp -- edge match cost
		// note: only the first case is ever used, as all calls to this method have j=p and qj=qp.
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
				// if edge exists or if qj == qp
				qjqpCost = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
			}

			// if edge types or node types do not match. cost set here may be recomputed in next block.
			if(patternGraph->adjacencyMatrix[j-1][p-1][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) {
				typeCost = 1000;
			}

			// case where a sheet is revisited by two consecutive strands
			if( (int)(patternGraph->adjacencyMatrix[j-1][p-1][0] + 0.01) == GRAPHEDGE_LOOP && 
				(int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET) {
				//cout << "----> revisiting sheet " << qj-1 << ". type cost 0, total cost " << fabs(jpCost - qjqpCost) << endl;
				typeCost = 0;
			}
			
			//cout << "cost of pattern " << patternGraph->adjacencyMatrix[j-1][p-1][0] << " and base " << baseGraph->adjacencyMatrix[qj-1][qp-1][0] << " is " << fabs(jpCost - qjqpCost) + typeCost << endl;
			return fabs(jpCost - qjqpCost) + typeCost;
		}

		// returns the cost of matching a loop from the pattern graph (with missing helices) to a loop in the base graph.
		// d is the start match in the pattern graph
		// m is the number of missing helices or sheets in the pattern graph
		// qj is the start node in the base graph
		// qp is the end node in the base graph
		double WongMatch15ConstrainedNoFuture::GetCost(int d, int m, int qj, int qp, bool debugMsg) {
			// TODO: Fix patthernLength and baseLength for sheet-to-sheet case.
			double patternLength = 0;
			double baseLength;

			// Count number of skipped helices and sheets
			int skippedHelices = 0;
			int skippedSheets = 0;

			// Adding the length of the skipped helixes
			for(int i = 1; i < m; i++) {
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
				if (patternGraph->adjacencyMatrix[d+i-1][d+i-1][0] == GRAPHNODE_SHEET) {
					patternLength += patternGraph->nodeWeights[d+i-1];
					skippedSheets++;
#ifdef VERBOSE
					if (debugMsg) { cout << "  -- found strand " << d+i << ", adding " << patternGraph->nodeWeights[d+i-1] << " to patternLength" << endl; }
#endif // VERBOSE
				} else {
					skippedHelices++;
				}
			}

			// Adding the length of the edges
			// Check that first and last edges are both loops, a requirement for all edges in the pattern graph
			bool firstIsLoop = false;
			bool lastIsLoop = false;
			for(int i = 0; i < m; i++) {
				lastIsLoop = ((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP) ;
				if(i==0) {
					firstIsLoop = lastIsLoop;
				}
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i][1];	
				
				/*
				// rescale helices if they are skipped
				if ((int)(patternGraph->adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_HELIX) {
					patternLength += (HELIX_C_ALPHA_TO_ANGSTROMS / LOOP_C_ALPHA_TO_ANGSTROMS)*(patternGraph->adjacencyMatrix[d+i-1][d+i][1]);	
					//patternLength += (LOOP_C_ALPHA_TO_ANGSTROMS / HELIX_C_ALPHA_TO_ANGSTROMS)*(patternGraph->adjacencyMatrix[d+i-1][d+i][1]);	
				} else {
					patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i][1];	
				}
				*/
			}
			// TODO: Fix, has bug. But getting closer.
			skippedHelices = skippedHelices / 2;
			//cout << "d=" << d << ", m=" << m << ", skipH=" << skippedHelices << ", skipS=" << skippedSheets << endl;

			bool euclideanEstimate = false;
			double weight = 1.0;

			// if edge begins with an unmatched node in the base graph
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
					case(GRAPHNODE_SHEET): // two strands in a row match to the same sheet
						//cout << "---> sheet to sheet case. parameters: " << d << "," << m << "," << qj << "," << qp << endl;
						weight = LOOP_WEIGHT_COEFFICIENT;
						break;
				}
				weight = euclideanEstimate? weight * EUCLIDEAN_LOOP_PENALTY: weight;
			}

			if(m == 1) { // not a skip edge
				if( (qj != -1) && // first node in pattern graph is matched
					!( ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == (int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] + 0.01)) ) && // types don't match exactly
					!( ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP) && ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHEDGE_LOOP_EUCLIDEAN)) && // not a loop-Euclidianloop match
					!( ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP) && ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHNODE_SHEET)) ) 	{ // not a loop-sheet match
					return -1;
				}		
#ifdef VERBOSE
				if (debugMsg) { cout << "  -- euclidean dist = " << baseGraph->euclideanMatrix[qj-1][qp-1] << ", patternLength = " << patternLength << ", loop fudge factor = " << EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS << ", helix fudge factor = " << EUCLIDEAN_VOXEL_TO_PDB_RATIO / HELIX_C_ALPHA_TO_ANGSTROMS << endl; }
				if (debugMsg) { cout << "  -- scalar ratio required = " << baseGraph->euclideanMatrix[qj-1][qp-1] / patternLength << ", additive headroom = " << baseGraph->euclideanMatrix[qj-1][qp-1] - patternLength << endl; }
#endif // VERBOSE

				if((qj != -1) && ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_HELIX) && (baseGraph->euclideanMatrix[qj-1][qp-1] > (patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO / HELIX_C_ALPHA_TO_ANGSTROMS )) ){					
#ifdef VERBOSE
					if (debugMsg) { cout << "  -- -- -- NOT ALLOWED (HELIX) -- -- -- " << endl; }
#endif // VERBOSE
					return -1;
				} else 
				if((qj != -1) && ((int)(patternGraph->adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP)) {
					if (((int)(patternGraph->adjacencyMatrix[d-1][d-1][0] + 0.01) == GRAPHNODE_SHEET || (int)(patternGraph->adjacencyMatrix[d][d][0] + 0.01) == GRAPHNODE_SHEET) && (baseGraph->euclideanMatrix[qj-1][qp-1] > (patternLength * 1.0 * EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS )) ){					
#ifdef VERBOSE
						if (debugMsg) { cout << "  -- -- -- NOT ALLOWED (LOOP WITH STRAND) -- -- -- " << endl; }		
#endif // VERBOSE
						return -1;
					}
					if (((int)(patternGraph->adjacencyMatrix[d-1][d-1][0] + 0.01) != GRAPHNODE_SHEET && (int)(patternGraph->adjacencyMatrix[d][d][0] + 0.01) != GRAPHNODE_SHEET) && (baseGraph->euclideanMatrix[qj-1][qp-1] > (patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS )) ){					
#ifdef VERBOSE
						if (debugMsg) { cout << "  -- -- -- NOT ALLOWED (LOOP) -- -- -- " << endl; }		
#endif // VERBOSE
						return -1;
					}
				}
			} else { // a skip edge
				// not sure if these checks really help or if they just waste time
				if( !(firstIsLoop && lastIsLoop) || // pattern graph edge doesn't start and end with loops OR 
					( ( ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) != GRAPHEDGE_LOOP)) &&			// (base graph edge not a loop AND
					( ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) != GRAPHEDGE_LOOP_EUCLIDEAN)) &&	// base graph edge not a Euclidian loop AND
					( ((int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) != GRAPHNODE_SHEET)) ) ) {		// base graph edge not a sheet)
					return -1;
				}
				// check here to sum up the parts of the skip edge and compare to the euclidian distance, if it's a euclidian edge in the base graph
				if((qj != -1) && (baseGraph->euclideanMatrix[qj-1][qp-1] > (patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO ))){
					return -1;
				}
			}


			switch(COST_FUNCTION)
			{
			
			//case(1):
			//	if ( (int)(baseGraph->adjacencyMatrix[qj-1][qp-1][0] +0.01) == GRAPHEDGE_HELIX ) {
			//		return weight * fabs(patternLength - baseLength);
			//	} else {
			//		// TODO: Remove this arbitrary weight!
			//		return 50.0*weight * fabs(patternLength - baseLength) / (patternLength + baseLength);
			//	}
			//	break;
			case(1):
				return weight * fabs(patternLength - baseLength);
				break;
			case(2):
				return weight * fabs(patternLength - baseLength) / (patternLength + baseLength);
				break;
			case(3):
				return weight * pow((patternLength - baseLength),2);
				break;
			}
			// this line should be unreachable
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
			//queue->remove(currentNode, cost);
			queue->PopFirst(cost, currentNode);
		#ifdef VERBOSE
			timeInQueue += clock() - start;
		#endif
		}


		// add in penalties for skipped helices and sheets
		// m is the number of nodes involved in the match. m=1 is no skipped helices or sheets.
		double WongMatch15ConstrainedNoFuture::GetPenaltyCost(int d, int m, bool debugMsg) {
			//if (d==0) {cout << "d=" << d << ", m=" << m << endl; debugMsg=true;}
			double cost = 0.0;
			int lastPatternNode = patternGraph->GetNodeCount() - 1;
			bool startAtBeginning = ( d == 0 );
			bool finishAtEnd = ( d + m-1 == lastPatternNode );
			bool pastFirst = true;
			bool firstHelixFound = false;
			for(int k = d; k < d + m-1; k++) {
				//if (debugMsg) {cout << "+++++++++++++++++++inside loop k=" << k << endl;}
				//cout << "  GetPenaltyCost(" << d << "," << m << "). k=" << k << ". " << endl;
				// add penalties for all skipped helices
				if((int)(patternGraph->adjacencyMatrix[k][k+1][0] + 0.01) == GRAPHEDGE_HELIX) {
					//cout << "    GetPenaltyCost(" << d << "," << m << "). SKIP HELIX. k=" << k << endl;
					cost += MISSING_HELIX_PENALTY;
					cost += patternGraph->adjacencyMatrix[k][k+1][1] * MISSING_HELIX_PENALTY_SCALED;
#ifdef VERBOSE
					if (debugMsg) { cout << "  -- adding missing helix penalties: fixed=" << MISSING_HELIX_PENALTY << ", scaled=" << patternGraph->nodeWeights[k] * MISSING_HELIX_PENALTY_SCALED << endl; }
#endif // VERBOSE

					if (startAtBeginning && !firstHelixFound) {
						cost += START_END_MISSING_HELIX_PENALTY;
#ifdef VERBOSE
						if (debugMsg) { cout << "  -- adding start_end_miss_helix_pen" << endl; }
#endif // VERBOSE
					}
					if (finishAtEnd && !firstHelixFound) {
						cost += START_END_MISSING_HELIX_PENALTY;
#ifdef VERBOSE
						if (debugMsg) { cout << "  -- adding start_end_miss_helix_pen" << endl; }
#endif // VERBOSE
					}
					firstHelixFound = true;
				}
				// add penalties for skipped strands, unless the strand falls at the beginning of the sequence and is not the first node
				else if( (startAtBeginning || pastFirst) && ((int)(patternGraph->adjacencyMatrix[k][k][0] + 0.01) == GRAPHNODE_SHEET) ) {
					cost += MISSING_SHEET_PENALTY;
					cost += patternGraph->nodeWeights[k] * MISSING_SHEET_PENALTY_SCALED;
#ifdef VERBOSE
					if (debugMsg) { cout << "  -- adding missing sheet penalties: fixed=" << MISSING_SHEET_PENALTY << ", scaled=" << patternGraph->nodeWeights[k] * MISSING_SHEET_PENALTY_SCALED << endl; }
#endif // VERBOSE
				}
				//if (startAtBeginning && debugMsg) { cout << "STARTATBEGIN" << endl;}
				pastFirst = true;
			}

			if (finishAtEnd && patternGraph->adjacencyMatrix[lastPatternNode-1][lastPatternNode-1][0] + 0.01 == GRAPHNODE_SHEET){
					cost += MISSING_SHEET_PENALTY;
					cost += patternGraph->nodeWeights[lastPatternNode-1] * MISSING_SHEET_PENALTY_SCALED;
			}
			//if (debugMsg) { cout << "  -- returning cost=" << cost << endl; }
			return cost;
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
				//printf(" %d elements matched! (%f kB Memory Used)\n", longestMatch, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0);
				printf(" %d elements matched!\n", longestMatch);
				//cout << "WongMatch15ConstrainedNoFuture::ExpandNode: " << longestMatch << " elements expanded (" << ((queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0) << " kB memory used)" << endl;
			}
#endif //VERBOSE
			
			int currentM1Top = patternGraph->nodeCount - currentNode->depth; // remaining unmatched nodes in sequence
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
					int skippedHelixNodes = 0;
					int skippedSheetNodes = 0;
					for (int j = 0; (j <= currentM1Top) && (skippedHelixNodes + currentNode->missingHelixNodesUsed <= missingHelixCount * 2) && (skippedSheetNodes + currentNode->missingSheetNodesUsed <= missingSheetCount); ) {
						// i is the node from baseGraph being matched to currentNode
						// j is the number of missing helix or sheet nodes from patternGraph to be skipped for this match

						notConstrained = true;

						for(int k = currentNode->n1Node + 1; k <= currentNode->n1Node + j; k++) {
							notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
						}
						notConstrained = notConstrained && IsNodeAssignmentAllowed(currentNode->n1Node + j + 1, i);

						// used later for case where first helix is missing
						bool firstMissing = false;

						if(notConstrained) {	
							// store the current node as temp
							temp = currentNode; 
							// create new current node. i is the index of the new node(?), j is the number of skipped nodes.


							// check whether i is a revisitable node (a sheet)
							bool revisitable = ( (int)(baseGraph->adjacencyMatrix[i-1][i-1][0] + 0.01) == GRAPHNODE_SHEET );

							if(((temp->depth == 0) && (j > 0)) || 
								((patternGraph->nodeCount - currentNode->depth == 0) && (currentNode->n2Node == -1))) {
									if (skippedHelixNodes == 0 && patternGraph->adjacencyMatrix[0][0][0] == GRAPHNODE_HELIX) {
										skippedHelixNodes = 1;
#ifdef VERBOSE
										cout << "node skipped. adding one to skippedHelixNodes. result is " << skippedHelixNodes << endl;
#endif // VERBOSE
									}
							}

							// generate a current node, marking it as revisitable or not depending on result from test
							// the constructor marches forward along the sequence, skipping j nodes
							currentNode = new LinkedNode(currentNode, currentStub, i, skippedHelixNodes, skippedSheetNodes, revisitable);

							currentNode->costGStar = 0;

							// if previous node was at top of tree and it was skipped
							if(((temp->depth == 0) && (j > 0)) || 
								((patternGraph->nodeCount - currentNode->depth == 0) && (currentNode->n2Node == -1))) {
								if (skippedHelixNodes > 0) {
									//cout << "first helix is missing." << endl;
									firstMissing = true;
								}
							}	

							// if previous node was at top of tree
							if(temp->depth == 0) {
								edgeCost = 0;
							} else {								
								edgeCost = GetCost(temp->n1Node, j+1, temp->n2Node, currentNode->n2Node, false);
							}
							
							// if this is an allowed match:
							if(edgeCost >= 0) {
								//worked! currentNode->costGStar += temp->costGStar + edgeCost + GetC(currentNode->n1Node, currentNode->n2Node, currentNode);
								currentNode->costGStar += temp->costGStar + edgeCost + GetC(currentNode->n1Node, currentNode->n2Node);

								// add costs for skipped helices and sheets
								currentNode->costGStar += GetPenaltyCost(temp->n1Node, j+1, false);
								
								currentNode->cost = GetF();			
								//currentNode->PrintNodeConcise(-1, true, true);
								//queue->add(currentNode, currentNode->cost);
								queue->Add(currentNode->cost, currentNode);
								expanded = true;
							} else { // not an allowed match
								delete currentNode;
							}
							// set currentNode pointer back to the current node
							currentNode = temp;	
						}
						// if this node is a helix, increment j by one more to prepare for the next iteration
						switch ( (int)(patternGraph->adjacencyMatrix[currentNode->n1Node + j][currentNode->n1Node + j][0] + 0.01)) {
							case GRAPHNODE_HELIX:
								skippedHelixNodes += 2;
								j+=2;
								break;
							default:
								skippedSheetNodes += 1;
								j++;
								break;
						}
					}
				}
			}


			// Expanding nodes with a dummy terminal node:
			// Count the number of sheets and helices remaining in the helix, and then try to make
			// a long skip edge to pass over all of them.
			
			// count the number of helix and sheet nodes that are not yet matched
			int remainingSheetNodes = 0;
			int remainingHelixNodes = 0;
			for (int l = currentNode->n1Node + 1; l <= patternGraph->nodeCount; l++) {
				if (patternGraph->adjacencyMatrix[l-1][l-1][0] == GRAPHNODE_HELIX) {
					remainingHelixNodes++;
				} else if (patternGraph->adjacencyMatrix[l-1][l-1][0] == GRAPHNODE_SHEET) {
					remainingSheetNodes++;
				}
			}

			// if possible, create an edge to jump to the end of the sequence
			if(2*missingHelixCount - currentNode->missingHelixNodesUsed >= remainingHelixNodes && 2*missingSheetCount - currentNode->missingSheetNodesUsed >= remainingSheetNodes) {
				notConstrained = true;
				for(int k = currentNode->n1Node + 1; k <= patternGraph->nodeCount; k++) {
					notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
				}

				if(notConstrained) {
					temp = currentNode;
					currentNode = new LinkedNode(temp);
					currentNode->depth = (char)patternGraph->nodeCount;
					currentNode->costGStar = temp->costGStar;
					currentNode->costGStar += GetPenaltyCost(temp->n1Node, remainingHelixNodes + remainingSheetNodes, false);
					currentNode->cost = currentNode->costGStar;
					//queue->add(currentNode, currentNode->cost);
					queue->Add(currentNode->cost, currentNode);
					currentNode = temp;
				}
			}
			return expanded;
		}

		void WongMatch15ConstrainedNoFuture::AnalyzeResults(int results[][MAX_NODES], int groundTruth[]) {

			int numNodes = patternGraph->GetNodeCount();
			int nh=0, ns=0;
			// count the number of helices and sheets
			for (int i=0; i < numNodes; i++) {
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
					nh++;
				}
				else if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					ns++;
				}
			}

			// compute the % of helices and sheets correctly predicted by each result
			int nCorrectHelices[RESULT_COUNT];
			int nCorrectHelicesFlipped[RESULT_COUNT];
			int nCorrectSheets[RESULT_COUNT];

			for (int res = 0; res < RESULT_COUNT; res++) {
				// Check if all helices or all sheets were correctly matched
				for (int i=0; i < numNodes; i++) {
					if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
						if (results[res][i]==SOLUTION[i]) {
							nCorrectHelices[res]++;
						}
						if (results[res][i]==SOLUTION[i] || results[res][i]==SOLUTION[max(0,i-1)] || results[res][i]==SOLUTION[min(numNodes-1,i+1)]) {
							nCorrectHelicesFlipped[res]++;
						}
					}
					if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
						if (results[res][i]==SOLUTION[i]) {
							nCorrectSheets[res]++;
						}
					}
				}
			}

			cout << endl;
			//for (int i = 0; i < foundCount; i++){
			//	cout << "Result " << i << " has " << nCorrectHelices[i]/2 << " correct helices (" << nCorrectHelicesFlipped[i]/2 << " if direction is ignored) and " << nCorrectSheets[i] << " correct strands." << endl;
			//}
			//for (int i = 0; i < foundCount; i++){
			//	cout << "Result " << i << " has " << (double)nCorrectHelices[i]/(nh) << " correct helices (" << (double)nCorrectHelicesFlipped[i]/nh << " if direction is ignored) and " << (double)nCorrectSheets[i]/ns << " correct strands." << endl;
			//}

			// compute the average % of correct helices and sheets over all results
			

			// count up how many votes each node receives
			int votes[MAX_NODES][MAX_NODES];
			for(int i = 0; i < MAX_NODES; i++) {
				for(int j = 0; j < MAX_NODES; j++) {
					votes[i][j] = 0;
				}
			}
			for (int i=0; i<foundCount; i++) {
				for (int j=0; j<numNodes; j++) {
					int thisVote = results[i][j];
					thisVote = max(thisVote,0); // store votes for -1 at location 0, which is otherwise unused
					//cout << "casting vote " << thisVote << " at votes[" << j << "][" << thisVote << "]" << endl;
					votes[j][thisVote]++;
				}
			}

			/*
			cout << "votes: " << endl;
			for (int i = 0; i < MAX_NODES; i++) {
				for (int j = 0; j < numNodes; j++) {
					cout.width(2);
					cout << votes[j][i];
				}
				cout << endl;
			}*/
			
			// report the % of votes recieved by the ground truth
			int tots=0, toth=0, tothf=0;
			for (int i = 0; i < foundCount; i++) {
				tots += nCorrectSheets[i];
				toth += nCorrectHelices[i];
				tothf += nCorrectHelicesFlipped[i];
			}
			cout << "Average helix predictions accuracy is " << (double)toth/(foundCount*nh) << " (" << (double)tothf/(foundCount*nh) << " if direction is ignored). Average sheet accuracy is " << (double)tots/(foundCount*ns) << endl;


			// compute # of results that had perfect helix and sheets scores
			int nAllH=0, nAllHf=0, nAllS=0;
			for (int i=0; i < foundCount; i++) {
				if (nCorrectSheets[i]==ns) {nAllS++;}
				if (nCorrectHelices[i]==nh) {nAllH++;}
				if (nCorrectHelicesFlipped[i]==nh) {nAllHf++;}
			}
			cout << "Results with all helices correct: " << nAllH << " (" << nAllHf << " counting flips). Results with all sheets correct: " << nAllS << endl;

			// count # of votes that each ground truth node gets
			int groundTruthVotes[MAX_NODES];
			for (int i = 0; i < numNodes; i++) {
				int truthKey=max(groundTruth[i],0);
				groundTruthVotes[i]=votes[i][truthKey];
			}
			cout << "The ground truth solution gets the following number of votes:" << endl;
			cout << " seq #  ";
			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				cout.width(2);
				cout << i+1 << " ";
			}
			cout << endl;
			cout << " truth  ";
			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				cout.width(2);
				cout << groundTruth[i] << " ";
			}
			cout << endl;
			cout << " votes  ";
			for (int i = 0; i < patternGraph->GetNodeCount(); i++) {
				cout.width(2);
				cout << groundTruthVotes[i] << " ";
			}
			cout << endl;

			// find which structure gets the most votes for each node
			int maxVotesIndex[MAX_NODES];
			int maxVotesCount[MAX_NODES];
			for (int i = 0; i < numNodes; i++) {
				int maxCount = -1;
				int maxCountIndex = -1;
				for (int j = 0; j < MAX_NODES; j++) {
					if (votes[i][j]>maxCount) {
						maxCount = votes[i][j];
						//cout << "maxcount[" << i << "][" << j << "]=" << maxCount << endl;
						maxCountIndex=j;
					}
				}
				maxVotesIndex[i]=maxCountIndex;
				maxVotesCount[i]=maxCount;
			}
			cout << "The max vote solution gets the following number of votes:" << endl;
			cout << " index  ";
			for (int i = 0; i < numNodes; i++) {
				cout.width(2);
				if (maxVotesIndex[i]==0) {
					cout << -1 << " ";
				} else {
					cout << maxVotesIndex[i] << " ";
				}
			}
			cout << endl;
			cout << " votes  ";
			for (int i = 0; i < numNodes; i++) {
				cout.width(2);
				cout << maxVotesCount[i] << " ";
			}
			cout << endl;

			// compute success rate of max vote guess
			double maxVoteSuccess=0.0;
			double maxVoteSuccessHelix=0.0;
			double maxVoteSuccessHelixFlipped=0.0;
			double maxVoteSuccessSheet=0.0;
			for (int i = 0; i < numNodes; i++) {
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
					if (maxVotesIndex[i]==0 && groundTruth[i]==-1) {
						maxVoteSuccess += 1.0;
						maxVoteSuccessHelix += 1.0;
						maxVoteSuccessHelixFlipped += 1.0;
						//cout << " exact match (missing Helix) detected i=" << i << endl;
					}
					if (maxVotesIndex[i] == groundTruth[i]) {
						maxVoteSuccess += 1.0;
						maxVoteSuccessHelix += 1.0;
						maxVoteSuccessHelixFlipped += 1.0;
						//cout << " exact match detected i=" << i << endl;
					}
					// first test for a flipped helix
					if (i>0 && maxVotesIndex[i-1]==groundTruth[i] && maxVotesIndex[i]==groundTruth[i-1]) {
						// another test to make sure these are really the same helix
						int mini=min(maxVotesIndex[i],maxVotesIndex[i-1]);
						int maxi=max(maxVotesIndex[i],maxVotesIndex[i-1]);
						if (mini%2==1 && maxi==mini+1) {
							maxVoteSuccessHelixFlipped += 1.0;
							//cout << " flip1 detected i=" << i << endl;
						}
					}
					// first test for a flipped helix
					if (i<numNodes-1 && maxVotesIndex[i+1]==groundTruth[i] && maxVotesIndex[i]==groundTruth[i+1]) {
						// another test to make sure these are really the same helix
						int mini=min(maxVotesIndex[i],maxVotesIndex[i+1]);
						int maxi=max(maxVotesIndex[i],maxVotesIndex[i+1]);
						if (mini%2==1 && maxi==mini+1) {
							maxVoteSuccessHelixFlipped += 1.0;
							//cout << " flip2 detected i=" << i << endl;
						}
					}
				} else if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					if (maxVotesIndex[i] == groundTruth[i]) {
						maxVoteSuccess += 1.0;
						maxVoteSuccessSheet += 1.0;
					}
				}
			}
			maxVoteSuccess /= (double)numNodes;
			maxVoteSuccessHelix /= (double)nh;
			maxVoteSuccessHelixFlipped /= (double)nh;
			maxVoteSuccessSheet /= (double)ns;
			cout << "The max vote guess success rate is " << maxVoteSuccess << ". Helix: " << maxVoteSuccessHelix << " (" << maxVoteSuccessHelixFlipped << " allowing flips). Sheet: " << maxVoteSuccessSheet << "." << endl;

			// compare the # of votes of ground truth with # of votes received by the most-voted 
			double voteRatio[MAX_NODES];
			double averageVoteRatio=0.0;
			double averageVoteRatioHelix=0.0;
			double averageVoteRatioSheet=0.0;
			for (int i = 0; i < numNodes; i++) {
				voteRatio[i]=(double)groundTruthVotes[i]/(double)maxVotesCount[i];
				averageVoteRatio+=voteRatio[i];
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
					averageVoteRatioHelix+=voteRatio[i];
				}
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					averageVoteRatioSheet+=voteRatio[i];
				}
			}
			averageVoteRatio /= numNodes;
			averageVoteRatioHelix /= nh;
			averageVoteRatioSheet /= ns;
			cout << " % true ";
			for (int i = 0; i < numNodes; i++) {
				cout.width(2);
				cout << voteRatio[i] << " ";
			}
			cout << endl;
			cout << "Average vote ratio is " << averageVoteRatio << ". Helices: " << averageVoteRatioHelix << ". Sheets: " << averageVoteRatioSheet << endl;
			




			// print the voting rank of the ground truth for each node




		}

		// Compute the cost of the ground truth solution which is submitted by the user.
		void WongMatch15ConstrainedNoFuture::ComputeSolutionCost(int solution[], bool extraMessages) {
			if (extraMessages) {cout << "starting ComputeSolutionCost" << endl;}
			int n1=0, n2=1;
			double edgeCost = 0.0;

			double helixCost = 0.0;
			double helixPenaltyCost = 0.0;
			double loopCost = 0.0;
			double loopPenaltyCost = 0.0;
			double sheetCost = 0.0;
			double sheetPenaltyCost = 0.0;
			double skipPenaltyCost = 0.0;

			double edgePenaltyCost = 0.0;
			double nodeCost = 0.0;
			int skippedHelixNodes = 0;
			int skippedSheetNodes = 0;

			int numNodes = patternGraph->GetNodeCount();

			// iterate over all correspondences, adding each to the previous solution
			while (n2 < numNodes) { 

				// check if first node is skipped helix or sheet
				if (solution[n1] == -1) {
					if (extraMessages) {cout << "skipped node found at " << n1+1 << " with adj matrix value " << patternGraph->adjacencyMatrix[n1][n1][0] << endl;}
					if (patternGraph->adjacencyMatrix[n1][n1][0] == GRAPHNODE_HELIX) {
						skippedHelixNodes++;
					}
					if (patternGraph->adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET) {
						skippedSheetNodes++;
					}
				}

				// find the end of the current correspondence
				//cout << "begin while block. n1 = " << n1 << ", n2 = " << n2 << endl;
				//while (solution[n2] == -1 && n2 < numNodes) {
				while (solution[n2] == -1 && n2 < numNodes-1) {
					//cout << "skipped node found at " << n2+1 << " with adj matrix value " << patternGraph->adjacencyMatrix[n2][n2][0] << endl;
					if (patternGraph->adjacencyMatrix[n2][n2][0] == GRAPHNODE_HELIX) {
						skippedHelixNodes++;
					}
					if (patternGraph->adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET) {
						skippedSheetNodes++;
					}
					n2++;
				}
	
				//cout << "after advancing n2, n1 = " << n1 << ", n2 = " << n2 << ", skippedHN = " << skippedHelixNodes << ", skippedSN = " << skippedSheetNodes << endl;

				// add edge cost
				if (extraMessages) {cout << "adding (" << n1+1 << "," << n2+1 << "," << solution[n1] << "," << solution[n2] << ")" << endl;}
				double singleEdgeCost = 1000;
				double singleEdgePenaltyCost = 0;

				// if edge exists in base graph, find the cost of this correspondence.
				if (solution[n1] == -1) {
					singleEdgeCost = 0;
					//singleEdgePenaltyCost = GetPenaltyCost(n1+1, n2-n1, extraMessages);
					//singleEdgePenaltyCost = GetPenaltyCost(n1, n2-n1+1, extraMessages);
					singleEdgePenaltyCost = GetPenaltyCost(n1, n2-n1+1, extraMessages);
					//if (extraMessages) {cout << "  GetPenaltyCost("<<n1<<","<<n2-n1<<")="<<singleEdgePenaltyCost<<endl;}
					if (extraMessages) {cout << "  GetPenaltyCost("<<n1<<","<<n2-n1+1<<")="<<singleEdgePenaltyCost<<endl;}
					//if (extraMessages) {cout << "  No edge cost for initial skip edge" << endl;}
					/*if (patternGraph->adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET) {
						sheetPenaltyCost += singleEdgePenaltyCost;
						skipPenaltyCost += singleEdgePenaltyCost;
						singleEdgePenaltyCost = MISSING_SHEET_PENALTY;
					}*/
					skipPenaltyCost += singleEdgePenaltyCost;
				} else if (baseGraph->EdgeExists(solution[n1]-1, solution[n2]-1)) {
					if (solution[n2] == -1 && n2==numNodes-1) {
						// last edge is skip edge
						singleEdgeCost = 0; 
					} else {
						singleEdgeCost = GetCost(n1+1, n2-n1, solution[n1], solution[n2], extraMessages);
					}
					singleEdgePenaltyCost = GetPenaltyCost(n1+1, n2-n1, extraMessages);
					//if (patternGraph->adjacencyMatrix[n1][n2][0] == GRAPHEDGE_LOOP) { // misses skip edges
					if (baseGraph->adjacencyMatrix[solution[n1]-1][solution[n2]-1][0] == GRAPHEDGE_HELIX) {
						//cout << "[H" << solution[n1] << "-" << solution[n2] << "]";
						helixCost += singleEdgeCost;
						helixPenaltyCost += singleEdgePenaltyCost;
						skipPenaltyCost += singleEdgePenaltyCost;
					} else {
						loopCost += singleEdgeCost;
						skipPenaltyCost += singleEdgePenaltyCost;
					}
					if (extraMessages) {cout << "  GetCost("<<n1+1<<","<<n2-n1<<","<<solution[n1]<<","<<solution[n2]<<")="<<singleEdgeCost<<endl;}
					if (extraMessages) {cout << "  GetPenaltyCost("<<n1+1<<","<<n2-n1<<")="<<singleEdgePenaltyCost<<endl;}
					if (singleEdgeCost == -1) {cout << "  MATCH FROM NODE " << solution[n1] << " TO NODE " << solution[n2] << " IS NOT ALLOWED (CUTOFF OR TYPE MISMATCH?)" << endl;} 
				} else {
					cout << "  BASE GRAPH DOES NOT HAVE AN EDGE FROM NODE " << solution[n1] << " TO NODE " << solution[n2] << ". THIS SOLUTION NOT POSSIBLE!" << endl;
				}

				// check if first or last helix is unmatched
				if ((n1 == 0 && singleEdgeCost == -1) || (n2 == numNodes && singleEdgeCost == -1)){
					if (extraMessages) {cout << "  first helix or sheet is unmatched. adding penalty of " << singleEdgeCost << endl;}
				} else {
					if (extraMessages) {cout << "  cost of this addition is " << singleEdgeCost << endl;}
				}

				// add node cost
				double singleNodeCost = GetC(n2+1, solution[n2]);
				// if at beginning of sequence, check first node
				if (patternGraph->adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET) {
					sheetCost += singleNodeCost;
				}
				if (n1==0 && patternGraph->adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET) {
					double firstNodeCost = GetC(n1+1, solution[n1]);
					sheetCost += firstNodeCost;
					singleNodeCost += firstNodeCost;
				}
				if (extraMessages) {cout << "  node cost for nodes " << n2+1 << " and " << solution[n2] << " is " << singleNodeCost << endl;}

				// add the costs from this iteration to the running totals
				edgeCost += singleEdgeCost;
				edgePenaltyCost += singleEdgePenaltyCost;
				nodeCost += singleNodeCost;

				// prepare for next iteration
				n1 = n2;
				n2++;
			}

			// Check if all helices or all sheets were correctly matched
			bool sheetsCorrect=true;
			bool helicesCorrect=true;
			bool helicesCorrectFlipped=true;
			double ratioCorrectHelices = 0;
			double ratioCorrectHelicesFlipped = 0;
			double ratioCorrectSheets = 0;
			int ns = 0, nh=0;
			for (int i=0; i < numNodes; i++) {
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
					nh++;
					if (solution[i]==SOLUTION[i]) {
						ratioCorrectHelices++;
					} else {
						helicesCorrect=false;
					}
					if (solution[i]==SOLUTION[i] || solution[i]==SOLUTION[max(0,i-1)] || solution[i]==SOLUTION[min(numNodes-1,i+1)]) {
						ratioCorrectHelicesFlipped++;
					} else {
						helicesCorrectFlipped=false;
					}

					/*
					helicesCorrect = helicesCorrect && (solution[i]==SOLUTION[i]);
					helicesCorrectFlipped = helicesCorrectFlipped && 
						(solution[i]==SOLUTION[i] || 
						solution[i]==SOLUTION[max(0,i-1)] ||
						solution[i]==SOLUTION[min(numNodes-1,i+1)]);
					*/
					
				}
				if (patternGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					ns++;
					if (solution[i]==SOLUTION[i]) {
						ratioCorrectSheets++;
					} else {
						sheetsCorrect=false;
					}
					//sheetsCorrect = sheetsCorrect && (solution[i]==SOLUTION[i]);
				}
			}
			int incorrectHelixCount = (nh - (int)(ratioCorrectHelices+0.1))/2;
			int incorrectStrandCount = ns - (int)(ratioCorrectSheets+0.1);
			if (ns>0) {ratioCorrectSheets /= ns;}
			if (nh>0) {ratioCorrectHelices /= nh;}
			if (nh>0) {ratioCorrectHelicesFlipped /= nh;}

			char sheetChar;
			if (sheetsCorrect) {
				sheetChar='S';
			} else {
				sheetChar='-';
			}

			char helixChar;
			if (helicesCorrect) {
				helixChar='H';
			} else if (helicesCorrectFlipped) {
				helixChar='h';
			} else {
				helixChar='-';
			}

			if (extraMessages) {cout << "total edge cost is " << edgeCost << endl;}
			if (extraMessages) {cout << "total edge penalty cost is " << edgePenaltyCost << endl;}
			if (extraMessages) {cout << "total node cost is " << nodeCost << endl;}

			double helixPenalty = skippedHelixNodes/2 * MISSING_HELIX_PENALTY; 
			double sheetPenalty = skippedSheetNodes * MISSING_SHEET_PENALTY;

			if (extraMessages) {cout << "missing helices: " << skippedHelixNodes/2 << " contribute cost of " << helixPenalty << endl;}
			if (extraMessages) {cout << "missing sheets:  " << skippedSheetNodes << " contribute cost of " << sheetPenalty << endl;}
			if (extraMessages) {cout << "together, missing helices and sheets contribute cost of " << sheetPenalty + helixPenalty << endl;}
			if (extraMessages) {cout << "algorithm thinks missing helices and sheets should contribute cost of " << edgePenaltyCost << endl;}


			double cost = edgeCost + nodeCost + helixPenalty + sheetPenalty;

			if (extraMessages) {cout << "total cost is " << cost << endl;}
			cout << "C=" << loopCost+helixCost+sheetCost+skipPenaltyCost << "(" << helixChar << sheetChar << ")(h" << incorrectHelixCount << ",s" << incorrectStrandCount << ")(" << ratioCorrectHelices << "," << ratioCorrectHelicesFlipped << "," << ratioCorrectSheets << ")(L=" << loopCost << ",H=" << helixCost << ",S=" << sheetCost << ",P=" << skipPenaltyCost << ")";
			if (extraMessages) {cout << endl;}
		}

		void WongMatch15ConstrainedNoFuture::NormalizeGraphs() {
#ifdef VERBOSE
			printf("Normalizing Graphs\n");
#endif // VERBOSE


#ifdef VERBOSE
			printf("\tNormalizing the base graph from Angstroms to amino acids\nNormalized Graph:\n");
#endif // VERBOSE
			for(int i = 0; i < baseGraph->nodeCount; i++) {
				for(int j = 0; j < baseGraph->nodeCount; j++) {
					// base graph
					if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT && baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX) {
						baseGraph->SetCost(i+1,j+1, baseGraph->adjacencyMatrix[i][j][1] / HELIX_C_ALPHA_TO_ANGSTROMS);
					} else if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT) {
						baseGraph->SetCost(i+1,j+1, baseGraph->adjacencyMatrix[i][j][1] / LOOP_C_ALPHA_TO_ANGSTROMS);
					}
					// euclidean distance matrix
					if(baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX) {
						baseGraph->euclideanMatrix[i][j] = baseGraph->euclideanMatrix[i][j] / HELIX_C_ALPHA_TO_ANGSTROMS;
					} else {
						baseGraph->euclideanMatrix[i][j] = baseGraph->euclideanMatrix[i][j] / LOOP_C_ALPHA_TO_ANGSTROMS;
					}
				}
			}	


#ifdef VERBOSE
			baseGraph->PrintGraph();
#endif // VERBOSE
		}




		void WongMatch15ConstrainedNoFuture::NormalizeSheets() {
#ifdef VERBOSE
			printf("\tNormalizing the sheet nodes in the base graph based on sheet ratio\nNormalized Graph:\n");
#endif // VERBOSE
			// TODO: Also normalize the sheet capacity here?
			double totalSheetSize = 0;
			double totalStrandLength = 0;

			for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
				if (baseGraph->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					totalSheetSize += (double)baseGraph->skeletonHelixes[i]->length;
#ifdef VERBOSE
					cout << "after sheet " << i << ", total sheet size is now " << totalSheetSize << endl;
#endif // VERBOSE
				}
			}

			for(int i = 0; i < (int)patternGraph->pdbStructures.size(); i++) {
				//cout << "strand " << i << " has type " << patternGraph->pdbStructures[i]->secondaryStructureType << endl;
				if(patternGraph->pdbStructures[i]->secondaryStructureType == GRAPHEDGE_SHEET) {
					totalStrandLength += patternGraph->pdbStructures[i]->GetLengthResidues();
					//totalStrandLength += patternGraph->pdbStructures[i]->GetLengthAngstroms();
#ifdef VERBOSE
					cout << "After adding strand " << i << " with length " << patternGraph->pdbStructures[i]->GetLengthResidues() << ", total strand length is now " << totalStrandLength << endl;
#endif // VERBOSE
				}
			}

			// scale the sheet sizes so that the units are in amino acids
			double ratio = totalStrandLength / totalSheetSize;
#ifdef VERBOSE
			cout << "sheet sizes must be scaled by a factor of " << ratio << endl;
#endif // VERBOSE


#ifdef VERBOSE
			printf("\tNormalizing the base graph sheets from voxels to scaled voxels\nNormalized Graph:\n");
#endif // VERBOSE
			for(int i = 0; i < baseGraph->nodeCount; i++) {
				if(baseGraph->adjacencyMatrix[i][i][1] != MAXINT && baseGraph->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					// scale the sheet weight to the # of amino acids
					baseGraph->SetCost(i+1, baseGraph->nodeWeights[i] * ratio);
					// take sqrt for matching algorithm
					baseGraph->SetCost(i+1, sqrt(baseGraph->nodeWeights[i]));
				}
			}	
			
			
#ifdef VERBOSE
			baseGraph->PrintGraph();
#endif // VERBOSE
		}



		unsigned long long WongMatch15ConstrainedNoFuture::EncodeNode(unsigned long long bitmap, int node) {
			if(node == -1)
				return bitmap;

			return (bitmap | ((unsigned long long)1 << node));

		}

		// code copied from LinkedNode::PrintNodeConcise
		// Adding a breakdown of the cost into loops, nodes, and helices
		void WongMatch15ConstrainedNoFuture::PrintNodeConcise(LinkedNode * node, int rank, bool endOfLine, bool printCostBreakdown) {
			bool used[MAX_NODES];
			int n1[MAX_NODES];
			int n2[MAX_NODES];
			int top = 0;
			for(int i = 0; i < MAX_NODES; i++) {
				used[i] = false;
			}

			LinkedNodeStub * currentNode = node;
			bool continueLoop = true;
			while(continueLoop) {
				if(currentNode->parentNode == NULL) {
					 break;
				}
				n1[top] = currentNode->n1Node;
				n2[top] = currentNode->n2Node;
				used[(int)currentNode->n1Node] = true;
				top++;
				currentNode = currentNode->parentNode;		
			}

			for(int i = 1; i <= node->depth; i++) {
				if(!used[i]) {
					n1[top] = i;
					n2[top] = -1;
					top++;
				}
			}

			int minIndex;
			int temp;
			for(int i = 0; i < top - 1; i++) {
				minIndex = i;
				for(int j = i+1; j < top; j++) {
					if(n1[minIndex] > n1[j]) {
						minIndex = j;
					}
				}
				temp = n1[minIndex];
				n1[minIndex] = n1[i];
				n1[i] = temp;

				temp = n2[minIndex];
				n2[minIndex] = n2[i];
				n2[i] = temp;
			}
			
			if(node->IsUserSpecifiedSolution()) {
				printf("**");
			} else {
				printf("  ");
			}
			

			if(rank != -1) {
				printf("%d)", rank);
			}
			printf("\t");
			for(int i = 0; i < top; i++) {
				printf("%2d ", n2[i]);
			}

			// print the cost of the current solution
			if(INCLUDE_STRANDS) {
				ComputeSolutionCost(n2,false);
			}
			for (int i = 0; i < MAX_NODES; i++){
				bestMatches[rank-1][i]=n2[i];
			}

			if(printCostBreakdown) {
				printf(" - %f = %f + %f", node->cost, node->costGStar, node->cost - node->costGStar);
			} else {
				printf(" - %f", node->cost);
			}
			if(endOfLine) {
				printf("\n");
			}
		}


	
	}
}
#endif
