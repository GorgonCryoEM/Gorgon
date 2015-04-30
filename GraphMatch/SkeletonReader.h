// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads skeleton files and creates graphs from them.



#ifndef SKELETONREADER_H
#define SKELETONREADER_H

#include <SkeletonMaker/reader.h>
#include <SkeletonMaker/volume.h>
#include <MathTools/BasicDefines.h>
#include "GeometricShape.h"
#include <vector>
#include <queue>
#include <list>
#include "GlobalConstants.h"

using namespace std;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraphMatch {


		class SkeletonReader {
		public:
			static int GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, int cornerNum);
			static int GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, Point3Int * point);
			static StandardGraph * ReadFile(char * volumeFile, char * helixFile, char * sseFile, char * sheetFile);
			static Volume* getSheetsNoThreshold( Volume * vol, int minSize );
			static void ReadSheetFile(char * sheetFile, vector<GeometricShape*> & helixes);
			static void ReadHelixFile(char * helixFile, char * sseFile, vector<GeometricShape*> & helixes);
			static void FindSizes(int startHelix, int startCell, vector<GeometricShape*> & helixList, Volume * vol, Volume * coloredVol, StandardGraph * graph);
			static void FindPaths(StandardGraph * graph);
			static void FindPath(int startIx, int endIx, vector<vector<Vector3DInt> > nodes, Volume * maskVol, StandardGraph * graph, bool eraseMask);
			static void FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<GeometricShape*> & helixes, int sheetId);
			static int isSkeletonSheet(Volume * vol, int ox, int oy, int oz );

		};

		// returns the graph node index for corner cornerNum of helix/sheet helixNum
		int SkeletonReader::GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, int cornerNum) {
			int numH = 0;
			for (unsigned int i = 0; i < (int)helixes.size(); i++) {
				if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					numH++;
				}
			}
			if (helixNum < numH) {
				// if this is a helix, index is 2*helixNum + cornerNum
				return helixNum*2 + helixes[helixNum]->cornerCells[cornerNum].node;
			} else {
				// if this is a sheet, index is # helices + helixNum
				return helixNum + numH + 1;
			}
		}

		// returns the graph node index for the corner of helix/sheet helixNum nearest to point point.
		int SkeletonReader::GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, Point3Int * point) {
			int node = 1;
			double minDistance = MAXINT;
			double dist;
			// find nearest corner to the input point. node = 1 or 2 for a helix.
			for(int i = 0; i < (int)helixes[helixNum]->cornerCells.size(); i++) {
				dist = Point3Int::EuclideanDistance(helixes[helixNum]->cornerCells[i], *point);
				if(dist < minDistance) {
					node = helixes[helixNum]->cornerCells[i].node;
					minDistance = dist;
				}
			}

			// count the helices
			int numH = 0;
			for (unsigned int i = 0; i < (int)helixes.size(); i++) {
				if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					numH++;
				}
			}

			// if the point is near the corner, return the corner number.
			if(minDistance <= BORDER_MARGIN_THRESHOLD) {
				if (helixNum < numH) {
					return helixNum*2 + node;
				} else {
					return helixNum + numH + 1;
				}
			} else {
				return -1;
			}
		}

		StandardGraph * SkeletonReader::ReadFile(char * volumeFile, char * helixFile, char * sseFile, char * sheetFile) {

			// Read the volume file and load volume data structure
			Volume * vol = (MRCReaderPicker::pick(volumeFile))->getVolume();
			//vol->toMathematicaFile("myVolume.nb");
#ifdef VERBOSE
			printf("Constructing 'paintedVol'...\n");
#endif
				
			Volume * paintedVol = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ());

#ifdef VERBOSE
			printf("Finished reading volume file, now moving on to helixes...\n");
#endif

			// Read the helix file
			vector<GeometricShape*> helixes;
			helixes.clear();
			ReadHelixFile(helixFile, sseFile, helixes);



#ifdef INCLUDE_SHEETS

#ifdef VERBOSE
			printf("Finished reading helix file, now moving on to sheets...\n");
#endif // VERBOSE

			vector<GeometricShape*> sheets;

			//ReadSheetFile(sheetFile, helixes);
			ReadSheetFile(sheetFile, sheets);

#ifdef VERBOSE
			printf("Finished reading sheet file.\n");
#endif // VERBOSE

#endif // INCLUDE_SHEETS
				
			Point3 point, pointScaled;



			// Finding all points inside of the helixes.
			for(int x = 0; x < vol->getSizeX(); x++) {
				point[0] = vol->getOriginX() + x * vol->getSpacingX();
				for(int y = 0; y < vol->getSizeY(); y++) {
					point[1] = vol->getOriginY() + y * vol->getSpacingY();
					for(int z = 0; z < vol->getSizeZ(); z++) {
						point[2] = vol->getOriginZ() + z * vol->getSpacingZ();
						// if this voxel nonzero
						if(vol->getDataAt(x, y, z) > 0) {
							// check all helices to see if it's inside one
							bool inHelix = false;
							for(int i = 0; i < (int)helixes.size(); i++) {
								// if i is a helix and if point is inside helix i
								if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX && helixes[i]->IsInsideShape(point)) {
									// store helix number for this point in the volume
									paintedVol->setDataAt(x, y, z, i+1);
									// add this point as as internal cell of the helix
									inHelix = true;
									helixes[i]->AddInternalCell(Point3Int(x, y, z, 0));
								}
							}
						}
					}
				}
			}

			Volume* sheetClusters = getSheetsNoThreshold(vol, MINIMUM_SHEET_SIZE);

			// make the offset and scale of sheetClusters volume match the vol volume
			sheetClusters->setOrigin(vol->getOriginX(), vol->getOriginY(), vol->getOriginZ() );
			sheetClusters->setSpacing(vol->getSpacingX(), vol->getSpacingY(), vol->getSpacingZ() );

			int numSkeletonSheets = (int) sheetClusters->getMax();

			//cout << "min sheet size = " << MINIMUM_SHEET_SIZE << ", num skeleton sheets = " << numSkeletonSheets << ", num SSEs = " << (int)helixes.size() + (int)sheets.size() << endl;

			vector<vector<double> > sheetDistance(numSkeletonSheets+1, vector<double> ((int)sheets.size()) );

			// for each sheet
			for (int i = 1; i <= numSkeletonSheets; i++) { 
				// for each point (x,y,z)
				int count = 0;
				for(int x = 0; x < sheetClusters->getSizeX(); x++) {
					point[0] = sheetClusters->getOriginX() + x * sheetClusters->getSpacingX();
					for(int y = 0; y < sheetClusters->getSizeY(); y++) {
						point[1] = sheetClusters->getOriginY() + y * sheetClusters->getSpacingY();
						for(int z = 0; z < sheetClusters->getSizeZ(); z++) {
							point[2] = sheetClusters->getOriginZ() + z * sheetClusters->getSpacingZ();
							// if this voxel nonzero
							if(sheetClusters->getDataAt(x, y, z) == i) {
								count++;

								// measure distance to every SSE sheet, add to running total
								for(int j = 0; j < (int)sheets.size(); j++) {
									// find plates on skeleton that are associated with sheets
									if(sheets[j]->geometricShapeType == GRAPHEDGE_SHEET) {
										sheetDistance[i][j] += sheets[j]->MinimumDistanceToPoint(point);
									}
								}
							}
						}
					}
				}
				// divide running total by number of sheet voxels to give average shortest distance
				for(int j = 0; j < (int)sheets.size(); j++) {
					sheetDistance[i][j] /= (double) count;
				}
			}

#ifdef VERBOSE
			cout << "min distance matrix: " << endl;
			for (int i = 1; i <= numSkeletonSheets; i++) { 
				cout << "skeleton sheet " << i << ": ";
				for(int j = 0; j < (int)sheets.size(); j++) {
					cout << sheetDistance[i][j] << " - ";
				}
				cout << endl;
			}
#endif // VERBOSE

			vector<int> sseSheetMapping(numSkeletonSheets+1, -1);
			vector<int> helixesMapping(numSkeletonSheets+1, -1);
			for (int i = 1; i <= numSkeletonSheets; i++) { 
				double minDist = MAXIMUM_DISTANCE_SHEET_SKELETON;
				for (int j = 0; j < (int)sheets.size(); j++) { 
					if (sheets[j]->geometricShapeType == GRAPHEDGE_SHEET && sheetDistance[i][j] < minDist) {
						minDist = sheetDistance[i][j];
						sseSheetMapping[i] = j;
					}
				}
#ifdef VERBOSE
				cout << "skeleton sheet " << i << " maps to SSEHunter sheet " << sseSheetMapping[i] << endl;
#endif // VERBOSE
			}

			// Add all sheets from sheet data structures to helixes list
			for (int i = 1; i <= numSkeletonSheets; i++) { 
				//cout << "checking sheet " << i << " which maps to " << sseSheetMapping[i] << endl;
				if (sseSheetMapping[i] != -1) {
					helixes.push_back(sheets[sseSheetMapping[i]]);
					//cout << "added sheet " << i << " as element " << helixes.size()-1 << " of helixes vector" << endl;
					helixesMapping[i] = helixes.size() - 1;
				}
			}

			// Add all points in each sheet to the helixes data structure
			// for each point (x,y,z)
			for(int x = 0; x < sheetClusters->getSizeX(); x++) {
				point[0] = sheetClusters->getOriginX() + x * sheetClusters->getSpacingX();
				for(int y = 0; y < sheetClusters->getSizeY(); y++) {
					point[1] = sheetClusters->getOriginY() + y * sheetClusters->getSpacingY();
					for(int z = 0; z < sheetClusters->getSizeZ(); z++) {
						point[2] = sheetClusters->getOriginZ() + z * sheetClusters->getSpacingZ();
						// check which sheet is associated with this voxel
						int skeletonSheetNum = (int)sheetClusters->getDataAt(x, y, z);
						// for voxels that are assigned to some sheet
						if (skeletonSheetNum > 0) {
							int sseSheetNum = helixesMapping[skeletonSheetNum];
							if (sseSheetNum != -1) {
								// associate this voxel with this sheet
								paintedVol->setDataAt(x, y, z, sseSheetNum+1);
								// add this point as as internal cell of the helix
								helixes[sseSheetNum]->AddInternalCell(Point3Int(x, y, z, 0));
							}
						}
					}
				}
			}

			// Save separate sheets in helixes data structure
			int numSheets = (int) sheetClusters->getMax();

			// vector to hold all the sheet volumes
			vector<Volume*> skeletonSheets;
			skeletonSheets.push_back(sheetClusters);

			// create an individual volume for each sheet from the sheetClusters volume
			for (int i = 1; i <= numSheets; i++) {
				Volume* singleSheet = new Volume(sheetClusters->getSizeX(), sheetClusters->getSizeY(), sheetClusters->getSizeZ(), 1.0); // trying something new
				singleSheet->applyMask(sheetClusters,i,true);
				singleSheet->threshold( 0.1, 0, 1 ) ;
				skeletonSheets.push_back(singleSheet);
			}

#ifdef VERBOSE
			printf("Finished finding points inside helices and sheets.\n");
#endif // VERBOSE

			int numH = 0;
			int numS = 0;
			for (unsigned int i = 0; i < (int)helixes.size(); i++) {
				if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					numH++;
				}
				if(helixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					numS++;
				}
			}

			StandardGraph * graph = new StandardGraph(2 * numH + numS);

			// create a graph with one node per helix end point and with edges connecting nodes that
			// are connected along the volume.
			//cout << "adding " << (int)helixes.size() << " helices and sheets to adjacency matrix" << endl;
			for(unsigned int i = 0; i < (int)helixes.size(); i++) {
				if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					// assign node numbers for helix ends
					int node1 = (i*2)+1;
					int node2 = (i*2)+2;

					// find the two corner cells in this helix
					helixes[i]->FindCornerCellsInHelix();
					//cout << "helix " << i << " has " << helixes[i]->cornerCells.size() << " corners." << endl;
					for (unsigned int j = 0; j < helixes[i]->cornerCells.size(); j++) {
						//cout << "corner " << j << " is associated with node " << helixes[i]->cornerCells[j].node << endl;
					}

					// length of this helix
					float length = helixes[i]->length;

					// populate adjacency matrix
					// no cost to go from a helix end back to itself
					graph->SetCost(node1, node1, 0);
					graph->SetType(node1, node1, GRAPHNODE_HELIX); 
					graph->SetCost(node2, node2, 0);
					graph->SetType(node2, node2, GRAPHNODE_HELIX); 
					// cost of traversing a helix is the helix length
					graph->SetCost(node1, node2, length);
					// mark this edge type as helix or sheet, determined by helixes graph
					graph->SetType(node1, node2, helixes[i]->geometricShapeType);
					// same for reverse direction
					graph->SetCost(node2, node1, length);
					graph->SetType(node2, node1, helixes[i]->geometricShapeType);
				} else if (helixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					// assign node number this sheet
					int sheetNode = numH + i + 1; // each helix takes two nodes

					// find all the corner cells in this sheet
					FindCornerCellsInSheet(vol, paintedVol, helixes, i);

					// cost is length of self-loops
					graph->SetCost(sheetNode, sheetNode, SHEET_SELF_LOOP_LENGTH); // nonzero so it shows up as edge in StandardGraph::EdgeExists
					graph->SetType(sheetNode, sheetNode, GRAPHNODE_SHEET); 
				}

			}	
			//cout << "adding sheet sizes as sheet node costs" << endl;
			for (unsigned int s = 0; s < skeletonSheets.size(); s++) {
				int sseSheetNum = helixesMapping[s];
				//cout << "node " << s << " corresponds to sheet " << helixesMapping[s] << endl;
				if (sseSheetNum != -1) {
					int sheetSize = skeletonSheets[s]->getNonZeroVoxelCount();
					int sheetNode = numH + sseSheetNum + 1; // each helix takes two nodes
					// TODO: Scale the sheet size by the geometric scale factor to make units match
					graph->SetCost(sheetNode, sheetSize); // nonzero so it shows up as edge in StandardGraph::EdgeExists
					//cout << "adding sheet " << sseSheetNum << "(s=" << s << ") with size " << sheetSize << " as node " << sheetNode << endl;
				}
			}
#ifdef VERBOSE
			for (int i = 0; i < graph->GetNodeCount(); i++) {
				cout << "cost of node " << i << " is " << graph->nodeWeights[i] << endl;
			}
#endif // VERBOSE

#ifdef VERBOSE
			printf("Finished creating connectivity graph.\n");
#endif // VERBOSE

			// populate graph->skeletonHelixes with list of helices and sheets
			for(int i = 0; i < (int)helixes.size(); i++) {
				graph->skeletonHelixes.push_back(helixes[i]);
			}

			// find the costs of all other paths along the volume, from any helix end to any other helix end
			// store the results as edges in the graph
			for(int i = 0; i < (int)helixes.size(); i++) {
				// for every entry and exit point of that helix/sheet
				for(int j = 0; j < (int)helixes[i]->cornerCells.size(); j++) {
					// find all the paths from the entry/exit point to every other helix.
					// results are stored in vol and paintedVol and as graph edges.
					FindSizes(i, j, helixes, vol, paintedVol, graph);
				}
			}
			
#ifdef VERBOSE
			printf("Finished running FindSizes2.\n");
#endif // VERBOSE

#ifdef VERBOSE
			printf("Finished creating a list of helices and sheets.\n");
#endif // VERBOSE

			// save results to graph->skeletonVolume
			graph->skeletonVolume = vol;
			delete paintedVol;

			// save skeleton sheet volume to graph->skeletonSheetVolume
			graph->skeletonSheetVolume = sheetClusters;

			// erase extra volumes
			while ((int)skeletonSheets.size() > 1) {
				delete skeletonSheets[1];
				skeletonSheets.erase(skeletonSheets.begin() + 1);
			}

#ifdef VERBOSE
			printf("Graph saved to object.\n");
#endif // VERBOSE

			// measure Euclidian distance between all pairs of nodes and add edges between those nodes that are
			// closer than EUCLIDIAN_DISTANCE_THRESHOLD
			graph->GenerateEuclidianMatrix(vol);

#ifdef VERBOSE
			printf("Euclidian matrix generated.\n");
#endif // VERBOSE
			
#ifdef VERBOSE
			printf("Merging pairs of sheets that are close to each other.\n");
#endif // VERBOSE

			graph->MergeSheets(SHEET_MERGE_THRESHOLD);

#ifdef VERBOSE
			printf("Done merging pairs of sheets.\n");
#endif // VERBOSE


			return graph;
		}

		// Returns a collection of individual sheets with minimum size minSize, created from input volume vol
		// This is a copy of getSheets from volume.h, with the final thresholding step removed.
		Volume* SkeletonReader::getSheetsNoThreshold( Volume * vol, int minSize ) {
			int i, j, k ;

			//Initialize volume
#ifdef VERBOSE
			printf("Initialize volume at %d %d %d\n",  vol->getSizeX(), vol->getSizeY(), vol->getSizeZ() ) ;
#endif // VERBOSE
			Volume* svol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ() ) ;
			
			//Initialize cluster counters
			int sheets[MAX_SHEETS] ;
			for ( i = 0 ; i < MAX_SHEETS ; i ++ )
			{
				sheets[ i ] = 0 ;
			}
			int totSheets = 1 ;

			//Start clustering
#ifdef VERBOSE
			printf("Start clustering...\n" ) ;
#endif // VERBOSE
			int ox, oy, oz ;
			for ( i = 0 ; i < vol->getSizeX() ; i ++ )
				for ( j = 0 ; j < vol->getSizeY() ; j ++ )
					for ( k = 0 ; k < vol->getSizeZ() ; k ++ )
					{
						if ( vol->getDataAt(i,j,k) <= 0 || svol->getDataAt(i,j,k) != 0 )
						{
							// Not a data point or has been visited
							continue ;
						}
						if ( ! isSkeletonSheet( vol, i, j, k ) )
						{
							// Not a sheet point
							continue ;
						}

						//Initialize queue
						int numNodes = 1 ;
						svol->setDataAt( i, j, k, totSheets ) ;
						GridQueue* queue = new GridQueue() ;
						queue->pushQueue( i, j, k ) ;
						while ( queue->popQueue(ox, oy, oz) )
						{
							// Test if neighbors satisfy sheet condition
							if ( isSkeletonSheet(vol, ox, oy, oz ) )
							{
								for ( int m = 0 ; m < 6 ; m ++ )
								{
									int nx = ox + neighbor6[m][0] ;
									int ny = oy + neighbor6[m][1] ;
									int nz = oz + neighbor6[m][2] ;

									//if ( vol->getDataAt(nx,ny,nz) > 0 && svol->getDataAt(nx,ny,nz) == 0 )
									if ( vol->getDataAt(nx,ny,nz) > 0 && svol->getDataAt(nx,ny,nz) == 0 && isSkeletonSheet(vol,nx,ny,nz) )
									{
										svol->setDataAt(nx,ny,nz,totSheets);
										queue->pushQueue(nx,ny,nz) ;
										numNodes ++ ;
									}
								}
							}
						}

						delete queue ;
						if ( numNodes > 0 )
						{
						//	printf("Sheet %d contain %d nodes.\n", totSheets, numNodes) ;
							sheets[ totSheets ] = numNodes ;
							totSheets ++ ;
						}
					}

			// Removing clusters less than minSize
#ifdef VERBOSE
			printf("Removing small clusters.\n") ;
#endif // VERBOSE
			for ( i = 0 ; i < vol->getSizeX() ; i ++ )
				for ( j = 0 ; j < vol->getSizeY() ; j ++ )
					for ( k = 0 ; k < vol->getSizeZ() ; k ++ )
					{
						int cnt = (int) svol->getDataAt(i,j,k) ;
						if ( cnt > 0 && sheets[ cnt ] < minSize )
						{
							svol->setDataAt(i,j,k,-1) ;
						}
					}
			return svol ;
		}

		// Returns a collection of individual sheets with minimum size minSize, created from input volume vol
		// This is a copy of isSheet from volume.h, with the minimum number of corner nodes changed from 3 to 1.
		int SkeletonReader::isSkeletonSheet(Volume * vol, int ox, int oy, int oz )
		{
			int cn = 12 ;
			int nx, ny, nz ;

			for ( int i = 0 ; i < 12 ; i ++ )
			{	
				for ( int j = 0 ; j < 4 ; j ++ )
				{
					nx = ox + sheetNeighbor[i][j][0] ;
					ny = oy + sheetNeighbor[i][j][1] ;
					nz = oz + sheetNeighbor[i][j][2] ;

					if ( vol->getDataAt( nx, ny, nz ) <= 0 )
					{
						cn -- ;
						break ;
					}
				}
			}

			return ( cn >= 1 ) ; // isSheet has (ch >= 3) here
		}

		// finds all the corner cells in a sheet
		// corner cells are cells that are inside the sheet but have more than one neighbor on the skeleton that lies outside the sheet
		void SkeletonReader::FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<GeometricShape*> & helixes, int sheetId) {

			// helper function for iterating over 6 neighbor voxels
			int d[6][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
			d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
			d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;

			int outsideCounter; // count of neighbors that lie outside the sheet

			// for each voxel that is inside the sheet
			for(int i = 0; i < (int)helixes[sheetId]->internalCells.size(); i++) {
				outsideCounter = 0;

				// iterate over 6 neighbor voxels, counting how many lie outside the sheet
				for(int j = 0; j < 6; j++) {
					if((vol->getDataAt(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) > 0) && 
						(paintedVol->getDataAt(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) == 0)) {
						outsideCounter++;
					}
				}

				// if some neighbor voxels are outside the sheet
				if(outsideCounter > 0) {
					bool found = false;
					// iterate over all corner cells of this sheet, checking to see if this voxel has been labeled as a corner cell yet
					for(int k = 0; k < (int)helixes[sheetId]->cornerCells.size(); k++) {
						found = found || ((helixes[sheetId]->cornerCells[k].x == helixes[sheetId]->internalCells[i].x) && 
							(helixes[sheetId]->cornerCells[k].y == helixes[sheetId]->internalCells[i].y) && 
							(helixes[sheetId]->cornerCells[k].z == helixes[sheetId]->internalCells[i].z));
					}
					// if this voxel has not already been labeled as a corner cell, do so now
					if(!found) {
						helixes[sheetId]->cornerCells.push_back(helixes[sheetId]->internalCells[i]);	
						//helixes[sheetId]->cornerCells[helixes[sheetId]->cornerCells.size()-1].node = 1;
						helixes[sheetId]->cornerCells[helixes[sheetId]->cornerCells.size()-1].node = helixes[sheetId]->cornerCells.size();
#ifdef VERBOSE
						cout << "Sheet corner cell found at sheet " << sheetId << " node " << i << ", corner " << helixes[sheetId]->cornerCells.size() << ", coordinates (" << helixes[sheetId]->internalCells[i].x << "," << helixes[sheetId]->internalCells[i].y << "," << helixes[sheetId]->internalCells[i].z << "), outsideCounter = " << outsideCounter << endl;
#endif // VERBOSE
					}
				}
			}
#ifdef VERBOSE
			cout << "Done finding corner cells for sheet " << sheetId << ". " << helixes[sheetId]->cornerCells.size() << " corner cells.  " << helixes[sheetId]->internalCells.size() << " internal cells." << endl;
#endif // VERBOSE
			//assert(helixes[sheetId]->cornerCells.size() >= 2);
			helixes[sheetId]->length = (float)helixes[sheetId]->internalCells.size();
		}

		// Parses sheetFile, a .wrl file containing a list of polygons that form a sheet.
		// Creates a GeometricShape object consisting of a collection of polygons (triangles) for each sheet.
		// Adds these sheet objects to helixes.
		void SkeletonReader::ReadSheetFile(char * sheetFile, vector<GeometricShape*> & helixes){
			FILE* fin = fopen(sheetFile, "rt");
			if (fin == NULL) {
				printf("Error reading sheet input file %s.  Skipping sheets.\n", sheetFile) ;
			} else {			
				GeometricShape * shape = NULL;

				char token[80];
				double x,y,z;
				int a,b,c,d;
				Polygon p;
				bool lastSheet = false;
				while (!feof(fin)) {
					fscanf(fin, "%s", token);
					// add shape from previous iteration to list of SSEs
					if(strcmp(token, TOKEN_VRML_SHAPE) == 0) {
						if(shape != NULL) {
							helixes.push_back(shape);
						}
						shape = new GeometricShape();		
						shape->geometricShapeType = GRAPHEDGE_SHEET;
						lastSheet = false;
					// adds new 3d points to polygonPoints
					} else if(strcmp(token, TOKEN_VRML_POINT) == 0) {
						fscanf(fin, "%s", token);
						while (fscanf(fin, "%lf %lf %lf,", &x, &y, &z)!= feof(fin)) {
							shape->polygonPoints.push_back(Point3(x, y, z));
						}
						lastSheet = true;
					// adds new polygons built from list of polygonPoints to shape
					} else if(strcmp(token, TOKEN_VRML_COORDINDEX) == 0) {
						fscanf(fin, "%s", token);
						while (fscanf(fin, "%d,%d,%d,%d", &a, &b, &c, &d)!= feof(fin)) {
							p.pointIndex1 = a;
							p.pointIndex2 = b;
							p.pointIndex3 = c;
							p.pointIndex4 = d;
							shape->polygons.push_back(p);
						}
						lastSheet = true;
					}
				}
				// add last shape to list of SSEs
				if(lastSheet) {
					helixes.push_back(shape);
				}

				fclose(fin);
			}
		}

		void SkeletonReader::ReadHelixFile(char * helixFile, char * sseFile, vector<GeometricShape*> & helixes){
			FILE* fin = fopen(helixFile, "rt");
			if (fin == NULL) {
				printf("Error reading helix input file %s.  Skipping helices.\n", helixFile) ;
			} else {			
				GeometricShape * shape = new GeometricShape();
				shape->geometricShapeType = GRAPHEDGE_HELIX;

				char token[80];
				double x,y,z,a;

				// read in helices, one at a time, adding each to helixes
				while (!feof(fin)) {
					fscanf(fin, "%s", token);
					if(strcmp(token, TOKEN_VRML_TRANSLATION) == 0) {
						fscanf(fin, "%lf %lf %lf", &x, &y, &z);
						//shape->Translate(Vector3(x, y, z));
						shape->SetCenter(Point3(x, y, z));
					} else if(strcmp(token, TOKEN_VRML_ROTATION) == 0) {
						fscanf(fin, "%lf %lf %lf %lf", &x, &y, &z, &a);
						shape->Rotate(Vector3(x, y, z), a);
					} else if(strcmp(token, TOKEN_VRML_HEIGHT) == 0) {
						fscanf(fin, "%lf", &a);
						//shape->Scale(1.0, a, 1.0);
						shape->SetHeight(a);
					} else if(strcmp(token, TOKEN_VRML_RADIUS) == 0) {
						fscanf(fin, "%lf", &a);
						//shape->Scale(a*2, 1.0, a*2);
						shape->SetRadius(a);

						// add shape to list of helices 
						helixes.push_back(shape);

						// reinitialize shape variable
						shape = new GeometricShape();
						shape->geometricShapeType = GRAPHEDGE_HELIX;
					}
				}
				delete shape;

				fclose(fin);

				// if sseFile was provided as an argument, parse it to get lengths of helices.
				// store the helix lengths in this file with the helices in helixes
				// assume that the lengths in this file are provided in the same order as the helices were stored above.
				if(sseFile != NULL) {
					fin = fopen(sseFile, "rt");
					if (fin == NULL) {
						printf("Error reading helix length file %s. Skipping helix lengths\n", sseFile) ;
					} else {

						char t1[80], t2[80], t3[80];
						int length; 
						unsigned int count = 0;

						while (!feof(fin)) {
							fscanf(fin, "%s", token);
							if(strcmp(token, TOKEN_SSE_ALPHA) == 0 && count < helixes.size() ) { // size check prevents crash when lengths file has more entries than helices loaded above
								fscanf(fin, "%s %s %s %d", t1, t2, t3, &length);
								helixes[count]->length = (float)length * HELIX_C_ALPHA_TO_ANGSTROMS;
								count++;
							}
						}
						fclose(fin);
					}
				}
			}
		}

		// finds the loops from the helix/sheet corner given by helixList[startHelix]->cornerCells[startCell] to 
		// all other helices/sheets by flooding outward along the skeleton volume
		// stores the resulting loops in the graph object using graph->SetCost and graph->SetType
		void SkeletonReader::FindSizes(int startHelix, int startCell, vector<GeometricShape*> & helixList, Volume * vol, Volume * coloredVol, StandardGraph * graph) {
			vector<Point3Int *> oldStack;
			vector<Point3Int *> newStack;
			int currentHelix;

			// create arrays to store paint color (paintVol) and a path direction indicator (backVol) for each voxel in maskVol
			int * backVol = new int[coloredVol->getSizeX() * coloredVol->getSizeY() * coloredVol->getSizeZ()];
			// initialize to -1
			for(int i = 0; i < coloredVol->getSizeX() * coloredVol->getSizeY() * coloredVol->getSizeZ(); i++) {
				backVol[i] = -1;
			}
			int newIx;

			// calculate starting point, in unscaled voxel coords
			Point3Int * startPoint = new Point3Int(helixList[startHelix]->cornerCells[startCell].x, helixList[startHelix]->cornerCells[startCell].y, helixList[startHelix]->cornerCells[startCell].z, 0);

			// add to list of voxels to be explored
			oldStack.push_back(startPoint);

			Point3Int * currentPoint; //CurrentPoint
			Point3 cPt, nPt;
			int x, y, z, xx, yy, zz;

			// helper array for finding one of 26 neighbors of a voxel.
			// number on right is index in d that moves in the opposite direction. stored as back[] below and used for retracing paths.
			int d[26][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1; // 1
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;  // 0
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;  // 5
			d[3][0] = 0;		d[3][1] = -1;		d[3][2] = -1; // 7
			d[4][0] = 0;		d[4][1] = -1;		d[4][2] = 1;  // 6
			d[5][0] = 0;		d[5][1] = 1;		d[5][2] = 0;  // 2
			d[6][0] = 0;		d[6][1] = 1;		d[6][2] = -1; // 4
			d[7][0] = 0;		d[7][1] = 1;		d[7][2] = 1;  // 3
			d[8][0] = -1;		d[8][1] = 0;		d[8][2] = 0;  // 17
			d[9][0] = -1;		d[9][1] = 0;		d[9][2] = -1; // 19
			d[10][0] = -1;		d[10][1] = 0;		d[10][2] = 1; // 18
			d[11][0] = -1;		d[11][1] = -1;		d[11][2] = 0; // 23
			d[12][0] = -1;		d[12][1] = -1;		d[12][2] = -1;// 25
			d[13][0] = -1;		d[13][1] = -1;		d[13][2] = 1; // 24
			d[14][0] = -1;		d[14][1] = 1;		d[14][2] = 0; // 20
			d[15][0] = -1;		d[15][1] = 1;		d[15][2] = -1;// 22
			d[16][0] = -1;		d[16][1] = 1;		d[16][2] = 1; // 21
			d[17][0] = 1;		d[17][1] = 0;		d[17][2] = 0; // 8
			d[18][0] = 1;		d[18][1] = 0;		d[18][2] = -1;// 10
			d[19][0] = 1;		d[19][1] = 0;		d[19][2] = 1; // 9
			d[20][0] = 1;		d[20][1] = -1;		d[20][2] = 0; // 14
			d[21][0] = 1;		d[21][1] = -1;		d[21][2] = -1;// 16
			d[22][0] = 1;		d[22][1] = -1;		d[22][2] = 1; // 15
			d[23][0] = 1;		d[23][1] = 1;		d[23][2] = 0; // 11
			d[24][0] = 1;		d[24][1] = 1;		d[24][2] = -1;// 13
			d[25][0] = 1;		d[25][1] = 1;		d[25][2] = 1; // 12

			int back[26] = {1, 0, 5, 7, 6, 2, 4, 3, 17, 19, 18, 23, 25, 24, 20, 22, 21, 8, 10, 9, 14, 16, 15, 11, 13, 12};    



			// mark all voxels as unvisited
			Volume * visited = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ());

			int helixCount = graph->GetHelixCount();

			bool expand;

			// while the previous iteration ended with more voxels to expand
			while(oldStack.size() > 0) {
				// oldStack is list of points from previous iteration
				// newStack is list of points found so far in this iteration
				newStack.clear();
				for(int i = 0; i < (int)oldStack.size(); i++) {
					currentPoint = oldStack[i];
					cPt = Point3(currentPoint->x * vol->getSpacingX(), currentPoint->y * vol->getSpacingY(), currentPoint->z * vol->getSpacingZ());
					expand = true;
					xx = currentPoint->x;	
					yy = currentPoint->y;	
					zz = currentPoint->z;
					currentHelix = round(coloredVol->getDataAt(xx,yy,zz)) - 1;
					// mark this point as visited
					visited->setDataAt(xx, yy, zz, 1);



					// if the current point is inside some helix/sheet other than the start helix/sheet,
					// save the cost in the adjacency matrix and expand no further from this point
					if((currentHelix >= 0) && (currentHelix != startHelix)) {
						int n1, n2;
						// n1 is the graph index of start helix/sheet in helixList. 
						// n2 is the graph index of currentPoint, which is some other helix/sheet.
						n1 = GetGraphIndex(helixList, startHelix, startCell);
						n2 = GetGraphIndex(helixList, currentHelix, currentPoint);
						bool found = false;
						if( (n1 >= 0) && (n2 >= 0) && (currentPoint->distance < graph->GetCost(n1, n2)) ) { // includes check for previously found shorter path
							// store the distance to the currentPoint as the cost of going from the start helix/sheet to the currentPoint helix/sheet
							graph->SetCost(n1, n2, currentPoint->distance);
							// this is a loop type
							graph->SetType(n1, n2, GRAPHEDGE_LOOP);
							// save the same info for the reverse direction
							graph->SetCost(n2, n1, currentPoint->distance);
							graph->SetType(n2, n1, GRAPHEDGE_LOOP);
							found = true;
						}

						// if the found SSE is a helix, stop expanding. if a sheet, keep expanding. this prevents paths from passing through helices
						// and allows them to cross sheets.
						if( (currentHelix < helixCount) ) {
							// stop expanding, since some other helix has been found
							expand = false;
						} else {
							// keep expanding, since a sheet was found, and paths are allowed to cross sheets
							expand = true;
						}

						// retrace the path
						bool backFound = false;

						// if a path was found above, retrace it using info from backVol and store the path in graph->paths
						Vector3DInt currentPos = Vector3DInt(xx, yy, zz); // update the current position 
						if(found) {	
							// store endPosition to path vector
							graph->paths[n2-1][n1-1].clear();
							graph->paths[n2-1][n1-1].push_back(currentPos);
							graph->paths[n1-1][n2-1].clear();
							// store opposite direction path
							graph->paths[n1-1][n2-1].insert(graph->paths[n1-1][n2-1].begin(),currentPos);
							while(!backFound) {
								newIx = coloredVol->getIndex(currentPos.XInt(), currentPos.YInt(), currentPos.ZInt());
								backFound = (backVol[newIx] < 0);
								if(!backFound) {
									// move in the direction indicated by backVol for this point, to retrace path found above
									currentPos = currentPos + Vector3DInt(d[backVol[newIx]][0], d[backVol[newIx]][1], d[backVol[newIx]][2]);
									// add the next point to the path
									graph->paths[n2-1][n1-1].push_back(currentPos);
									// store in opposite direction path as well
									graph->paths[n1-1][n2-1].insert(graph->paths[n1-1][n2-1].begin(),currentPos);
								}
							}
						}

					}

					// if no other helix/sheet has been found yet, keep expanding
					if(expand) {
						// for each of 26 neighbors
						for(int j = 0; j < 26; j++) {
							// get coords of this neighbor point
							x = currentPoint->x+d[j][0];	
							y = currentPoint->y+d[j][1];	
							z = currentPoint->z+d[j][2];
							// scale coords to volume space 
							nPt = Point3(x * vol->getSpacingX(), y * vol->getSpacingY(), z * vol->getSpacingZ());

							// if neighbor point is inside the volume
							if((x >= 0) && (x < vol->getSizeX()) && (y >=0) && (y < vol->getSizeY()) && (z >= 0) && (z < vol->getSizeZ())) {
								// if all these conditions met:
								//    this point not yet visited
								//    volume at this point has value at least 0.001
								//    this point is not inside the start helix or on the start sheet
								//  TODO: Consider case where this voxel may have already been visited but with a longer path
								if((visited->getDataAt(x, y, z) <= 0.001) && (vol->getDataAt(x, y, z) > 0.001) &&
									(round(coloredVol->getDataAt(x, y, z)) - 1 != startHelix)) {
									// add this point to newStack with distance = | cPt - nPt |
									// the distance is the length of the vector from the cPt voxel to this neighbor nPt
									newStack.push_back(new Point3Int(x, y, z, currentPoint->distance + (float)(cPt - nPt).length()));
									// mark this point as visited
									visited->setDataAt(x, y, z, 1.0);
									// Look up array index in backVol
									newIx = coloredVol->getIndex(x,y,z);
									backVol[newIx] = back[j];
								}
							}
						}
					}
					delete currentPoint;
				}
				oldStack.clear();
				oldStack = newStack;
				newStack.clear();
			}
			delete visited;
			delete backVol;
		}


		// Find all paths in a graph
		void SkeletonReader::FindPaths(StandardGraph * graph) {
			vector<Vector3DInt> endPoints;
			vector< vector<Vector3DInt> > nodes;
			Point3Int pt = Point3Int(0,0,0,0);
			vector<Vector3DInt> node;

#ifdef VERBOSE
			printf("Storing helix endpoints.\n");
#endif // VERBOSE
			
			// find all graph nodes
			for(unsigned int i = 0; i < graph->skeletonHelixes.size(); i++) {
				// store all helix endpoints in nodes vector
				// 2 pts per helix: even are helix start, odd are helix end
				if (graph->skeletonHelixes[i]->IsHelix()) {
					for(unsigned int j = 1; j <= 2; j++) {
						pt = graph->skeletonHelixes[i]->GetCornerCell(j);
						node = vector<Vector3DInt>();
						endPoints.push_back(Vector3DInt(pt.x, pt.y, pt.z));
						node.push_back(Vector3DInt(pt.x, pt.y, pt.z));
						nodes.push_back(node);
					}
				}

				// store all sheet corners in nodes vector
				if (graph->skeletonHelixes[i]->IsSheet()) {
					node = vector<Vector3DInt>();
					for(unsigned int j = 1; j <= graph->skeletonHelixes[i]->cornerCells.size(); j++) {
						pt = graph->skeletonHelixes[i]->GetCornerCell(j);
						node.push_back(Vector3DInt(pt.x, pt.y, pt.z));
					}
					nodes.push_back(node);
				}
			}

			Volume * maskVol = new Volume(graph->skeletonVolume->getSizeX(), graph->skeletonVolume->getSizeY(), graph->skeletonVolume->getSizeZ(), 0, 0, 0, graph->skeletonVolume);

#ifdef VERBOSE
			printf("Finding all paths through helices.\n");
#endif // VERBOSE
			
			// add all paths through helices, from (odd) start index to (even) end index
			for(unsigned int i = 0; i < nodes.size(); i+=2) {
				if (graph->skeletonHelixes[i/2]->IsHelix()) {
					FindPath(i, i+1, nodes, maskVol, graph, true); // eraseMask=true means don't render?
				}
			}

#ifdef VERBOSE
			printf("Finding all paths between helices.\n");
#endif // VERBOSE
			// for every pair of endpoints i!=j, add a path, if this path is not already defined
			for(unsigned int i = 0; i < nodes.size()-1; i++) {
				for(unsigned int j = i+1; j < nodes.size(); j++) {
					cout << "finding path between nodes " << i << " and " << j << endl;
					if(graph->paths[i][j].size() == 0) {
						FindPath(i, j, nodes, maskVol, graph, false);						
					}
				}
			}

			cout << "after adding all paths, maskVol has " << maskVol->getNonZeroVoxelCount() << " nonzero voxels" << endl;

#ifdef VERBOSE
			printf("Done finding all paths between helices.\n");
#endif // VERBOSE

			delete maskVol;
		}

		// Find path from endpoints[startIx] to endpoints[endIx] in a graph.
		// The path grows outward from start point to end point along voxels in the maskVol with values > 0.5.
		// The path is stored in graph->paths[startIx][endIx] and also painted in maskVol.
		// If eraseMask is set, maskVol voxels inside the startIx and endIx helices are not painted.
		void SkeletonReader::FindPath(int startIx, int endIx, vector<vector<Vector3DInt> > nodes, Volume * maskVol, StandardGraph * graph, bool eraseMask) {
			// erase any old path
			graph->paths[startIx][endIx] = vector<Vector3DInt>();

			// positions will store the path
			queue<Vector3DInt> positions;

			int shortestPathLength = MAXINT;
			vector<Vector3DInt> shortestPath;

			for (unsigned int s = 0; s < nodes[startIx].size(); s++) {
				for (unsigned int t = 0; t < nodes[endIx].size(); t++) {
					// start with empty queue
					positions = queue<Vector3DInt>();
					Vector3DInt currentPos = nodes[startIx][s], newPos, endPos = nodes[endIx][t];			
					positions.push(currentPos);
					// create arrays to store paint color (paintVol) and a path direction indicator (backVol) for each voxel in maskVol
					int * paintVol = new int[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];
					int * backVol = new int[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];
					// initialize to -1
					for(int i = 0; i < maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ(); i++) {
						paintVol[i] = -1;
						backVol[i] = -1;
					}
					// paint start point
					paintVol[maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z())] = 0;
					// local vars for while loop
					bool found = false;
					int currVal, newIx;
					int currentPathLength = 0;
					vector<Vector3DInt> currentPath;

					// grows outward repeatedly from the start point to each of 26 neighbors until the end point is found
					// only paints voxels with maskVol > 0.5
					// stops the first time the end point is found, or when all paths have already been explored and no path found
					while(!found && !positions.empty()) {
						// get current voxel position from queue
						currentPos = positions.front(); 
						positions.pop();
						// look up paint color of current voxel
						currVal = paintVol[maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z())];
						// check if current voxel is the end voxel
						found = (currentPos == endPos);
						
						// if not already at the end voxel
						if(!found) {
							// for each of the 26 neighboring voxels
							for(int i = 0; i < 26; i++) {
								// set newPos to the neighbor voxel location
								newPos = currentPos + Vector3DInt(D26[i][0], D26[i][1], D26[i][2]);
								// find the maskVol index number corresponding to this voxel
								newIx = maskVol->getIndex(newPos.X(), newPos.Y(), newPos.Z());
								// must satisfy one of two cases:
								//   new voxel is unvisited, and is an allowed voxel according to maskVol
								//   voxel has already been visited, but with a longer path from the source 
								if((maskVol->getDataAt(newIx) > 0.5) && ((paintVol[newIx] < 0) || (paintVol[newIx] > currVal + 1))) {
									// add new voxel to queue
									positions.push(newPos);
									// associate a new path length with the new voxel
									paintVol[newIx] = currVal + 1;
									// store a number in backVol to describe this path
									backVol[newIx] = BACK26[i];
								}
							}
						}
					}
					
					

					bool backFound = false;

					// if a path was found above, retrace it using info from backVol and store the path in graph->paths
					if(found) {	
						// set currentPos to the end of the path
						currentPos = Vector3DInt(endPos.X(), endPos.Y(), endPos.Z());
						// store endPosition to path vector
						//graph->paths[startIx][endIx].push_back(currentPos);
						currentPath.push_back(currentPos);
						while(!backFound) {
							newIx = maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z());
							backFound = (backVol[newIx] < 0);
							if(!backFound) {
								// move in the direction indicated by backVol for this point, to retrace path found above
								currentPos = currentPos + Vector3DInt(D26[backVol[newIx]][0], D26[backVol[newIx]][1], D26[backVol[newIx]][2]);
								// add the next point to the path
								currentPath.push_back(currentPos);
								currentPathLength++;
							}
						}
					}
					delete [] paintVol;
					delete [] backVol;

					if (found && currentPathLength < shortestPathLength) {
						shortestPathLength = currentPathLength;
						shortestPath = currentPath;
					}
				}
			}
			if (shortestPathLength < MAXINT) { 
				graph->paths[startIx][endIx] = shortestPath;
			}

			if(eraseMask) {
				// for each voxel along the path found above
				for(int i = 1; i < (int)graph->paths[startIx][endIx].size()-1; i++) {
					Vector3DInt currentPos = graph->paths[startIx][endIx][i];
					Point3 pt = Point3(currentPos.X(), currentPos.Y(), currentPos.Z());
					// if this voxel is inside either the start helix or the end helix for this path
					if(graph->skeletonHelixes[(int)startIx/2]->IsInsideShape(pt) || 
							graph->skeletonHelixes[(int)endIx/2]->IsInsideShape(pt)) {
						// erase the voxel from maskVol
						maskVol->setDataAt(currentPos.X(), currentPos.Y(), currentPos.Z(), 0.0);
					}
				}
			}
		}
	}
}

#endif
