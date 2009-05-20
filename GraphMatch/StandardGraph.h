// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Storage structure for an attributed relational graph

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.13  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.12  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//


#ifndef STANDARDGRAPH_H
#define STANDARDGRAPH_H

#include <cassert>
#include <vector>
#include "GlobalConstants.h"
#include "Matcher2Helix.h"
#include "SecondaryStructure.h"
#include "GeometricShape.h"
#include <SkeletonMaker/volume.h>
#include <MathTools/Vector3D.h>
#include <SkeletonMaker/volume.h>
using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class StandardGraph {
		public:
			int nodeCount;

		public:
			// Constructors
			StandardGraph(int nodeCount);
			StandardGraph(char* fname);
			~StandardGraph();

			int GetType(int i, int j); // The type of the edge (index starting from 1)
			double GetCost(int i, int j); // The cost based on the graph labels (index starting from 1)
			int GetNodeCount(); // Returns the number of nodes
			void SetType(int i, int j, int type); // The type of the edge (index starting from 1)
			void SetCost(int i, int j, double cost); // The cost based on the graph labels (index starting from 1)
			void SetNodeCount(int nodeCount); // Sets the number of nodes
			void PrintGraph();
			void GenerateEuclidianMatrix(Volume * vol);
			vector<Matcher2Helix> GetHelixLengths();
			bool EdgeExists(int n, int m);
		public:
			double adjacencyMatrix[MAX_NODES][MAX_NODES][2]; // 0th dimension edge type... 1st dimension distance
			double euclideanMatrix[MAX_NODES][MAX_NODES];
			vector<Vector3DInt> paths[MAX_NODES][MAX_NODES];
			vector<SecondaryStructure*> pdbStructures;
			vector<GeometricShape*> skeletonHelixes;
			Volume * skeletonVolume;
			Volume * skeletonSheetVolume;
		private:
		};

		StandardGraph::~StandardGraph() {
			int i;
			for(i = 0; i < (int)pdbStructures.size(); i++) {
				delete pdbStructures[i];
			}
			pdbStructures.clear();

			for(i = 0; i < (int)skeletonHelixes.size(); i++) {
				delete skeletonHelixes[i];
			}

			skeletonHelixes.clear();

			if(skeletonVolume != NULL) {
				delete skeletonVolume;
			}

			if(skeletonSheetVolume != NULL) {
				delete skeletonSheetVolume;
			}
		}

		StandardGraph::StandardGraph(int nodeCount){
			skeletonVolume = NULL;
			skeletonSheetVolume = NULL;
			this->nodeCount = nodeCount;
			for(int i = 0; i < nodeCount; i++) {
				for(int j = 0; j < nodeCount; j++) {
					adjacencyMatrix[i][j][0] = 3;
					adjacencyMatrix[i][j][1] = MAXINT;
				}
			}
			this->pdbStructures.clear();
		}

		StandardGraph::StandardGraph(char* fname) {
			skeletonVolume = NULL;
			skeletonSheetVolume = NULL;
			FILE* fin = fopen(fname, "rt");
			if (fin == NULL)
			{
				printf("Error reading input file %s.\n", fname) ;
				exit(0) ;
			}

			fscanf(fin, "%d\n", &nodeCount);
			for(int i = 0; i < nodeCount; i++) {
				for(int j = 0; j < nodeCount; j++) {
					fscanf(fin, "%lf", &(adjacencyMatrix[i][j][1]));
				}
				fscanf(fin, "\n");
			}
			fclose( fin ) ;
			this->pdbStructures.clear();
		}		

		bool StandardGraph::EdgeExists(int n, int m) {
			return (fabs(adjacencyMatrix[n][m][1] - MAXINT) > 0.01);
		}

		int StandardGraph::GetType(int i, int j) {
			return (int)adjacencyMatrix[i-1][j-1][0];
		}

		double StandardGraph::GetCost(int i, int j) {
			if(adjacencyMatrix[i-1][j-1][1] == MAXINT) {
				return 1000;
			} else {
				return adjacencyMatrix[i-1][j-1][1];
			}
		}
		int StandardGraph::GetNodeCount() {
			return nodeCount;
		}

		void StandardGraph::SetType(int i, int j, int type) {
			assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
			adjacencyMatrix[i-1][j-1][0] = type;
		}

		void StandardGraph::SetCost(int i, int j, double cost) {
			assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
			adjacencyMatrix[i-1][j-1][1] = cost;
		}

		void StandardGraph::SetNodeCount(int nodeCount) {
			this->nodeCount = nodeCount;
		}


		void StandardGraph::PrintGraph() {
			char temp;
			for(int i = 0; i < (int)pdbStructures.size(); i++) {
				if(pdbStructures[i]->secondaryStructureType == GRAPHEDGE_HELIX) {
					printf("\tHelix %d \t\t( %d, %d )\t Length: %f \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->serialNumber, i*2+1, i*2+2, pdbStructures[i]->GetLengthAngstroms(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
				} else {
					printf("\tSheet Strand %s-%d \t( %d, %d )\t Length: %f \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->secondaryStructureID, pdbStructures[i]->serialNumber, i*2+1, i*2+2, pdbStructures[i]->GetLengthAngstroms(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
				}
			}

			for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
				if(skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					printf("\tHelix #%d \t( %d, %d )\t Length: %f\n", i+1, i*2+1, i*2+2, skeletonHelixes[i]->length);
				} else if(skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					printf("\tSheet #%d \t( %d, %d )\t Length: %f\n", i+1, i*2+1, i*2+2, skeletonHelixes[i]->length);
				}
			}

			printf("\t");
			for(int i = 0; i < nodeCount; i++) {
				printf("      %d    \t|", i+1);
			}
			printf("\n");
			double used = 0.0;
			double euclideanUsed = 0.0;


			for(int i = 0; i < nodeCount; i++) {
				printf("   %d\t", i+1);
				for(int j = 0; j < nodeCount; j++) {
					if(adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX) {
						temp = 'H';
						used++;
					} else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP) {
						temp = 'L';
						used++;
					} else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_SHEET) {
						temp = 'S';
						used++;
					} else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP_EUCLIDEAN) {
						temp = 'E';
						euclideanUsed++;

					} else {
						temp = ' ';
					}

					if(adjacencyMatrix[i][j][1] == MAXINT) {
						printf(" %c         \t|", temp);
					} else {
						//printf(" %c %d\t|", temp, (int)(adjacencyMatrix[i][j][1] + 0.5));
						printf(" %c %f\t|", temp, adjacencyMatrix[i][j][1]);
					}
				}
				printf("\n");
			}
			printf("Graph Density %f%%,   (With Euclidean edges %f%%)\n", used * 100 / (nodeCount * nodeCount), (used + euclideanUsed) * 100 / (nodeCount * nodeCount));
		}

		vector<Matcher2Helix> StandardGraph::GetHelixLengths() {
			vector<Matcher2Helix> helixes;
			helixes.clear();
			for(int i = 0 ; i < (int)pdbStructures.size(); i++) {
				helixes.push_back(Matcher2Helix(pdbStructures[i]->GetLengthAngstroms(), i*2, i*2+1));
			}
			for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
				helixes.push_back(Matcher2Helix(skeletonHelixes[i]->length, i*2, i*2+1));
			}

			return helixes;
		}

		void StandardGraph::GenerateEuclidianMatrix(Volume * vol) {
			double xSpacing = vol->getSpacingX();
			double ySpacing = vol->getSpacingY();
			double zSpacing = vol->getSpacingZ();
			GeometricShape * helix1;
			GeometricShape * helix2; 
			for(int i = 0; i < nodeCount; i++) {
				helix1 = skeletonHelixes[i/2];
				Point3Int loc1 = helix1->GetCornerCell(1);
				if(helix1->geometricShapeType == GRAPHEDGE_HELIX) {
					loc1 = helix1->GetCornerCell(i%2 + 1);
				}

				for(int j = 0; j < nodeCount; j++) {
					helix2 = skeletonHelixes[j/2];
					Point3Int loc2 = helix2->GetCornerCell(j%2 + 1);				
					euclideanMatrix[i][j] = sqrt(pow(xSpacing * (double)(loc1.x - loc2.x), 2) + pow(ySpacing *(double)(loc1.y - loc2.y), 2) + pow(zSpacing *(double)(loc1.z - loc2.z), 2));
					if((adjacencyMatrix[i][j][1] == MAXINT) && (euclideanMatrix[i][j] <= EUCLIDEAN_DISTANCE_THRESHOLD))
					{
						adjacencyMatrix[i][j][1] = euclideanMatrix[i][j];
						adjacencyMatrix[i][j][0] = GRAPHEDGE_LOOP_EUCLIDEAN;
					}
					//printf("%f \t", euclideanMatrix[i][j]);
				}
				//printf("\n");
			}
		}
	}
}
#endif

