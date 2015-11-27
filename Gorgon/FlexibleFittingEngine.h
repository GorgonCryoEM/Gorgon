// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying skeletons



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
			vector<Vector3DFloat> Deform(int, MatrixFloat, bool);
			vector<float> returnFlattenedLocations(vector<Vector3DFloat>);
			void addAtomLocation(Vector3DFloat);
			void addHardHandleLocation(Vector3DFloat);
			void addSoftHandleLocation(Vector3DFloat);
			void prepareDeform();
			void setLaplacainW(float);
			void setHardHandleW(float);
			void setSoftHandleW(float);
			void addHelix(int start, int stop);
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
			MatrixFloat GetPairRigidTransform(int corrIx, int helixIx1, int helixIx2);
			void GetHelixEnds(int corrIx, int helixIx, Vector3DFloat&, Vector3DFloat&);
			bool GetIsForward(int corrIx, int helixIx);
			MatrixFloat GetSideChainTransform(vector<Vector3DFloat> origLocations, vector<Vector3DFloat> newLocations);
		private:			
			void BuildSortedHelixDistances(int );

			static const int SAMPLE_COUNT = 10;
			SSECorrespondenceFinder finder;
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			vector<Vector3DFloat> pdbHelixAtomLocs;
			float jointAngleThreshold, dihedralAngleThreshold, lengthThreshold, centroidDistanceThreshold;
			vector<Vector3DFloat> origLocations;
			vector<Vector3DFloat> hardHandles;
			vector<Vector3DFloat> softHandles;
			CurveDeformer cd;
			vector < vector < vector<SSECorrespondenceNode> > > corrs;
			vector<boost::tuple<int, int, int> > helixDistancesSorted;

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
			hardHandles.clear();
			softHandles.clear();
			origLocations.clear();
			helixDistancesSorted.clear();
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
			//Creates a correspondence between SSE elements SSEIx and PDBIx, removing them from their 
			//current cluster and placing them in their own.
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
			//Merges clusters given their index in corrs

			vector < vector<SSECorrespondenceNode> >  newCorrs;

			//creating new cluster
			vector<SSECorrespondenceNode> newCluster;
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster1].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster1][i]);
			}
			for(unsigned int i = 0; i < corrs[alignmentIx][cluster2].size(); ++i){
				newCluster.push_back(corrs[alignmentIx][cluster2][i]);
			}
			
			//copying old clusters.
			for(unsigned int i = 0; i < corrs[alignmentIx].size(); ++i){
				if(i != cluster1 && i != cluster2){
					newCorrs.push_back(corrs[alignmentIx][i]);
				}else if(i == cluster1){//new cluster retains same index as cluster1
					newCorrs.push_back(newCluster);
				}
			}
			
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
			for(unsigned int i = 1; i < corrs[corrIx].size(); i++) {
				if(corrs[corrIx][i].size() > corrs[corrIx][maxIndex].size()) {
					maxIndex = i;
				}
			}
			return finder.GetTransform(corrs[corrIx][maxIndex], SAMPLE_COUNT);			
		}

		//Gets a rigid transform given the alignment we are working with and the cluster #
		MatrixFloat FlexibleFittingEngine::GetRigidTransform2(int corrIx, int clusterIx) {
			//If of size one we can handle it
			if(corrs[corrIx][clusterIx].size() > 1){
				return finder.GetTransform(corrs[corrIx][clusterIx], SAMPLE_COUNT);	
			}
			else{
				vector<boost::tuple<int, int> > PQMatchings;
				if(helixDistancesSorted.size() == 0){
					BuildSortedHelixDistances(corrIx);
				}			
				
				int helixIx = corrs[corrIx][clusterIx][0].GetPIndex();//index of 0 because only one element
				
				//creating a temporary cluster so we can use SSECorrespondenceFinder to get transform
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

		MatrixFloat FlexibleFittingEngine::GetSideChainTransform(vector<Vector3DFloat> origLocations, vector<Vector3DFloat> newLocations){
			/*cout << "Going from " ; origLocations[0].Print();			
			cout << " and "; origLocations[origLocations.size()-1].Print();
			cout << "Going to " ; newLocations[0].Print();			
			cout << " and "; newLocations[newLocations.size()-1].Print(); cout << endl;*/
			return finder.GetTransform(origLocations, newLocations, SAMPLE_COUNT);
		}

		//finds a transform to line a PDB helix up exactly with an SSE Helix
		MatrixFloat FlexibleFittingEngine::GetHelixFlexibleTransform(int corrIx, int helixIx) {
			int corrNum = -1;
			int qIndex = -1;
			bool flip = false;

			//These will be combined into final transformation
			MatrixFloat clusterTrans = MatrixFloat(4,4);
			MatrixFloat translationTrans = MatrixFloat(4,4);
			MatrixFloat rotationTrans = MatrixFloat(4,4);
			MatrixFloat negCentroidTrans = MatrixFloat(4,4);
			MatrixFloat posCentroidTrans= MatrixFloat(4,4);

			//obtaining relevant indices and info
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					if(corrs[corrIx][i][j].GetPIndex() == helixIx) {
						qIndex = corrs[corrIx][i][j].GetQIndex();
						flip = !corrs[corrIx][i][j].IsForward();
						if(corrs[corrIx][i].size() > 1){
							clusterTrans = finder.GetTransform(corrs[corrIx][i], SAMPLE_COUNT);
						}
						else{
							corrNum = i;
							break;
						}
					}
				}
			}
			//if we didnt find cluster or the cluster transform is bad return identity matrix
			if((corrNum == -1 && clusterTrans.GetValue(3,3) < 0.5) || qIndex == -1){ //doesn't have a correspondence
				MatrixFloat result = MatrixFloat(4,4);
				return result;
			}

			if(helixDistancesSorted.size() == 0){
				BuildSortedHelixDistances(corrIx);
			}
			
			//cluster of size one, have to build a temp cluster and find its transform
			if(clusterTrans.GetValue(3,3) == 0.0){
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
			}
			

			//need the translation between centroid and origin in order to do a rotation
			Vector3DFloat beforeTrans = featureList1[helixIx].GetCentroid();
			Vector3DFloat center1 = beforeTrans.Transform(clusterTrans);
			Vector3DFloat center2 = featureList2[qIndex].GetCentroid();
			Vector3DFloat diff = center2 - center1;			
			translationTrans.SetValue(diff[0], 0, 3);	translationTrans.SetValue(diff[1], 1, 3);	translationTrans.SetValue(diff[2], 2, 3);
			translationTrans.SetValue(1,0,0);	translationTrans.SetValue(1,1,1);	translationTrans.SetValue(1,2,2);	translationTrans.SetValue(1,3,3);


			//applying the transforms so far in order to get correct rotation trans
			MatrixFloat trans = translationTrans * clusterTrans;

			//Calculating rotation transform
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
			//R is desired rotation matrix, A is original locations, B is locations after rotation
			//A*R = B
			//R = A^-1*B
			//A = I
			//R = B
			Vector3DFloat row1 = Vector3DFloat(1.0,0.0,0.0);	row1 = row1.Rotate(normal,angle);
			Vector3DFloat row2 = Vector3DFloat(0.0,1.0,0.0);	row2 = row2.Rotate(normal,angle);
			Vector3DFloat row3 = Vector3DFloat(0.0,0.0,1.0);	row3 = row3.Rotate(normal,angle);
			rotationTrans.SetValue(row1[0],0,0);	rotationTrans.SetValue(row1[1],0,1);	rotationTrans.SetValue(row1[2],0,2);			
			rotationTrans.SetValue(row2[0],1,0);	rotationTrans.SetValue(row2[1],1,1);	rotationTrans.SetValue(row2[2],1,2);			
			rotationTrans.SetValue(row3[0],2,0);	rotationTrans.SetValue(row3[1],2,1);	rotationTrans.SetValue(row3[2],2,2);			
			rotationTrans.SetValue(1,3,3);

			//Calculating transform back from origin to original location
			Vector3DFloat centroid = (end1+end2)*0.5;
			negCentroidTrans.SetValue(-centroid[0],0,3);	negCentroidTrans.SetValue(-centroid[1],1,3);	negCentroidTrans.SetValue(-centroid[2],2,3);
			negCentroidTrans.SetValue(1,0,0);	negCentroidTrans.SetValue(1,1,1);	negCentroidTrans.SetValue(1,2,2);	negCentroidTrans.SetValue(1,3,3);

			posCentroidTrans.SetValue(centroid[0],0,3);	posCentroidTrans.SetValue(centroid[1],1,3);	posCentroidTrans.SetValue(centroid[2],2,3);
			posCentroidTrans.SetValue(1,0,0);	posCentroidTrans.SetValue(1,1,1);	posCentroidTrans.SetValue(1,2,2);	posCentroidTrans.SetValue(1,3,3);

			//Multiply them in the reverse order that they would have been applied to a point to get final matrix
			return posCentroidTrans*rotationTrans*negCentroidTrans*translationTrans*clusterTrans;
		}

		//for each pdb helix finds and stores the 3 closest pdb helices by centroid
		//stored in helixDistancesSorted
		void FlexibleFittingEngine::BuildSortedHelixDistances(int corrIx){
			vector<boost::tuple<int, int> > PQMatchings;
			
			//saving which SSE elements correspond
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					PQMatchings.push_back(boost::tuple<int,int>(corrs[corrIx][i][j].GetPIndex(), corrs[corrIx][i][j].GetQIndex()));
				}
			}	

			//calculating distances bewteen elements and saving closest 3 distances and indices
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
				helixDistancesSorted.push_back(boost::tuple<int, int, int>(ind1, ind2, ind3));
			}			

		}

		

		void FlexibleFittingEngine::GetHelixEnds(int corrIx, int helixIx, Vector3DFloat& end1, Vector3DFloat& end2){
			end1 = featureList1[helixIx].GetEndPoint(0);
			end2 = featureList1[helixIx].GetEndPoint(1);
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

		//gets a transformation matrix to align the PDB rigidly to helixIx1 and helixIx2
		//used for rigid init
		MatrixFloat FlexibleFittingEngine::GetPairRigidTransform(int corrIx, int helixIx1, int helixIx2){
			if(helixIx1 == helixIx2){
				for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
					for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
						if(corrs[corrIx][i][j].GetPIndex() == helixIx1){
							cout << "returning cluster transform for cluster" << i << " with helixIx " << helixIx1 << endl;
							return GetRigidTransform2(corrIx, i);
						}
					}
				}
				
			}
			vector<SSECorrespondenceNode> tempCluster;
			for(unsigned int i = 0; i < corrs[corrIx].size(); i++) {
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); j++) {
					int p = corrs[corrIx][i][j].GetPIndex();
					if(p == helixIx1 || p == helixIx2){
						tempCluster.push_back(corrs[corrIx][i][j]);
					}
				}
			}

			return finder.GetTransform(tempCluster, SAMPLE_COUNT);
			
		}

		//runs the actual flexible deformation
		vector<Vector3DFloat> FlexibleFittingEngine::Deform(int numNeighbors, MatrixFloat transform, bool rigidInitialization){
			if(transform.GetValue(3,3) > 0/5 && rigidInitialization){
				for(int i =0; i < origLocations.size(); ++i){
					origLocations[i] = origLocations[i].Transform(transform);
				}
			}
			//if(rigidInitialization)
			//	return origLocations;
			//else
				return cd.Deform(origLocations, hardHandles, softHandles, numNeighbors);
		}

		//flattens a vector of Vector3DFloat into a vector of floats where every 3 represent a point
		vector<float> FlexibleFittingEngine::returnFlattenedLocations(vector<Vector3DFloat> in){
			vector<float> result;
			for(int i = 0; i < in.size(); ++i){
				result.push_back(in[i].X());
				result.push_back(in[i].Y());
				result.push_back(in[i].Z());
			}
			return result;
		}

		//adds an atom location used in the flexible deformation
		void FlexibleFittingEngine::addAtomLocation(Vector3DFloat in){
			origLocations.push_back(in);
		}

		//adds a hard handle for the flexible deformation
		void FlexibleFittingEngine::addHardHandleLocation(Vector3DFloat in){
			hardHandles.push_back(in);
		}

		//adds a soft handle for the flexible deformation
		void FlexibleFittingEngine::addSoftHandleLocation(Vector3DFloat in){
			softHandles.push_back(in);
		}

		//resets the part of the engine needed for doing flexible fitting
		void FlexibleFittingEngine::prepareDeform(){
			hardHandles.clear();
			softHandles.clear();
			origLocations.clear();
		}


		//Sets weights for flexible deformation
		void FlexibleFittingEngine::setLaplacainW(float in){
			cd.setLaplacainW(in);
		}
		void FlexibleFittingEngine::setHardHandleW(float in){
			cd.setHardHandleW(in);
		}
		void FlexibleFittingEngine::setSoftHandleW(float in){
			cd.setSoftHandleW(in);
		}

		//returns a vector arranged by QIndex as to whether or not that SSE element and its corresponding PDB are 'isforward' or not
		//according to the corrs data structure
		vector<bool> FlexibleFittingEngine::GetCorrespondenceFlips(int corrIx){
			vector<bool> result(featureList2.size());
			cout << "Size of result: " << result.size() << endl;
			cout << "Size of corrs: " << corrs[corrIx].size() << endl;
			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					result[corrs[corrIx][i][j].GetQIndex()] = corrs[corrIx][i][j].IsForward();
				}
			}
			return result;
		}


		//returns a vector containing only its
		//every pair of two numbers represents the first number (PDB index) and the second (SSE index)
		//are paired in corrs
		vector<int> FlexibleFittingEngine::GetAllCorrespondencesFlat(int corrIx){
			vector<int> result;
			for(unsigned int i = 0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j = 0; j < corrs[corrIx][i].size(); ++j){
					result.push_back(corrs[corrIx][i][j].GetPIndex());
					result.push_back(corrs[corrIx][i][j].GetQIndex());
				}
			}
			return result;
		}
		
		//returns the endpoints of the features in featueList1 as pairs of Vector3DFloats
		vector<Vector3DFloat> FlexibleFittingEngine::GetAllCAlphaFeatureVecsFlat(){
			vector<Vector3DFloat> result;
			for(unsigned int i = 0; i < featureList1.size(); ++i){
				result.push_back(featureList1[i].GetEndPoint(0));
				result.push_back(featureList1[i].GetEndPoint(1));
			}			
			return result;
		}

		//Flips the orientation of a PDB SSE Match in corrs
		void FlexibleFittingEngine::FlipCorrespondencePair(int corrIx, int SSEIndex){
			for(unsigned int i=0; i < corrs[corrIx].size(); ++i){
				for(unsigned int j=0; j < corrs[corrIx][i].size(); ++j){
					if(corrs[corrIx][i][j].GetQIndex() == SSEIndex){
						cout << "Flipping # " << j << " in " << i << "the cluster with QIndex " << corrs[corrIx][i][j].GetQIndex() << endl;
						cout << "Used to be: " << corrs[corrIx][i][j].IsForward() << endl;
						corrs[corrIx][i][j].SetForward(!corrs[corrIx][i][j].IsForward());
						cout << "Now is: " << corrs[corrIx][i][j].IsForward() << endl;
					}
				}
			}
		}

		//loads and saves 'corrs'.  Necessary because they get destroyed when a search is run and searchs have to be run
		void FlexibleFittingEngine::SaveCorrs(){
			savedCorrs = corrs;
		}
		void FlexibleFittingEngine::LoadSavedCorrs(){
			corrs = savedCorrs;
		}		

	}

}

#endif
