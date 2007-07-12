/*
Graph Matching algorithm based on the following paper:
  Wong et al., An algorithm for graph optimal monomorphism, 
  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636

  Modified to support missing helixes and constrained nodes

Author: Sasakthi S. Abeysinghe
Date  : 08/14/2006
*/

#ifndef WONGMATCH15CONSTRAINEDNOFUTURE_H
#define WONGMATCH15CONSTRAINEDNOFUTURE_H

#include "StandardGraph.h"
#include "LinkedNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"
#include "PathGenerator.h"
#include "Structures.h"
#include <time.h>
#include "../SkeletonMaker/PriorityQueue.h"

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
			LinkedNode * GetResult(int rank);
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
			vector<LinkedNode*> solutions;
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

			for(unsigned int i = 0; i < solutions.size(); i++) {
				delete solutions[i];
			}
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
			usedNodes.clear();
			queue = new PriorityQueue<LinkedNode, double> (PRIORITYQUEUESIZE);
			this->patternGraph = patternGraph;
			this->baseGraph = baseGraph;
			expandCount = 0;

			// TODO: Make this more accurate
			missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
			missingSheetCount = 0;

			if(!PERFORMANCE_COMPARISON_MODE) {
				NormalizeGraphs();
			}
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
			bool continueLoop = true;
			clock_t finishTime;
			while(continueLoop)
			{
				PopBestNode();		
				if(currentNode == NULL) {
					break;
				}
				if(currentNode->depth == patternGraph->nodeCount) {
					finishTime = clock();
					foundCount++;
					currentNode->PrintNodeConcise(foundCount, false);
					printf(": (%d expanded) (%f seconds) (%fkB Memory) (%d queue size) (%d parent size)\n", expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0, queue->getLength(), usedNodes.size());
					solutions.push_back(currentNode);

					#ifdef MAKE_FINAL_MRC
						char fileName[80];
						sprintf(fileName, "Solution%d.mrc", foundCount);
						pathGenerator->GenerateGraph(currentNode, fileName);
					#endif
				} else {
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
			return foundCount;
		}

		LinkedNode * WongMatch15ConstrainedNoFuture::GetResult(int rank) {
			if(rank <= solutions.size() && (rank >= 1)) {
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

		double WongMatch15ConstrainedNoFuture::GetC(int p, int qp) {
			return GetC(p, p, qp, qp);
		}

		double WongMatch15ConstrainedNoFuture::GetC(int j, int p, int qj, int qp) {

			double jpCost;
			double qjqpCost;
			double typeCost = 0;

			if(patternGraph->adjacencyMatrix[j-1][p-1][1] == MAXINT) {
				jpCost = 1000;
			} else {
				jpCost = patternGraph->adjacencyMatrix[j-1][p-1][1];
			}

			if(baseGraph->adjacencyMatrix[qj-1][qp-1][1] == MAXINT) {
				qjqpCost = 1000;
			} else {
				qjqpCost = baseGraph->adjacencyMatrix[qj-1][qp-1][1];
			}

			if(patternGraph->adjacencyMatrix[j-1][p-1][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) {
				typeCost = 1000;
			}

			return fabs(jpCost - qjqpCost) + typeCost;
		}

		double WongMatch15ConstrainedNoFuture::GetCost(int d, int m, int qj, int qp) {
			double patternLength = 0;
			double baseLength;

			// Adding the length of the skipped helixes
			for(int i = 1; i < m; i++) {
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
			}

			// Adding the length of the edges
			bool firstIsLoop;
			bool lastIsLoop;
			for(int i = 0; i < m; i++) {
				lastIsLoop = ((patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP) || (patternGraph->adjacencyMatrix[d+i-1][d+i][0] == GRAPHEDGE_LOOP_EUCLIDEAN));
				if(i==0) {
					firstIsLoop = lastIsLoop;
				}
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i][1];		
			}


			bool euclideanEstimate = false;
			double weight = 1.0;

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
				euclideanEstimate = (baseGraph->adjacencyMatrix[qj-1][qp-1][0] == GRAPHEDGE_LOOP_EUCLIDEAN);
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
				}
				weight = euclideanEstimate? weight * EUCLIDEAN_LOOP_PENALTY: weight;
			}


			if(m == 1) {		
				if((qj!= -1) && (patternGraph->adjacencyMatrix[d-1][d][0] != baseGraph->adjacencyMatrix[qj-1][qp-1][0]) &&
					!((patternGraph->adjacencyMatrix[d-1][d][0] == GRAPHEDGE_LOOP) && (baseGraph->adjacencyMatrix[qj-1][qp-1][0] == GRAPHEDGE_LOOP_EUCLIDEAN))) 	{
					return -1;
				}		
				if((qj != -1) && (baseGraph->euclideanMatrix[qj-1][qp-1] > (patternGraph->adjacencyMatrix[d-1][d][1] * EUCLIDEAN_VOXEL_TO_PDB_RATIO ))){
					return -1;
				}
			} else {
				if(!firstIsLoop || !lastIsLoop) {
					return -1;
				}
			}


			switch(COST_FUNCTION)
			{
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

		bool WongMatch15ConstrainedNoFuture::ExpandNode(LinkedNodeStub * currentStub) {
			bool expanded = false;
			expandCount++;
			LinkedNode * temp;
			double edgeCost;
		#ifdef VERBOSE
			if(longestMatch < currentNode->depth) {
				longestMatch = currentNode->depth;
				printf(" %d elements matched! (%f kB Memory Used)\n", longestMatch, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0);
			}
		#endif //VERBOSE
			
			int currentM1Top = patternGraph->nodeCount - currentNode->depth;
			int currentM2Top = baseGraph->nodeCount - currentNode->depth + currentNode->missingNodesUsed;
			bool notConstrained;
			//currentNode->PrintNodeConcise(0, true, true);

			// Expanding nodes with a real terminal node
			for(int i = 1; i <= baseGraph->nodeCount; i++) {		
				if((currentNode->depth == 0) || 
					(LinkedNode::IsNodeInBitmap(currentNode->m2Bitmap, i) && (baseGraph->EdgeExists(currentNode->n2Node-1, i-1)))) {
					for(int j = 0; j <= min(missingHelixCount * 2 - currentNode->missingNodesUsed + 1, currentM1Top); j += 2) {  // Stepping by two since we jump every 2 loops
						notConstrained = true;

						for(int k = currentNode->n1Node + 1; k <= currentNode->n1Node + j; k++) {
							notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
						}
						notConstrained = notConstrained && IsNodeAssignmentAllowed(currentNode->n1Node + j + 1, i);

						if(notConstrained) {
							temp = currentNode;
							currentNode = new LinkedNode(currentNode, currentStub, i, j);
							currentNode->costGStar = 0;

							if(((temp->depth == 0) && (j > 0)) || 
								((patternGraph->nodeCount - currentNode->depth == 0) && (currentNode->n2Node == -1))) {
								currentNode->costGStar += START_END_MISSING_HELIX_PENALTY;
							}


							if(temp->depth == 0) {
								edgeCost = 0;
							} else {
								edgeCost = GetCost(temp->n1Node, j+1, temp->n2Node, currentNode->n2Node);
							}

							if(edgeCost >= 0) {
								currentNode->costGStar += temp->costGStar + edgeCost +	MISSING_HELIX_PENALTY * (j/2.0) + GetC(currentNode->n1Node, currentNode->n2Node);
								currentNode->cost = GetF();			
								//currentNode->PrintNodeConcise(-1, true, true);
								queue->add(currentNode, currentNode->cost);
								expanded = true;
							} else {
								delete currentNode;
							}
							currentNode = temp;	
						}
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
					currentNode->depth = patternGraph->nodeCount;
					currentNode->costGStar = temp->costGStar + MISSING_HELIX_PENALTY * (patternGraph->nodeCount - temp->depth) / 2.0 + START_END_MISSING_HELIX_PENALTY;
					currentNode->cost = currentNode->costGStar;
					queue->add(currentNode, currentNode->cost);
					currentNode = temp;
				}
			}
			return expanded;
		}

		void WongMatch15ConstrainedNoFuture::NormalizeGraphs() {

		#ifdef VERBOSE
			printf("\tNormalizing the base graph based on helix length ratio\nNormalized Graph:\n");
		#endif
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