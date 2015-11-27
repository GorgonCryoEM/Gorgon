// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A library that generates random graphs


#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H

#include "StandardGraph.h"
#include "GlobalConstants.h"
#include "SecondaryStructure.h"
#include <vector>

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class GraphGenerator {
		public:
			static StandardGraph * GeneratePDBGraph(int nodeCount);
			static StandardGraph * GeneratePDBGraphWithNodeAttributes(int nodeCount);
			static StandardGraph * GenerateSkeletonGraph(StandardGraph * patternGraph, int density);
			static StandardGraph * GenerateSkeletonGraphWithNodeAttributes(StandardGraph * patternGraph, int density);
		private:
			static StandardGraph * ShufflePatternGraph(StandardGraph * patternGraph);
			static StandardGraph * ShufflePatternGraphWithNodeAttributes(StandardGraph * patternGraph);
			static void AddExtraPaths(StandardGraph * baseGraph, int density);
			static void PerturbEdges(StandardGraph * baseGraph);
			static int GetRandom(int maxNumber);
			static double GetRandom(double maxNumber);
		};

		int GraphGenerator::GetRandom(int maxNumber) {
			return ((float)rand() / (float)(RAND_MAX + 1))* (float)(maxNumber + 1);
		}

		double GraphGenerator::GetRandom(double maxNumber) {
			return ((double)rand() * maxNumber) / (double)RAND_MAX;
		}

		StandardGraph * GraphGenerator::GeneratePDBGraph(int nodeCount) {
			StandardGraph * pg = new StandardGraph(nodeCount);
			pg->adjacencyMatrix[0][0][1] = 0;
			SecondaryStructure * pdbStruct;

			for(int i = 0; i < nodeCount-1; i++) { 
				pg->adjacencyMatrix[i+1][i+1][1] = 0;	
				if(i%2 == 0) {	// Helix
					pg->adjacencyMatrix[i][i+1][0] = GRAPHEDGE_HELIX;
					pg->adjacencyMatrix[i+1][i][0] = GRAPHEDGE_HELIX;
					pg->adjacencyMatrix[i][i+1][1] = GetRandom((double)MAX_RANDOM_HELIX_SIZE);
					pg->adjacencyMatrix[i+1][i][1] = pg->adjacencyMatrix[i][i+1][1];

					pdbStruct = new SecondaryStructure();
					pdbStruct->serialNumber = i/2;
					pdbStruct->startPosition = 1;
					pdbStruct->endPosition = 1 + (int)pg->adjacencyMatrix[i][i+1][1];
					pdbStruct->secondaryStructureType = GRAPHEDGE_HELIX;
					pg->pdbStructures.push_back(pdbStruct);

				} else {		// Loop
					pg->adjacencyMatrix[i][i+1][0] = GRAPHEDGE_LOOP;
					pg->adjacencyMatrix[i+1][i][0] = GRAPHEDGE_LOOP;
					pg->adjacencyMatrix[i][i+1][1] = GetRandom((double)MAX_RANDOM_LOOP_SIZE);
					pg->adjacencyMatrix[i+1][i][1] = pg->adjacencyMatrix[i][i+1][1];			
				}		
			}	
			return pg;
		}

		StandardGraph * GraphGenerator::GeneratePDBGraphWithNodeAttributes(int nodeCount) {
			StandardGraph * pg = new StandardGraph(nodeCount);
			SecondaryStructure * pdbStruct;

			for(int i = 0; i < nodeCount; i++) { 
				// Helix
				pg->adjacencyMatrix[i][i][1] = GetRandom((double)MAX_RANDOM_HELIX_SIZE);	
				pg->adjacencyMatrix[i][i][0] = GRAPHEDGE_HELIX;

				pdbStruct = new SecondaryStructure();
				pdbStruct->serialNumber = i;
				pdbStruct->startPosition = 1;
				pdbStruct->endPosition = 1 + (int)pg->adjacencyMatrix[i][i][1];
				pdbStruct->secondaryStructureType = GRAPHEDGE_HELIX;
				pg->pdbStructures.push_back(pdbStruct);

				
				if(i > 0) {		// Loop
					pg->adjacencyMatrix[i-1][i][0] = GRAPHEDGE_LOOP;
					pg->adjacencyMatrix[i][i-1][0] = GRAPHEDGE_LOOP;
					pg->adjacencyMatrix[i-1][i][1] = GetRandom((double)MAX_RANDOM_LOOP_SIZE);
					pg->adjacencyMatrix[i][i-1][1] = pg->adjacencyMatrix[i-1][i][1];			
				}		
			}	
			return pg;
		}

		StandardGraph * GraphGenerator::GenerateSkeletonGraph(StandardGraph * patternGraph, int density) {
			StandardGraph * bg = ShufflePatternGraph(patternGraph);
			AddExtraPaths(bg, density);
			PerturbEdges(bg);
			return bg;
		}

		StandardGraph * GraphGenerator::GenerateSkeletonGraphWithNodeAttributes(StandardGraph * patternGraph, int density) {
			StandardGraph * bg = ShufflePatternGraphWithNodeAttributes(patternGraph);
			AddExtraPaths(bg, density);
			PerturbEdges(bg);
			return bg;
		}


		void GraphGenerator::AddExtraPaths(StandardGraph * baseGraph, int density) {	
			int i, j;
			vector<double> loopSizes;

			for(i = 0; i < baseGraph->nodeCount; i++) {
				for(j = 0; j < i; j++) {
					if(baseGraph->adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP) {
						loopSizes.push_back(baseGraph->adjacencyMatrix[i][j][1]);
					}
				}
			}


			for(i = 0; i < baseGraph->nodeCount; i++) {
				for(j = 0; j < i; j++) {
					if((baseGraph->adjacencyMatrix[i][j][0] != GRAPHEDGE_HELIX) && !baseGraph->EdgeExists(i, j)) {
						if(GetRandom(100) < density) {
							baseGraph->adjacencyMatrix[i][j][0] = GRAPHEDGE_LOOP;
							baseGraph->adjacencyMatrix[i][j][1] = loopSizes[GetRandom((int)(loopSizes.size()-1))];
							baseGraph->adjacencyMatrix[j][i][0] = GRAPHEDGE_LOOP;
							baseGraph->adjacencyMatrix[j][i][1] = baseGraph->adjacencyMatrix[i][j][1];

						}
					}
				}
			}

			loopSizes.clear();
		}

		void GraphGenerator::PerturbEdges(StandardGraph * baseGraph) {
			int i,j;
			for(i = 0; i < baseGraph->nodeCount; i++) {
				for(j = 0; j <= i; j++) {
					if(baseGraph->adjacencyMatrix[i][j][0] != GRAPHEDGE_OTHER) {
						baseGraph->adjacencyMatrix[i][j][1] += GetRandom(2.0);
						baseGraph->adjacencyMatrix[j][i][1] = baseGraph->adjacencyMatrix[i][j][1];
					}
				}
			}
		}

		StandardGraph * GraphGenerator::ShufflePatternGraph(StandardGraph * patternGraph) {
			StandardGraph * bg = new StandardGraph(patternGraph->nodeCount);
			bool helixAllocation[MAX_NODES/2];
			int helixIsomorphism[MAX_NODES/2];
			int nodeIsomorphism[MAX_NODES];
			vector<GeometricShape *> skeletonShape;
			GeometricShape * shape;
			int i, j, index, count;
			int helixCount = patternGraph->nodeCount / 2;

			for(i = 0; i < helixCount; i++) {
				helixAllocation[i] = false;
				shape = new GeometricShape();
				shape->length = (int)patternGraph->adjacencyMatrix[i*2][i*2+1][1];
				skeletonShape.push_back(shape);
			}

			for(i = 0; i < helixCount; i++) {
				index = GetRandom(helixCount - i - 1) + 1;
				count = 0;
				for(j = 0; j < helixCount; j++) {
					if(!helixAllocation[j]) {
						count++;
					}
					if(count==index) {
						helixIsomorphism[i] = j;
						helixAllocation[j] = true;
						break;
					}
				}
			}

			for(i = 0; i < helixCount; i++) {
				int bit = GetRandom(1);
				nodeIsomorphism[i*2] = helixIsomorphism[i]*2 + bit;
				nodeIsomorphism[i*2+1] = helixIsomorphism[i]*2 + ((bit + 1) % 2);
			}

			for(i = 0; i < bg->nodeCount; i++) {
				for(j = 0; j < bg->nodeCount; j++) {
					bg->adjacencyMatrix[i][j][0] = patternGraph->adjacencyMatrix[nodeIsomorphism[i]][nodeIsomorphism[j]][0];
					bg->adjacencyMatrix[i][j][1] = patternGraph->adjacencyMatrix[nodeIsomorphism[i]][nodeIsomorphism[j]][1];
				}
			}

			for(i = 0; i < helixCount; i++) {
				bg->skeletonHelixes.push_back(skeletonShape[i]);
			}

			return bg;
		}

		StandardGraph * GraphGenerator::ShufflePatternGraphWithNodeAttributes(StandardGraph * patternGraph) {
			StandardGraph * bg = new StandardGraph(patternGraph->nodeCount);
			bool helixAllocation[MAX_NODES];
			int nodeIsomorphism[MAX_NODES];
			vector<GeometricShape *> skeletonShape;
			GeometricShape * shape;
			int i, j, index, count;
			int helixCount = patternGraph->nodeCount;

			for(i = 0; i < helixCount; i++) {
				helixAllocation[i] = false;
				shape = new GeometricShape();
				shape->length = (int)patternGraph->adjacencyMatrix[i][i][1];
				skeletonShape.push_back(shape);
			}

			for(i = 0; i < helixCount; i++) {
				index = GetRandom(helixCount - i - 1) + 1;
				count = 0;
				for(j = 0; j < helixCount; j++) {
					if(!helixAllocation[j]) {
						count++;
					}
					if(count==index) {
						nodeIsomorphism[i] = j;
						helixAllocation[j] = true;
						break;
					}
				}
			}


			for(i = 0; i < bg->nodeCount; i++) {
				for(j = 0; j < bg->nodeCount; j++) {
					bg->adjacencyMatrix[i][j][0] = patternGraph->adjacencyMatrix[nodeIsomorphism[i]][nodeIsomorphism[j]][0];
					bg->adjacencyMatrix[i][j][1] = patternGraph->adjacencyMatrix[nodeIsomorphism[i]][nodeIsomorphism[j]][1];
				}
			}

			for(i = 0; i < helixCount; i++) {
				bg->skeletonHelixes.push_back(skeletonShape[i]);
			}

			return bg;
		}
	}
}
#endif
