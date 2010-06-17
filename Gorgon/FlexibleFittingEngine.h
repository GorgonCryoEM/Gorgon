// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying skeletons

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.3  2010/05/21 15:45:16  ssa1
//   Flexible fitting implemented in Gorgon
//
//   Revision 1.2  2010/05/20 21:55:53  ssa1
//   Rigid body alignment based on largest flexible cluster
//
//   Revision 1.1  2010/05/06 21:50:11  ssa1
//   Fixing performance bug when moving a volume
//


#ifndef GORGON_FLEXIBLE_FITTING_ENGINE_H
#define GORGON_FLEXIBLE_FITTING_ENGINE_H

#include <ProteinMorph/SSECorrespondenceFinder.h>
#include <ProteinMorph/SSECorrespondenceFeature.h>
#include <ProteinMorph/SSECorrespondenceNode.h>
#include <vector>
#include "CAlphaRenderer.h"
#include "SSERenderer.h"
#include <MathTools/LinearSolver.h>

using namespace wustl_mm::Protein_Morph;

namespace wustl_mm {
	namespace Visualization {	
		class FlexibleFittingEngine {
		public:
			FlexibleFittingEngine();				
			void StartPDBHelix();
			void AddPDBAtomLocation(Vector3DFloat pos);
			void EndPDBHelix();
			void AddSSEHelix(Vector3DFloat pt1, Vector3DFloat pt2);
			void StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold, int maxSolutionCount);
			int GetCorrespondenceCount();
			int GetClusterCount(int corrIx);
			int GetHelixCount(int corrIx, int clusterIx);
			void ResetEngine();
			SSECorrespondenceNode GetCorrespondenceNode(int corrIx, int clusterIx, int helixIx);
			MatrixFloat GetRigidTransform(int corrIx);
			MatrixFloat GetHelixFlexibleTransform(int corrIx, int helixIx);
		private:
			static const int SAMPLE_COUNT = 10;
			SSECorrespondenceFinder finder;
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			vector<Vector3DFloat> pdbHelixAtomLocs;
			float jointAngleThreshold, dihedralAngleThreshold, lengthThreshold, centroidDistanceThreshold;
			vector < vector < vector<SSECorrespondenceNode> > > corrs;

		};

		FlexibleFittingEngine::FlexibleFittingEngine() {
			ResetEngine();
		}

		void FlexibleFittingEngine::ResetEngine() {
			featureList1.clear();
			featureList2.clear();
			corrs.clear();
		}


		void FlexibleFittingEngine::StartPDBHelix() {
			pdbHelixAtomLocs.clear();
		}

		void FlexibleFittingEngine::AddPDBAtomLocation(Vector3DFloat pos) {
			pdbHelixAtomLocs.push_back(pos);
		}

		void FlexibleFittingEngine::EndPDBHelix() {
			Vector3DFloat pt1, pt2;
			LinearSolver::FindBestFitLine(pt1, pt2, pdbHelixAtomLocs);
			featureList1.push_back(SSECorrespondenceFeature(pt1, pt2));
		}

		void FlexibleFittingEngine::AddSSEHelix(Vector3DFloat pt1, Vector3DFloat pt2) {
			featureList2.push_back(SSECorrespondenceFeature(pt1, pt2));
		}

		void FlexibleFittingEngine::StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold, int maxSolutionCount) {
			finder.InitializeFeatures(featureList1, featureList2);
			finder.InitializeConstants(0, lengthThreshold, 0, 0, 0, 0, 0, jointAngleThreshold, dihedralAngleThreshold, centroidDistanceThreshold, maxSolutionCount, 10);
			finder.PrintFeatureListsMathematica();
			corrs = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(true, true, 4);
		}

		int FlexibleFittingEngine::GetCorrespondenceCount() {
			return corrs.size();
		}

		int FlexibleFittingEngine::GetClusterCount(int corrIx) {
			return corrs[corrIx].size();
		}

		int FlexibleFittingEngine::GetHelixCount(int corrIx, int clusterIx) {
			return corrs[corrIx][clusterIx].size();
		}

		SSECorrespondenceNode FlexibleFittingEngine::GetCorrespondenceNode(int corrIx, int clusterIx, int helixIx) {
			return corrs[corrIx][clusterIx][helixIx];
		}


		MatrixFloat FlexibleFittingEngine::GetRigidTransform(int corrIx) {
			int maxIndex = 0;
			for(unsigned int i = 1; i < corrs[corrIx].size(); i++) {
				if(corrs[corrIx][i].size() > corrs[corrIx][maxIndex].size()) {
					maxIndex = i;
				}
			}
			return finder.GetTransform(corrs[corrIx][maxIndex], SAMPLE_COUNT);			
		}

		MatrixFloat FlexibleFittingEngine::GetHelixFlexibleTransform(int corrIx, int helixIx) {
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						return finder.GetTransform(corrs[corrIx][i], SAMPLE_COUNT);
					}
				}
			}
			return MatrixFloat(4,4);  // Can check this case if the element in the last row last column is not equal to 1 (since this is a homogeneous matrix)
		}	

	}
}

#endif