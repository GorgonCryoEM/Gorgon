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
//   Revision 1.13.2.8  2009/07/21 14:55:46  schuhs
//   Adding paths for visualization when the Euclidian matrix is built
//
//   Revision 1.13.2.7  2009/07/14 19:51:43  schuhs
//   Adding methods to count the number of helices and sheets in the graph
//
//   Revision 1.13.2.6  2009/07/03 16:32:22  schuhs
//   Adding an array to storage node costs
//
//   Revision 1.13.2.5  2009/06/02 17:33:29  schuhs
//   Changing GenerateEuclidianMatrix method to work for sheet nodes and helix nodes
//
//   Revision 1.13.2.4  2009/05/28 17:22:50  schuhs
//   Fixing bug in destructor that caused the same volume object to be deleted twice
//
//   Revision 1.13.2.3  2009/05/22 19:29:00  schuhs
//   Adding array to store correspondences between SSEBuilder sheets and sheets in graph
//
//   Revision 1.13.2.2  2009/05/20 15:44:46  schuhs
//   Adding vector of volumes to store the individual skeleton sheets
//
//   Revision 1.13.2.1  2009/05/20 14:51:40  schuhs
//   Adding extra volume containing indexed skeleton sheets
//
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
			int GetHelixCount(); // Returns the number of helices
			int GetSheetCount(); // Returns the number of sheets
			void SetType(int i, int j, int type); // The type of the edge (index starting from 1)
			void SetCost(int i, int j, double cost); // The cost based on the graph labels (index starting from 1)
			void SetCost(int i, double cost); // The cost of a node
			void SetNodeCount(int nodeCount); // Sets the number of nodes
			void PrintGraph();
			void GenerateEuclidianMatrix(Volume * vol);
			vector<Matcher2Helix> GetHelixLengths();
			bool EdgeExists(int n, int m);
		public:
			double adjacencyMatrix[MAX_NODES][MAX_NODES][2]; // 0th dimension edge type... 1st dimension distance
			double nodeWeights[MAX_NODES];
			double euclideanMatrix[MAX_NODES][MAX_NODES];
			vector<Vector3DInt> paths[MAX_NODES][MAX_NODES];
			vector<SecondaryStructure*> pdbStructures;
			vector<GeometricShape*> skeletonHelixes;
			Volume * skeletonVolume;
			vector<Volume*> skeletonSheets;
			int skeletonSheetCorrespondence[MAX_NODES];
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

			for(i = 1; i < (int)skeletonSheets.size(); i++) { // start at index 1 since 0 is same as skeletonSheetVolume
				delete skeletonSheets[i];
			}

			skeletonSheets.clear();

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

		int StandardGraph::GetHelixCount() {
			int count = 0;
			for (int i = 0; i < nodeCount; i++) {
				if (adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
					count++;
				}
			}
			return (count/2);
			//return helixCount;
		}

		int StandardGraph::GetSheetCount() {
			int count = 0;
			for (int i = 0; i < nodeCount; i++) {
				if (adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
					count++;
				}
			}
			return count;
			//return sheetCount;
		}

		void StandardGraph::SetType(int i, int j, int type) {
			assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
			adjacencyMatrix[i-1][j-1][0] = type;
		}

		void StandardGraph::SetCost(int i, int j, double cost) {
			assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
			adjacencyMatrix[i-1][j-1][1] = cost;
		}

		void StandardGraph::SetCost(int i, double cost) {
			assert((i >= 1) && (i <= nodeCount));
			nodeWeights[i-1] = cost;
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

		// measures Euclidian distance between all pairs of nodes, stores the distances in euclidianMatrix, and adds
		// graph edges (stored in adjacencyMatrix) where the Euclidian distance is below EUCLIDEAN_DISTANCE_THRESHOLD
		void StandardGraph::GenerateEuclidianMatrix(Volume * vol) {
			double xSpacing = vol->getSpacingX();
			double ySpacing = vol->getSpacingY();
			double zSpacing = vol->getSpacingZ();
			GeometricShape * iSse;
			GeometricShape * jSse; 
			Point3Int iLoc(0,0,0,0);
			Point3Int iBestLoc(0,0,0,0);
			int iCorners;
			Point3Int jLoc(0,0,0,0);
			Point3Int jBestLoc(0,0,0,0);
			int jCorners;

			// count number of helices
			int numH = 0;
			for (unsigned int i = 0; i < (int)skeletonHelixes.size(); i++) {
				if(skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					numH++;
				}
			}

			// for each graph node i
			for(int i = 0; i < nodeCount; i++) {
				// if i is a helix node
				if (i < 2 * numH) {
					iSse = skeletonHelixes[i/2];
					iCorners = 1;
				} else { // if i is a sheet node
					iSse = skeletonHelixes[i - numH];
					iCorners = iSse->cornerCells.size();
				}

				// for every other graph node j
				for(int j = 0; j < nodeCount; j++) {
					// if j is a helix node
					if (j < 2 * numH) {
						jSse = skeletonHelixes[j/2];
						//jCorners = 1;
						jCorners = skeletonHelixes[j/2]->cornerCells.size();
					} else { // if j is a sheet node
						jSse = skeletonHelixes[j - numH];
						jCorners = jSse->cornerCells.size();
					}

					double minDist = MAXDOUBLE;
					double dist;

					// for every corner s of node i
					for (int s = 1; s <= iCorners; s++) {
						if (i < 2 * numH) {		// i is a helix node
							// TODO: looks like the following line just returns a single corner for the helix
							iLoc = iSse->GetCornerCell(i%2 + 1);
							//iLoc = iSse->GetCornerCell(s);
						} else {				 // i is a sheet node
							//iLoc = iSse->GetCornerCell(s);
							iLoc = iSse->cornerCells[s-1];
						}

						// for every corner t of node j
						for (int t = 1; t <= jCorners; t++) {
							if (j < 2 * numH) {		// j is a helix node
								// TODO: looks like the following line just returns a single corner for the helix
								jLoc = jSse->GetCornerCell(j%2 + 1);
							} else {				// j is a sheet node 
								//jLoc = jSse->GetCornerCell(t);
								jLoc = jSse->cornerCells[t-1];
							}
							
							// measure distance between corner s of node i and corner t of node j
							dist = sqrt(pow(xSpacing * (double)(iLoc.x - jLoc.x), 2) + pow(ySpacing *(double)(iLoc.y - jLoc.y), 2) + pow(zSpacing *(double)(iLoc.z - jLoc.z), 2));
							// if this distance is closer than the smallest found previously, save it
							if (dist < minDist) {
								minDist = dist;
								iBestLoc = iLoc;
								jBestLoc = jLoc;
							}
							//cout << "distance. i=" << i << ",j=" << j << ",s=" << s << ",t=" << t << ", dist= " << dist << ", minDist=" << minDist << endl;
						}
					}
					// store the distance between the points corresponding to nodes i and j in euclidianMatrix
					euclideanMatrix[i][j] = minDist;

					// if the distance is less than EUCLIDEAN_DISTANCE_THRESHOLD, store this in the graph as a Euclidian loop edge
					if((adjacencyMatrix[i][j][1] == MAXINT) && (euclideanMatrix[i][j] <= EUCLIDEAN_DISTANCE_THRESHOLD))
					{
						adjacencyMatrix[i][j][1] = euclideanMatrix[i][j];
						adjacencyMatrix[i][j][0] = GRAPHEDGE_LOOP_EUCLIDEAN;
						adjacencyMatrix[j][i][1] = euclideanMatrix[i][j];
						adjacencyMatrix[j][i][0] = GRAPHEDGE_LOOP_EUCLIDEAN;

						Vector3DInt iVec = Vector3DInt(iBestLoc.x, iBestLoc.y, iBestLoc.z);
						Vector3DInt jVec = Vector3DInt(jBestLoc.x, jBestLoc.y, jBestLoc.z);
						//Vector3DInt iVec = Vector3DInt(iLoc.x, iLoc.y, iLoc.z);
						//Vector3DInt jVec = Vector3DInt(jLoc.x, jLoc.y, jLoc.z);
						paths[i][j].clear();
						paths[j][i].clear();
						paths[i][j].push_back(jVec);
						paths[i][j].push_back(iVec);
						//paths[j][i].push_back(iVec);
						//paths[j][i].push_back(jVec);
						//cout << "after adding Euclidian path from " << i << " to " << j << ", paths vector has size " << paths[i][j].size() << endl;
						//cout << "after adding Euclidian path from " << j << " to " << j << ", paths vector has size " << paths[j][i].size() << endl;
					}
					//printf("%f \t", euclideanMatrix[i][j]);
				}
				//printf("\n");
			}
		}
	}
}
#endif

