// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A clique based fitting tool which attempts to find semi-rigid correspondence between 2 pairs of secondary structure elements

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.3  2009/09/29 19:23:39  ssa1
//   Fixing indexing bugs when performing sse correspondence search.
//
//   Revision 1.2  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.1  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//


#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_FINDER
#define PROTEINMORPH_SSE_CORRESPONDENCE_FINDER

#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>
#include <vector>
#include <SkeletonMaker/PriorityQueue.h>
#include "SSECorrespondenceFeature.h"
#include "SSECorrespondenceNode.h"
#include "SSECorrespondenceSearchNode.h"
#include <Foundation/GraphBase.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBHelix.h>


using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::GraphMatch;
using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceFinder {
		public:
			SSECorrespondenceFinder();
			void InitializeFeatures(vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2);
			void InitializeFeaturesFromPDBFiles(string file1, string file2);
			void InitializeConstants(float rigidityThreshold, float featureChangeThreshold, float rigidityAngleCoeff, float rigidityCentroidDistanceCoeff, float rigidityFeatureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff, unsigned int maxSolutionCount);		
			vector< vector < vector<SSECorrespondenceNode> > > GetCliqueBasedFeatureCorrespondence();
			vector< vector < vector<SSECorrespondenceNode> > > GetValenceBasedFeatureCorrespondence();
			void PrintFeatureListsMathematica();
		private:
			float GetFeatureCompatibilityScore(SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2);
			float GetFeaturePairRigidityScore(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2);		
			void GetFeatureAngles(float & joint1, float & joint2, float & dihedral, SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2);
			vector< vector<float> > GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> GetAllNodes(vector< vector<float> > & featureCompatibilityScores);
			vector< vector<float> > GetAllNodePairCompatibilityScores(vector<SSECorrespondenceNode> & nodes);			
			void AnalyzeCorrespondence(vector<unsigned int> pFeatures, vector<unsigned int> qFeatures);
			GraphBase<unsigned int, bool> RemoveCliqueAndRelatedNodes(GraphBase<unsigned int, bool> parentGraph, vector<unsigned long long> clique);
		private:
			vector<SSECorrespondenceFeature> featureList1;
			vector<SSECorrespondenceFeature> featureList2;
			float rigidityThreshold, featureChangeThreshold, rigidityAngleCoeff, rigidityCentroidDistanceCoeff, featureChangeCoeff, rigidComponentCoeff, intraComponentCoeff;			
			unsigned int maxSolutionCount;
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
		}

		void SSECorrespondenceFinder::InitializeFeatures(vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2) {
			this->featureList1 = featureList1;
			this->featureList2 = featureList2;
		}

		void SSECorrespondenceFinder::InitializeFeaturesFromPDBFiles(string file1, string file2) {
			vector<PDBHelix> helices1 = PDBReader::ReadHelixPositions(file1);
			vector<PDBHelix> helices2 = PDBReader::ReadHelixPositions(file2);

			for(unsigned int i = 0; i < helices1.size(); i++) {
				featureList1.push_back(SSECorrespondenceFeature(helices1[i].GetEndPosition(0), helices1[i].GetEndPosition(1)));
			}
			for(unsigned int i = 0; i < helices2.size(); i++) {
				featureList2.push_back(SSECorrespondenceFeature(helices2[i].GetEndPosition(0), helices2[i].GetEndPosition(1)));
			}
			helices1.clear();
			helices2.clear();
		}

		void SSECorrespondenceFinder::InitializeConstants(float rigidityThreshold, float featureChangeThreshold, float rigidityAngleCoeff, float rigidityCentroidDistanceCoeff, float rigidityFeatureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff, unsigned int maxSolutionCount) {
			this->rigidityThreshold = rigidityThreshold;
			this->featureChangeThreshold = featureChangeThreshold;
			this->rigidityAngleCoeff = rigidityAngleCoeff;
			this->rigidityCentroidDistanceCoeff = rigidityCentroidDistanceCoeff;
			this->featureChangeCoeff = featureChangeCoeff;
			this->rigidComponentCoeff = rigidComponentCoeff;
			this->intraComponentCoeff = intraComponentCoeff;
			this->maxSolutionCount = maxSolutionCount;
		}

		float SSECorrespondenceFinder::GetFeatureCompatibilityScore(SSECorrespondenceFeature feature1, SSECorrespondenceFeature feature2) {
			//return pow(feature1.Length() - feature2.Length(), 2);
			return abs(feature1.Length() - feature2.Length());
		}

		float SSECorrespondenceFinder::GetFeaturePairRigidityScore(SSECorrespondenceFeature p1, SSECorrespondenceFeature q1, SSECorrespondenceFeature p2, SSECorrespondenceFeature q2) {
			float pJoint1, pJoint2, pDihedral, qJoint1, qJoint2, qDihedral;
			GetFeatureAngles(pJoint1, pJoint2, pDihedral, p1, p2);
			GetFeatureAngles(qJoint1, qJoint2, qDihedral, q1, q2);

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

		vector<SSECorrespondenceNode> SSECorrespondenceFinder::GetAllNodes(vector< vector<float> > & featureCompatibilityScores) {
			vector<SSECorrespondenceNode> nodes;
			for(unsigned int i = 0; i < featureList1.size(); i++) {
				for(unsigned int j = 0; j < featureList2.size(); j++) {
					if(featureCompatibilityScores[i][j] <= featureChangeThreshold) {
						nodes.push_back(SSECorrespondenceNode(i, j));
					}
				}
			}
			return nodes;
		}

		vector< vector<float> > SSECorrespondenceFinder::GetAllNodePairCompatibilityScores(vector<SSECorrespondenceNode> & nodes) {
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
			for(unsigned int i = 0; i < nodes.size()-1; i++) {
				p1 = nodes[i].GetPIndex();
				q1 = nodes[i].GetQIndex();
				for(unsigned int j = i+1; j < nodes.size(); j++) {
					p2 = nodes[j].GetPIndex();
					q2 = nodes[j].GetQIndex();
					scores[i][j] = GetFeaturePairRigidityScore(featureList1[p1], featureList2[q1], featureList1[p2], featureList2[q2]);
					scores[j][i] = scores[i][j];
				}
			}

			return scores;			
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetCliqueBasedFeatureCorrespondence() {
			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores);
			vector< vector<float> > pairCompatibility = GetAllNodePairCompatibilityScores(nodes);

			GraphBase<unsigned int, bool> parentGraph;

			for(unsigned int i = 0; i < nodes.size(); i++) {
				parentGraph.AddVertex(featureCompatibilityScores[nodes[i].GetPIndex()][nodes[i].GetQIndex()], i);
			}

			for(unsigned int i = 0; i < nodes.size(); i++) {
				for(unsigned int j = 0; j < nodes.size(); j++) {
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

			PriorityQueue<SSECorrespondenceSearchNode, float> nodeQueue = PriorityQueue<SSECorrespondenceSearchNode, float>(100000);
			nodeQueue.add(parentNode, parentNode->GetCost());

			unsigned int solutionCount = 0;
			SSECorrespondenceSearchNode * currentNode;
			vector<SSECorrespondenceSearchNode *> childNodes;
			float currentCost;
			while((solutionCount < maxSolutionCount) && !nodeQueue.isEmpty()) {
				nodeQueue.remove(currentNode, currentCost);				
				//currentNode->PrintSolution(nodes);	
				childNodes = currentNode->GetChildNodes(nodes, pairCompatibility, featureChangeCoeff, rigidComponentCoeff, intraComponentCoeff);
				if(childNodes.size() == 0) {
					//printf("Solution found: \t");
					currentNode->PrintSolution(nodes);							
					
					solutionCount++;
				} else {
					//printf("Child Nodes: \n");
					for(unsigned int i = 0; i < childNodes.size(); i++) {
						nodeQueue.add(childNodes[i], childNodes[i]->GetCost());						
						//printf("\t");
						//childNodes[i]->PrintSolution(nodes);
					}
				}
				delete currentNode;
			}


			while(!nodeQueue.isEmpty()) {
				nodeQueue.remove(currentNode, currentCost);
				delete currentNode;
			}					
			
			return correspondence;
		}

		vector< vector < vector<SSECorrespondenceNode> > > SSECorrespondenceFinder::GetValenceBasedFeatureCorrespondence() {
			vector< vector < vector<SSECorrespondenceNode> > > correspondence;
			vector< vector<float> > featureCompatibilityScores = GetAllFeatureCompatibilityScores();
			vector<SSECorrespondenceNode> nodes = GetAllNodes(featureCompatibilityScores);
			vector< vector<float> > pairCompatibility = GetAllNodePairCompatibilityScores(nodes);

			GraphBase<unsigned int, bool> graph;

			for(unsigned int i = 0; i < nodes.size(); i++) {
				graph.AddVertex(featureCompatibilityScores[nodes[i].GetPIndex()][nodes[i].GetQIndex()], i);
			}

			for(unsigned int i = 0; i < nodes.size(); i++) {
				for(unsigned int j = 0; j < nodes.size(); j++) {
					if((nodes[i].GetPIndex() != nodes[j].GetPIndex()) && 
						(nodes[i].GetQIndex() != nodes[j].GetQIndex()) &&
						(i != j) && (pairCompatibility[i][j] <= rigidityThreshold)) {
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
			node.PrintSolution(nodes);
			return correspondence;
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
	}
}


#endif
