// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Analyzes the difference between two volumes


#ifndef GRAYSKELETONCPP_VOLUME_DELTA_ANALYZER_H
#define GRAYSKELETONCPP_VOLUME_DELTA_ANALYZER_H

#include <vector>
#include <MathTools/DataStructures.h>
#include <SkeletonMaker/volume.h>
#include "DiscreteMesh.h"
#include "GlobalDefinitions.h"
#include "SkeletalCurve.h"
#include "DiscreteMesh.h"

using namespace std;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		struct SurfaceQueueElement {
			int x;
			int y;
			int z;
			unsigned char d;
		};

		class VolumeDeltaAnalyzer {
		public:
			VolumeDeltaAnalyzer(Volume * oldVolume, Volume * newVolume);
			~VolumeDeltaAnalyzer();

			void GetNewPoints(int & pointCount, Vector3DInt * &pointList);
			void GetNewCurves(int & curveCount, SkeletalCurve * &curveList);
			void GetNewSurfaces(int & surfaceCount, SkeletalCurve * &surfaceList);
		private:
			
			SkeletalCurve GetCurveAt(int x, int y, int z, unsigned char curveType, DiscreteMesh * availableMesh, DiscreteMesh * newMesh, DiscreteMesh * oldMesh);
			SkeletalCurve GetSurfaceAt(int x, int y, int z, unsigned char surfaceType, DiscreteMesh * availableMesh, DiscreteMesh * newMesh, DiscreteMesh * oldMesh);
			
		private:
			DiscreteMesh * oldMesh;
			DiscreteMesh * newMesh;
			Volume * oldVolume;
			Volume * newVolume;
			int sizeX, sizeY, sizeZ;
		};

		VolumeDeltaAnalyzer::VolumeDeltaAnalyzer(Volume * oldVolume, Volume * newVolume) {		
			sizeX = newVolume->getSizeX();
			sizeY = newVolume->getSizeY();
			sizeZ = newVolume->getSizeZ();
			oldMesh = new DiscreteMesh(oldVolume);
			newMesh = new DiscreteMesh(newVolume);

		}

		VolumeDeltaAnalyzer::~VolumeDeltaAnalyzer() {
			delete oldMesh;
			delete newMesh;
		}


		void VolumeDeltaAnalyzer::GetNewPoints(int & pointCount, Vector3DInt * &pointList) {
			vector<Vector3DInt> points;
			pointCount = 0;
			Vector3DInt currentPoint;			
			int index;

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					for(int z = 0; z < sizeZ; z++) {
						index = newMesh->GetIndex(x, y, z);
						if(newMesh->IsPointPresent(x, y, z) && !oldMesh->IsPointPresent(x, y, z)) {
							pointCount++;
							currentPoint.values[0] = x;
							currentPoint.values[1] = y;
							currentPoint.values[2] = z;
							points.push_back(currentPoint);
						}
					}
				}
			}			

			pointList = new Vector3DInt[pointCount];
			for(int i = 0; i < pointCount; i++) {
				pointList[i].values[0] = points[i].values[0];
				pointList[i].values[1] = points[i].values[1];
				pointList[i].values[2] = points[i].values[2];
				
			}
			points.clear();

		}

		void VolumeDeltaAnalyzer::GetNewCurves(int & curveCount, SkeletalCurve * &curveList) {
			vector<SkeletalCurve> curves;
			curves.clear();
			curveCount = 0;
			DiscreteMesh * tempMesh = new DiscreteMesh(newMesh);

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					for(int z = 0; z < sizeZ; z++) {
						for(int ct = 0; ct < 3; ct++) {
							if(tempMesh->IsCurvePresent(x, y, z, CURVE_TYPES_3[ct]) && !oldMesh->IsCurvePresent(x, y, z, CURVE_TYPES_3[ct])) {
								curves.push_back(GetCurveAt(x, y, z, CURVE_TYPES_3[ct], tempMesh, newMesh, oldMesh));
							}
						}
					}
				}
			}
			delete tempMesh;

			curveCount = curves.size();
			curveList = new SkeletalCurve[curveCount];
			for(int i = 0; i < curveCount; i++) {
				curveList[i] = curves[i];
			}
			curves.clear();
		}

		void VolumeDeltaAnalyzer::GetNewSurfaces(int & surfaceCount, SkeletalCurve * &surfaceList) {
			vector<SkeletalCurve> surfaces;
			surfaces.clear();
			surfaceCount = 0;
			DiscreteMesh * tempMesh = new DiscreteMesh(newMesh);

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					for(int z = 0; z < sizeZ; z++) {
						for(int ct = 0; ct < 3; ct++) {
							if(tempMesh->IsSurfacePresent(x, y, z, SURFACE_TYPES_3[ct]) && !oldMesh->IsSurfacePresent(x, y, z, CURVE_TYPES_3[ct])) {
								surfaces.push_back(GetSurfaceAt(x, y, z, SURFACE_TYPES_3[ct], tempMesh, newMesh, oldMesh));
							}
						}
					}
				}
			}
			delete tempMesh;

			surfaceCount = surfaces.size();
			surfaceList = new SkeletalCurve[surfaceCount];
			for(int i = 0; i < surfaceCount; i++) {
				surfaceList[i] = surfaces[i];
			}
			surfaces.clear();
		}

		SkeletalCurve VolumeDeltaAnalyzer::GetCurveAt(int x, int y, int z, unsigned char curveType, DiscreteMesh * availableMesh, DiscreteMesh * newMesh, DiscreteMesh * oldMesh) {
			SkeletalCurve curve;

			Vector3DInt p1, p2, pOld, pNew;
			p1.values[0] = x;		p1.values[1] = y;	p1.values[2] = z;
			p2.values[0] = x;		p2.values[1] = y;	p2.values[2] = z;

			switch(curveType) {
				case CURVE_TYPE_XPOS:
					p2.values[0]++;
					break;
				case CURVE_TYPE_YPOS:
					p2.values[1]++;
					break;
				case CURVE_TYPE_ZPOS:
					p2.values[2]++;
					break;
			}
			
			availableMesh->RemoveCurve(p1, p2);		
			curve.points.push_back(p1);
			bool doLoop = (newMesh->GetCurveNeighborsCount(p1.values[0], p1.values[1], p1.values[2]) <= 2);
			pOld = p1;
			while(doLoop) {
				pNew = pOld;
				if(availableMesh->FollowCurve(pNew.values[0], pNew.values[1], pNew.values[2])) {
					if(oldMesh->IsCurvePresent(pOld, pNew)) {
						doLoop = false;
					} else {
						availableMesh->RemoveCurve(pOld, pNew);
						curve.points.push_back(pNew);
					}

					if(newMesh->GetCurveNeighborsCount(pNew.values[0], pNew.values[1], pNew.values[2]) > 2) {
						doLoop = false;
					}
				} else {
					doLoop = false;
				}
				pOld = pNew;
			}

			curve.points.insert(curve.points.begin(), p2);
			doLoop = (newMesh->GetCurveNeighborsCount(p2.values[0], p2.values[1], p2.values[2]) <= 2);
			pOld = p2;
			while(doLoop) {
				pNew = pOld;
				if(availableMesh->FollowCurve(pNew.values[0], pNew.values[1], pNew.values[2])) {
					if(oldMesh->IsCurvePresent(pOld, pNew)) {
						doLoop = false;
					} else {
						availableMesh->RemoveCurve(pOld, pNew);
						curve.points.insert(curve.points.begin(), pNew);
					}

					if(newMesh->GetCurveNeighborsCount(pNew.values[0], pNew.values[1], pNew.values[2]) > 2) {
						doLoop = false;
					}
				} else {
					doLoop = false;
				}
				pOld = pNew;
			}


			return curve;
		}

		SkeletalCurve VolumeDeltaAnalyzer::GetSurfaceAt(int x, int y, int z, unsigned char surfaceType, DiscreteMesh * availableMesh, DiscreteMesh * newMesh, DiscreteMesh * oldMesh){
			
			vector<SurfaceQueueElement> queue;
			SurfaceQueueElement curr, temp;
			Volume * points = new Volume(sizeX, sizeY, sizeZ);

			queue.clear();
			curr.x = x;		curr.y = y;		curr.z = z;		curr.d = surfaceType;
			queue.push_back(curr);
			Vector3DInt * sPoints;
			int * sNeighbors;
			int sNeighborCount;
			int rCount;

			while(queue.size() > 0) {
				curr = queue[0];
				queue.erase(queue.begin());
				if(availableMesh->IsSurfacePresent(curr.x, curr.y, curr.z, curr.d)) {

					availableMesh->GetSurfacePoints(curr.x, curr.y, curr.z, curr.d, sPoints);
					for(int i = 0; i < 4; i++) {
						points->setDataAt(sPoints[i].values[0], sPoints[i].values[1], sPoints[i].values[2], 1);
					}

					availableMesh->RemoveSurface(curr.x, curr.y, curr.z, curr.d); 

					sNeighborCount = availableMesh->GetSurfaceNeighbors(
						sPoints[0].values[0], sPoints[0].values[1], sPoints[0].values[2], 
						sPoints[1].values[0], sPoints[1].values[1], sPoints[1].values[2], sNeighbors);
					rCount = newMesh->GetSurfaceNeighborCount(
						sPoints[0].values[0], sPoints[0].values[1], sPoints[0].values[2], 
						sPoints[1].values[0], sPoints[1].values[1], sPoints[1].values[2]);
					if((sNeighborCount == 1) && (rCount <= 2)){					
						temp.x = sNeighbors[0];		temp.y = sNeighbors[1];		
						temp.z = sNeighbors[2];		temp.d = sNeighbors[3];
						if(!oldMesh->IsSurfacePresent(temp.x, temp.y, temp.z, temp.d)) {
							queue.push_back(temp);
						}
					}
					delete [] sNeighbors;

					sNeighborCount = availableMesh->GetSurfaceNeighbors(
						sPoints[1].values[0], sPoints[1].values[1], sPoints[1].values[2], 
						sPoints[2].values[0], sPoints[2].values[1], sPoints[2].values[2], sNeighbors);
					rCount = newMesh->GetSurfaceNeighborCount(
						sPoints[1].values[0], sPoints[1].values[1], sPoints[1].values[2], 
						sPoints[2].values[0], sPoints[2].values[1], sPoints[2].values[2]);
					if((sNeighborCount == 1) && (rCount <= 2)){
						temp.x = sNeighbors[0];		temp.y = sNeighbors[1];		
						temp.z = sNeighbors[2];		temp.d = sNeighbors[3];
						if(!oldMesh->IsSurfacePresent(temp.x, temp.y, temp.z, temp.d)) {
							queue.push_back(temp);
						}
					}
					delete [] sNeighbors;

					sNeighborCount = availableMesh->GetSurfaceNeighbors(
						sPoints[3].values[0], sPoints[3].values[1], sPoints[3].values[2], 
						sPoints[2].values[0], sPoints[2].values[1], sPoints[2].values[2], sNeighbors);
					rCount = newMesh->GetSurfaceNeighborCount(
						sPoints[3].values[0], sPoints[3].values[1], sPoints[3].values[2], 
						sPoints[3].values[0], sPoints[2].values[1], sPoints[2].values[2]);
					if((sNeighborCount == 1) && (rCount <= 2)){		
						temp.x = sNeighbors[0];		temp.y = sNeighbors[1];		
						temp.z = sNeighbors[2];		temp.d = sNeighbors[3];
						if(!oldMesh->IsSurfacePresent(temp.x, temp.y, temp.z, temp.d)) {
							queue.push_back(temp);
						}
					}
					delete [] sNeighbors;

					sNeighborCount = availableMesh->GetSurfaceNeighbors(
						sPoints[0].values[0], sPoints[0].values[1], sPoints[0].values[2], 
						sPoints[3].values[0], sPoints[3].values[1], sPoints[3].values[2], sNeighbors);
					rCount = newMesh->GetSurfaceNeighborCount(
						sPoints[0].values[0], sPoints[0].values[1], sPoints[0].values[2], 
						sPoints[3].values[0], sPoints[3].values[1], sPoints[3].values[2]);
					if((sNeighborCount == 1) && (rCount <= 2)){		
						temp.x = sNeighbors[0];		temp.y = sNeighbors[1];		
						temp.z = sNeighbors[2];		temp.d = sNeighbors[3];
						if(!oldMesh->IsSurfacePresent(temp.x, temp.y, temp.z, temp.d)) {
							queue.push_back(temp);
						}
					}
					delete [] sNeighbors;

					delete [] sPoints;
				}
			}

			SkeletalCurve surfacePoints;

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					for(int z = 0; z < sizeZ; z++) {
						if(points->getDataAt(x, y, z) > 0) {
							surfacePoints.points.push_back(Vector3DInt(x, y, z));
						}
					}
				}
			}

			delete points;

			return surfacePoints;

		}
	}
}


#endif
