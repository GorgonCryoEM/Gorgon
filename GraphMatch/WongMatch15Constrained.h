// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Graph Matching algorithm based on the following paper:
//				  Wong et al., An algorithm for graph optimal monomorphism, 
//				  IEEE Transactions on Systems, Man, and Cybernetics, Vol 20, No 3, May/June 1990, pp628-636
//				  Modified to support missing helixes and constrained nodes
//				  Contains all future information



#ifndef WONGMATCH15CONSTRAINED_H
#define WONGMATCH15CONSTRAINED_H

#include "StandardGraph.h"
#include "LinkedNode.h"
#include "NodeList.h"
#include "GlobalConstants.h"
#include "PathGenerator.h"
#include "Structures.h"
#include <ctime>
#include <SkeletonMaker/PriorityQueue.h>

namespace wustl_mm {
	namespace GraphMatch {

		class WongMatch15Constrained{
		public:
			StandardGraph * patternGraph;
			StandardGraph * baseGraph;
		public:
			WongMatch15Constrained(StandardGraph * patternGraph, StandardGraph * baseGraph);
			WongMatch15Constrained(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
			~WongMatch15Constrained();
			void RunMatching(clock_t startTime);
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
			int missingHelixCount;
			int missingSheetCount;
			int expandCount;
			int foundCount;
			int longestMatch;
			EdgeMinCostEntry edgeMinCosts[MAX_NODES][MAX_NODES*MAX_NODES*MAX_NODES];  // pattern graph, base graph
			int edgeMinCostCount[MAX_NODES];
			PathGenerator * pathGenerator;

		private:
			void Init(StandardGraph * patternGraph, StandardGraph * baseGraph);
			double GetC(int p, int qp);
			double GetC(int j, int p, int qj, int qp);
			double GetCost(int d, int m, int qj, int qp);
			double GetCPrime(int a, int b, int c, int d);
			double GetK(int p, int qp);
			double GetKPrime(int i, int q);
			double GetA();
			double GetB();
			double GetF();
			void PopBestNode(); // Gets the best (first) node from the active nodes list.
			bool ExpandNode(LinkedNodeStub * currentStub);  // Expands all the children of the current node.
			void NormalizeGraphs();
			void InitializeEdgeMinCosts();
			unsigned long long EncodeNode(unsigned long long bitmap, int node);

		};

		WongMatch15Constrained::WongMatch15Constrained(StandardGraph * patternGraph, StandardGraph * baseGraph) {
			Init(patternGraph, baseGraph);
		}

		WongMatch15Constrained::WongMatch15Constrained(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
			Init(patternGraph, baseGraph);
			this->missingHelixCount = missingHelixCount;
			this->missingSheetCount = missingSheetCount;
		}

		WongMatch15Constrained::~WongMatch15Constrained() {
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

			delete queue;
			
			delete pathGenerator;
		}
		void WongMatch15Constrained::Init(StandardGraph * patternGraph, StandardGraph * baseGraph) {	
			usedNodes.clear();
			queue = new PriorityQueue<LinkedNode, double> (PRIORITYQUEUESIZE);
			this->patternGraph = patternGraph;
			this->baseGraph = baseGraph;
			expandCount = 0;

			// TODO: Make this more accurate
			missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
			missingSheetCount = 0;

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
			InitializeEdgeMinCosts();

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




		void WongMatch15Constrained::InitializeEdgeMinCosts() {	
			//for(int i = 0; i < patternGraph->nodeCount; i++) {	
			//	edgeMinCosts[i][0].bitmap = 0;
			//	edgeMinCosts[i][0].cost = MISSING_HELIX_PENALTY;
			//	edgeMinCosts[i][0].noOfEdges = 1;
			//	edgeMinCostCount[i] = 1;
			//}

			unsigned long long bitmap;
			double cost;

			for(int i = 0; i < patternGraph->nodeCount; i++) {	
				for(int edgeCount = 1; edgeCount <= missingHelixCount * 2 + 1; edgeCount++) {
					for(int q = 0; q < baseGraph->nodeCount-1; q++) {
						for(int r = q+1; r < baseGraph->nodeCount; r++) {
							if(baseGraph->EdgeExists(q, r)) {
								bitmap = EncodeNode(0, q+1);
								bitmap = EncodeNode(bitmap, r+1);
								cost = GetCost(i+1, edgeCount, q+1, r+1) / (double)edgeCount;
								if(cost >= 0) {
									for(int kk = 0; kk < min(edgeCount, patternGraph->nodeCount - i); kk++) {
										edgeMinCosts[i+kk][edgeMinCostCount[i+kk]].noOfEdges = edgeCount;
										edgeMinCosts[i+kk][edgeMinCostCount[i+kk]].bitmap = bitmap;
										edgeMinCosts[i+kk][edgeMinCostCount[i+kk]].cost = cost;
										edgeMinCostCount[i+kk]++;
									}
								}
							}
						}
					}
				}
			}


			int minIndex;
			EdgeMinCostEntry temp;
			for(int i = 0; i < patternGraph->nodeCount; i++) {
				for(int k = 0; k < edgeMinCostCount[i] - 1; k++) {
					minIndex = k;
					for(int l = k+1; l < edgeMinCostCount[i]; l++) {
						if(edgeMinCosts[i][l].cost < edgeMinCosts[i][minIndex].cost) {
							minIndex = l;
						}
					}
					temp = edgeMinCosts[i][k];
					edgeMinCosts[i][k] = edgeMinCosts[i][minIndex];
					temp = edgeMinCosts[i][minIndex] = temp;
				}
			}

			//for(int i = 0; i < patternGraph->nodeCount; i++) {
			//	for(int k = 0; k < edgeMinCostCount[i] - 1; k++) {
			//		printf("\t%f", edgeMinCosts[i][k].cost);
			//	}
			//	printf("\n");
			//}
		}

		void WongMatch15Constrained::RunMatching(clock_t startTime) {
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
					printf(": (%d expanded) (%f seconds) (%fkB Memory) (%d queue size) (%d parent size)\n", expandCount, (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC, (queue->getLength() * sizeof(LinkedNode) + usedNodes.size() * sizeof(LinkedNodeStub)) / 1024.0, queue->getLength(), (int)usedNodes.size());

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
				}
				delete currentNode;
				continueLoop = (foundCount < RESULT_COUNT);
			}
		}

		void WongMatch15Constrained::SaveResults(){
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

		double WongMatch15Constrained::GetC(int p, int qp) {
			return GetC(p, p, qp, qp);
		}

		double WongMatch15Constrained::GetC(int j, int p, int qj, int qp) {

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

		double WongMatch15Constrained::GetCost(int d, int m, int qj, int qp) {
			double patternLength = 0;
			double baseLength;
			double additionalCost = (m - 1) * MISSING_HELIX_PENALTY / 2.0;

			// Adding the length of the skipped helixes
			for(int i = 1; i < m; i++) {
				patternLength += patternGraph->adjacencyMatrix[d+i-1][d+i-1][1];
			}

			// Adding the length of the edges
			bool firstIsLoop = false;
			bool lastIsLoop = false;
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
				if((qj != -1) && (baseGraph->euclideanMatrix[qj-1][qp-1] > patternGraph->adjacencyMatrix[d-1][d][1])){
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
				return weight * fabs(patternLength - baseLength) + additionalCost; 
				break;
			case(2):
				return weight * fabs(patternLength - baseLength) / (patternLength + baseLength) + additionalCost;
				break;
			case(3):
				return weight * pow((patternLength - baseLength),2) + additionalCost;
				break;
			}
			return 0;
		}

		double WongMatch15Constrained::GetA() {
		#ifdef VERBOSE
			clock_t startTime = clock();
		#endif
			double cost = 0;
			double minCost;
			// Cost for node matching = 0 since nodes dont have values
			//for(int i = 1; i <= patternGraph->nodeCount; i++)
			//{
			//	if(LinkedNode::IsNodeInBitmap(currentNode->m1Bitmap, i)) {			
			//		minCost = MISSING_HELIX_PENALTY;
			//		for(int j = 1; j <= baseGraph->nodeCount; j++) {
			//			if(LinkedNode::IsNodeInBitmap(currentNode->m2Bitmap, j)) {
			//				minCost = min(minCost, GetC(i, j));
			//			}
			//		}
			//		cost += minCost;
			//	}
			//}
			
			// Cost for the first edge matched
			if(currentNode->depth != patternGraph->nodeCount) {
				int lastPatternNode = currentNode->n1Node;
				int lastBaseNode = currentNode->n2Node;
				int usableEdgeCount = min(patternGraph->nodeCount - currentNode->depth, missingHelixCount * 2 - currentNode->missingNodesUsed + 1);

				unsigned long long bitmap = currentNode->GetN2Bitmap();
				LinkedNode::RemoveNodeFromBitmap(bitmap, currentNode->n2Node);
				unsigned long long edgeBitmap = EncodeNode(0, lastBaseNode);

				minCost = MAXINT;
				
				for(int i = 0; i < edgeMinCostCount[lastPatternNode-1]; i++) {
					if(((edgeMinCosts[lastPatternNode-1][i].bitmap & bitmap) == 0) &&
							((edgeMinCosts[lastPatternNode-1][i].bitmap & edgeBitmap) == edgeBitmap) &&
							(edgeMinCosts[lastPatternNode-1][i].noOfEdges <= usableEdgeCount)) {
						minCost = min(minCost, edgeMinCosts[lastPatternNode-1][i].cost);
						break;
					}
				}
				cost += minCost;
			}
		#ifdef VERBOSE
			timeInGetA += clock() - startTime;
		#endif
			return cost;
		}

		double WongMatch15Constrained::GetB() {
		#ifdef VERBOSE
			clock_t startTime = clock();
		#endif
			
			int i;
			double minCost = 0, cost = 0;
			unsigned long long bitmap = currentNode->GetN2Bitmap();
			int usableEdges = min(patternGraph->nodeCount - currentNode->depth, missingHelixCount * 2 - currentNode->missingNodesUsed + 1);;

			for(i = 1; i < patternGraph->nodeCount; i++) {
				if(LinkedNode::IsNodeInBitmap(currentNode->m1Bitmap, i)) {
					for(int j = 0; j < edgeMinCostCount[i-1]; j++) {					
						if(((edgeMinCosts[i-1][j].bitmap & bitmap) == 0)  && 
							(edgeMinCosts[i-1][j].noOfEdges <= usableEdges)) {
							minCost = edgeMinCosts[i-1][j].cost;
							break;
						}
					}

					cost += minCost;
				}
			}
		#ifdef VERBOSE
			timeInGetB += clock() - startTime;
		#endif
			return cost;
		}

		double WongMatch15Constrained::GetF() {
			return currentNode->costGStar + GetA() + GetB();	
		}

		void WongMatch15Constrained::PopBestNode(){
		#ifdef VERBOSE
			clock_t start = clock();
		#endif
			double cost;
			queue->remove(currentNode, cost);
		#ifdef VERBOSE
			timeInQueue += clock() - start;
		#endif
		}

		bool WongMatch15Constrained::ExpandNode(LinkedNodeStub * currentStub) {
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
								currentNode->costGStar += temp->costGStar + edgeCost + GetC(currentNode->n1Node, currentNode->n2Node);
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
					currentNode->depth = (char)patternGraph->nodeCount;
					currentNode->costGStar = temp->costGStar + MISSING_HELIX_PENALTY * (patternGraph->nodeCount - temp->depth) / 2.0 + START_END_MISSING_HELIX_PENALTY;
					currentNode->cost = currentNode->costGStar;
					queue->add(currentNode, currentNode->cost);
					currentNode = temp;
				}
			}
			return expanded;
		}

		void WongMatch15Constrained::NormalizeGraphs() {

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

		unsigned long long WongMatch15Constrained::EncodeNode(unsigned long long bitmap, int node) {
			if(node == -1)
				return bitmap;

			return (bitmap | ((unsigned long long)1 << node));
		}
	}
}
#endif
