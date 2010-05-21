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
			void StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold);
			MatrixFloat GetRigidTransform();
			MatrixFloat GetHelixFlexibleTransform(int helixIx);
		private:
			static const int SAMPLE_COUNT = 10;
			SSECorrespondenceFinder finder;
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			vector<Vector3DFloat> pdbHelixAtomLocs;
			float jointAngleThreshold, dihedralAngleThreshold, lengthThreshold, centroidDistanceThreshold;
			vector < vector<SSECorrespondenceNode> > corr;
		};

		FlexibleFittingEngine::FlexibleFittingEngine() {
			featureList1.clear();
			featureList2.clear();
			corr.clear();
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

		void FlexibleFittingEngine::StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold) {
			finder.InitializeFeatures(featureList1, featureList2);
			finder.InitializeConstants(0, lengthThreshold, 0, 0, 0, 0, 0, jointAngleThreshold, dihedralAngleThreshold, centroidDistanceThreshold, 1, 10);
			//finder.PrintFeatureListsMathematica();
			corr = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(true, true, 4)[0];
		}

		MatrixFloat FlexibleFittingEngine::GetRigidTransform() {
			int maxIndex = 0;
			for(unsigned int i = 1; i < corr.size(); i++) {
				if(corr[i].size() > corr[maxIndex].size()) {
					maxIndex = i;
				}
			}
			return finder.GetTransform(corr[maxIndex], SAMPLE_COUNT);
		}

		MatrixFloat FlexibleFittingEngine::GetHelixFlexibleTransform(int helixIx) {
			for(unsigned int i = 0; i < corr.size(); i++) {
				for(unsigned int j = 0; j < corr[i].size(); j++) {
					if(corr[i][j].GetPIndex() == helixIx) {
						return finder.GetTransform(corr[i], SAMPLE_COUNT);
					}
				}
			}
			return MatrixFloat(4,4);  // Can check this case if the element in the last row last column is not equal to 1 (since this is a homogeneous matrix)
		}

	}
}

#endif