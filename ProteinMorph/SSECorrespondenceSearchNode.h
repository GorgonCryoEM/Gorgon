// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A search node in the A* search used to find the correspondence between feature pairs.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.8  2009/12/07 05:00:52  ssa1
//   Adding in Matrix functionality for Singular Value Decomposition
//
//   Revision 1.7  2009/11/30 04:23:44  ssa1
//   Triangle based A* search for flexible fitting
//
//   Revision 1.6  2009/11/19 18:19:25  ssa1
//   Improved flexible fitting.. (Split nodes to guarantee direction)
//
//   Revision 1.5  2009/11/04 20:29:38  ssa1
//   Implementing Triangle based clique search and chain based flexible fitting.
//
//   Revision 1.4  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.3  2009/09/29 19:23:39  ssa1
//   Fixing indexing bugs when performing sse correspondence search.
//
//   Revision 1.2  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.1  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//

#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE_H

#include <Foundation/GraphBase.h>
#include <vector>
#include <set>
#include <MathTools/LinearSolver.h>

using namespace wustl_mm::Foundation;
using namespace wustl_mm::MathTools;
using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceSearchNode {
		public:
			SSECorrespondenceSearchNode(GraphBase<unsigned int, bool> graph, vector< vector<unsigned int> > solution, float cost);
			GraphBase<unsigned int, bool> GetGraph();
			vector< vector<unsigned int> > GetSolution();
			vector< vector<SSECorrespondenceNode> > GetSolution(vector<SSECorrespondenceNode> & allNodes);			

			float GetCost();
			vector<SSECorrespondenceSearchNode *> GetChildNodes(vector<SSECorrespondenceNode> & allNodes, vector< vector<float> > & pairCompatibility, float featureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff);
			vector<SSECorrespondenceSearchNode *> GetChildNodesTriangleApprox(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2);
			void PrintSolution(vector<SSECorrespondenceNode> & allNodes, bool useDirection);
			GraphBase<unsigned int, bool> GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique);
			GraphBase<unsigned int, bool> GetOnlySymmetriesGraph(vector<SSECorrespondenceNode> & allNodes, set<unsigned long long> clique, GraphBase<unsigned int, bool> & parentGraph, map<unsigned int, unsigned int> & parentVertexIndices);
			MatrixFloat GetTransform(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, int sampleCount);
		private:
			vector< set<unsigned long long> > GetSymmetricCliquesTriangleApprox(int maxSizeDifference, vector<SSECorrespondenceNode> & allNodes);

			float GetCliqueCost(vector<unsigned long long> & clique, float featureChangeCoeff, float rigidComponentCoeff);
			float GetIntraCliqueCost(vector<unsigned int> & nodeList, vector<unsigned int> & parentNodeList, vector< vector<float> > & pairCompatibility, float intraComponentCoeff);
			float GetIntraCliqueCost2(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2);

			Vector3DFloat GetFeature1Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1);
			Vector3DFloat GetFeature2Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList2);

		private:
			GraphBase<unsigned int, bool> graph;
			vector< vector<unsigned int> > solution;
			float cost;
			
		};

		SSECorrespondenceSearchNode::SSECorrespondenceSearchNode(GraphBase<unsigned int, bool> graph, vector< vector<unsigned int> > solution, float cost) {
			this->graph = graph;
			this->solution = solution;
			this->cost = cost;
		}

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetGraph() {
			return graph;
		}

		vector< vector<unsigned int> > SSECorrespondenceSearchNode::GetSolution() {
			return solution;
		}

		float SSECorrespondenceSearchNode::GetCost() {
			return cost;
		}

		vector< set<unsigned long long> > SSECorrespondenceSearchNode::GetSymmetricCliquesTriangleApprox(int maxSizeDifference, vector<SSECorrespondenceNode> & allNodes) {
			vector< set<unsigned long long> > cliques;

			GraphBase<unsigned int, bool> tempGraph(graph);

			set<unsigned long long> currClique = tempGraph.GetLowestCostCliqueTriangleApprox();
			cliques.push_back(currClique);

			int minSize = max(1, (int)currClique.size() - maxSizeDifference);

			GraphBase<unsigned int, bool> prunedGraph;
			
			map<unsigned int, unsigned int> parentVertexIndices;

			for(unsigned int i = 0; i < tempGraph.GetVertexCount(); i++) {
				parentVertexIndices[i] = i;
			}

			vector<unsigned long long> relativeClique, absoluteClique;
			while(prunedGraph.GetVertexCount() > 0) {
				prunedGraph = GetOnlySymmetriesGraph(allNodes, currClique, tempGraph, parentVertexIndices);
				currClique = prunedGraph.GetLowestCostCliqueTriangleApprox();
				if(currClique.size() >= minSize) {
					relativeClique = prunedGraph.VertexSetToVector(currClique);
					absoluteClique.clear();
					for(unsigned int i = 0; i < relativeClique.size(); i++) {
						absoluteClique.push_back(parentVertexIndices[relativeClique[i]]);
					}
					cliques.push_back(prunedGraph.VertexVectorToSet(absoluteClique));
				}
				tempGraph = prunedGraph;
			}		
			return cliques;
		}

		vector<SSECorrespondenceSearchNode *> SSECorrespondenceSearchNode::GetChildNodes(vector<SSECorrespondenceNode> & allNodes, vector< vector<float> > & pairCompatibility, float featureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff) {
			vector<SSECorrespondenceSearchNode *> childNodes;
			if(graph.GetVertexCount() > 0) {
				vector< set<unsigned long long> > cliques = graph.GetLargestMaximalCliques2();
				
				vector<unsigned int> childSolutionElement;
				vector< vector<unsigned int> > childSolution;
				vector<unsigned long long> clique;			
				GraphBase<unsigned int, bool> childGraph;
				float childCost;

				for(unsigned int i = 0; i < cliques.size(); i++) {				
					clique = graph.VertexSetToVector(cliques[i]);
					childSolutionElement.clear();
					for(unsigned int j = 0; j < clique.size(); j++) {
						childSolutionElement.push_back(graph.GetVertex(clique[j]).GetTag());
					}
					childSolution = solution;
					childSolution.push_back(childSolutionElement);
					childGraph = GetChildGraph(allNodes, clique);
					childCost = cost + GetCliqueCost(clique, featureChangeCoeff, rigidComponentCoeff) + GetIntraCliqueCost(childSolutionElement, solution[solution.size()-1], pairCompatibility, intraComponentCoeff);
					
					childNodes.push_back(new SSECorrespondenceSearchNode(childGraph, childSolution, childCost));				
				}
			}
			return childNodes;
		}


		vector<SSECorrespondenceSearchNode *> SSECorrespondenceSearchNode::GetChildNodesTriangleApprox(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2) {
			vector<SSECorrespondenceSearchNode *> childNodes;
			if(graph.GetVertexCount() > 0) {
				vector< set<unsigned long long> > cliques = GetSymmetricCliquesTriangleApprox(2, allNodes);
				
				vector<unsigned int> childSolutionElement;
				vector< vector<unsigned int> > childSolution;
				vector<unsigned long long> clique;			
				GraphBase<unsigned int, bool> childGraph;
				float childCost;

				for(unsigned int i = 0; i < cliques.size(); i++) {				
					clique = graph.VertexSetToVector(cliques[i]);
					childSolutionElement.clear();
					for(unsigned int j = 0; j < clique.size(); j++) {
						childSolutionElement.push_back(graph.GetVertex(clique[j]).GetTag());
					}
					childSolution = solution;
					childSolution.push_back(childSolutionElement);
					childGraph = GetChildGraph(allNodes, clique);
					childCost = cost + GetIntraCliqueCost2(childSolutionElement, allNodes, featureList1, featureList2);

					childNodes.push_back(new SSECorrespondenceSearchNode(childGraph, childSolution, childCost));				
				}
			}
			return childNodes;
		}

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique) {
			set<unsigned int> pNodes;
			set<unsigned int> qNodes;
			map<unsigned int, unsigned int> vertexIndices;
			GraphBase<unsigned int, bool> childGraph;

			for(unsigned int i = 0; i < clique.size(); i++) {
				pNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetPIndex());
				qNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetQIndex());
			}

			unsigned int vertexNode;
			unsigned int childIndex;
			for(unsigned int i = 0; i < graph.GetVertexCount(); i++) {
				vertexNode = graph.GetVertex(i).GetTag();
				if((pNodes.find(allNodes[vertexNode].GetPIndex()) == pNodes.end()) && (qNodes.find(allNodes[vertexNode].GetQIndex()) == qNodes.end())) {
					childIndex = childGraph.AddVertex(graph.GetVertex(i).GetWeight(), vertexNode);
					vertexIndices[childIndex] = i;					
				}				
			}

			for(int i = 0; i < (int)childGraph.GetVertexCount()-1; i++) {
				for(int j = i+1; j < (int)childGraph.GetVertexCount(); j++) {
					if(graph.IsEdge(vertexIndices[i], vertexIndices[j])) {
						childGraph.AddEdge(i, j, graph.GetEdge(vertexIndices[i], vertexIndices[j]));
					}
				}
			}

			return childGraph;
		}

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetOnlySymmetriesGraph(vector<SSECorrespondenceNode> & allNodes, set<unsigned long long> cliqueSet, GraphBase<unsigned int, bool> & parentGraph, map<unsigned int, unsigned int> & parentVertexIndices) {
			set<unsigned int> pNodes;
			set<unsigned int> qNodes;			
			GraphBase<unsigned int, bool> childGraph;
			vector<unsigned long long> clique = parentGraph.VertexSetToVector(cliqueSet);

			for(unsigned int i = 0; i < clique.size(); i++) {
				pNodes.insert(allNodes[parentGraph.GetVertex(clique[i]).GetTag()].GetPIndex());
				qNodes.insert(allNodes[parentGraph.GetVertex(clique[i]).GetTag()].GetQIndex());
			}

			unsigned int vertexNode;
			unsigned int childIndex;
			for(unsigned int i = 0; i < parentGraph.GetVertexCount(); i++) {
				vertexNode = parentGraph.GetVertex(i).GetTag();
				// Add only if PNode was part of clique, and not a node which was in original clique
				if((pNodes.find(allNodes[vertexNode].GetPIndex()) != pNodes.end()) && (cliqueSet.find(i) == cliqueSet.end())) {
					childIndex = childGraph.AddVertex(parentGraph.GetVertex(i).GetWeight(), vertexNode);
					parentVertexIndices[childIndex] = parentVertexIndices[i];					
				}				
			}

			for(int i = 0; i < (int)childGraph.GetVertexCount()-1; i++) {
				for(int j = i+1; j < (int)childGraph.GetVertexCount(); j++) {
					if(parentGraph.IsEdge(parentVertexIndices[i], parentVertexIndices[j])) {
						childGraph.AddEdge(i, j, parentGraph.GetEdge(parentVertexIndices[i], parentVertexIndices[j]));
					}
				}
			}

			return childGraph;
		}



		float SSECorrespondenceSearchNode::GetCliqueCost(vector<unsigned long long> & clique, float featureChangeCoeff, float rigidComponentCoeff) {
			float cliqueCost = 0.0f;
			for(unsigned int i = 0; i < clique.size(); i++) {
				cliqueCost += featureChangeCoeff * graph.GetVertex(clique[i]).GetWeight();
			}

			for(unsigned int i = 0; i < clique.size()-1; i++) {
				for(unsigned int j = i+1; j < clique.size(); j++) {					
					cliqueCost += rigidComponentCoeff * graph.GetEdge(clique[i], clique[j]).GetWeight();
				}
			}
			return cliqueCost;
		}

		float SSECorrespondenceSearchNode::GetIntraCliqueCost(vector<unsigned int> & nodeList, vector<unsigned int> & parentNodeList, vector< vector<float> > & pairCompatibility, float intraComponentCoeff) {
			float intraCliqueCost = 0.0f;
			for(unsigned int i = 0; i < parentNodeList.size(); i++) {
				for(unsigned int j = 0; j < nodeList.size(); j++) {
					intraCliqueCost += intraComponentCoeff * pairCompatibility[parentNodeList[i]][nodeList[j]];
				}
			}
			return intraCliqueCost;
		}


		Vector3DFloat SSECorrespondenceSearchNode::GetFeature1Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1) {
			Vector3DFloat centroid;
			if(nodeList.size() > 0) {
				vector<unsigned int> features;
				for(unsigned int i = 0; i < nodeList.size(); i++) {
					features.push_back(allNodes[nodeList[i]].GetPIndex());
				}

				for(unsigned int i = 0; i < features.size(); i++) {
					centroid += featureList1[features[i]].GetCentroid();
				}
				centroid = centroid * (1.0 / (float)features.size());
			}
			return centroid;
		}

		Vector3DFloat SSECorrespondenceSearchNode::GetFeature2Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList2) {
			Vector3DFloat centroid;
			if(nodeList.size() > 0) {
				vector<unsigned int> features;
				for(unsigned int i = 0; i < nodeList.size(); i++) {
					features.push_back(allNodes[nodeList[i]].GetQIndex());
				}

				for(unsigned int i = 0; i < features.size(); i++) {
					centroid += featureList2[features[i]].GetCentroid();
				}
				centroid = centroid * (1.0 / (float)features.size());
			}
			return centroid;
		}


		float SSECorrespondenceSearchNode::GetIntraCliqueCost2(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2) {
			Vector3DFloat c1, c2, p1, p2;
			c1 = GetFeature1Centroid(nodeList, allNodes, featureList1);
			c2 = GetFeature2Centroid(nodeList, allNodes, featureList2);

			float cost = 0;
			float d1, d2;
			for(unsigned int i = 0; i < solution.size(); i++) {
				if(solution[i].size() > 0) {
					p1 = GetFeature1Centroid(solution[i], allNodes, featureList1);
					p2 = GetFeature2Centroid(solution[i], allNodes, featureList2);
					d1 = (c1 - p1).Length();
					d2 = (c2 - p2).Length();

					cost += abs(d1-d2) * exp(-max(d1, d2)/4.0);
				}
			}

			return cost;
			
		}

		vector< vector<SSECorrespondenceNode> > SSECorrespondenceSearchNode::GetSolution(vector<SSECorrespondenceNode> & allNodes) {
			vector< vector<SSECorrespondenceNode> > corr;
			vector<SSECorrespondenceNode> corrItem;

			vector<unsigned int> nodes;
			for(unsigned int i = 0; i < solution.size(); i++) {
				nodes = solution[i];
				corrItem.clear();
				if(nodes.size() > 0) {
					for(unsigned int j = 0; j < nodes.size(); j++) {
						corrItem.push_back(allNodes[nodes[j]]);
					}
					corr.push_back(corrItem);
				}
			}
			return corr;
		}

		void SSECorrespondenceSearchNode::PrintSolution(vector<SSECorrespondenceNode> & allNodes, bool useDirection) {
			vector<unsigned int> nodes;
			int firstCorr = -1;
			printf("corr= {\n");
			for(unsigned int i = 0; i < solution.size(); i++) {
				nodes = solution[i];
				if((firstCorr == -1) && (nodes.size() > 0)) {
					firstCorr = i+1;
				}
				if(i != 0) {
					printf(",\n");
				}
				printf("\t{");
				for(unsigned int j = 0; j < nodes.size(); j++) {
					if(j != 0) {
						printf(",");
					}
					if(useDirection) {
						if(allNodes[nodes[j]].IsForward()) {
							printf("{{%d, %d}, {%d, %d}}", allNodes[nodes[j]].GetPIndex(), 0, allNodes[nodes[j]].GetQIndex(), 0);
						} else {
							printf("{{%d, %d}, {%d, %d}}", allNodes[nodes[j]].GetPIndex(), 0, allNodes[nodes[j]].GetQIndex(), 1);
						}
					}
					else {
						printf("{%d, %d}", allNodes[nodes[j]].GetPIndex(), allNodes[nodes[j]].GetQIndex());
					}
				}
				printf("}");
			}
			printf("};\n\n");
			printf("cost = %f;\n", this->GetCost());
			printf("corr = Sort[corr, Length[#1] > Length[#2] &];\n");
			firstCorr = 1;
			printf("printFinalOutput[corr, fl1, fl2, %d]\n", firstCorr);
		}

		MatrixFloat SSECorrespondenceSearchNode::GetTransform(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, int sampleCount) {
			vector<Vector3DFloat> fl1, fl2;
			fl1.clear();
			fl2.clear();
			Vector3DFloat p1, p2, q1, q2;
			Vector3DFloat sp, sq;

			for(unsigned int i = 0; i < nodeList.size(); i++) {
				p1 = featureList1[allNodes[nodeList[i]].GetPIndex()].GetEndPoint(0);
				p2 = featureList1[allNodes[nodeList[i]].GetPIndex()].GetEndPoint(1);
				
				q1 = featureList2[allNodes[nodeList[i]].GetQIndex()].GetEndPoint(allNodes[nodeList[i]].IsForward()?0:1);
				q2 = featureList2[allNodes[nodeList[i]].GetQIndex()].GetEndPoint(allNodes[nodeList[i]].IsForward()?1:0);

				float offset;
				for(unsigned int j = 0; j < sampleCount; j++) {
					offset = (float)j / (float)(sampleCount - 1);

					sp = p1*(1.0f - offset) + p2 * offset;
					sq = q1*(1.0f - offset) + q2 * offset;
					fl1.push_back(sp);
					fl2.push_back(sq);					
				}
			}

			return LinearSolver::FindRotationTranslation(fl1, fl2);
		}
	}
}

#endif