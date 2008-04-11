#ifndef GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H
#define GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H

#include "Graph3D.h"
#include <queue>
#include "GlobalDefinitions.h"
#include <ProteinMorph/NonManifoldMesh.h>

using namespace wustl_mm::Protein_Morph;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		typedef Graph3D<float> GraphType;

		class InteractiveSkeletonizer : public VolumeSkeletonizer {
		public:
			InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo = false);
			~InteractiveSkeletonizer();
			vector<Vector3DInt> GetPath(Vector3DInt endPoint);			
			Vector3DInt FindClosestSkeletalPoint(Vector3DInt point);
			void SetGraphWeights(double skeletonRatio, double structureTensorRatio);
			void CalculateMinimalSpanningTree(Vector3DInt seedPoint);
			Volume * GetSkeleton();
									
		protected:
			double GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type);			
			GraphType * skeletonGraph;
			GraphType * stGraph;
			GraphType * mergedGraph;
			Volume * skeleton;
			char * returnPath;
			Vector3DInt seedPoint;
			Vector3DInt offset;
			Vector3DInt * vectors26;
			Vector3DFloat * nVectors26;
			EigenResults3D * volumeEigens;

			static const int CONNECTIVITY = 26;
			static const char SEED_POINT_FLAG = 50;
		};

		InteractiveSkeletonizer::InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo) : VolumeSkeletonizer(0, curveRadius, 0,0) {
			appTimeManager.PushCurrentTime();
			//NormalizeVolume(sourceVol);
			//CleanupVolume(sourceVol, minGray, maxGray);
			mergedGraph = NULL;		
			offset = Vector3DInt(MAX_GAUSSIAN_FILTER_RADIUS, MAX_GAUSSIAN_FILTER_RADIUS, MAX_GAUSSIAN_FILTER_RADIUS);
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			returnPath = new char[sourceVol->getSizeX() * sourceVol->getSizeY() * sourceVol->getSizeZ()];
			
			vectors26 = new Vector3DInt[CONNECTIVITY];
			nVectors26 = new Vector3DFloat[CONNECTIVITY];
			for(int i = 0; i < CONNECTIVITY; i++) {
				vectors26[i] = Vector3DInt(VOLUME_NEIGHBORS_26[i][0], VOLUME_NEIGHBORS_26[i][1], VOLUME_NEIGHBORS_26[i][2]);
				nVectors26[i] = Vector3DFloat(VOLUME_NEIGHBORS_26[i][0], VOLUME_NEIGHBORS_26[i][1], VOLUME_NEIGHBORS_26[i][2]);
				nVectors26[i].Normalize();
			}

			skeletonGraph = new GraphType(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ(), CONNECTIVITY);
			stGraph = new GraphType(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ(), CONNECTIVITY);
			int index;
			
			Volume * nullVol = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
			appTimeManager.PushCurrentTime();
			skeleton = GetImmersionThinning(sourceVol, nullVol, minGray, maxGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			appTimeManager.PopAndDisplayTime("	Thinning completed: %f seconds\n");
			delete nullVol;
			PruneCurves(skeleton, minCurveSize);
			#ifdef SAVE_INTERMEDIATE_RESULTS
				skeleton->toMRCFile("CurveSkeleton.mrc");				
			#endif

			Vector3DFloat * volumeGradient = GetVolumeGradient2(sourceVol);					
			volumeEigens = GetEigenResults2(skeleton, volumeGradient, gaussianFilterCurveRadius, curveRadius, true);
			float cost; 

			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < sourceVol->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < sourceVol->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < sourceVol->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = skeleton->getIndex(x, y, z);
							for(int i = 0; i < CONNECTIVITY; i++) {														
								if(skeleton->getDataAt(x + VOLUME_NEIGHBORS_26[i][0], y + VOLUME_NEIGHBORS_26[i][1], z + VOLUME_NEIGHBORS_26[i][2]) > 0) {
									cost = 1.0 - (skeleton->getDataAt(x, y, z) + skeleton->getDataAt(x + VOLUME_NEIGHBORS_26[i][0], y + VOLUME_NEIGHBORS_26[i][1], z + VOLUME_NEIGHBORS_26[i][2]))/(2*255);
									skeletonGraph->SetEdgeCost(cost, x, y, z, i);
									stGraph->SetEdgeCost(GetStructureTensorProjectedScore(volumeEigens[index], nVectors26[i], 2, PRUNING_CLASS_PRUNE_CURVES), x, y, z, i);
								}
							}
						}
					}
				}
			}	

			stGraph->Normalize();

			delete [] volumeGradient;
			if(!storeEigenInfo) {
				delete [] volumeEigens; 
				volumeEigens = NULL;
			}
			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			appTimeManager.PopAndDisplayTime("Creating graphs: %f seconds!\n");
		}


		InteractiveSkeletonizer::~InteractiveSkeletonizer() {
			delete skeletonGraph;
			delete stGraph;
			if(mergedGraph != NULL) {
				delete mergedGraph;
			}
			delete skeleton;

			if(volumeEigens != NULL) {
				delete [] volumeEigens;
			}

			delete [] returnPath;
			delete [] vectors26;
			delete [] nVectors26;
		}

		double InteractiveSkeletonizer::GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type) {
			float score = 0.0;
			switch(type) {
				case PRUNING_CLASS_PRUNE_CURVES:
					for(int i = 0 ; i < 3; i++) {
						score +=  pow(abs(skeletonDirection * imageEigen.vectors[i]) * sqrt(imageEigen.values[i]), power);
					}
					break;
				default:
					score = 1.0;
			}
			return score;
		}
		vector<Vector3DInt> InteractiveSkeletonizer::GetPath(Vector3DInt endPoint) {
			//appTimeManager.PushCurrentTime();

			vector<Vector3DInt> path;
			vector<Vector3DInt> outPath;
			
			endPoint = endPoint + offset;
			Vector3DInt currentPoint = endPoint;

			path.push_back(currentPoint);
			outPath.push_back(currentPoint - offset);

			Vector3DInt oldPoint;
			int returnIndex;
			bool found = true;
			bool completed = false;

			while(!completed && found) {
				returnIndex = returnPath[skeleton->getIndex(currentPoint.X(), currentPoint.Y(), currentPoint.Z())];
				found = (returnIndex >= 0);				
				completed = (returnIndex == SEED_POINT_FLAG);
				//printf("{%d %d %d} -> %d \n", currentPoint.X(), currentPoint.Y(), currentPoint.Z(), returnIndex);

				if(!completed) {
					if(found) {
						currentPoint = currentPoint - vectors26[returnIndex];
						path.push_back(currentPoint);
						outPath.push_back(currentPoint - offset);
						//printf(".");
					} else {
						//printf("N");
						outPath.clear();
					}
				}
			}

			//appTimeManager.PopAndDisplayTime("Finding path: %f seconds!\n");
			return outPath;
		}
		Vector3DInt InteractiveSkeletonizer::FindClosestSkeletalPoint(Vector3DInt point) {
			appTimeManager.PushCurrentTime();
			printf("Starting finding closest skeletal point\n");
			point = point + offset;
			Volume * visited = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ());
			
			visited->setDataAt(point.X(), point.Y(), point.Z(), 1);

			queue<Vector3DInt> visitList;
			visitList.push(point);
			Vector3DInt currentPoint, newPoint;

			bool found = false;
			while(!found && !visitList.empty()) {
				currentPoint = visitList.front();	
				//printf("{%d %d %d}\t", currentPoint.X(), currentPoint.Y(), currentPoint.Z());
				visitList.pop();

				if(skeleton->getDataAt(currentPoint.X(), currentPoint.Y(), currentPoint.Z()) > 0.5) {
					found = true;
				} else {
					
					for(int i = 0; i < CONNECTIVITY; i++) {
						newPoint = currentPoint + vectors26[i];
						if((newPoint.X() >= 0) && (newPoint.X() < skeleton->getSizeX()) && 
							(newPoint.Y() >= 0) && (newPoint.Y() < skeleton->getSizeY()) && 
							(newPoint.Z() >= 0) && (newPoint.Z() < skeleton->getSizeZ()) && (
							visited->getDataAt(newPoint.X(), newPoint.Y(), newPoint.Z()) < 0.5)) {
							visitList.push(newPoint);
							visited->setDataAt(newPoint.X(), newPoint.Y(), newPoint.Z(), 1);
						}
					}
				}
			}

			delete visited;
			appTimeManager.PopAndDisplayTime("Found closest skeletal point: %f seconds\n");
			printf("{%d %d %d}\n", currentPoint.X(), currentPoint.Y(), currentPoint.Z());
			return currentPoint - offset;
		}

		void InteractiveSkeletonizer::SetGraphWeights(double skeletonRatio, double structureTensorRatio){
			appTimeManager.PushCurrentTime();
			if(mergedGraph != NULL) {
				delete mergedGraph;
			}
			mergedGraph = GraphType::MergeGraphs(skeletonGraph, stGraph, skeletonRatio, structureTensorRatio);
			appTimeManager.PopAndDisplayTime("Merging graphs: %f seconds!\n");
		}
		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(Vector3DInt seedPoint) {
			Volume * costMap = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ());
			appTimeManager.PushCurrentTime();
			seedPoint = seedPoint + offset;
			this->seedPoint = seedPoint;
			for(int x = 0; x < costMap->getSizeX(); x++) {
				for(int y = 0; y < costMap->getSizeY(); y++) {
					for(int z = 0; z < costMap->getSizeZ(); z++) {
						costMap->setDataAt(x, y, z, -1.0);
						returnPath[costMap->getIndex(x, y, z)] = -1;
					}
				}
			}

			queue<Vector3DInt> pointList;
			costMap->setDataAt(seedPoint.X(), seedPoint.Y(), seedPoint.Z(), 0);
			returnPath[costMap->getIndex(seedPoint.X(), seedPoint.Y(), seedPoint.Z())] = SEED_POINT_FLAG;
			pointList.push(seedPoint);			
			Vector3DInt currentPoint;
			Vector3DInt newPoint;
			int index;
			double cost1, cost2, oldCost;

			while(!pointList.empty()) {				
				currentPoint = pointList.front();
				pointList.pop();
				cost1 = costMap->getDataAt(currentPoint.X(), currentPoint.Y(), currentPoint.Z());

				for(int i = 0; i < CONNECTIVITY; i++) {
					index = mergedGraph->GetIndex(currentPoint.X(), currentPoint.Y(), currentPoint.Z()); 					
					if(mergedGraph->IsEdge(index, i)) {
						newPoint = currentPoint + vectors26[i];
						cost2 = cost1 + mergedGraph->GetEdgeCost(index, i);
						oldCost = costMap->getDataAt(newPoint.X(), newPoint.Y(), newPoint.Z());
						if((oldCost < 0) || (cost2 < oldCost)) {
							costMap->setDataAt(newPoint.X(), newPoint.Y(), newPoint.Z(), cost2);
							returnPath[costMap->getIndex(newPoint.X(), newPoint.Y(), newPoint.Z())] = i;
							pointList.push(newPoint);
						}					
					}
				}

			}
			#ifdef SAVE_INTERMEDIATE_RESULTS
				costMap->toMRCFile("costMap.mrc");
			#endif

			delete costMap;
			appTimeManager.PopAndDisplayTime("Initializing seed point: %f seconds!\n");
		}

		Volume * InteractiveSkeletonizer::GetSkeleton() {
			return skeleton;
		}
	}
}


#endif