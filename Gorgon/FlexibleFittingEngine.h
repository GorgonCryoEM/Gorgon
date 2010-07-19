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
//   Revision 1.4  2010/06/17 19:31:47  ssa1
//   Visually displaying flexible fitting clusters.
//
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
#include <ProteinMorph/CurveDeformer.h>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <limits>

#include <MathTools/LinearSolver.h>

using namespace wustl_mm::Protein_Morph;
using namespace boost::tuples;

namespace wustl_mm {
	namespace Visualization {	
		class FlexibleFittingEngine {
		public:
			FlexibleFittingEngine();				
			void StartPDBHelix();
			void AddPDBAtomLocation(Vector3DFloat pos);
			void EndPDBHelix();
			void AddSSEHelix(Vector3DFloat pt1, Vector3DFloat pt2);
			vector<float> Deform(int);
			vector<float> returnFlattenedLocations(vector<Vector3DFloat>);
			void addAtomLocation(Vector3DFloat);
			void addHardHandleLocation(Vector3DFloat);
			void addSoftHandleLocation(Vector3DFloat);
			void prepareDeform();
			void setLaplacainW(float);
			void setHardHandleW(float);
			void setSoftHandleW(float);
			void FlexibleFittingEngine::addHelix(int start, int stop);
			void ClearPDBPoints();
			void ResetTransforms();
			void StartSearch(float jointAngleThreshold, float dihedralAngleThreshold, float lengthThreshold, float centroidDistanceThreshold, int maxSolutionCount);
			void AddCorrespondence(int alignmentIx, int SSEIx, int PDBIx);
			void MergeClusters(int alignmentIx, int cluster1, int cluster2);
			int GetCorrespondenceCount();
			int GetClusterCount(int corrIx);
			int GetHelixCount(int corrIx, int clusterIx);
			void ResetEngine();
			SSECorrespondenceNode GetCorrespondenceNode(int corrIx, int clusterIx, int helixIx);
			MatrixFloat GetRigidTransform(int corrIx);
			MatrixFloat GetRigidTransform2(int corrIx, int clusterIx);
			MatrixFloat GetHelixFlexibleTransform(int corrIx, int helixIx);
			vector<bool> GetCorrespondenceFlips(int corrIx);
			vector<int> GetAllCorrespondencesFlat(int corrIx);
			vector<Vector3DFloat> GetAllCAlphaFeatureVecsFlat();
			void FlipCorrespondencePair(int corrIx, int SSEIndex);
			void SaveCorrs();
			void LoadSavedCorrs();
			
			MatrixFloat GetHelixTranslationTransform(int corrIx, int helixIx);
			void GetHelixEnds(int corrIx, int helixIx, Vector3DFloat&, Vector3DFloat&);
			bool GetIsForward(int corrIx, int helixIx);
		private:
			static const int SAMPLE_COUNT = 10;
			SSECorrespondenceFinder finder;
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			vector<Vector3DFloat> pdbHelixAtomLocs;
			float jointAngleThreshold, dihedralAngleThreshold, lengthThreshold, centroidDistanceThreshold;
			vector < vector<SSECorrespondenceNode> > corr;
			vector<Vector3DFloat> origLocations;
			vector<Vector3DFloat> hardHandles;
			vector<Vector3DFloat> softHandles;
			CurveDeformer cd;
			vector<int> helixStarts;
			vector<int> helixStops;
			vector < vector < vector<SSECorrespondenceNode> > > corrs;
			vector<tuple<int, int, int>> helixDistancesSorted;

			vector < vector < vector<SSECorrespondenceNode> > > savedCorrs;
			vector<SSECorrespondenceFeature> savedFeatureList1;
			vector<SSECorrespondenceFeature> savedFeatureList2;
			

		};

		FlexibleFittingEngine::FlexibleFittingEngine() {
			ResetEngine();
		}

		void FlexibleFittingEngine::ResetEngine() {
			featureList1.clear();
			featureList2.clear();
			corr.clear();
			hardHandles.clear();
			softHandles.clear();
			origLocations.clear();
		}
		void FlexibleFittingEngine::ClearPDBPoints(){
			featureList1.clear();	
		}
		void FlexibleFittingEngine::ResetTransforms(){
			finder.InitializeFeatures(featureList1, featureList2);
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

		void FlexibleFittingEngine::AddCorrespondence(int alignmentIx, int SSEIx, int PDBIx){
			vector < vector<SSECorrespondenceNode> >  newCorrs;
		
			//loop through every cluster
			for(unsigned int i = 0; i < corrs[alignmentIx].size(); ++i){
				if(corrs[alignmentIx][i].size() > 1){
					newCorrs.push_back(vector<SSECorrespondenceNode>());
					for(unsigned int j = 0; j < corrs[alignmentIx][i].size(); ++j){
						if(corrs[alignmentIx][i][j].GetPIndex() != PDBIx && corrs[alignmentIx][i][j].GetQIndex() != SSEIx){
							newCorrs[newCorrs.size()-1].push_back(corrs[alignmentIx][i][j]);
						}
					}
				}
				//size of one is a boundary case if we have to not copy that element
				else if(corrs[alignmentIx][i].size() == 1){
					if(corrs[alignmentIx][i][0].GetPIndex() != PDBIx && corrs[alignmentIx][i][0].GetQIndex() != SSEIx){
						newCorrs.push_back(vector<SSECorrespondenceNode>());
						newCorrs[newCorrs.size()-1].push_back(corrs[alignmentIx][i][0]);
					}
				}
			}

			

			newCorrs.push_back(vector<SSECorrespondenceNode>());
			newCorrs[newCorrs.size()-1].push_back(SSECorrespondenceNode(PDBIx, SSEIx, true));//TODO <--------- FIX THIS (last paramater)
			corrs[alignmentIx].clear();
			corrs[alignmentIx] = newCorrs;

		}

		void FlexibleFittingEngine::MergeClusters(int alignmentIx, int cluster1, int cluster2){
			vector < vector<SSECorrespondenceNode> >  newCorrs;
			
			for(unsigned int i = 0; i < corrs[alignmentIx].size(); ++i){
				if(i != cluster1 && i != cluster2){
					newCorrs.push_back(corrs[alignmentIx][i]);
				}
			}
			vector<SSECorrespondenceNode> newCluster;
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster1].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster1][i]);
			}
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster2].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster2][i]);
			}
			newCorrs.push_back(newCluster);
			corrs[alignmentIx].clear();
			corrs[alignmentIx] = newCorrs;
			


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
			int secondLargest = 1;
			for(unsigned int i = 1; i < corrs[corrIx].size(); i++) {
				if(corrs[corrIx][i].size() > corrs[corrIx][maxIndex].size()) {
					secondLargest = maxIndex;
					maxIndex = i;
				}
			}
			//cout << secondLargest << "     " << maxIndex << endl;
			return finder.GetTransform(corrs[corrIx][maxIndex], SAMPLE_COUNT);			
		}

		MatrixFloat FlexibleFittingEngine::GetRigidTransform2(int corrIx, int clusterIx) {
			if(corrs[corrIx][clusterIx].size() > 1){
				return finder.GetTransform(corrs[corrIx][clusterIx], SAMPLE_COUNT);	
			}
			else{
				vector<tuple<int, int>> PQMatchings;
				if(helixDistancesSorted.size() == 0){
					cout << "Building helix dists list" << endl;
					cout << "Saving PQMatchings" << endl;
					for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
						for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
							PQMatchings.push_back(tuple<int,int>(corrs[corrIx][i][j].GetPIndex(), corrs[corrIx][i][j].GetQIndex()));
						}
					}
					
					cout << "Saving closest inds from calculated centroid distances" << endl;
					for(unsigned int i = 0; i < featureList1.size(); ++i){
						float dist1, dist2, dist3;
						int ind1, ind2, ind3;
						float dist = numeric_limits<float>::infinity();
						dist1 = dist; dist2 = dist; dist3 = dist;
						ind1=-1; ind2=-1; ind3=-1;
						for(unsigned int j = 0; j < featureList1.size(); ++j){
							if(i != j){
								dist = (featureList1[i].GetCentroid() - featureList1[j].GetCentroid()).Length();
								if(dist < dist1){
									dist3 = dist2; ind3 = ind2;
									dist2 = dist1; ind2 = ind1;
									dist1 = dist; ind1 = j;
								}
								else if(dist < dist2){
									dist3 = dist2; ind3 = ind2;
									dist2 = dist; ind2 = j;
								}
								else if(dist < dist3){
									dist3 = dist; ind3 = j;
								}
							}
						}
						helixDistancesSorted.push_back(tuple<int, int, int>(ind1, ind2, ind3));
					}
				}
				int helixIx = corrs[corrIx][clusterIx][0].GetPIndex();
				vector<SSECorrespondenceNode> tempCluster;
				int ind1 = helixDistancesSorted[helixIx].get<0>();
				int ind2 = helixDistancesSorted[helixIx].get<1>();
				int ind3 = helixDistancesSorted[helixIx].get<2>();
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						int p = corrs[corrIx][i][j].GetPIndex();
						if(p == ind1 || p == ind2 || p == ind3 || p == helixIx){
							tempCluster.push_back(corrs[corrIx][i][j]);
						}
					}
				}
				return finder.GetTransform(tempCluster, SAMPLE_COUNT);
			}
		}

		MatrixFloat FlexibleFittingEngine::GetHelixFlexibleTransform(int corrIx, int helixIx) {
			int corrNum = -1;
			int qIndex = -1;
			bool flip = false;
			MatrixFloat clusterTrans = MatrixFloat(4,4);
			MatrixFloat translationTrans = MatrixFloat(4,4);
			MatrixFloat rotationTrans = MatrixFloat(4,4);
			MatrixFloat negCentroidTrans = MatrixFloat(4,4);
			MatrixFloat posCentroidTrans= MatrixFloat(4,4);
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						qIndex = corrs[corrIx][i][j].GetQIndex();
						flip = !corrs[corrIx][i][j].IsForward();
						if(corrs[corrIx][i].size() > 1)
							clusterTrans = finder.GetTransform(corrs[corrIx][i], SAMPLE_COUNT);
						else{
							corrNum = i;
							break;
						}
					}
				}
			}
			if((corrNum == -1 && clusterTrans.GetValue(3,3) < 0.5) || qIndex == -1){ //doesn't have a correspondence
				cout << "returning empty matrix corrnum: " << corrNum << " qIndex: " << qIndex << endl;
				return MatrixFloat(4,4);
			}

			vector<tuple<int, int>> PQMatchings;
			if(helixDistancesSorted.size() == 0){
				cout << "Building helix dists list" << endl;
				cout << "Saving PQMatchings" << endl;
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						PQMatchings.push_back(tuple<int,int>(corrs[corrIx][i][j].GetPIndex(), corrs[corrIx][i][j].GetQIndex()));
					}
				}
				
				cout << "Saving closest inds from calculated centroid distances" << endl;
				for(unsigned int i = 0; i < featureList1.size(); ++i){
					float dist1, dist2, dist3;
					int ind1, ind2, ind3;
					float dist = numeric_limits<float>::infinity();
					dist1 = dist; dist2 = dist; dist3 = dist;
					ind1=-1; ind2=-1; ind3=-1;
					for(unsigned int j = 0; j < featureList1.size(); ++j){
						if(i != j){
							dist = (featureList1[i].GetCentroid() - featureList1[j].GetCentroid()).Length();
							if(dist < dist1){
								dist3 = dist2; ind3 = ind2;
								dist2 = dist1; ind2 = ind1;
								dist1 = dist; ind1 = j;
							}
							else if(dist < dist2){
								dist3 = dist2; ind3 = ind2;
								dist2 = dist; ind2 = j;
							}
							else if(dist < dist3){
								dist3 = dist; ind3 = j;
							}
						}
					}
					helixDistancesSorted.push_back(tuple<int, int, int>(ind1, ind2, ind3));
				}
			}
			
			vector<SSECorrespondenceNode> tempCluster;
			int ind1 = helixDistancesSorted[helixIx].get<0>();
			int ind2 = helixDistancesSorted[helixIx].get<1>();
			int ind3 = helixDistancesSorted[helixIx].get<2>();
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					int p = corrs[corrIx][i][j].GetPIndex();
					if(p == ind1 || p == ind2 || p == ind3 || p == helixIx){
						tempCluster.push_back(corrs[corrIx][i][j]);
					}
				}
			}
			clusterTrans =  finder.GetTransform(tempCluster, SAMPLE_COUNT);
			


			Vector3DFloat beforeTrans = featureList1[helixIx].GetCentroid();
			Vector3DFloat center1 = beforeTrans.Transform(clusterTrans);
			Vector3DFloat center2 = featureList2[qIndex].GetCentroid();
			Vector3DFloat diff = center2 - center1;
			
			translationTrans.SetValue(diff[0], 0, 3);	translationTrans.SetValue(diff[1], 1, 3);	translationTrans.SetValue(diff[2], 2, 3);
			translationTrans.SetValue(1,0,0);	translationTrans.SetValue(1,1,1);	translationTrans.SetValue(1,2,2);	translationTrans.SetValue(1,3,3);


			MatrixFloat trans = translationTrans * clusterTrans;

			Vector3DFloat end1 = featureList1[helixIx].GetEndPoint(0).Transform(trans);
			Vector3DFloat end2 = featureList1[helixIx].GetEndPoint(1).Transform(trans);
			Vector3DFloat featVec1 = end2 - end1;
			Vector3DFloat featVec2 = featureList2[qIndex].GetFeatureVector();
			featVec1.Normalize();
			featVec2.Normalize();
			Vector3DFloat normal = featVec1^featVec2; //cross product
			normal.Normalize();
			double angle = acos(featVec1*featVec2);

			if(angle > PI/2)
				angle = PI - angle;
			if(flip)
				angle = angle*-1;
			cout << "Angle: " << angle << endl;
			Vector3DFloat row1 = Vector3DFloat(1.0,0.0,0.0);	row1 = row1.Rotate(normal,angle);
			Vector3DFloat row2 = Vector3DFloat(0.0,1.0,0.0);	row2 = row2.Rotate(normal,angle);
			Vector3DFloat row3 = Vector3DFloat(0.0,0.0,1.0);	row3 = row3.Rotate(normal,angle);

			rotationTrans.SetValue(row1[0],0,0);	rotationTrans.SetValue(row1[1],0,1);	rotationTrans.SetValue(row1[2],0,2);			
			rotationTrans.SetValue(row2[0],1,0);	rotationTrans.SetValue(row2[1],1,1);	rotationTrans.SetValue(row2[2],1,2);			
			rotationTrans.SetValue(row3[0],2,0);	rotationTrans.SetValue(row3[1],2,1);	rotationTrans.SetValue(row3[2],2,2);			
			rotationTrans.SetValue(1,3,3);

			
			Vector3DFloat centroid = (end1+end2)*0.5;
			negCentroidTrans.SetValue(-centroid[0],0,3);	negCentroidTrans.SetValue(-centroid[1],1,3);	negCentroidTrans.SetValue(-centroid[2],2,3);
			negCentroidTrans.SetValue(1,0,0);	negCentroidTrans.SetValue(1,1,1);	negCentroidTrans.SetValue(1,2,2);	negCentroidTrans.SetValue(1,3,3);

			posCentroidTrans.SetValue(centroid[0],0,3);	posCentroidTrans.SetValue(centroid[1],1,3);	posCentroidTrans.SetValue(centroid[2],2,3);
			posCentroidTrans.SetValue(1,0,0);	posCentroidTrans.SetValue(1,1,1);	posCentroidTrans.SetValue(1,2,2);	posCentroidTrans.SetValue(1,3,3);
			//return posCentroidTrans*negCentroidTrans*translationTrans*clusterTrans;
			return posCentroidTrans*rotationTrans*negCentroidTrans*translationTrans*clusterTrans;
		}

		

		void FlexibleFittingEngine::GetHelixEnds(int corrIx, int helixIx, Vector3DFloat& end1, Vector3DFloat& end2){
			end1 = featureList1[helixIx].GetEndPoint(0);
			end2 = featureList1[helixIx].GetEndPoint(1);
			/*for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						end1 = corrs[corrIx][i][j].GetEndPoint(0);
						end2 = corrs[corrIx][i][j].GetEndPoint(1);
						return;
					}
				}
			}*/
		}

		bool FlexibleFittingEngine::GetIsForward(int corrIx, int helixIx){
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						return corrs[corrIx][i][j].IsForward();
					}
				}
			}
			return false;
		}		

		vector<float> FlexibleFittingEngine::Deform(int numNeighbors){
			cd.addHelices(helixStarts, helixStops);
			//cout << "size of corrs " << corrs[0].size() << endl;

			return returnFlattenedLocations(cd.Deform(origLocations, hardHandles, softHandles, numNeighbors));
		}

		vector<float> FlexibleFittingEngine::returnFlattenedLocations(vector<Vector3DFloat> in){
			vector<float> result;
			for(int i = 0; i < in.size(); ++i){
				result.push_back(in[i].X());
				result.push_back(in[i].Y());
				result.push_back(in[i].Z());
			}
			return result;
		}

		void FlexibleFittingEngine::addAtomLocation(Vector3DFloat in){
			origLocations.push_back(in);
		}

		void FlexibleFittingEngine::addHardHandleLocation(Vector3DFloat in){
			hardHandles.push_back(in);
		}

		void FlexibleFittingEngine::addSoftHandleLocation(Vector3DFloat in){
			softHandles.push_back(in);
		}

		void FlexibleFittingEngine::prepareDeform(){
			hardHandles.clear();
			softHandles.clear();
			origLocations.clear();
			helixStarts.clear();
			helixStops.clear();
		}

		void FlexibleFittingEngine::addHelix(int start, int stop){
			helixStarts.push_back(start);
			helixStops.push_back(stop);
		}

		void FlexibleFittingEngine::setLaplacainW(float in){
			cd.setLaplacainW(in);
		}
		void FlexibleFittingEngine::setHardHandleW(float in){
			cd.setHardHandleW(in);
		}
		void FlexibleFittingEngine::setSoftHandleW(float in){
			cd.setSoftHandleW(in);
		}

		vector<bool> FlexibleFittingEngine::GetCorrespondenceFlips(int corrIx){
			vector<bool> result(featureList2.size());
			cout << "Size of result: " << result.size() << endl;
			cout << "Size of corrs: " << corrs[corrIx].size() << endl;
			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					if(!corrs[corrIx][i][j].IsForward())
						cout << "Cluster " << i << " helix " <<  j << " with index " << corrs[corrIx][i][j].GetQIndex() << " is flipped" << endl;
					else
						cout << "Cluster " << i << " helix " <<  j << " with index " << corrs[corrIx][i][j].GetQIndex() << " is NOT flipped" << endl;
					result[corrs[corrIx][i][j].GetQIndex()] = corrs[corrIx][i][j].IsForward();
				}
			}
			return result;
		}

		vector<int> FlexibleFittingEngine::GetAllCorrespondencesFlat(int corrIx){
			vector<int> result;

			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					//cout << "Putting in P: " << corrs[corrIx][i][j].GetPIndex()+1 << " and Q: " << corrs[corrIx][i][j].GetQIndex() << endl;

					result.push_back(corrs[corrIx][i][j].GetPIndex());
					result.push_back(corrs[corrIx][i][j].GetQIndex());
				}
			}
			return result;
		}

		vector<Vector3DFloat> FlexibleFittingEngine::GetAllCAlphaFeatureVecsFlat(){
			vector<Vector3DFloat> result;

			for(unsigned int i = 0; i < featureList1.size(); ++i){
				result.push_back(featureList1[i].GetEndPoint(0));
				result.push_back(featureList1[i].GetEndPoint(1));
			}
			
			return result;
		}


		void FlexibleFittingEngine::FlipCorrespondencePair(int corrIx, int SSEIndex){
			for(unsigned int i=0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j=0; j < corrs[corrIx][i].size(); ++j){
					if(corrs[corrIx][i][j].GetQIndex() == SSEIndex){
						cout << "Flipping # " << j << " in " << i << "th cluster with QIndex " << corrs[corrIx][i][j].GetQIndex() << endl;
						cout << "Used to be: " << corrs[corrIx][i][j].IsForward() << endl;
						corrs[corrIx][i][j].SetForward(!corrs[corrIx][i][j].IsForward());
						cout << "Now is: " << corrs[corrIx][i][j].IsForward() << endl;
					}
				}
			}
		}


		void FlexibleFittingEngine::SaveCorrs(){
			savedCorrs = corrs;
		}
		void FlexibleFittingEngine::LoadSavedCorrs(){
			corrs = savedCorrs;
		}


		

	}





}

#endif