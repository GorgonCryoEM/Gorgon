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
//   Revision 1.13.2.14  2009/10/08 21:49:51  schuhs
//   Storing paths in both directions
//
//   Revision 1.13.2.13  2009/10/08 19:13:59  schuhs
//   Removing data structures that are unnecessary and that require a lot of memory. Adding method to merge nearby sheets.
//
//   Revision 1.13.2.12  2009/10/01 22:13:25  schuhs
//   Storing individual sheets as meshes rather than volumes.
//
//   Revision 1.13.2.11  2009/09/17 17:13:16  schuhs
//   After creating graph, print correct node numbers for sheets and helices
//
//   Revision 1.13.2.10  2009/09/08 14:58:16  schuhs
//   When SMIPAPER_MODE flag is set, removing scaling from path distance algorithm. Adding code to print node weights whenever the adjacency matrix is printed.
//
//   Revision 1.13.2.9  2009/07/23 15:10:49  schuhs
//   Fixing bug that assigned longer-than-necessary Euclidian loops to sheets
//
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
#include <ProteinMorph/NonManifoldMesh.h>
using namespace std;
using namespace wustl_mm::Protein_Morph;

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
			void MergeSheets(double maxDist); // Merge all sheets separated by maxDist or less
		public:
			// indexed by graph nodes:
			double adjacencyMatrix[MAX_NODES][MAX_NODES][2]; // 0th dimension edge type... 1st dimension distance
			double nodeWeights[MAX_NODES];
			double euclideanMatrix[MAX_NODES][MAX_NODES];
			vector<Vector3DInt> paths[MAX_NODES][MAX_NODES];
			// sequence graph only. indexed by structure number along the sequence.
			vector<SecondaryStructure*> pdbStructures;
			// no indexing
			Volume * skeletonVolume;
			Volume * skeletonSheetVolume;
			// skeleton graph only. indexed by structure; helices first, then sheets.
			vector<GeometricShape*> skeletonHelixes;
			// skeleton graph only. indexed by structure. sheets only.
			//vector<Volume*> skeletonSheets;
			//vector<NonManifoldMesh_NoTags*> skeletonSheetMeshes;
			// skeleton graph only. indexing: skeletonSheetCorrespondence[i] = j. i is 
			//int skeletonSheetCorrespondence[MAX_NODES];
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

			/*
			cout << "~StandardGraph starting to delete " << (int)skeletonSheets.size() << " sheets "  << endl;
			for(i = 1; i < (int)skeletonSheets.size(); i++) { // start at index 1 since 0 is same as skeletonSheetVolume
				cout << "~StandardGraph deleting sheet " << i << endl;
				delete skeletonSheets[i];
			}
			cout << "~StandardGraph done deleting sheets" << endl;
			skeletonSheets.clear();
			*/

			/*
			cout << "~StandardGraph starting to delete " << (int)skeletonSheetMeshes.size() << " sheet meshes"  << endl;
			for(i = 0; i < (int)skeletonSheetMeshes.size(); i++) { 
				cout << "~StandardGraph deleting sheet " << i << endl;
				delete skeletonSheetMeshes[i];
			}
			cout << "~StandardGraph done deleting sheet meshes" << endl;
			skeletonSheetMeshes.clear();
			*/

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
			int seqNode = 1;
			for(int i = 0; i < (int)pdbStructures.size(); i++) {
				if(pdbStructures[i]->secondaryStructureType == GRAPHEDGE_HELIX) {
					printf("\tHelix %d \t\t(%2d,%2d)\t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->serialNumber, seqNode, seqNode+1, pdbStructures[i]->GetLengthResidues(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					//printf("\tHelix %d \t\t(%2d,%2d)\t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->serialNumber, seqNode, seqNode+1, pdbStructures[i]->GetLengthBonds(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					//printf("\tHelix %d \t\t(%2d,%2d)\t Length: %f \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->serialNumber, seqNode, seqNode+1, pdbStructures[i]->GetLengthAngstroms(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					seqNode += 2;
				} else {
					printf("\tSheet Strand %s-%d \t(%2d)   \t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->secondaryStructureID, pdbStructures[i]->serialNumber, seqNode, pdbStructures[i]->GetLengthResidues(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					//printf("\tSheet Strand %s-%d \t(%2d)   \t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->secondaryStructureID, pdbStructures[i]->serialNumber, seqNode, pdbStructures[i]->GetLengthBonds(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					//printf("\tSheet Strand %s-%d \t(%2d)   \t Length: %f \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->secondaryStructureID, pdbStructures[i]->serialNumber, seqNode, pdbStructures[i]->GetLengthAngstroms(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
					seqNode += 1;
				}
			}

			int skelNode = 1;
			for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
				if(skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					printf("\tHelix #%d \t(%2d,%2d)\t Length: %f\n", i+1, skelNode, skelNode+1, skeletonHelixes[i]->length);
					skelNode += 2;
				} else if(skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					printf("\tSheet #%d \t(%2d)   \t Length: %f\n", i+1, skelNode, skeletonHelixes[i]->length);
					skelNode += 1;
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
						/*
						if (adjacencyMatrix[i][j][0] == -1) {
							temp = '-';
						} else if (adjacencyMatrix[i][j][0] == 0) {
							temp = '0';
						} else if (adjacencyMatrix[i][j][0] == 1) {
							temp = '1';
						} else if (adjacencyMatrix[i][j][0] == 2) {
							temp = '2';
						} else if (adjacencyMatrix[i][j][0] == 3) {
							temp = '3';
						} else if (adjacencyMatrix[i][j][0] == 4) {
							temp = '4';
						} else if (adjacencyMatrix[i][j][0] == 5) {
							temp = '5';
						} else if (adjacencyMatrix[i][j][0] == 6) {
							temp = '6';
						} else if (adjacencyMatrix[i][j][0] == 7) {
							temp = '7';
						}
						*/
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
			
			// print out the euclidean matrix
			printf("\n  Euclidean distances:\n");
			for(int i = 0; i < nodeCount; i++) {
				printf("   %d\t", i+1);
				for(int j = 0; j < nodeCount; j++) {
					temp = 'E';
					if(euclideanMatrix[i][j] == MAXINT) {
						printf(" %c         \t|", temp);
					} else {
						printf(" %c %f\t|", temp, euclideanMatrix[i][j]);
					}
				}
				printf("\n");
			}



			// print out the node weights
			bool hasWeightedNodes = false;
			for(int i = 0; i < nodeCount; i++) {
				hasWeightedNodes = hasWeightedNodes || (nodeWeights[i] > 0);
			}
			if(hasWeightedNodes) {
				printf("\n  Node weights:\n");
				for(int i = 0; i < nodeCount; i++) {
					printf("   %d\t", i+1);
					if(adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
						temp = 'S';
						used++;
					} else {
						temp = ' ';
					}
					if(nodeWeights[i] == MAXINT) {
						printf(" %c         \t|", temp);
					} else {
						printf(" %c %f\t|", temp, nodeWeights[i]);
					}
					printf("\n");
				}
			}
			// 
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
							if (SMIPAPER_MODE == 1) {
								dist = sqrt(pow( (double)(iLoc.x - jLoc.x), 2) + pow( (double)(iLoc.y - jLoc.y), 2) + pow( (double)(iLoc.z - jLoc.z), 2));
							} else {
								dist = sqrt(pow(xSpacing * (double)(iLoc.x - jLoc.x), 2) + pow(ySpacing *(double)(iLoc.y - jLoc.y), 2) + pow(zSpacing *(double)(iLoc.z - jLoc.z), 2));
							}
							//dist = sqrt(pow(xSpacing * (double)(iLoc.x - jLoc.x), 2) + pow(ySpacing *(double)(iLoc.y - jLoc.y), 2) + pow(zSpacing *(double)(iLoc.z - jLoc.z), 2));

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
						paths[j][i].push_back(jVec);
						paths[j][i].push_back(iVec);
						// add reverse direction path
						paths[i][j].push_back(iVec);
						paths[i][j].push_back(jVec);
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

		// Merge all sheets separated by maxDist or less
		void StandardGraph::MergeSheets(double maxDist) { 
			cout << "=== graph before merging sheets ===" << endl;
			PrintGraph();
			cout << "=== ===" << endl;
			int firstSheet = GetHelixCount();
			int numSheets = GetSheetCount();
			cout << "beginning merge with " << firstSheet << " helices and " << numSheets << " sheets. maxDist = " << maxDist << ". size of helixes vector is " << skeletonHelixes.size() << endl;
			bool repeat = true;
			while (repeat) {
				repeat = false;
				for (int i = firstSheet; i < skeletonHelixes.size(); i++) {
					for (int j = skeletonHelixes.size()-1; j > i; j--) {
						int nodei = i + firstSheet;
						int nodej = j + firstSheet;
						cout << "starting iteration with " << numSheets << " sheets." << endl;
						cout << "dist between sheets " << i << " and " << j << " is " << adjacencyMatrix[nodei][nodej][1] << endl;
						if ( (adjacencyMatrix[nodei][nodej][1] < maxDist) || (skeletonHelixes[i] == skeletonHelixes[j]) ) {
							cout << "merging sheet " << j << " (node " << nodej << ") into sheet " << i << " (node " << nodei << ")" << endl;

							// modify skeletonHelixes vector
							if (skeletonHelixes[i] != skeletonHelixes[j]) {
								//cout << "not the same sheet structure. merging two structures." << endl;
								// internal cells
								//cout << "before: sheet " << i << " has " << skeletonHelixes[i]->internalCells.size() << " internal cells." << endl;
								//cout << "before: sheet " << j << " has " << skeletonHelixes[j]->internalCells.size() << " internal cells." << endl;
								int internalCellCount = (int)skeletonHelixes[j]->internalCells.size();
								for (int k = 0; k < internalCellCount; k++){
									//skeletonHelixes[i]->AddInternalCell(skeletonHelixes[j]->internalCells[k]);
									skeletonHelixes[i]->internalCells.push_back(skeletonHelixes[j]->internalCells[k]);
								}
								//cout << "after: sheet " << i << " has " << skeletonHelixes[i]->internalCells.size() << " internal cells." << endl;
								
								// cornerCells
								//cout << "before: sheet " << i << " has " << skeletonHelixes[i]->cornerCells.size() << " corner cells." << endl;
								//cout << "before: sheet " << j << " has " << skeletonHelixes[j]->cornerCells.size() << " corner cells." << endl;
								for (int k = 0; k < skeletonHelixes[j]->cornerCells.size(); k++){
									skeletonHelixes[i]->cornerCells.push_back(skeletonHelixes[j]->cornerCells[k]);
								}
								//cout << "after: sheet " << i << " has " << skeletonHelixes[i]->cornerCells.size() << " corner cells." << endl;
								
								// polygonPoints
								//cout << "before: sheet " << i << " has " << skeletonHelixes[i]->polygonPoints.size() << " polygon points." << endl;
								//cout << "before: sheet " << j << " has " << skeletonHelixes[j]->polygonPoints.size() << " polygon points." << endl;
								for (int k = 0; k < skeletonHelixes[j]->polygonPoints.size(); k++){
									skeletonHelixes[i]->polygonPoints.push_back(skeletonHelixes[j]->polygonPoints[k]);
								}
								//cout << "after: sheet " << i << " has " << skeletonHelixes[i]->polygonPoints.size() << " polygon points." << endl;
								
								// polygons
								//cout << "before: sheet " << i << " has " << skeletonHelixes[i]->polygons.size() << " polygons." << endl;
								//cout << "before: sheet " << j << " has " << skeletonHelixes[j]->polygons.size() << " polygons." << endl;
								for (int k = 0; k < skeletonHelixes[j]->polygons.size(); k++){
									skeletonHelixes[i]->polygons.push_back(skeletonHelixes[j]->polygons[k]);
								}
								//cout << "after: sheet " << i << " has " << skeletonHelixes[i]->polygons.size() << " polygons." << endl;

								// length
								//cout << "before: sheet " << i << " has length " << skeletonHelixes[i]->length << endl;
								//cout << "before: sheet " << j << " has length " << skeletonHelixes[j]->length << endl;
								skeletonHelixes[i]->length += skeletonHelixes[j]->length;
								//cout << "after: sheet " << i << " has length " << skeletonHelixes[i]->length << endl;

								// cleanup
								//cout << "deleting element " << j << " of skeletonHelixes" << endl;
								delete skeletonHelixes[j];
							} else {
								//cout << "both point to exactly the same sheet structure. not modifying that structure." << endl;
							}

							cout << "before cleanup, skeletonHelixes has size " << skeletonHelixes.size() << endl;
							//for (int k = j; k < numSheets-1; k++) {
							//cout << "j=" << j << ", firstSheet+numSheets=" << firstSheet + numSheets << endl;
							for (int k = j; k < firstSheet + numSheets - 1; k++) {
								skeletonHelixes[k] = skeletonHelixes[k+1];
								//cout << "moving skeletonHelixes[" << k+1 << "] to [" << k << "]" << endl;
							}
							skeletonHelixes.pop_back();
							cout << "after cleanup, skeletonHelixes has size " << skeletonHelixes.size() << endl;


							// adjacency matrix

							// merge rows and columns i and j:
							int lastRowColumn = firstSheet * 2 + numSheets - 1;

							//cout << "merging rows " << nodei << " and " << nodej << endl;
							for (int k = 0; k <= lastRowColumn; k++){
								int itype = adjacencyMatrix[k][nodei][0];
								int jtype = adjacencyMatrix[k][nodej][0];
								int ilength = adjacencyMatrix[k][nodei][1];
								int jlength = adjacencyMatrix[k][nodej][1];
								// i same type and j shorter than i, overwrite i
								if ( (itype == jtype) && (jlength < ilength) ) {
									adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
									adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
									// modify paths vectors
									paths[k][nodei] = paths[k][nodej];
									paths[nodei][k] = paths[nodej][k];
									//paths[nodei][k] = paths[k][nodej];
								}
								// if no edge at i and loop or euclidean loop at j, overwrite i
								if ( (itype == GRAPHEDGE_OTHER) && ( ( jtype == GRAPHEDGE_LOOP) || (jtype == GRAPHEDGE_LOOP_EUCLIDEAN) ) ) {
									adjacencyMatrix[k][nodei][0] = adjacencyMatrix[k][nodej][0];
									adjacencyMatrix[nodei][k][0] = adjacencyMatrix[k][nodej][0];
									adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
									adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
									paths[k][nodei] = paths[k][nodej];
									paths[nodei][k] = paths[nodej][k];
								}
								// if euclidean loop at i and loop at j, overwrite i
								if ( (itype == GRAPHEDGE_LOOP_EUCLIDEAN) && (jtype == GRAPHEDGE_LOOP) ) {
									adjacencyMatrix[k][nodei][0] = adjacencyMatrix[k][nodej][0];
									adjacencyMatrix[nodei][k][0] = adjacencyMatrix[k][nodej][0];
									adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
									adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
									paths[k][nodei] = paths[k][nodej];
									// TODO fix path indexing: problem with the order
									paths[nodei][k] = paths[nodej][k];
								}

							}

							// merge rows in Euclidean matrix
							//cout << "Euclidean matrix: merging rows " << nodei << " and " << nodej << endl;
							for (int k = 0; k <= lastRowColumn; k++){
								int ilength = euclideanMatrix[k][nodei];
								int jlength = euclideanMatrix[k][nodej];
								// i same type and j shorter than i, overwrite i
								if (jlength < ilength) {
									euclideanMatrix[k][nodei] = euclideanMatrix[k][nodej];
									euclideanMatrix[nodei][k] = euclideanMatrix[k][nodej];
								}
							}


							// shift all rows > j up, then shift all columns > nodej to the left
							//cout << "shifting rows > " << nodej << " up" << endl;
							for (int k = nodej; k <= lastRowColumn; k++) {
								//cout << "column k=" << k << endl;
								for (int m = 0; m <= lastRowColumn; m++) {
									adjacencyMatrix[k][m][0] = adjacencyMatrix[k+1][m][0];
									adjacencyMatrix[k][m][1] = adjacencyMatrix[k+1][m][1];
									adjacencyMatrix[k][m][2] = adjacencyMatrix[k+1][m][2];
									euclideanMatrix[k][m] = euclideanMatrix[k+1][m];
									paths[k][m] = paths[k+1][m];
									//cout << "replacing adjacencyMatrix[" << k << "][" << m << "][1] with ["  << k+1 << "][" << m << "][1]." << endl;
								}
							}
							
							//cout << "shifting columns > " << nodej << " to the left" << endl;
							for (int k = nodej; k <= lastRowColumn; k++) {
								//cout << "row k=" << k << endl;
								for (int m = 0; m < firstSheet*2 + numSheets + 1; m++) {
									adjacencyMatrix[m][k][0] = adjacencyMatrix[m][k+1][0];
									adjacencyMatrix[m][k][1] = adjacencyMatrix[m][k+1][1];
									adjacencyMatrix[m][k][2] = adjacencyMatrix[m][k+1][2];
									euclideanMatrix[m][k] = euclideanMatrix[m][k+1];
									paths[m][k] = paths[m][k+1];
									//cout << "replacing adjacencyMatrix[" << m << "][" << k << "][1] with ["  << m << "][" << k+1 << "][1]." << endl;
								}
							}
							//cout << "removing last row (" << lastRowColumn << ")" << endl;
							for (int m = 0; m <= lastRowColumn; m++) {
								adjacencyMatrix[lastRowColumn][m][0] = 3;
								adjacencyMatrix[lastRowColumn][m][1] = MAXINT;
								adjacencyMatrix[lastRowColumn][m][2] = 0;
								euclideanMatrix[lastRowColumn][m] = 0;
								//paths[lastRowColumn][m] = ? ; how to clear a path?
							}
							//cout << "removing last column (" << lastRowColumn << ")" << endl;
							for (int m = 0; m <= lastRowColumn; m++) {
								adjacencyMatrix[m][lastRowColumn][0] = 3;
								adjacencyMatrix[m][lastRowColumn][1] = MAXINT;
								adjacencyMatrix[m][lastRowColumn][2] = 0;
								euclideanMatrix[m][lastRowColumn] = 0;
								//paths[m][lastRowColumn] = ?; how to clear a path?
							}

							// modify nodeWeights vector
							nodeWeights[nodei] = nodeWeights[nodei] + nodeWeights[nodej];
							for (int k = nodej; k <= lastRowColumn; k++) {
								nodeWeights[k] = nodeWeights[k+1];
							}
							nodeWeights[lastRowColumn] = 0;

							
							numSheets--; // local to this loop
							nodeCount--; // for the class
						}
					}
				}
			}
			cout << "=== graph after merging sheets ===" << endl;
			PrintGraph();
			cout << "=== ===" << endl;

		}
	}
}
#endif

