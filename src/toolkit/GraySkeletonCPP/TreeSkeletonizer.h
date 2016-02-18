// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Creates skeletons of tree-like structures


#ifndef GRAYSKELETONCPP_TREE_SKELETONIZER_H
#define GRAYSKELETONCPP_TREE_SKELETONIZER_H

#include "InteractiveSkeletonizer.h"
#include <ProteinMorph/NonManifoldMesh.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {

		class TreeSkeletonizer : public InteractiveSkeletonizer {
		public:
			TreeSkeletonizer(Volume * sourceVol, int minGray, int maxGray, int stepSize, int curveRadius, int minCurveSize);
			~TreeSkeletonizer();

			NonManifoldMesh_NoTags * BuildTree(Vector3DInt rootSeed, double skeletonRatio, double structureTensorRatio);
		private:
			vector<Vector3DInt> seedPoints;
		};

		TreeSkeletonizer::TreeSkeletonizer(Volume * sourceVol, int minGray, int maxGray, int stepSize, int curveRadius, int minCurveSize): InteractiveSkeletonizer(sourceVol, minGray, maxGray, stepSize, curveRadius, 0, true) {
			
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			appTimeManager.PushCurrentTime();
			Volume * prunedSkeleton = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), 0, 0, 0, skeleton);
			PruneUsingStructureTensor(prunedSkeleton, sourceVol, volumeEigens, DEFAULT_PRUNE_THRESHOLD, PRUNING_CLASS_PRUNE_CURVES, "");

			delete [] volumeEigens;
			volumeEigens = NULL;

			PruneCurves(prunedSkeleton, minCurveSize);
			prunedSkeleton->toMRCFile("PrunedSkeleton.mrc");

			for(int x = 0; x < prunedSkeleton->getSizeX(); x++) {
				for(int y = 0; y < prunedSkeleton->getSizeY(); y++) {
					for(int z = 0; z < prunedSkeleton->getSizeZ(); z++) {
						if(prunedSkeleton->getDataAt(x, y, z) > 0.5) {
							seedPoints.push_back(Vector3DInt(x, y, z)-offset);
						}
					}
				}
			}

			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

			delete prunedSkeleton;
		}

		TreeSkeletonizer::~TreeSkeletonizer() {
			seedPoints.clear();
		}



		NonManifoldMesh_NoTags * TreeSkeletonizer::BuildTree(Vector3DInt rootSeed, double skeletonRatio, double structureTensorRatio) {
			SetGraphWeights(skeletonRatio, structureTensorRatio);
			CalculateMinimalSpanningTree(rootSeed);

			int * vertexIds;
			int size = skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ();
			vertexIds = new int[size];			
			for(int i = 0; i < size; i++) {
				vertexIds[i] = -1;
			}

			NonManifoldMesh_NoTags * tree = new NonManifoldMesh_NoTags();

			vector<Vector3DInt> branch;
			int v1Id, v2Id;
			for(unsigned int i = 0; i < seedPoints.size(); i++) {
				branch = GetPath(seedPoints[i]);
				for(int j = 0; j < (int)branch.size()-1; j++) {

					if(vertexIds[skeleton->getIndex(branch[j].X(), branch[j].Y(), branch[j].Z())] < 0) {
						vertexIds[skeleton->getIndex(branch[j].X(), branch[j].Y(), branch[j].Z())] = tree->AddVertex(Vector3DInt2Float(branch[j]));
					}
					if(vertexIds[skeleton->getIndex(branch[j+1].X(), branch[j+1].Y(), branch[j+1].Z())] < 0) {
						vertexIds[skeleton->getIndex(branch[j+1].X(), branch[j+1].Y(), branch[j+1].Z())] = tree->AddVertex(Vector3DInt2Float(branch[j+1]));
					} 

					v1Id = vertexIds[skeleton->getIndex(branch[j].X(), branch[j].Y(), branch[j].Z())];
					v2Id = vertexIds[skeleton->getIndex(branch[j+1].X(), branch[j+1].Y(), branch[j+1].Z())];

					if(!tree->IsEdgePresent(v1Id, v2Id)) {
						tree->AddEdge(v1Id, v2Id, false);
					}
				}
				branch.clear();
			}

			delete[] vertexIds;

			return tree;		
		}
	}
}

#endif
