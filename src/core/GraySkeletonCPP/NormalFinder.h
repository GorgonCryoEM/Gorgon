// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A method to find the surface normals


#ifndef GRAYSKELETONCPP_NORMAL_FINDER_H
#define GRAYSKELETONCPP_NORMAL_FINDER_H

#include <MathTools/Vector3D.h>

using namespace wustl_mm::MathTools;
using namespace wustl_mm::SkeletonMaker;
using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		const int nodeVectors[6][3] = {{-1,0,0}, {0,1,0}, {1,0,0}, {0,-1,0}, {0,0,-1}, {0,0,1}};

		class NormalFinder {
		public:
			NormalFinder();
			~NormalFinder();

			void InitializeGraph(Volume * vol, int x, int y, int z);
			Vector3DFloat GetSurfaceNormal();

		private:
			void SetCurveIndex(int x, int y, int z, int index);
			int GetCurveIndex(int x, int y, int z);
			bool FindOrdering();
			bool FindOrdering(int level);
			bool IsSameOrdering();
			bool CyclesBackToStart();

		private:
			int edgeCount;
			bool graph[6][6];
			int faceIndex[3][3][3];
			bool usedValidNodes[6];
			int validNodes[6];
			int validNodeCount;
			int ordering[6];
			int correctOrdering[6];
			int solutionCount;
		};

		NormalFinder::NormalFinder() {
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					for(int z = 0; z < 3; z++) {
						faceIndex[x][y][z] = -1;
					}
				}
			}
			SetCurveIndex(-1,0,0,0);
			SetCurveIndex(0,1,0,1);
			SetCurveIndex(1,0,0,2);
			SetCurveIndex(0,-1,0,3);
			SetCurveIndex(0,0,-1,4);
			SetCurveIndex(0,0,1,5);
		}

		NormalFinder::~NormalFinder() {
		}

		void NormalFinder::InitializeGraph(Volume * vol, int x, int y, int z) {
			bool found[6] = {false, false, false, false, false, false};
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					graph[i][j] = false;
				}
			}

			if(vol->getDataAt(x, y, z) <= 0) {
				return;
			}
			for (int i = 0; i < 12; i++) {
				if((vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][0][0], y + VOLUME_NEIGHBOR_FACES[i][0][1], z + VOLUME_NEIGHBOR_FACES[i][0][2]) > 0) &&
				   (vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][1][0], y + VOLUME_NEIGHBOR_FACES[i][1][1], z + VOLUME_NEIGHBOR_FACES[i][1][2]) > 0) &&
				   (vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][2][0], y + VOLUME_NEIGHBOR_FACES[i][2][1], z + VOLUME_NEIGHBOR_FACES[i][2][2]) > 0)) {
						graph[GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][0][0], VOLUME_NEIGHBOR_FACES[i][0][1], VOLUME_NEIGHBOR_FACES[i][0][2])]
						     [GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][2][0], VOLUME_NEIGHBOR_FACES[i][2][1], VOLUME_NEIGHBOR_FACES[i][2][2])] = true;
						graph[GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][2][0], VOLUME_NEIGHBOR_FACES[i][2][1], VOLUME_NEIGHBOR_FACES[i][2][2])]
						     [GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][0][0], VOLUME_NEIGHBOR_FACES[i][0][1], VOLUME_NEIGHBOR_FACES[i][0][2])] = true;
						found[GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][0][0], VOLUME_NEIGHBOR_FACES[i][0][1], VOLUME_NEIGHBOR_FACES[i][0][2])] = true;
						found[GetCurveIndex(VOLUME_NEIGHBOR_FACES[i][2][0], VOLUME_NEIGHBOR_FACES[i][2][1], VOLUME_NEIGHBOR_FACES[i][2][2])] = true;
				}
			}

			validNodeCount = 0;

			for (int i = 0; i < 6; i++) {
				if(found[i]) {
					validNodes[validNodeCount] = i;
					validNodeCount++;
				}
			}
		}

		void NormalFinder::SetCurveIndex(int x, int y, int z, int index) {
			faceIndex[x+1][y+1][z+1] = index;
		}

		int NormalFinder::GetCurveIndex(int x, int y, int z){
			int ix = faceIndex[x+1][y+1][z+1];
			assert(ix >= 0);
			return ix;
		}

		bool NormalFinder::IsSameOrdering() {
			bool isSame = false;
			bool allMatched;
			for(int i = 0; i < validNodeCount; i++) {
				allMatched = true;
				for(int j = 0; j < validNodeCount; j++) {
					allMatched = allMatched && (ordering[j] == correctOrdering[(i+j)%validNodeCount]);
				}
				isSame = isSame || allMatched;
			}

			for(int i = 0; i < validNodeCount; i++) {
				allMatched = true;
				for(int j = 0; j < validNodeCount; j++) {
					allMatched = allMatched && (ordering[j] == correctOrdering[(validNodeCount + i - j)%validNodeCount]);
				}
				isSame = isSame || allMatched;
			}
			return isSame;
		}

		bool NormalFinder::FindOrdering(int level) {
			if(solutionCount > 1) {
				return false;
			}

			if(level == validNodeCount) {
				if((solutionCount == 0) || !IsSameOrdering()) {
					solutionCount++;
				}

				for(int i = 0; i < validNodeCount; i++) {
					correctOrdering[i] = ordering[i];					
				}

				return (solutionCount == 1);

			}

			bool found = false;

			for(int i = 0; i < validNodeCount; i++) {
				if(!usedValidNodes[i] && 
					(((level >= 1) && graph[ordering[level-1]][validNodes[i]]) || (level == 0))) {
					ordering[level] = validNodes[i];
					usedValidNodes[i] = true;
					found = found || FindOrdering(level+1);
					usedValidNodes[i] = false;
				}
			}
			return found;
		}


		bool NormalFinder::FindOrdering() {
			if(validNodeCount == 0) {
				return false;
			}

			for(int i = 0; i < 6; i++) {
				usedValidNodes[i] = false;
			}
			return FindOrdering(0);
		}


		bool NormalFinder::CyclesBackToStart() {			
			for(int i = 0; i < validNodeCount-1; i++) {
				if((i != correctOrdering[0]) && graph[correctOrdering[validNodeCount-1]][i]) {
					return false;
				}
			}
			return true;
		}

		Vector3DFloat NormalFinder::GetSurfaceNormal() {
			bool graph2[6][6];
			for(int i = 0; i < 6; i++) {
				for(int j = 0; j < 6; j++) {
					graph2[i][j] = graph[i][j];
				}
			}
			solutionCount = 0;

			Vector3DFloat normal;
			Vector3DFloat v1,v2;
			int from, to;
			if(FindOrdering()){
				normal = Vector3DFloat(0,0,0);
				for(int i = 0; i < validNodeCount; i++) {
					from = correctOrdering[i];
					if(i == validNodeCount-1) {
						to = correctOrdering[0];
					} else {
						to = correctOrdering[i+1];
					}

					if(graph2[from][to]) {
						v1 = Vector3DFloat(nodeVectors[from][0], nodeVectors[from][1], nodeVectors[from][2]);
						v2 = Vector3DFloat(nodeVectors[to][0], nodeVectors[to][1], nodeVectors[to][2]);
						normal += (v1^v2);			
						graph2[from][to] = false;
						graph2[to][from] = false;
					}
				}
				normal.Normalize();
				
				for(int i = 0; i < 6; i++) {
					for(int j = 0; j < 6; j++) {
						if(graph2[i][j]) {
							normal = Vector3DFloat(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
							break;
						}
					}
				}
			} else {
				normal = Vector3DFloat(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
			}			
			return normal;
		}
	};
};

#endif //NORMAL_FINDER_H
