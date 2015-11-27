// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A search node in the A* search used to find the correspondence between feature pairs.


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
			vector<SSECorrespondenceSearchNode *> GetChildNodesTriangleApprox(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2, int smallestCliqueSize);
			vector<SSECorrespondenceSearchNode *> GetChildNodesTriangleApproxCliqueDistance(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2, int smallestCliqueSize);
			void PrintSolution(vector<SSECorrespondenceNode> & allNodes, bool useDirection, bool onlyCorr = false);
			GraphBase<unsigned int, bool> GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique);
			GraphBase<unsigned int, bool> GetOnlySymmetriesGraph(vector<SSECorrespondenceNode> & allNodes, set<unsigned long long> clique, GraphBase<unsigned int, bool> & parentGraph, GraphBase<unsigned int, bool> & rootGraph, map<unsigned int, unsigned int> & parentVertexIndices);
			MatrixFloat GetTransform(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, int sampleCount);
			void FindErrorMatrixBasedCorrespondences(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, vector<SSECorrespondenceNode> & allNodes, float maxError);
			
		private:
			float GetAverageError(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, MatrixFloat & transform, SSECorrespondenceNode node, int sampleCount);
			float GetAverageError(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, MatrixFloat & transform, vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, int sampleCount);			
			vector< set<unsigned long long> > GetSymmetricCliquesTriangleApprox(int maxSizeDifference, vector<SSECorrespondenceNode> & allNodes, int smallestCliqueSize);

			float GetCliqueCost(vector<unsigned long long> & clique, float featureChangeCoeff, float rigidComponentCoeff);
			float GetCliqueCost2(vector<unsigned long long> & clique);
			float GetCliqueDistances(vector<unsigned int> & nodeList1, vector<unsigned int> & nodeList2, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);

			float GetIntraCliqueCost(vector<unsigned int> & nodeList, vector<unsigned int> & parentNodeList, vector< vector<float> > & pairCompatibility, float intraComponentCoeff);
			float GetIntraCliqueCost2(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);			
			float GetIntraCliqueCost3(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);
			float GetIntraCliqueCost4(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);

			Vector3DFloat GetFeature1Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1);
			Vector3DFloat GetFeature2Centroid(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList2);

		private:
			GraphBase<unsigned int, bool> graph;
			vector< vector<unsigned int> > solution;
			float cost;
			static const int SAMPLE_COUNT = 10;
			static const int MAX_CLIQUE_SIZE_DIFFERENCE = 2;
			
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

		vector< set<unsigned long long> > SSECorrespondenceSearchNode::GetSymmetricCliquesTriangleApprox(int maxSizeDifference, vector<SSECorrespondenceNode> & allNodes, int smallestCliqueSize) {
			vector< set<unsigned long long> > cliques;

			GraphBase<unsigned int, bool> tempGraph(graph);

			set<unsigned long long> currClique = tempGraph.GetLowestCostCliqueTriangleApprox(smallestCliqueSize);
			if(currClique.size() > 0) {
				cliques.push_back(currClique);
				int minSize = max(smallestCliqueSize, (int)currClique.size() - maxSizeDifference);

				GraphBase<unsigned int, bool> prunedGraph;
				prunedGraph.AddVertex(0, false);
				
				map<unsigned int, unsigned int> parentVertexIndices;

				for(unsigned int i = 0; i < tempGraph.GetVertexCount(); i++) {
					parentVertexIndices[i] = i;
				}

				vector<unsigned long long> relativeClique, absoluteClique;
				while(prunedGraph.GetVertexCount() > 0) {
					prunedGraph = GetOnlySymmetriesGraph(allNodes, currClique, tempGraph, graph, parentVertexIndices);
					currClique = prunedGraph.GetLowestCostCliqueTriangleApprox(smallestCliqueSize);
					if(currClique.size() >= minSize) {
						relativeClique = prunedGraph.VertexSetToVector(currClique);
						absoluteClique.clear();
						for(int i = 0; i < (int)relativeClique.size(); i++) {
							absoluteClique.push_back(parentVertexIndices[relativeClique[i]]);
						}
						cliques.push_back(prunedGraph.VertexVectorToSet(absoluteClique));
					}
					tempGraph = prunedGraph;
				}		
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


		vector<SSECorrespondenceSearchNode *> SSECorrespondenceSearchNode::GetChildNodesTriangleApprox(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2, int smallestCliqueSize) {
			vector<SSECorrespondenceSearchNode *> childNodes;

			if(graph.GetVertexCount() > 0) {
				vector< set<unsigned long long> > cliques = GetSymmetricCliquesTriangleApprox(2, allNodes, smallestCliqueSize);
			
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
					childCost = cost + GetIntraCliqueCost3(childSolutionElement, allNodes, featureList1, featureList2);

					childNodes.push_back(new SSECorrespondenceSearchNode(childGraph, childSolution, childCost));				
				}
			}
			return childNodes;
		}

		vector<SSECorrespondenceSearchNode *> SSECorrespondenceSearchNode::GetChildNodesTriangleApproxCliqueDistance(vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> featureList1, vector<SSECorrespondenceFeature> featureList2, int smallestCliqueSize) {
			vector<SSECorrespondenceSearchNode *> childNodes;

			if(graph.GetVertexCount() > 0) {
				vector< set<unsigned long long> > cliques = GetSymmetricCliquesTriangleApprox(MAX_CLIQUE_SIZE_DIFFERENCE, allNodes, smallestCliqueSize);
			
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
					childCost = cost + 
						//GetCliqueCost2(clique) * GetIntraCliqueCost4(childSolutionElement, allNodes, featureList1, featureList2) * 
						GetIntraCliqueCost4(childSolutionElement, allNodes, featureList1, featureList2) * 
						(float)(1.0/(1.0-exp(-(float)childSolutionElement.size()/10.0)));

					childNodes.push_back(new SSECorrespondenceSearchNode(childGraph, childSolution, childCost));				
				}
			}
			return childNodes;
		}

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetChildGraph(vector<SSECorrespondenceNode> & allNodes, vector<unsigned long long> clique) {
			set<unsigned int> pNodes;
			set<unsigned int> qNodes;

			for(unsigned int i = 0; i < clique.size(); i++) {
				pNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetPIndex());
				qNodes.insert(allNodes[graph.GetVertex(clique[i]).GetTag()].GetQIndex());
			}

			unsigned int vertexNode;
			vector<unsigned long long> childVertices;

			for(unsigned int i = 0; i < graph.GetVertexCount(); i++) {
				vertexNode = graph.GetVertex(i).GetTag();
				if((pNodes.find(allNodes[vertexNode].GetPIndex()) == pNodes.end()) && (qNodes.find(allNodes[vertexNode].GetQIndex()) == qNodes.end())) {
					childVertices.push_back(i);
				}				
			}

			return graph.GetSubgraph(childVertices);
		}

		GraphBase<unsigned int, bool> SSECorrespondenceSearchNode::GetOnlySymmetriesGraph(vector<SSECorrespondenceNode> & allNodes, set<unsigned long long> cliqueSet, GraphBase<unsigned int, bool> & parentGraph, GraphBase<unsigned int, bool> & rootGraph, map<unsigned int, unsigned int> & parentVertexIndices) {
			set<unsigned int> pNodes;
			set<unsigned int> qNodes;			
			vector<unsigned long long> clique = parentGraph.VertexSetToVector(cliqueSet);

			for(unsigned int i = 0; i < clique.size(); i++) {
				pNodes.insert(allNodes[parentGraph.GetVertex(clique[i]).GetTag()].GetPIndex());
				qNodes.insert(allNodes[parentGraph.GetVertex(clique[i]).GetTag()].GetQIndex());
			}

			unsigned int vertexNode;
			unsigned int childIndex = 0;
			vector<unsigned long long> childVertices;
			for(unsigned int i = 0; i < parentGraph.GetVertexCount(); i++) {
				vertexNode = parentGraph.GetVertex(i).GetTag();
				// Add only if PNode was part of clique, and not a node which was in original clique
				if((pNodes.find(allNodes[vertexNode].GetPIndex()) != pNodes.end()) && (cliqueSet.find(i) == cliqueSet.end())) {
					childVertices.push_back(i);					
					parentVertexIndices[childIndex] = parentVertexIndices[i];					
					childIndex++;
				}				
			}

			return parentGraph.GetSubgraph(childVertices);
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

		float SSECorrespondenceSearchNode::GetCliqueCost2(vector<unsigned long long> & clique) {
			float nodeCost = 0.0f;
			for(unsigned int i = 0; i < clique.size(); i++) {
				nodeCost += (graph.GetVertex(clique[i]).GetWeight() / (float)clique.size());
			}

			float edgeCost = 0.0f;
			int count = 0;
			for(unsigned int i = 0; i < clique.size()-1; i++) {
				for(unsigned int j = i+1; j < clique.size(); j++) {					
					count++;
					edgeCost += graph.GetEdge(clique[i], clique[j]).GetWeight();
				}
			}
			if(count > 0) {
				edgeCost = edgeCost / (float)count;
			} else {
				edgeCost = 1.0f;
			}

			return nodeCost * edgeCost;
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


		float SSECorrespondenceSearchNode::GetIntraCliqueCost2(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
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

		float SSECorrespondenceSearchNode::GetIntraCliqueCost3(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
			MatrixFloat transform = MatrixFloat(4,4);
			float cost = 0;
			for(unsigned int i = 0; i < solution.size(); i++) {
				if(solution[i].size() > 0) {
					transform = GetTransform(featureList1, featureList2, solution[i], allNodes, SAMPLE_COUNT);
					cost += GetAverageError(featureList1, featureList2, transform, nodeList, allNodes, SAMPLE_COUNT);	
				}
			}
			return cost;
		}

		float SSECorrespondenceSearchNode::GetCliqueDistances(vector<unsigned int> & nodeList1, vector<unsigned int> & nodeList2, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
			float dist = MAX_FLOAT, dist1, dist2;
						
			if((nodeList1.size() == 0) || (nodeList2.size() == 0)) {
				return 0;
			} else {
				for(unsigned int i = 0; i < nodeList1.size(); i++) {
					for(unsigned int j = 0; j < nodeList2.size(); j++) {
						dist1 = (float)(featureList1[allNodes[nodeList1[i]].GetPIndex()].GetCentroid() - featureList1[allNodes[nodeList2[j]].GetPIndex()].GetCentroid()).Length();
						dist2 = (float)(featureList2[allNodes[nodeList1[i]].GetQIndex()].GetCentroid() - featureList2[allNodes[nodeList2[j]].GetQIndex()].GetCentroid()).Length();
						dist = min(dist, abs(dist1-dist2));
					}
				}
			}
			return dist;
		}

		float SSECorrespondenceSearchNode::GetIntraCliqueCost4(vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
			if(solution.size() == 0) {
				return 0;
			}

			float minCost = MAX_FLOAT;
			float dist, cost;
			float level = (float)solution.size();

			bool found = false;
			for(unsigned int i = 0; i < solution.size(); i++) {
				if(solution[i].size() > 0) {
					found = true;
					dist = GetCliqueDistances(nodeList, solution[i], allNodes, featureList1, featureList2);
					cost = dist;
					//cost = pow(dist, (float)(1.0/(1.0-exp(-level))));
					minCost = min(cost, minCost);
				}
			}
			if(found) {
				return minCost;
			} else {
				return 0;
			}
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

		void SSECorrespondenceSearchNode::PrintSolution(vector<SSECorrespondenceNode> & allNodes, bool useDirection, bool onlyCorr) {
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
			if(!onlyCorr) {
				printf("cost = %f;\n", this->GetCost());
				printf("corr = Sort[corr, Length[#1] > Length[#2] &];\n");
				firstCorr = 1;
				printf("printFinalOutput[corr, fl1, fl2, %d, groundTruth]\n", firstCorr);
			} else {
				printf("\t\tcost = %f;\n", this->GetCost());
			}
		}


		void SSECorrespondenceSearchNode::FindErrorMatrixBasedCorrespondences(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, vector<SSECorrespondenceNode> & allNodes, float maxError) {
			vector<bool> unmappedPFeatures, unmappedQFeatures;
			for(unsigned int i = 0; i < featureList1.size(); i++) {
				unmappedPFeatures.push_back(true);
			}

			for(unsigned int i = 0; i < featureList2.size(); i++) {
				unmappedQFeatures.push_back(true);
			}

			for(unsigned int i = 0; i < solution.size(); i++) {
				for(unsigned int j = 0; j < solution[i].size(); j++) {					
					unmappedPFeatures[allNodes[solution[i][j]].GetPIndex()] = false;
					unmappedQFeatures[allNodes[solution[i][j]].GetQIndex()] = false;
				}
			}

			GorgonPriorityQueue<float, SSECorrespondenceNode> costMatrix = GorgonPriorityQueue<float, SSECorrespondenceNode>(false);

			MatrixFloat transform = MatrixFloat(4,4);
			float cost;			
			SSECorrespondenceNode node;

			for(unsigned int m = 0; m < solution.size(); m++) {
				if(solution[m].size() > 0) {
					transform = GetTransform(featureList1, featureList2, solution[m], allNodes, SAMPLE_COUNT);

					for(unsigned int i = 0; i < featureList1.size(); i++) {
						if(unmappedPFeatures[i]) {
							for(unsigned int j = 0; j < featureList2.size(); j++) {
								if(unmappedQFeatures[j]) {
									for(unsigned int k = 0; k < 2; k++) {
										node = SSECorrespondenceNode(i, j, (k==0));
										cost = GetAverageError(featureList1, featureList2, transform, node, SAMPLE_COUNT);
										costMatrix.Add(cost, node);	
									}
								}
							}
						}
					}
				}
			}

			bool terminate = false;
			
			vector<unsigned int> independentClique;
			while(!costMatrix.IsEmpty() && (!terminate)) {
				costMatrix.PopFirst(cost, node);
				terminate = (cost > maxError);
				if(!terminate && unmappedPFeatures[node.GetPIndex()] && unmappedQFeatures[node.GetQIndex()]) {					
					allNodes.push_back(node);
					independentClique.clear();
					independentClique.push_back(allNodes.size()-1);
					solution.push_back(independentClique);		
					unmappedPFeatures[node.GetPIndex()] = false;
					unmappedQFeatures[node.GetQIndex()] = false;
				}			
			}
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
				for(int j = 0; j < sampleCount; j++) {
					offset = (float)j / (float)(sampleCount - 1);

					sp = p1*(1.0f - offset) + p2 * offset;
					sq = q1*(1.0f - offset) + q2 * offset;
					fl1.push_back(sp);
					fl2.push_back(sq);					
				}
			}

			return LinearSolver::FindRotationTranslation(fl1, fl2);
		}

		float SSECorrespondenceSearchNode::GetAverageError(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, MatrixFloat & transform, SSECorrespondenceNode node, int sampleCount) {
			Vector3DFloat p1, p2, q1, q2, sp, sq;
			float totalError = 0, nodeError, offset;				

			p1 = featureList1[node.GetPIndex()].GetEndPoint(0).Transform(transform);
			p2 = featureList1[node.GetPIndex()].GetEndPoint(1).Transform(transform);
			
			q1 = featureList2[node.GetQIndex()].GetEndPoint(node.IsForward()?0:1);
			q2 = featureList2[node.GetQIndex()].GetEndPoint(node.IsForward()?1:0);

			nodeError = 0;
			for(int j = 0; j < sampleCount; j++) {				
				offset = (float)j / (float)(sampleCount - 1);

				sp = p1*(1.0f - offset) + p2 * offset;
				sq = q1*(1.0f - offset) + q2 * offset;

				nodeError += (sp - sq).Length() / (float)sampleCount;
			}

			return nodeError;
		}

		float SSECorrespondenceSearchNode::GetAverageError(vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2, MatrixFloat & transform, vector<unsigned int> & nodeList, vector<SSECorrespondenceNode> & allNodes, int sampleCount) {
			float totalError = 0;

			for(unsigned int i = 0; i < nodeList.size(); i++) {
				//totalError += GetAverageError(featureList1, featureList2, transform, allNodes[nodeList[i]], sampleCount) / (float)nodeList.size();
				totalError += GetAverageError(featureList1, featureList2, transform, allNodes[nodeList[i]], sampleCount);
			}
			return totalError;
		}
	}
}

#endif
