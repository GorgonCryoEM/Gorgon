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
//   Revision 1.2  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.1  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//

#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE_H

#include<Foundation/GraphBase.h>
#include<vector>
#include<set>

using namespace wustl_mm::Foundation;
using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceSearchNode {
		public:
			SSECorrespondenceSearchNode(GraphBase<unsigned int, bool> graph, vector< vector<unsigned int> > solution, float cost);
			GraphBase<unsigned int, bool> GetGraph();
			vector< vector<unsigned int> > GetSolution();
			float GetCost();
			vector<SSECorrespondenceSearchNode *> GetChildNodes(vector<SSECorrespondenceNode> & allNodes, vector< vector<float> > & pairCompatibility, float featureChangeCoeff, float rigidComponentCoeff, float intraComponentCoeff);
			void PrintSolution(vector<SSECorrespondenceNode> & allNodes);
			GraphBase<unsigned int, bool> GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique);
		private:
			float GetCliqueCost(vector<unsigned long long> & clique, float featureChangeCoeff, float rigidComponentCoeff);
			float GetIntraCliqueCost(vector<unsigned int> & nodeList, vector<unsigned int> & parentNodeList, vector< vector<float> > & pairCompatibility, float intraComponentCoeff);

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

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique) {
			set<unsigned int> pNodes;
			set<unsigned int> qNodes;
			map<unsigned int, unsigned int> nodeIndices;
			map<unsigned int, unsigned int> vertexIndices;
			GraphBase<unsigned int, bool> childGraph;

			for(unsigned int i = 0; i < clique.size(); i++) {
				pNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetPIndex());
				qNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetQIndex());
			}

			unsigned int vertexNode;			
			for(unsigned int i = 0; i < graph.GetVertexCount(); i++) {
				vertexNode = graph.GetVertex(i).GetTag();
				if((pNodes.find(allNodes[vertexNode].GetPIndex()) == pNodes.end()) && (qNodes.find(allNodes[vertexNode].GetQIndex()) == qNodes.end())) {
					nodeIndices[vertexNode] = childGraph.AddVertex(graph.GetVertex(i).GetWeight(), vertexNode);
					vertexIndices[nodeIndices[vertexNode]] = i;					
				}				
			}

			for(unsigned int i = 0; i < childGraph.GetVertexCount(); i++) {
				for(unsigned int j = 0; j < childGraph.GetVertexCount(); j++) {
					if(graph.IsEdge(vertexIndices[i], vertexIndices[j])) {
						childGraph.AddEdge(i, j, graph.GetEdge(vertexIndices[i], vertexIndices[j]));
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

		void SSECorrespondenceSearchNode::PrintSolution(vector<SSECorrespondenceNode> & allNodes) {
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
					printf("{%d, %d}", allNodes[nodes[j]].GetPIndex(), allNodes[nodes[j]].GetQIndex());
				}
				printf("}");
			}
			printf("};\n\n");
			printf("Print[\"Cost = \", %f];\n", cost);
			printf("printFeatures[performSemiRigidDeformation[{fl1, fl2}, corr[[%d]]], fl2]\n", firstCorr);
		}
	}
}

#endif