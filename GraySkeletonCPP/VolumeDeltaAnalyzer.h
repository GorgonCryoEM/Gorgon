#ifndef VOLUME_DELTA_ANALYZER_H
#define VOLUME_DELTA_ANALYZER_H

#include <vector>
#include "../MatlabInterface/DataStructures.h";
#include "../SkeletonMaker/volume.h"
#include "DiscreteMesh.h"
#include "GlobalDefinitions.h"
#include "SkeletalCurve.h"
#include "DiscreteMesh.h"

using namespace std;
using namespace wustl_mm::MatlabInterface;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeDeltaAnalyzer {
		public:
			VolumeDeltaAnalyzer(Volume * oldVolume, Volume * newVolume);
			~VolumeDeltaAnalyzer();

			void GetNewPoints(int & pointCount, Vector3DInt * &pointList);
			void GetNewCurves(int & curveCount, SkeletalCurve * &curveList);
		private:
			
			SkeletalCurve GetCurveAt(int x, int y, int z, unsigned char curveType, DiscreteMesh * availableMesh, DiscreteMesh * newMesh, DiscreteMesh * oldMesh);
			
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
			SkeletalCurve * currentCurve;
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
	}
}


#endif;