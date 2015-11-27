// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Gets a path and marks it in the skeleton.


#ifndef PATHGENERATOR_H
#define PATHGENERATOR_H

#include "StandardGraph.h"
#include "LinkedNode.h"
#include "LinkedNodeStub.h"
#include <SkeletonMaker/volume.h>
#include "VectorMath.h"
#include <vector>

using namespace std;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraphMatch {
		class PathGenerator {
		public:
			PathGenerator(StandardGraph * graph);
			void GenerateGraph(StandardNode * node, char * outFileName);
			void GenerateGraph(LinkedNodeStub * node, char * outFileName);
			bool MarkPath(int startHelix, int startCorner, int endHelix, int endCorner, Volume * skeletonVol, Volume * newVol);

		private:
			StandardGraph * graph;
		};

		PathGenerator::PathGenerator(StandardGraph * graph) {
			this->graph = graph;
		}

		void PathGenerator::GenerateGraph(StandardNode * node, char * outFileName)  {
			Volume * skeletonVol = graph->skeletonVolume;
			Volume * newVol = new Volume(skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());
			int startNode = 0;
			int endNode = 1;
			while (endNode < node->n2Top) {
				while((startNode < node->n2Top) && (node->n2[startNode] >= (int)graph->skeletonHelixes.size() * 2)) {
					startNode++;			
				}
				while((endNode < node->n2Top) && (node->n2[endNode] >= (int)graph->skeletonHelixes.size() * 2)) {
					endNode++;			
				}

				if((startNode < node->n2Top) && (endNode < node->n2Top)) {
					int startHelix = node->n2[startNode]/2;
					int startCorner = node->n2[startNode]%2 + 1;
					int endHelix = node->n2[endNode]/2;
					int endCorner = node->n2[endNode]%2 + 1;
					bool marked = MarkPath(startHelix, startCorner, endHelix, endCorner, skeletonVol, newVol);

					if(!marked) {
						printf("Path between %d and %d was not marked \n", startHelix + 1, endHelix + 1);
					}


				}
				startNode = endNode;
				endNode++;
			}

			newVol->toMRCFile(outFileName);
			delete newVol;
		}

		void PathGenerator::GenerateGraph(LinkedNodeStub * node, char * outFileName)  {
			Volume * skeletonVol = graph->skeletonVolume;
			Volume * newVol = new Volume(skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());

			int startHelix = -1, startCorner = -1, endHelix = -1, endCorner = -1;
			bool marked;
			bool continueLoop = true;
			LinkedNodeStub * currentNode = node;

			while(continueLoop) {
				if(currentNode->parentNode == NULL) {
					 break;
				}
				endHelix = (currentNode->n2Node-1)/2;
				endCorner = ((currentNode->n2Node-1)%2) + 1;

				if((startHelix != -1) && (endHelix != -1)) {
					marked = MarkPath(startHelix, startCorner, endHelix, endCorner, skeletonVol, newVol);

		#ifdef VERBOSE
					if(!marked) {
						printf("\t Path between %d and %d was not marked \n", startHelix + 1, endHelix + 1);
					}
		#endif
				}
				startHelix = endHelix;
				startCorner = endCorner;

				currentNode = currentNode->parentNode;		
			}

			newVol->toMRCFile(outFileName);
			delete newVol;
		}


		bool PathGenerator::MarkPath(int startHelix, int startCorner, int endHelix, int endCorner, Volume * skeletonVol, Volume * newVol) {
			vector<Point3Int *> oldStack;
			vector<Point3Int *> newStack;

			Point3Int tempCell = Point3Int(1,1,1,0);

			for(unsigned int i = 0; i < graph->skeletonHelixes[startHelix]->cornerCells.size(); i++) {
				tempCell = graph->skeletonHelixes[startHelix]->cornerCells[i];
				if(tempCell.node == startCorner) {			
					oldStack.push_back(new Point3Int(tempCell.x, tempCell.y, tempCell.z, 1));
				}
			}	

			Point3Int * currentPoint; //CurrentPoint
			int x, y, z, xx = 0, yy = 0, zz = 0;
			int d[6][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
			d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
			d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;
			Volume * visited = new Volume(skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());

			for(int i = 0; i < (int)graph->skeletonHelixes.size(); i++) {
				if(((i != endHelix) && (i != startHelix))) {
				//if(i != endHelix) {
					for(unsigned int j = 0; j < graph->skeletonHelixes[i]->internalCells.size(); j++) {
						Point3Int cell = graph->skeletonHelixes[i]->internalCells[j];
						visited->setDataAt(cell.x, cell.y, cell.z, 100000);
					}
				}


				for(unsigned int j = 0; j < graph->skeletonHelixes[i]->cornerCells.size(); j++) {
					Point3Int cell = graph->skeletonHelixes[i]->cornerCells[j];
					if(((i==startHelix) && (cell.node == startCorner))  || ((i==endHelix) && (cell.node == endCorner))) {
						visited->setDataAt(cell.x, cell.y, cell.z, 0);
					}
				}
			}



			bool found = false;
			int foundDistance = -1;

			while((!found) && (oldStack.size() > 0)) {
				newStack.clear();
				for(unsigned int i = 0; (i < oldStack.size() && !found); i++) {
					currentPoint = oldStack[i];
					xx = currentPoint->x;	
					yy = currentPoint->y;	
					zz = currentPoint->z;

					for(unsigned int i = 0; i < graph->skeletonHelixes[endHelix]->cornerCells.size(); i++) {
						tempCell = graph->skeletonHelixes[endHelix]->cornerCells[i];
						if(tempCell.node == endCorner) {					
							found = found || ((xx == tempCell.x) && (yy == tempCell.y) && (zz == tempCell.z));
							if(found) {
								foundDistance = currentPoint->distance;
							}
						}
					}
					
					if(!found) {
						for(int j = 0; j < 6; j++) {
							x = currentPoint->x+d[j][0];	
							y = currentPoint->y+d[j][1];	
							z = currentPoint->z+d[j][2];
							if((x >= 0) && (x < skeletonVol->getSizeX()) && (y >=0) && (y < skeletonVol->getSizeY()) && (z >= 0) && (z < skeletonVol->getSizeZ())) {
								if((visited->getDataAt(x, y, z) <= 0.001) && (skeletonVol->getDataAt(x, y, z) > 0.001)) {
									Point3Int * newPoint = new Point3Int(x, y, z, currentPoint->distance + 1);

								
									newStack.push_back(newPoint);
									visited->setDataAt(x, y, z, currentPoint->distance);
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

			if(!found || (foundDistance == -1)) {
				return false;
			}
			

			newVol->setDataAt(xx, yy, zz, 1);	
			for(int i = foundDistance - 1; i >= 1; i--) {
				for(int j = 0; j < 6; j++) {
					x = xx+d[j][0];	
					y = yy+d[j][1];	
					z = zz+d[j][2];
					if((x >= 0) && (x < skeletonVol->getSizeX()) && (y >=0) && (y < skeletonVol->getSizeY()) && (z >= 0) && (z < skeletonVol->getSizeZ())) {
						if(int(visited->getDataAt(x, y, z) - (double)i + 0.5) == 0) {
							xx = x;
							yy = y;
							zz = z;
							newVol->setDataAt(xx, yy, zz, 1);
							break;
						}
					}
				}		
			}
			delete visited;
			return true;
		}
	}
}


#endif
