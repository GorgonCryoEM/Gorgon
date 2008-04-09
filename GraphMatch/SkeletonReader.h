/*
Reads Skeleton files and creates Graphs from them.

Author: Sasakthi S. Abeysinghe
Date  : 01/30/2006
*/

#ifndef SKELETONREADER_H
#define SKELETONREADER_H

#include <SkeletonMaker/reader.h>
#include <SkeletonMaker/volume.h>
#include "GeometricShape.h"
#include <vector>
#include "GlobalConstants.h"

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {


		int Round(double number);

		class SkeletonReader {
		public:
			static int GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, int cornerNum);
			static int GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, Point3Int * point);
			static StandardGraph * ReadFile(char * volumeFile, char * helixFile, char * sseFile, char * sheetFile);
			static void ReadSheetFile(char * sheetFile, vector<GeometricShape*> & helixes);
			static void ReadHelixFile(char * helixFile, char * sseFile, vector<GeometricShape*> & helixes);
			static void FindSizes(int startHelix, int startCell, vector<GeometricShape*> & helixList, Volume * vol, Volume * coloredVol, StandardGraph * graph);
			static void FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<GeometricShape*> & helixes, int sheetId);
		};

		int SkeletonReader::GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, int cornerNum) {
			return helixNum*2 + helixes[helixNum]->cornerCells[cornerNum].node;
		}

		int SkeletonReader::GetGraphIndex(vector<GeometricShape*> & helixes, int helixNum, Point3Int * point) {
			int node = 1;
			double minDistance = MAXINT;
			double dist;
			for(int i = 0; i < (int)helixes[helixNum]->cornerCells.size(); i++) {
				dist = Point3Int::EuclideanDistance(helixes[helixNum]->cornerCells[i], *point);
				if(dist < minDistance) {
					node = helixes[helixNum]->cornerCells[i].node;
					minDistance = dist;
				}
			}
			if(minDistance <= BORDER_MARGIN_THRESHOLD) {
				return helixNum*2 + node;
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
				ReadSheetFile(sheetFile, helixes);
			#endif
			Point3 point;

			double xOffset = 0;
			double yOffset = 0;
			double zOffset = 0;

			if(TRANSLATE_VOLUMETRIC_COORDINATES) {
				xOffset = vol->getSizeX() / 2.0;
				yOffset = vol->getSizeY() / 2.0;
				zOffset = vol->getSizeZ() / 2.0;
			}
			int count = 0;

			// Finding all points inside of the helixes.
			for(point[0] = -xOffset; point[0] < vol->getSizeX() - xOffset; point[0]++) {
				for(point[1] = -yOffset; point[1] < vol->getSizeY() - yOffset; point[1]++) {
					for(point[2] = -zOffset; point[2] < vol->getSizeY() - zOffset; point[2]++) {
						for(int i = 0; i < (int)helixes.size(); i++) {
							if((vol->getDataAt((int)(point[0]+xOffset), (int)(point[1]+yOffset), (int)(point[2]+zOffset)) > 0) && helixes[i]->IsInsideShape(point)) {						
								paintedVol->setDataAt((int)(point[0]+xOffset), (int)(point[1]+yOffset), (int)(point[2]+zOffset), i + 1);
								helixes[i]->AddInternalCell(Point3Int((int)(point[0]+xOffset), (int)(point[1]+yOffset), (int)(point[2]+zOffset), 0));
							}
						}
					}
				}
			}

			StandardGraph * graph = new StandardGraph(2*helixes.size());
			for(unsigned int i = 0; i < helixes.size(); i++) {
				if(helixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
					helixes[i]->FindCornerCellsInHelix();
				} else if (helixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
					FindCornerCellsInSheet(vol, paintedVol, helixes, i);
				}
				graph->SetCost((i*2)+1, (i*2)+1, 0);
				graph->SetCost((i*2)+2, (i*2)+2, 0);
				graph->SetCost((i*2)+1, (i*2)+2, helixes[i]->internalCells.size());
				graph->SetType((i*2)+1, (i*2)+2, helixes[i]->geometricShapeType);
				graph->SetCost((i*2)+2, (i*2)+1, helixes[i]->internalCells.size());
				graph->SetType((i*2)+2, (i*2)+1, helixes[i]->geometricShapeType);
			}

			for(int i = 0; i < (int)helixes.size(); i++) {
				for(int j = 0; j < (int)helixes[i]->cornerCells.size(); j++) {
					FindSizes(i, j, helixes, vol, paintedVol, graph);
				}
			}

			for(int i = 0; i < (int)helixes.size(); i++) {
				graph->skeletonHelixes.push_back(helixes[i]);
			}

			graph->skeletonVolume = vol;
			delete paintedVol;
			graph->GenerateEuclidianMatrix();
			return graph;
		}

		void SkeletonReader::FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<GeometricShape*> & helixes, int sheetId) {
			int d[6][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
			d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
			d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;
			int outsideCounter;
			for(int i = 0; i < (int)helixes[sheetId]->internalCells.size(); i++) {
				outsideCounter = 0;
				for(int j = 0; j < 6; j++) {
					if((vol->getDataAt(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) > 0) && 
						(paintedVol->getDataAt(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) == 0)) {
						outsideCounter++;
					}
				}
				if(outsideCounter > 0) {
					bool found = false;
					for(int k = 0; k < (int)helixes[sheetId]->cornerCells.size(); k++) {
						found = found || ((helixes[sheetId]->cornerCells[k].x == helixes[sheetId]->internalCells[i].x) && 
							(helixes[sheetId]->cornerCells[k].y == helixes[sheetId]->internalCells[i].y) && 
							(helixes[sheetId]->cornerCells[k].z == helixes[sheetId]->internalCells[i].z));
					}
					if(!found) {
						helixes[sheetId]->cornerCells.push_back(helixes[sheetId]->internalCells[i]);	
						helixes[sheetId]->cornerCells[helixes[sheetId]->cornerCells.size()-1].node = 1;
					}
				}
			}
			assert(helixes[sheetId]->cornerCells.size() >= 2);
			helixes[sheetId]->length = helixes[sheetId]->internalCells.size();
		}

		void SkeletonReader::ReadSheetFile(char * sheetFile, vector<GeometricShape*> & helixes){
			FILE* fin = fopen(sheetFile, "rt");
			if (fin == NULL) {
				printf("Error reading input file %s.\n", sheetFile) ;
				exit(0) ;
			}
			
			GeometricShape * shape = NULL;

			char token[80];
			double x,y,z;
			int a,b,c,d;
			Polygon p;
			bool lastSheet;
			while (!feof(fin)) {
				fscanf(fin, "%s", token);
				if(strcmp(token, TOKEN_VRML_SHAPE) == 0) {
					if(shape != NULL) {
						helixes.push_back(shape);
					}
					shape = new GeometricShape();		
					shape->geometricShapeType = GRAPHEDGE_SHEET;
					lastSheet = false;
				} else if(strcmp(token, TOKEN_VRML_POINT) == 0) {
					fscanf(fin, "%s", token);
					while (fscanf(fin, "%lf %lf %lf,", &x, &y, &z)!= feof(fin)) {
						shape->polygonPoints.push_back(Point3(x, y, z));
					}
					lastSheet = true;
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
			if(lastSheet) {
				helixes.push_back(shape);
			}

			fclose(fin);
		}

		void SkeletonReader::ReadHelixFile(char * helixFile, char * sseFile, vector<GeometricShape*> & helixes){
			FILE* fin = fopen(helixFile, "rt");
			if (fin == NULL) {
				printf("Error reading input file %s.\n", helixFile) ;
				exit(0) ;
			}

			
			GeometricShape * shape = new GeometricShape();
			shape->geometricShapeType = GRAPHEDGE_HELIX;

			char token[80];
			double x,y,z,a;
			while (!feof(fin)) {
				fscanf(fin, "%s", token);
				if(strcmp(token, TOKEN_VRML_TRANSLATION) == 0) {
					fscanf(fin, "%lf %lf %lf", &x, &y, &z);
					shape->Translate(Vector3(x, y, z));
				} else if(strcmp(token, TOKEN_VRML_ROTATION) == 0) {
					fscanf(fin, "%lf %lf %lf %lf", &x, &y, &z, &a);
					shape->Rotate(Vector3(x, y, z), a);
				} else if(strcmp(token, TOKEN_VRML_HEIGHT) == 0) {
					fscanf(fin, "%lf", &a);
					shape->Scale(1.0, a, 1.0);
				} else if(strcmp(token, TOKEN_VRML_RADIUS) == 0) {
					fscanf(fin, "%lf", &a);
					shape->Scale(a, 1.0, a);
					helixes.push_back(shape);
					shape = new GeometricShape();
					shape->geometricShapeType = GRAPHEDGE_HELIX;
				}
			}
			delete shape;

			fclose(fin);

			if(sseFile != NULL) {
				fin = fopen(sseFile, "rt");
				if (fin == NULL) {
					printf("Error reading input file %s.\n", sseFile) ;
					exit(0) ;
				}

				char t1[80], t2[80], t3[80];
				int length; 
				int count = 0;

				while (!feof(fin)) {
					fscanf(fin, "%s", token);
					if(strcmp(token, TOKEN_SSE_ALPHA) == 0) {
						fscanf(fin, "%s %s %s %d", t1, t2, t3, &length);
						helixes[count]->length = length;
						count++;
					}
				}
				fclose(fin);
			}
		}

		void SkeletonReader::FindSizes(int startHelix, int startCell, vector<GeometricShape*> & helixList, Volume * vol, Volume * coloredVol, StandardGraph * graph) {
			vector<Point3Int *> oldStack;
			vector<Point3Int *> newStack;
			int currentHelix;

			oldStack.push_back(new Point3Int(helixList[startHelix]->cornerCells[startCell].x, helixList[startHelix]->cornerCells[startCell].y, helixList[startHelix]->cornerCells[startCell].z, 0));

			Point3Int * currentPoint; //CurrentPoint
			int x, y, z, xx, yy, zz;
			int d[26][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = -1;		d[3][2] = -1;
			d[4][0] = 0;		d[4][1] = -1;		d[4][2] = 1;
			d[5][0] = 0;		d[5][1] = 1;		d[5][2] = 0;
			d[6][0] = 0;		d[6][1] = 1;		d[6][2] = -1;
			d[7][0] = 0;		d[7][1] = 1;		d[7][2] = 1;
			d[8][0] = -1;		d[8][1] = 0;		d[8][2] = 0;
			d[9][0] = -1;		d[9][1] = 0;		d[9][2] = -1;
			d[10][0] = -1;		d[10][1] = 0;		d[10][2] = 1;
			d[11][0] = -1;		d[11][1] = -1;		d[11][2] = 0;
			d[12][0] = -1;		d[12][1] = -1;		d[12][2] = -1;
			d[13][0] = -1;		d[13][1] = -1;		d[13][2] = 1;
			d[14][0] = -1;		d[14][1] = 1;		d[14][2] = 0;
			d[15][0] = -1;		d[15][1] = 1;		d[15][2] = -1;
			d[16][0] = -1;		d[16][1] = 1;		d[16][2] = 1;
			d[17][0] = 1;		d[17][1] = 0;		d[17][2] = 0;
			d[18][0] = 1;		d[18][1] = 0;		d[18][2] = -1;
			d[19][0] = 1;		d[19][1] = 0;		d[19][2] = 1;
			d[20][0] = 1;		d[20][1] = -1;		d[20][2] = 0;
			d[21][0] = 1;		d[21][1] = -1;		d[21][2] = -1;
			d[22][0] = 1;		d[22][1] = -1;		d[22][2] = 1;
			d[23][0] = 1;		d[23][1] = 1;		d[23][2] = 0;
			d[24][0] = 1;		d[24][1] = 1;		d[24][2] = -1;
			d[25][0] = 1;		d[25][1] = 1;		d[25][2] = 1;

			Volume * visited = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ());

			bool expand;

			while(oldStack.size() > 0) {
				newStack.clear();
				for(int i = 0; i < (int)oldStack.size(); i++) {
					currentPoint = oldStack[i];
					expand = true;
					xx = currentPoint->x;	
					yy = currentPoint->y;	
					zz = currentPoint->z;
					currentHelix = Round(coloredVol->getDataAt(xx,yy,zz)) - 1;
					visited->setDataAt(xx, yy, zz, 1);

					if((currentHelix >= 0) && (currentHelix != startHelix)) {
						int n1, n2;
						n1 = GetGraphIndex(helixList, startHelix, startCell);
						n2 = GetGraphIndex(helixList, currentHelix, currentPoint);
						if((n1 >= 0) && (n2 >= 0)) {
							graph->SetCost(n1, n2, currentPoint->distance);
							graph->SetType(n1, n2, GRAPHEDGE_LOOP);
							graph->SetCost(n2, n1, currentPoint->distance);
							graph->SetType(n2, n1, GRAPHEDGE_LOOP);
						}
						expand = false;
					}

					if(expand) {
						for(int j = 0; j < 26; j++) {
							x = currentPoint->x+d[j][0];	
							y = currentPoint->y+d[j][1];	
							z = currentPoint->z+d[j][2];
							if((x >= 0) && (x < vol->getSizeX()) && (y >=0) && (y < vol->getSizeY()) && (z >= 0) && (z < vol->getSizeZ())) {
								if((visited->getDataAt(x, y, z) <= 0.001) && (vol->getDataAt(x, y, z) > 0.001) &&
									(Round(coloredVol->getDataAt(x, y, z)) - 1 != startHelix)) {
									newStack.push_back(new Point3Int(x, y, z, currentPoint->distance + 1));
									visited->setDataAt(x, y, z, 1.0);
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
		}

		inline int Round(double number) {
			return (int)(number + 0.5);
		}
	}
}

#endif