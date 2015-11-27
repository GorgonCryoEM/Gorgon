// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A clique based fitting tool which attempts to find semi-rigid correspondence between 2 pairs of secondary structure elements



#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_FINDER
#define PROTEINMORPH_SSE_CORRESPONDENCE_FINDER

#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>
#include <vector>
#include <Foundation/GorgonPriorityQueue.h>
#include "SSECorrespondenceFeature.h"
#include "SSECorrespondenceNode.h"
#include "SSECorrespondenceSearchNode.h"
#include "SSECorrespondenceSearchNode2.h"
#include <Foundation/GraphBase.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBHelix.h>


using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::GraphMatch;
using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		const int SSE_CORRESPONDENCE_METHOD_ASTAR_FULL_CLIQUE = 1;
		const int SSE_CORRESPONDENCE_METHOD_GREEDY_VALENCE_CLIQUE = 2;
		const int SSE_CORRESPONDENCE_METHOD_GREEDY_TRIANGLE_CLIQUE = 3;
		const int SSE_CORRESPONDENCE_METHOD_ASTAR_RIGID_COST_TRIANGLE_CLIQUE = 4;
		const int SSE_CORRESPONDENCE_METHOD_ASTAR_NEIGHBOR_COST_TRIANGLE_CLIQUE = 5;
		const int SSE_CORRESPONDENCE_METHOD_CLIQUES_FIRST_ASTAR_NEIGHBOR_COST_TRIANGLE_CLIQUE = 6;

		class SSECorrespondenceFinder {
		public:
			SSECorrespondenceFinder();
			void InitializeFeatures(vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2);
			void InitializeFeaturesFromPDBFiles(string file1, string file2, bool multipleSearch);
			void InitializeConstants(float rigidityThreshold, float featureChangeThreshold, float rigidityAngleCoeff, 
				float rigidityCentroidDistanceCoeff, float rigidityFeatureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff, 
				float jointAngleThreshold, float dihedralAngleThreshold, float centroidDistanceThreshold, 
				unsigned int maxSolutionCount, float maxCostMatrixError);		
			void PrintFeatureListsMathematica();
			vector< vector < vector<SSECorrespondenceNode> > > GetAStarCliqueBasedFeatureCorrespondence(bool printOutput, bool useDirection);
			vector< vector < vector<SSECorrespondenceNode> > > GetAStarTriangleBasedFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize);			
			vector< vector < vector<SSECorrespondenceNode> > > GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize);						
			vector< vector < vector<SSECorrespondenceNode> > > GetCliquesFirstAStarTriangleBasedCliqueDistanceFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize, int levelSizeTolerence);	
			vector < vector<SSECorrespondenceNode> > GetValenceBasedFeatureCorrespondence(bool printOutput, bool useDirection);
			vector < vector<SSECorrespondenceNode> > GetValenceBasedFeatureCorrespondence2(bool printOutput, bool useDirection);
			vector < vector<SSECorrespondenceNode> > GetValenceTriangleBasedFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize);
			vector < vector < vector<SSECorrespondenceNode> > > GetMultiCorrespondence(int method, bool useDirection, int smallestCliqueSize);
			void FindErrorMatrixBasedCorrespondence(vector< vector < vector<SSECorrespondenceNode> > > & correspondence, float maxError, int sampleCount);
			void PrintTimes();
			MatrixFloat GetTransform(vector<SSECorrespondenceNode> cluster, int sampleCount);
			MatrixFloat GetTransform(vector<Vector3DFloat>, vector<Vector3DFloat>, int sampleCount);

		private:
			float GetFeatureCompatibilityScore(SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2);
			float GetFeaturePairRigidityScore(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, bool isForward1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2, bool isForward2, bool useDirection);		
			inline void GetFeatureAngles(float & joint1, float & joint2, float & dihedral, SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2);
			inline void GetFeatureAngles(float & joint1, float & joint2, float & dihedral, Vector3DFloat p, Vector3DFloat pDir, Vector3DFloat q, Vector3DFloat qDir);
			vector< vector<float> > GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> GetAllNodes(vector< vector<float> > & featureCompatibilityScores, bool splitOnDirection);
			vector< vector<float> > GetAllNodePairCompatibilityScores(vector<SSECorrespondenceNode> & nodes, bool useDirection);
			inline bool IsFeaturePairRigid(SSECorrespondenceNode n1, SSECorrespondenceNode n2, bool useDirection);	
			inline bool IsFeaturePairRigid(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, bool isForward1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2, bool isForward2, bool useDirection);
			void AnalyzeCorrespondence(vector<unsigned int> pFeatures, vector<unsigned int> qFeatures);
			GraphBase<unsigned int, bool> RemoveCliqueAndRelatedNodes(GraphBase<unsigned int, bool> parentGraph, vector<unsigned long long> clique);		
			GraphBase<unsigned int, bool> CreateGraph(vector< vector<float> > & featureCompatibilityScores, vector<SSECorrespondenceNode> & nodes, bool useDirection);
			void GetCliqueFeatures(vector< set<unsigned long long> > & cliqueFeaturesP, vector< set<unsigned long long> > & cliqueFeaturesQ, vector< set<unsigned long long> > &allCliques, vector<SSECorrespondenceNode> &nodes);
		private:
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			void PrintGroundTruth(vector<PDBHelix> & h1, vector<PDBHelix> & h2, bool multipleSearch);
			float rigidityThreshold, featureChangeThreshold, rigidityAngleCoeff, rigidityCentroidDistanceCoeff, featureChangeCoeff, rigidComponentCoeff, intraComponentCoeff;
			float jointAngleThreshold, dihedralAngleThreshold, centroidDistanceThreshold, maxCostMatrixError;
			unsigned int maxSolutionCount;
			long long AtomToHash(char chainId, int seqNo);
			long long CorrespondenceToHash(int h1, int h2);
			void HashToCorrespondence(long long hash, int & h1, int & h2);
			TimeManager timeManager;
			int totalWatch, graphConstructionWatch, searchWatch, cleaningWatch;
		};

		SSECorrespondenceFinder::SSECorrespondenceFinder() {
			featureList1.clear();
			featureList2.clear();
			rigidityThreshold = 0.0f;
			featureChangeThreshold = 0.0f;
			rigidityAngleCoeff = 10.0f;
			rigidityCentroidDistanceCoeff = 1.0f;
			featureChangeCoeff = 1.0f;
			rigidComponentCoeff = 1.0f;
			intraComponentCoeff = 1.0f;
			maxSolutionCount = 5;
			totalWatch = timeManager.StartStopWatch();
			graphConstructionWatch = timeManager.StartStopWatch();
			timeManager.PauseStopWatch(graphConstructionWatch);
			searchWatch = timeManager.StartStopWatch();
			timeManager.PauseStopWatch(searchWatch);
			cleaningWatch = timeManager.StartStopWatch();
			timeManager.PauseStopWatch(cleaningWatch);
		}

		void SSECorrespondenceFinder::InitializeFeatures(vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2) {
			this->featureList1.clear();
			this->featureList2.clear();
			this->featureList1 = featureList1;
			this->featureList2 = featureList2;
		}

		long long SSECorrespondenceFinder::AtomToHash(char chainId, int seqNo) {
			return (long long)chainId * 100000 + (long long)seqNo;
		}

		long long SSECorrespondenceFinder::CorrespondenceToHash(int h1, int h2) {
			return (long long)h1 * 100000 + (long long)h2;
		}

		void SSECorrespondenceFinder::HashToCorrespondence(long long hash, int & h1, int & h2) {
			h2 = hash % 100000;
			h1 = hash / 100000;
		}

		void SSECorrespondenceFinder::PrintGroundTruth(vector<PDBHelix> & h1, vector<PDBHelix> & h2, bool multipleSearch) {
			map<long long, int> h1Map;
			map<long long, bool> corrMap;
			char chainId;
			for(unsigned int i = 0; i < h1.size(); i++) {
				chainId = h1[i].GetInitialResidueChainId();
				if(multipleSearch) {
					chainId = ' ';
				}
				for(unsigned int j = h1[i].GetInitialResidueSeqNo(); j <= h1[i].GetEndResidueSeqNo(); j++) {
					h1Map[AtomToHash(chainId, j)] = i;
				}				
			}
			long long hash;
			for(unsigned int i = 0; i < h2.size(); i++) {
				chainId = h2[i].GetInitialResidueChainId();
				if(multipleSearch) {
					chainId = ' ';
				}

				for(unsigned int j = h2[i].GetInitialResidueSeqNo(); j <= h2[i].GetEndResidueSeqNo(); j++) {
					hash = AtomToHash(chainId, j);
					if(h1Map.find(hash) != h1Map.end()) {
						corrMap[CorrespondenceToHash(h1Map[hash], i)] = true;
					}
				}				
			}

			printf("\n\ngroundTruth = {");
			bool first = true;
			int ha1, ha2;
			for(map<long long, bool>::iterator i = corrMap.begin(); i != corrMap.end(); i++) {
				HashToCorrespondence(i->first, ha1, ha2);
				if(!first) {
					printf(", ");
				}
				first = false;
				printf("{%d, %d}", ha1, ha2);
			}

			printf("};\n");



		}

		void SSECorrespondenceFinder::InitializeFeaturesFromPDBFiles(string file1, string file2, bool multipleSearch) {
			vector<PDBHelix> helices1 = PDBReader::ReadHelixPositions(file1);
			vector<PDBHelix> helices2 = PDBReader::ReadHelixPositions(file2);
			PrintGroundTruth(helices1, helices2, multipleSearch);

			for(unsigned int i = 0; i < helices1.size(); i++) {
				featureList1.push_back(SSECorrespondenceFeature(helices1[i].GetEndPosition(0), helices1[i].GetEndPosition(1)));
			}
			for(unsigned int i = 0; i < helices2.size(); i++) {
				featureList2.push_back(SSECorrespondenceFeature(helices2[i].GetEndPosition(0), helices2[i].GetEndPosition(1)));
			}
			helices1.clear();
			helices2.clear();
		}

		void SSECorrespondenceFinder::InitializeConstants(float rigidityThreshold, float featureChangeThreshold, float rigidityAngleCoeff, float rigidityCentroidDistanceCoeff, float featureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff, float jointAngleThreshold, float dihedralAngleThreshold, float centroidDistanceThreshold, unsigned int maxSolutionCount, float maxCostMatrixError) {
			this->rigidityThreshold = rigidityThreshold;
			this->featureChangeThreshold = featureChangeThreshold;
			this->rigidityAngleCoeff = rigidityAngleCoeff;
			this->rigidityCentroidDistanceCoeff = rigidityCentroidDistanceCoeff;
			this->featureChangeCoeff = featureChangeCoeff;
			this->rigidComponentCoeff = rigidComponentCoeff;
			this->intraComponentCoeff = intraComponentCoeff;
			this->jointAngleThreshold = jointAngleThreshold;
			this->dihedralAngleThreshold = dihedralAngleThreshold;
			this->centroidDistanceThreshold = centroidDistanceThreshold;
			this->maxSolutionCount = maxSolutionCount;
			this->maxCostMatrixError = maxCostMatrixError;
		}

		float SSECorrespondenceFinder::GetFeatureCompatibilityScore(SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2) {
			//return pow(feature1.Length() - feature2.Length(), 2);
			return abs(feature1.Length() - feature2.Length());
		}

		float SSECorrespondenceFinder::GetFeaturePairRigidityScore(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, bool isForward1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2, bool isForward2, bool useDirection) {
			float pJoint1, pJoint2, pDihedral, qJoint1, qJoint2, qDihedral;
			Vector3DFloat q1p, q1Dir, q2p, q2Dir;

			if(useDirection) {	
				GetFeatureAngles(pJoint1, pJoint2, pDihedral, p1.GetEndPoint(0), p1.GetFeatureVector(), p2.GetEndPoint(0), p2.GetFeatureVector());
				if(isForward1) {
					q1p = q1.GetEndPoint(0);
					q1Dir = q1.GetFeatureVector();
				} else {
					q1p = q1.GetEndPoint(1);
					q1Dir = -q1.GetFeatureVector();
				}

				if(isForward2) {
					q2p = q2.GetEndPoint(0);
					q2Dir = q2.GetFeatureVector();
				} else {
					q2p = q2.GetEndPoint(1);
					q2Dir = -q2.GetFeatureVector();
				}

				GetFeatureAngles(qJoint1, qJoint2, qDihedral, q1p, q1Dir, q2p, q2Dir);
			} else {
				GetFeatureAngles(pJoint1, pJoint2, pDihedral, p1, p2);
				GetFeatureAngles(qJoint1, qJoint2, qDihedral, q1, q2);
			}

			float angleDifference = abs(pJoint1 - qJoint1) + abs(pJoint2 - qJoint2) + abs(pDihedral - qDihedral);
			float centroidDistance = abs((p1.GetCentroid() - p2.GetCentroid()).Length() - (q1.GetCentroid() - q2.GetCentroid()).Length());

			return rigidityAngleCoeff * angleDifference + rigidityCentroidDistanceCoeff * centroidDistance;
		}

		void SSECorrespondenceFinder::GetFeatureAngles(float & joint1, float & joint2, float & dihedral, SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2) {
			Vector3DFloat f1 = Vector3DFloat::Normalize(feature1.GetFeatureVector());
			Vector3DFloat f2 = Vector3DFloat::Normalize(feature2.GetFeatureVector());
			Vector3DFloat c = Vector3DFloat::Normalize(feature1.GetCentroid() - feature2.GetCentroid());
			joint1 = acos(f1 * c);
			joint1 = min(joint1, (float)(PI-joint1));
			joint2 = acos(f2 * c);
			joint2 = min(joint2, (float)(PI-joint2));

			Vector3DFloat f1n = Vector3DFloat::Normalize(f1 - c * (f1*c));
			Vector3DFloat f2n = Vector3DFloat::Normalize(f2 - c * (f2*c));
			dihedral = acos(f1n * f2n);
			dihedral = min(dihedral, (float)(PI-dihedral));
		}

		void SSECorrespondenceFinder::GetFeatureAngles(float & joint1, float & joint2, float & dihedral, Vector3DFloat p, Vector3DFloat pDir, Vector3DFloat q, Vector3DFloat qDir) {

			Vector3DFloat f1 = Vector3DFloat::Normalize(pDir);
			Vector3DFloat f2 = Vector3DFloat::Normalize(qDir);
			Vector3DFloat c = Vector3DFloat::Normalize((p+p+pDir)*0.5 - (q+q+qDir)*0.5);
			joint1 = acos(f1 * c);
			joint2 = acos(f2 * c);

			Vector3DFloat f1n = Vector3DFloat::Normalize(f1 - c * (f1*c));
			Vector3DFloat f2n = Vector3DFloat::Normalize(f2 - c * (f2*c));
			dihedral = acos(f1n * f2n);
		}

		vector< vector<float> > SSECorrespondenceFinder::GetAllFeatureCompatibilityScores() {
			vector< vector<float> > scores;
			vector<float> scoreRow;
			for(unsigned int i = 0; i < featureList1.size(); i++) {
				scoreRow.clear();
				for(unsigned int j = 0; j < featureList2.size(); j++) {
					scoreRow.push_back(GetFeatureCompatibilityScore(featureList1[i], featureList2[j]));
				}
				scores.push_back(scoreRow);
			}

			//for(unsigned int i = 0; i < featureList1.size(); i++) {
			//	printf("%f %f %f\n", featureList1[i].Length(), featureList2[i].Length(), featureList1[i].Length()- featureList2[i].Length());
			//}
			return scores;
		}

		vector<SSECorrespondenceNode> SSECorrespondenceFinder::GetAllNodes(vector< vector<float> > & featureCompatibilityScores, bool splitOnDirection) {
			vector<SSECorrespondenceNode> nodes;
			for(unsigned int i = 0; i < featureList1.size(); i++) {
				for(unsigned int j = 0; j < featureList2.size(); j++) {
					if(featureCompatibilityScores[i][j] <= featureChangeThreshold) {
						nodes.push_back(SSECorrespondenceNode(i, j, true));
						if(splitOnDirection) {
							nodes.push_back(SSECorrespondenceNode(i, j, false));
						}
					}
				}
			}
			return nodes;
		}

		vector< vector<float> > SSECorrespondenceFinder::GetAllNodePairCompatibilityScores(vector<SSECorrespondenceNode> & nodes, bool useDirection) {
			vector< vector<float> > scores;

			vector<float> scoreRow;
			for(unsigned int i = 0; i < nodes.size(); i++) {
				scoreRow.clear();
				for(unsigned int j = 0; j < nodes.size(); j++) {
					scoreRow.push_back(0.0f);
				}
				scores.push_back(scoreRow);
			}

			unsigned int p1, p2, q1, q2;
			bool isForward1, isForward2;
			for(unsigned int i = 0; i < nodes.size()-1; i++) {
				p1 = nodes[i].GetPIndex();
				q1 = nodes[i].GetQIndex();
				isForward1 = nodes[i].IsForward();
				for(unsigned int j = i+1; j < nodes.size(); j++) {
					p2 = nodes[j].GetPIndex();
					q2 = nodes[j].GetQIndex();
					isForward2 = nodes[i].IsForward();
					scores[i][j] = GetFeaturePairRigidityScore(featureList1[p1], featureList2[q1], isForward1, featureList1[p2], featureList2[q2], isForward2, useDirection);
					scores[j][i] = scores[i][j];
				}
			}

			return scores;			
		}

		bool SSECorrespondenceFinder::IsFeaturePairRigid(SSECorrespondenceNode n1, SSECorrespondenceNode n2, bool useDirection) {
			return IsFeaturePairRigid(featureList1[n1.GetPIndex()], featureList2[n1.GetQIndex()], n1.IsForward(), featureList1[n2.GetPIndex()], featureList2[n2.GetQIndex()], n2.IsForward(), useDirection);			
		}

		bool SSECorrespondenceFinder::IsFeaturePairRigid(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, bool isForward1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2, bool isForward2, bool useDirection) {
			float pJoint1, pJoint2, pDihedral, qJoint1, qJoint2, qDihedral;
			bool retVal = true;						

			if(useDirection) {	
				GetFeatureAngles(pJoint1, pJoint2, pDihedral, p1.GetEndPoint(0), p1.GetFeatureVector(), p2.GetEndPoint(0), p2.GetFeatureVector());
				Vector3DFloat q1p, q1Dir, q2p, q2Dir;
				if(isForward1) {
					q1p = q1.GetEndPoint(0);
					q1Dir = q1.GetFeatureVector();
				} else {
					q1p = q1.GetEndPoint(1);
					q1Dir = -q1.GetFeatureVector();
				}

				if(isForward2) {
					q2p = q2.GetEndPoint(0);
					q2Dir = q2.GetFeatureVector();
				} else {
					q2p = q2.GetEndPoint(1);
					q2Dir = -q2.GetFeatureVector();
				}

				GetFeatureAngles(qJoint1, qJoint2, qDihedral, q1p, q1Dir, q2p, q2Dir);
			} else {
				GetFeatureAngles(pJoint1, pJoint2, pDihedral, p1, p2);
				GetFeatureAngles(qJoint1, qJoint2, qDihedral, q1, q2);
			}

			float centroidDistance = abs((p1.GetCentroid() - p2.GetCentroid()).Length() - (q1.GetCentroid() - q2.GetCentroid()).Length());

			return retVal && (abs(pJoint1 - qJoint1) <= jointAngleThreshold) && (abs(pJoint2 - qJoint2) <= jointAngleThreshold) && (abs(pDihedral - qDihedral) <= dihedralAngleThreshold) && (centroidDistance <= centroidDistanceThreshold);
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetAStarCliqueBasedFeatureCorrespondence(bool printOutput, bool useDirection) {
			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);
			vector< vector<float> > pairCompatibility = GetAllNodePairCompatibilityScores(nodes, useDirection);

			GraphBase<unsigned int, bool> parentGraph;

			for(unsigned int i = 0; i < nodes.size(); i++) {
				parentGraph.AddVertex(featureCompatibilityScores[nodes[i].GetPIndex()][nodes[i].GetQIndex()], i);
			}

			for(int i = 0; i < (int)nodes.size()-1; i++) {
				for(int j = i+1; j < (int)nodes.size(); j++) {
					if((nodes[i].GetPIndex() != nodes[j].GetPIndex()) && 
						(nodes[i].GetQIndex() != nodes[j].GetQIndex()) &&
						(i != j) && (pairCompatibility[i][j] <= rigidityThreshold)) {
						parentGraph.AddEdge(i, j, pairCompatibility[i][j], false);
					}
				}
			}
			vector< vector<unsigned int> > parentSolution;
			vector<unsigned int> parentSolutionElement;
			parentSolution.push_back(parentSolutionElement);

			SSECorrespondenceSearchNode * parentNode = new SSECorrespondenceSearchNode(parentGraph, parentSolution, 0.0f);			

			GorgonPriorityQueue<float, SSECorrespondenceSearchNode *> nodeQueue = GorgonPriorityQueue<float, SSECorrespondenceSearchNode *>(false);
			nodeQueue.Add(parentNode->GetCost(), parentNode);

			unsigned int solutionCount = 0;
			SSECorrespondenceSearchNode * currentNode;
			vector<SSECorrespondenceSearchNode *> childNodes;
			float currentCost;
			while((solutionCount < maxSolutionCount) && !nodeQueue.IsEmpty()) {
				nodeQueue.PopFirst(currentCost, currentNode);
				//currentNode->PrintSolution(nodes);	
				childNodes = currentNode->GetChildNodes(nodes, pairCompatibility, featureChangeCoeff, rigidComponentCoeff, intraComponentCoeff);
				if(childNodes.size() == 0) {
					//printf("Solution found: \t");
					if(printOutput) {
						currentNode->PrintSolution(nodes, useDirection);							
					}
					correspondence.push_back(currentNode->GetSolution(nodes));
					
					solutionCount++;
				} else {
					//printf("Child Nodes: \n");
					for(unsigned int i = 0; i < childNodes.size(); i++) {
						nodeQueue.Add(childNodes[i]->GetCost(), childNodes[i]);					
						//printf("\t");
						//childNodes[i]->PrintSolution(nodes);
					}
				}
				delete currentNode;
			}


			while(!nodeQueue.IsEmpty()) {
				nodeQueue.PopFirst(currentCost, currentNode);
				delete currentNode;
			}					
			
			return correspondence;
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetAStarTriangleBasedFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize) {
			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);

			GraphBase<unsigned int, bool> parentGraph = CreateGraph(featureCompatibilityScores, nodes, useDirection);

			vector< vector<unsigned int> > parentSolution;
			vector<unsigned int> parentSolutionElement;
			parentSolution.push_back(parentSolutionElement);

			SSECorrespondenceSearchNode * parentNode = new SSECorrespondenceSearchNode(parentGraph, parentSolution, 0.0f);			

			GorgonPriorityQueue<float, SSECorrespondenceSearchNode *> nodeQueue = GorgonPriorityQueue<float, SSECorrespondenceSearchNode *>(false);
			nodeQueue.Add(parentNode->GetCost(), parentNode);

			unsigned int solutionCount = 0;
			SSECorrespondenceSearchNode * currentNode;
			vector<SSECorrespondenceSearchNode *> childNodes;
			float currentCost;
			//TimeManager tm;
			while((solutionCount < maxSolutionCount) && !nodeQueue.IsEmpty()) {
				//tm.PushCurrentTime();
				nodeQueue.PopFirst(currentCost, currentNode);				
				//currentNode->PrintSolution(nodes, true, true);	
				childNodes = currentNode->GetChildNodesTriangleApprox(nodes, featureList1, featureList2, smallestCliqueSize);
				if(childNodes.size() == 0) {
					//printf("Solution found: \t");
					if(printOutput) {
						currentNode->PrintSolution(nodes, useDirection);							
					}
					correspondence.push_back(currentNode->GetSolution(nodes));
				
					solutionCount++;
				} else {
					//printf("Child Nodes: \n");
					for(unsigned int i = 0; i < childNodes.size(); i++) {
						nodeQueue.Add(childNodes[i]->GetCost(), childNodes[i]);						
						//printf("\t");
						//childNodes[i]->PrintSolution(nodes);
					}
				}
				//tm.PopAndDisplayTime("Time spent at node: %f \n");
				delete currentNode;
			}


			while(!nodeQueue.IsEmpty()) {
				nodeQueue.PopFirst(currentCost, currentNode);
				delete currentNode;
			}					
			
			return correspondence;
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize) {			

			//Graph Construction
			timeManager.ResumeStopWatch(graphConstructionWatch);
			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector < vector<SSECorrespondenceNode> > currSolution;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);
			GraphBase<unsigned int, bool> parentGraph = CreateGraph(featureCompatibilityScores, nodes, useDirection);
			timeManager.PauseStopWatch(graphConstructionWatch);

			timeManager.ResumeStopWatch(searchWatch);
			vector< vector<unsigned int> > parentSolution;
			vector<unsigned int> parentSolutionElement;
			parentSolution.push_back(parentSolutionElement);

			// Add constraints into this parent node, so that we dont search for those results.
			// when adding these constraints we must:
			// 1. Remove the corresponding vertices from the parentGraph
			// 2. Add the solution elements based on the constraints to the parentSolution

			SSECorrespondenceSearchNode * parentNode = new SSECorrespondenceSearchNode(parentGraph, parentSolution, 0.0f);			

			GorgonPriorityQueue<float, SSECorrespondenceSearchNode *> nodeQueue = GorgonPriorityQueue<float, SSECorrespondenceSearchNode *>(false);
			nodeQueue.Add(parentNode->GetCost(), parentNode);

			unsigned int solutionCount = 0;
			SSECorrespondenceSearchNode * currentNode;
			vector<SSECorrespondenceSearchNode *> childNodes;
			float currentCost;
			//TimeManager tm;
			bool first = true;
			while((solutionCount < maxSolutionCount) && !nodeQueue.IsEmpty()) {
				//tm.PushCurrentTime();
				nodeQueue.PopFirst(currentCost, currentNode);				
				//currentNode->PrintSolution(nodes, true, true);	
				childNodes = currentNode->GetChildNodesTriangleApproxCliqueDistance(nodes, featureList1, featureList2, (first?3:smallestCliqueSize));
				if(childNodes.size() == 0) {
					//printf("Solution found: \t");
					//if(printOutput) {
					//	currentNode->PrintSolution(nodes, useDirection);							
					//}
					
					timeManager.PauseStopWatch(searchWatch);
					timeManager.ResumeStopWatch(cleaningWatch);
					currentNode->FindErrorMatrixBasedCorrespondences(featureList1, featureList2, nodes, maxCostMatrixError);
					timeManager.PauseStopWatch(cleaningWatch);
					if(printOutput) {
						//printf("(*After cleaning*)\n");
						currentNode->PrintSolution(nodes, useDirection);		
						#ifdef _WIN32
							flushall();
						#endif
					}					
					
					correspondence.push_back(currentNode->GetSolution(nodes));				
					solutionCount++;

					timeManager.ResumeStopWatch(searchWatch);
				} else {
					//printf("Child Nodes: \n");
					for(unsigned int i = 0; i < childNodes.size(); i++) {
						nodeQueue.Add(childNodes[i]->GetCost(), childNodes[i]);						
						//printf("\t");
						//childNodes[i]->PrintSolution(nodes, true, true);
					}
				}
				//tm.PopAndDisplayTime("Time spent at node: %f \n");
				delete currentNode;
				first = false;
			}
			timeManager.PauseStopWatch(searchWatch);
			while(!nodeQueue.IsEmpty()) {
				nodeQueue.PopFirst(currentCost, currentNode);
				delete currentNode;
			}					
			
			return correspondence;
		}



		void SSECorrespondenceFinder::GetCliqueFeatures(vector< set<unsigned long long> > & cliqueFeaturesP, vector< set<unsigned long long> > &cliqueFeaturesQ, vector< set<unsigned long long> > &allCliques, vector<SSECorrespondenceNode> &nodes) {
			vector<unsigned long long> clique;
			set<unsigned long long> featuresP, featuresQ;
			
			for(unsigned long long i=0; i < allCliques.size(); i++) {
				clique = GraphBase<bool, bool>::VertexSetToVector(allCliques[i]);
				featuresP.clear();
				featuresQ.clear();
				for(unsigned long long j=0; j < clique.size(); j++) {
					featuresP.insert(nodes[clique[j]].GetPIndex());
					featuresQ.insert(nodes[clique[j]].GetQIndex());
				}
				cliqueFeaturesP.push_back(featuresP);
				cliqueFeaturesQ.push_back(featuresQ);
			}
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetCliquesFirstAStarTriangleBasedCliqueDistanceFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize, int levelSizeTolerence) {			
			TimeManager tm;
			tm.PushCurrentTime();

			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);

			GraphBase<unsigned int, bool> parentGraph = CreateGraph(featureCompatibilityScores, nodes, useDirection);

			tm.PopAndDisplayTime("(*Graph constructed %f seconds!*)\n");
			
			tm.PushCurrentTime();
			vector< set<unsigned long long> > allCliques = parentGraph.GetAllCliquesTriangleApprox(smallestCliqueSize);
			printf("(*%d cliques found ", (int)allCliques.size());
			tm.PopAndDisplayTime("%f seconds!*)\n");

			tm.PushCurrentTime();
			vector< set<unsigned long long> > cliqueFeaturesP;
			vector< set<unsigned long long> > cliqueFeaturesQ;
			GetCliqueFeatures(cliqueFeaturesP, cliqueFeaturesQ, allCliques, nodes);

			GorgonPriorityQueue<float, SSECorrespondenceSearchNode2 *> queue = GorgonPriorityQueue<float, SSECorrespondenceSearchNode2 *>(false);

			SSECorrespondenceSearchNode2 * node = new SSECorrespondenceSearchNode2();
			// Add constraints into this search node.
			queue.Add(0, node);

			float firstCost;
			vector<SSECorrespondenceSearchNode2 *> childNodes;
			int solutionCount = 0;
			while((solutionCount < maxSolutionCount) && !queue.IsEmpty()) {
				queue.PopFirst(firstCost, node);
				childNodes = node->GetChildNodes(allCliques, cliqueFeaturesP, cliqueFeaturesQ, levelSizeTolerence);
				if(childNodes.size() == 0) { // Solution Found!
					tm.PopAndDisplayTime("(*Solution found %f seconds!*)\n");
					tm.PushCurrentTime();
					if(printOutput) {
						node->PrintSolution(allCliques, nodes, useDirection);							
						printf("(*cost = %f*)\n", firstCost);
					}
					//correspondence.push_back(currentNode->GetSolution(nodes));
				
					solutionCount++;					

				}

				for(unsigned int i = 0; i < childNodes.size(); i++) {					
					queue.Add(childNodes[i]->GetCost(firstCost, allCliques, nodes, featureList1, featureList2), childNodes[i]);
				}

				delete node;
			}

			tm.PopAndDisplayTime("(*%f seconds!*)\n");


			return correspondence;
		}


		vector < vector<SSECorrespondenceNode> > SSECorrespondenceFinder::GetValenceBasedFeatureCorrespondence(bool printOutput, bool useDirection) {
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);
			vector< vector<float> > pairCompatibility = GetAllNodePairCompatibilityScores(nodes, useDirection);

			GraphBase<unsigned int, bool> graph;

			for(unsigned int i = 0; i < nodes.size(); i++) {
				graph.AddVertex(featureCompatibilityScores[nodes[i].GetPIndex()][nodes[i].GetQIndex()], i);
			}

			for(int i = 0; i < (int)nodes.size()-1; i++) {
				for(int j = i+1; j < (int)nodes.size(); j++) {
					if((nodes[i].GetPIndex() != nodes[j].GetPIndex()) && 
						(nodes[i].GetQIndex() != nodes[j].GetQIndex()) &&
						IsFeaturePairRigid(nodes[i], nodes[j], useDirection)) {						
						graph.AddEdge(i, j, pairCompatibility[i][j], false);
					}
				}
			}

			vector< vector<unsigned int> > sol2;
			SSECorrespondenceSearchNode node = SSECorrespondenceSearchNode(graph, sol2, 0.0f);

			unsigned long long vertexIx;
			vector<unsigned long long> neighbors;
			while(graph.GetVertexCount() > 0) {				
				vertexIx = graph.GetHighestValenceVertexIx();
				vector<unsigned long long> clique = graph.VertexSetToVector(graph.GetLowestCostCliqueInOneRing(vertexIx));
				vector<unsigned int> cliqueSol;
				for(unsigned int i = 0; i < clique.size(); i++) {
					cliqueSol.push_back(graph.GetVertex(clique[i]).GetTag());
				}
				vector< vector<unsigned int> > sol = node.GetSolution();
				sol.push_back(cliqueSol);
				node = SSECorrespondenceSearchNode(node.GetChildGraph(nodes, clique), sol, 0);
				graph = node.GetGraph();
			}
			if(printOutput) {
				node.PrintSolution(nodes, useDirection);
			}
			return node.GetSolution(nodes);
		}

		vector < vector<SSECorrespondenceNode> > SSECorrespondenceFinder::GetValenceTriangleBasedFeatureCorrespondence(bool printOutput, bool useDirection, int smallestCliqueSize) {
			TimeManager m;
			m.PushCurrentTime();
			m.PushCurrentTime();
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores, useDirection);
			m.PopAndDisplayTime("(*Created Nodes :%f s*)\n");
			m.PushCurrentTime();
			m.PushCurrentTime();
			GraphBase<unsigned int, bool> graph = CreateGraph(featureCompatibilityScores, nodes, useDirection);

			m.PopAndDisplayTime("(*Create base graph:%f s*)\n");

			vector< vector<unsigned int> > sol2;
			SSECorrespondenceSearchNode node = SSECorrespondenceSearchNode(graph, sol2, 0.0f);

			vector<unsigned long long> neighbors;
			bool found = true;
			while(found && (graph.GetVertexCount() > 0)) {				
				//m.PushCurrentTime();
				vector<unsigned long long> clique = graph.VertexSetToVector(graph.GetLowestCostCliqueTriangleApprox(smallestCliqueSize));
				found = (clique.size() > 0);
				if(found) {
					vector<unsigned int> cliqueSol;
					for(unsigned int i = 0; i < clique.size(); i++) {
						cliqueSol.push_back(graph.GetVertex(clique[i]).GetTag());
					}
					vector< vector<unsigned int> > sol = node.GetSolution();
					sol.push_back(cliqueSol);
					node = SSECorrespondenceSearchNode(node.GetChildGraph(nodes, clique), sol, 0);
					graph = node.GetGraph();
				}
			}			
			if(printOutput) {
				node.PrintSolution(nodes, useDirection);
			}
			m.PopAndDisplayTime("(*Done!:%f s*)\n");
			return node.GetSolution(nodes);
		}

		vector < vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetMultiCorrespondence(int method, bool useDirection, int smallestCliqueSize) {
			maxSolutionCount = 1;
			vector<SSECorrespondenceNode> corrItemItem;
			vector < vector<SSECorrespondenceNode> > corrItem, newCorrItem;
			vector< vector < vector<SSECorrespondenceNode> > > corr, corrSet;
			bool found = true;

			vector<unsigned int> featureList2Indices;
			for(unsigned int i = 0; i < featureList2.size(); i++) {
				featureList2Indices.push_back(i);
			}

			while (found) {
				found = false;
				if(featureList2.size() > 0) {
					switch(method) {
						case SSE_CORRESPONDENCE_METHOD_GREEDY_TRIANGLE_CLIQUE:
							corrItem = GetValenceTriangleBasedFeatureCorrespondence(false, useDirection, smallestCliqueSize);
						case SSE_CORRESPONDENCE_METHOD_GREEDY_VALENCE_CLIQUE:
							corrItem = GetValenceBasedFeatureCorrespondence(false, useDirection);					
						case SSE_CORRESPONDENCE_METHOD_ASTAR_NEIGHBOR_COST_TRIANGLE_CLIQUE:
							corrSet = GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, useDirection, smallestCliqueSize);							
							corrItem = corrSet[0];

					} 
					found = (corrItem.size() > 0);
					if(found) {
						newCorrItem.clear();

						for(unsigned int i = 0; i < corrItem.size(); i++) {
							corrItemItem.clear();
							for(unsigned int j = 0; j < corrItem[i].size(); j++) {
								corrItemItem.push_back(SSECorrespondenceNode(corrItem[i][j].GetPIndex(), featureList2Indices[corrItem[i][j].GetQIndex()], corrItem[i][j].IsForward()));
							}
							newCorrItem.push_back(corrItemItem);
						}
						corr.push_back(newCorrItem);

						bool remove;
						for(int i = (int)featureList2.size()-1;  i >= 0; i--) {
							remove = false;
							for(unsigned int j = 0; !remove && (j < corrItem.size()); j++) {
								for(unsigned int k = 0; !remove && (k < corrItem[j].size()); k++) {
									remove = remove || (corrItem[j][k].GetQIndex() == i);
								}
							}
							if(remove) {
								featureList2.erase(featureList2.begin() + i);
								featureList2Indices.erase(featureList2Indices.begin() + i);	
							}
						}
					}
				}
			}


			printf("corrs = {\n");
			for(unsigned int k = 0; k < corr.size(); k++) {
				corrItem = corr[k];
				if(k != 0) {
					printf(",\n");
				}
				printf("\t{\n");

				for(unsigned int i = 0; i < corrItem.size(); i++) {
					if(i != 0) {
						printf(",\n");
					}
					printf("\t\t{");
					for(unsigned int j = 0; j < corrItem[i].size(); j++) {
						if(j != 0) {
							printf(", ");
						}
						if(useDirection) {
							if(corrItem[i][j].IsForward()) {
								printf("{{%d, %d}, {%d, %d}}", corrItem[i][j].GetPIndex(), 0, corrItem[i][j].GetQIndex(), 0);
							} else {
								printf("{{%d, %d}, {%d, %d}}", corrItem[i][j].GetPIndex(), 0, corrItem[i][j].GetQIndex(), 1);
							}
						} else {
							printf("{%d, %d}", corrItem[i][j].GetPIndex(), corrItem[i][j].GetQIndex());
						}
					}
					printf("}");
				}
				printf("}");
			}
			printf("};\n");			
			printf("printFinalMultiOutput[corrs, fl1, fl2, 1, groundTruth]\n");

			return corr;
		}

		void SSECorrespondenceFinder::PrintFeatureListsMathematica() {
			Vector3DFloat pt0, pt1;
			printf("fl1 = {\n");
			for(unsigned int i = 0; i < featureList1.size(); i++) {
				pt0 = featureList1[i].GetEndPoint(0);
				pt1 = featureList1[i].GetEndPoint(1);
				if(i != 0) {
					printf(",\n");
				}
				printf("\t{{%f, %f, %f}, {%f, %f, %f}}", pt0.X(), pt0.Y(), pt0.Z(), pt1.X(), pt1.Y(), pt1.Z());
			}
			printf("};\n\n");

			printf("fl2 = {\n");
			for(unsigned int i = 0; i < featureList2.size(); i++) {
				pt0 = featureList2[i].GetEndPoint(0);
				pt1 = featureList2[i].GetEndPoint(1);
				if(i != 0) {
					printf(",\n");
				}
				printf("\t{{%f, %f, %f}, {%f, %f, %f}}", pt0.X(), pt0.Y(), pt0.Z(), pt1.X(), pt1.Y(), pt1.Z());
			}
			printf("};\n\n");
		}


		GraphBase<unsigned int, bool> SSECorrespondenceFinder::CreateGraph(vector< vector<float> > & featureCompatibilityScores, vector<SSECorrespondenceNode> & nodes, bool useDirection) {
			GraphBase<unsigned int, bool> parentGraph;

			for(unsigned int i = 0; i < nodes.size(); i++) {
				parentGraph.AddVertex(featureCompatibilityScores[nodes[i].GetPIndex()][nodes[i].GetQIndex()], i);
			}

			for(int i = 0; i < (int)nodes.size()-1; i++) {
				for(int j = i+1; j < (int)nodes.size(); j++) {
					if((nodes[i].GetPIndex() != nodes[j].GetPIndex()) && 
						(nodes[i].GetQIndex() != nodes[j].GetQIndex()) &&
						IsFeaturePairRigid(nodes[i], nodes[j], useDirection)) {						
							parentGraph.AddEdge(i, j,  GetFeaturePairRigidityScore( featureList1[nodes[i].GetPIndex()], featureList2[nodes[i].GetQIndex()], nodes[i].IsForward(), featureList1[nodes[j].GetPIndex()], featureList2[nodes[j].GetQIndex()], nodes[j].IsForward(), useDirection), false);
					}
				}
			}
			return parentGraph;
		}

		void SSECorrespondenceFinder::PrintTimes() {
			timeManager.PauseStopWatch(totalWatch);

			timeManager.DisplayStopWatch(graphConstructionWatch, "(*  %fs Graph construction\n");
			timeManager.DisplayStopWatch(searchWatch, "    %fs A* Search\n");
			timeManager.DisplayStopWatch(cleaningWatch, "    %fs Cost matrix based cleaning\n");
			timeManager.DisplayStopWatch(totalWatch, "  %fs Total application execution*)\n");
		}

		MatrixFloat SSECorrespondenceFinder::GetTransform(vector<SSECorrespondenceNode> cluster, int sampleCount) {
			if(cluster.size() <= 1) {
				return MatrixFloat(4,4);
			}
			vector<Vector3DFloat> fl1, fl2;
			fl1.clear();
			fl2.clear();
			Vector3DFloat p1, p2, q1, q2;
			Vector3DFloat sp, sq;

			for(unsigned int i = 0; i < cluster.size(); i++) {
				p1 = featureList1[cluster[i].GetPIndex()].GetEndPoint(0);
				p2 = featureList1[cluster[i].GetPIndex()].GetEndPoint(1);
				
				q1 = featureList2[cluster[i].GetQIndex()].GetEndPoint(cluster[i].IsForward()?0:1);
				q2 = featureList2[cluster[i].GetQIndex()].GetEndPoint(cluster[i].IsForward()?1:0);

				//printf("Transforming to SSE index %d with corner #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", cluster[i].GetQIndex(), q1.X(), q1.Y(), q1.Z(), q2.X(), q2.Y(), q2.Z());

				float offset;
				for(int j = 0; j < sampleCount; j++) {
					offset = (float)j / (float)(sampleCount - 1);

					sp = p1*(1.0f - offset) + p2 * offset;
					sq = q1*(1.0f - offset) + q2 * offset;
					fl1.push_back(sp);
					fl2.push_back(sq);					
				}

				
			}

			MatrixFloat result = LinearSolver::FindRotationTranslation(fl1, fl2);
			
			for(unsigned int i = 0; i < cluster.size(); i++) {
				p1 = featureList1[cluster[i].GetPIndex()].GetEndPoint(0);
				p2 = featureList1[cluster[i].GetPIndex()].GetEndPoint(1);

				Vector3DFloat p1trans = p1.Transform(result);
				Vector3DFloat p2trans = p2.Transform(result);

				//printf("Transforming PDB Helix to SSE index %d with end #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", cluster[i].GetQIndex(), p1trans.X(), p1trans.Y(), p1trans.Z(), p2trans.X(), p2trans.Y(), p2trans.Z());
			}

				



			return result;

			// IF the cluster.size() == 1, then we need to find the rotation translation which is a mirror of what we just got, and then pick the one with the
			// lowest error based on oriented angle differences.
		}


		MatrixFloat SSECorrespondenceFinder::GetTransform(vector<Vector3DFloat> originalLocs, vector<Vector3DFloat> newLocs, int sampleCount) {

			vector<Vector3DFloat> fl1, fl2;
			fl1.clear();
			fl2.clear();
			Vector3DFloat p1, p2, q1, q2;
			Vector3DFloat sp, sq;

			//printf("Transforming to SSE index %d with corner #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", cluster[i].GetQIndex(), q1.X(), q1.Y(), q1.Z(), q2.X(), q2.Y(), q2.Z());
			for(unsigned int i = 0; i < originalLocs.size()-1; ++i){
				p1 = originalLocs[i];
				p2 = originalLocs[i+1];
				q1 = newLocs[i];
				q2 = newLocs[i+1];
				float offset;
				for(int j = 0; j < sampleCount; j++) {
					offset = (float)j / (float)(sampleCount - 1);
					sp = p1*(1.0f - offset) + p2 * offset;
					sq = q1*(1.0f - offset) + q2 * offset;
					fl1.push_back(sp);
					fl2.push_back(sq);				
				}
			}

			MatrixFloat result = LinearSolver::FindRotationTranslation(fl1, fl2);

			return result;

		}
		

	}
}


#endif
