// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph



#ifndef FOUNDATION_GRAPH_BASE_H
#define FOUNDATION_GRAPH_BASE_H

#include "GraphVertexBase.h"
#include "GraphEdgeBase.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "GorgonPriorityQueue.h"

using namespace std;

namespace wustl_mm {
	namespace Foundation {

		template <class TVertexTag, class TEdgeTag> class GraphBase {
		public:
			GraphBase();
			//GraphBase(GraphBase<TVertexTag,TEdgeTag> & graph);
			unsigned long long AddVertex(float weight, TVertexTag tag);
			unsigned long long AddVertex(unsigned long long externalIndex, float weight, TVertexTag tag);
			unsigned long long AddVertex(GraphVertexBase<TVertexTag> vertex);
			GraphVertexBase<TVertexTag> GetVertex(unsigned long long vertexIx);
			unsigned long long GetHighestValenceVertexIx();
			unsigned long long GetVertexCount();
			bool IsEdge(unsigned long long vertexIx1, unsigned long long vertexIx2);			
			void AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, float weight, TEdgeTag tag);
			void AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, GraphEdgeBase<TEdgeTag> edge);
			GraphEdgeBase<TEdgeTag> GetEdge(unsigned long long vertexIx1, unsigned long long vertexIx2);
			// Graph functions
			bool IsClique(set<unsigned long long> vertexSet);
			bool IsClique(vector<unsigned long long> vertexIndices);
			float GetCliqueEdgeCost(vector<unsigned long long> vertexIndices);
			vector< set<unsigned long long> > GetAllConnectedComponents();
			vector< set<unsigned long long> > GetAllConnectedComponents(vector<unsigned long long> vertexSet);
			vector< set<unsigned long long> > GetAllCliques();
			vector< set<unsigned long long> > GetAllCliquesTriangleApprox(int smallestCliqueSize);
			vector< set<unsigned long long> > GetAllMaximalCliques();
			vector< set<unsigned long long> > GetLargestMaximalCliques();
			vector< set<unsigned long long> > GetLargestMaximalCliques2();
			vector< set<unsigned long long> > GetLargestMaximalCliques(vector<unsigned long long> vertexSet);
			vector< set<unsigned long long> > GetLargestMaximalCliques2(vector<unsigned long long> vertexSet);
			set<unsigned long long> GetLowestCostCliqueInOneRing(unsigned long long vertexIx);
			set<unsigned long long> GetLowestCostCliqueTriangleApprox(int smallestCliqueSize);
			vector<unsigned long long> GetOneRingNeighbors(unsigned long long vertexIx);
			vector< GraphEdgeBase<TEdgeTag> * > GetEdgeReferences();
			void PrintAllCliques(vector< set<unsigned long long> > allCliques);
			void PrintMathematicaLists();
			GraphBase<TVertexTag, TEdgeTag> GetSubgraph(vector<unsigned long long> & vertexIxs);

			static vector<unsigned long long> VertexSetToVector(set<unsigned long long> vertexSet);
			static set<unsigned long long> VertexVectorToSet(vector<unsigned long long> vertexVector);				
			static set<unsigned long long> SetUnion(set<unsigned long long> set1, set<unsigned long long> set2);
			static bool IsSubset(set<unsigned long long> set1, set<unsigned long long> set2); 
			

		private:
			void PrintClique(set<unsigned long long> vertexSet);
			unsigned long long GetEdgeHash(unsigned long long vertexIx1, unsigned long long vertexIx2);			
			void GetEdgeVertices(unsigned long long & vertexIx1, unsigned long long & vertexIx2, unsigned long long hash);
			map<unsigned long long, unsigned long long> GetAllEdgeTriangleCount();			
			set<unsigned long long> GetEdgeTriangles(unsigned long long edgeHash);
		private:
			vector< GraphVertexBase<TVertexTag> > vertices;
			map< unsigned long long, GraphEdgeBase<TEdgeTag> > edges;
			static const unsigned long long MAX_VERTEX_COUNT = 4294967295;
		};

		template <class TVertexTag, class TEdgeTag> GraphBase<TVertexTag, TEdgeTag>::GraphBase() {
			vertices.clear();
			edges.clear();
		}

		//template <class TVertexTag, class TEdgeTag> GraphBase<TVertexTag, TEdgeTag>::GraphBase(GraphBase<TVertexTag,TEdgeTag> & graph) {
		//	vertices.clear();
		//	edges.clear();
		//	for(unsigned int i = 0; i < graph.GetVertexCount(); i++) {
		//		AddVertex(graph.GetVertex(i).GetExternalIndex(), graph.GetVertex(i).GetWeight(), graph.GetVertex(i).GetTag());
		//	}

		//	for(int i = 0; i < (int)GetVertexCount()-1; i++) {
		//		for(int j = i+1; j < (int)GetVertexCount(); j++) {
		//			if(graph.IsEdge(i, j)) {
		//				AddEdge(i, j, graph.GetEdge(i, j));
		//			}
		//		}
		//	}
		//}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::AddVertex(float weight, TVertexTag tag) {
			GraphVertexBase<TVertexTag> vertex = GraphVertexBase<TVertexTag>(0, weight, tag);
			return AddVertex(vertex);
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::AddVertex(unsigned long long externalIndex, float weight, TVertexTag tag) {
			GraphVertexBase<TVertexTag> vertex = GraphVertexBase<TVertexTag>(externalIndex, weight, tag);
			return AddVertex(vertex);
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::AddVertex(GraphVertexBase<TVertexTag> vertex) {
			unsigned long long ix = vertices.size();
			//vertex.SetIndex(ix);
			vertices.push_back(vertex);
			return ix;
		}

		template <class TVertexTag, class TEdgeTag> GraphVertexBase<TVertexTag> GraphBase<TVertexTag, TEdgeTag>::GetVertex(unsigned long long vertexIx) {
			return vertices[vertexIx];
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::GetVertexCount() {
			return vertices.size();
		}

		template <class TVertexTag, class TEdgeTag> bool GraphBase<TVertexTag, TEdgeTag>::IsEdge(unsigned long long vertexIx1, unsigned long long vertexIx2) {
			unsigned long long hash = GetEdgeHash(vertexIx1, vertexIx2);
			return edges.find(hash) != edges.end();
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, float weight, TEdgeTag tag) {
			GraphEdgeBase<TEdgeTag> edge = GraphEdgeBase<TEdgeTag>(weight, tag);
			AddEdge(vertexIx1, vertexIx2, edge);
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, GraphEdgeBase<TEdgeTag> edge) {
			unsigned long long hash = GetEdgeHash(vertexIx1, vertexIx2);			
			edges[hash] = edge;
			vertices[vertexIx1].AddEdge(hash);
			vertices[vertexIx2].AddEdge(hash);
		}


		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::GetEdgeHash(unsigned long long vertexIx1, unsigned long long vertexIx2) {
			unsigned long long v1, v2;
			if(vertexIx1 < vertexIx2) {
				v1 = vertexIx1;
				v2 = vertexIx2;
			} else {
				v1 = vertexIx2;
				v2 = vertexIx1;
			}
			return v1 * MAX_VERTEX_COUNT + v2;
		}

		template <class TVertexTag, class TEdgeTag> GraphEdgeBase<TEdgeTag> GraphBase<TVertexTag, TEdgeTag>::GetEdge(unsigned long long vertexIx1, unsigned long long vertexIx2) {
			assert(IsEdge(vertexIx1, vertexIx2));
			unsigned long long hash = GetEdgeHash(vertexIx1, vertexIx2);			
			return edges[hash];
		}

		template <class TVertexTag, class TEdgeTag> bool GraphBase<TVertexTag, TEdgeTag>::IsClique(set<unsigned long long> vertexSet) {
			return IsClique(VertexSetToVector(vertexSet));
		}

		template <class TVertexTag, class TEdgeTag> bool GraphBase<TVertexTag, TEdgeTag>::IsClique(vector<unsigned long long> vertexIndices) {
			bool isClique = true;
			for(unsigned int i = 0; isClique && (i < vertexIndices.size()-1); i++) {
				for(unsigned int j = i+1; isClique && (j < vertexIndices.size()); j++) {
					isClique = isClique && IsEdge(vertexIndices[i], vertexIndices[j]);
				}
			}
			return isClique;
		}

		template <class TVertexTag, class TEdgeTag> float GraphBase<TVertexTag, TEdgeTag>::GetCliqueEdgeCost(vector<unsigned long long> vertexIndices) {
			float cliqueEdgeCost = 0.0f;
			for(unsigned int i = 0; i < vertexIndices.size()-1; i++) {
				for(unsigned int j = i+1; j < vertexIndices.size(); j++) {
					cliqueEdgeCost += GetEdge(vertexIndices[i], vertexIndices[j]).GetWeight();
				}
			}
			return cliqueEdgeCost;
		}

		template <class TVertexTag, class TEdgeTag> vector<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::VertexSetToVector(set<unsigned long long> vertexSet) {
			vector<unsigned long long> vertices;
			for(set<unsigned long long>::iterator i = vertexSet.begin(); i != vertexSet.end(); i++) {
				vertices.push_back(*i);
			}
			return vertices;
		}

		template <class TVertexTag, class TEdgeTag> set<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::VertexVectorToSet(vector<unsigned long long> vertexVector) {
			set<unsigned long long> vertexSet;
			for(unsigned int i = 0 ; i < vertexVector.size(); i++) {
				vertexSet.insert(vertexVector[i]);
			}
			return vertexSet;
		}

		template <class TVertexTag, class TEdgeTag> set<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::SetUnion(set<unsigned long long> set1, set<unsigned long long> set2) {
			set<unsigned long long> returnSet = set1;
			for(set<unsigned long long>::iterator i = set2.begin(); i != set2.end(); i++) {
				returnSet.insert(*i);
			}
			return returnSet;
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllCliques() {
			vector< set<unsigned long long> > allCliques;
			set< set<unsigned long long> > duplicateCheck;

			for(unsigned int i = 0; i < vertices.size(); i++) {
				set<unsigned long long> clique;
				clique.insert(i);
				allCliques.push_back(clique);
				duplicateCheck.insert(clique);
			}

			vector< set<unsigned long long> > currentCliques;

			unsigned long long index = 0;
			set<unsigned long long> newClique;
			while(index < allCliques.size()) {
				currentCliques.clear();
				for(unsigned long long i = index+1; i < allCliques.size(); i++) {
					newClique = SetUnion(allCliques[index],allCliques[i]);
					if((newClique.size() == allCliques[index].size() + allCliques[i].size()) && IsClique(newClique)) {
						currentCliques.push_back(newClique);
					}
				}

				for(unsigned long long i = 0; i < currentCliques.size(); i++) {
					if(duplicateCheck.find(currentCliques[i]) == duplicateCheck.end()) {
						allCliques.push_back(currentCliques[i]);
						duplicateCheck.insert(currentCliques[i]);						
					}
				}
				index++;
			}
			return allCliques;
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::PrintClique(set<unsigned long long> vertexSet) {
			vector<unsigned long long> vertices = VertexSetToVector(vertexSet);
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				printf("%d ", vertices[i]);
			}
			printf("\n");
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::PrintAllCliques(vector< set<unsigned long long> > allCliques) {
			for(unsigned long long i = 0; i < allCliques.size(); i++) {
				PrintClique(allCliques[i]);
			}
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllMaximalCliques() {
			vector< set<unsigned long long> > maximalCliques;
			set< set<unsigned long long> > duplicateCheck;
			set< set<unsigned long long> > duplicateQueueCheck;
			vector< vector<unsigned long long> > queue;
			set<unsigned long long> currSet, childSet;
			
			vector<unsigned long long> curr, child;

			vector< set<unsigned long long> > connectedComponents = GetAllConnectedComponents();

			for(unsigned long long i = 0; i < connectedComponents.size(); i++) {
				queue.push_back(VertexSetToVector(connectedComponents[i]));
				duplicateQueueCheck.insert(connectedComponents[i]);
			}			

			while(queue.size() > 0) {
				curr = queue[0];
				queue.erase(queue.begin());
				if(IsClique(curr)) {
					currSet = VertexVectorToSet(curr);
					if(duplicateCheck.find(currSet) == duplicateCheck.end()) {						
						bool notSubset = true;
						for(unsigned int j = 0; notSubset && (j < maximalCliques.size()); j++) {
							notSubset = notSubset && !IsSubset(maximalCliques[j], currSet);
						}
						if(notSubset) {
							maximalCliques.push_back(currSet);
							duplicateCheck.insert(currSet);
						}
					}
				} else {
					for(unsigned long long i = 0; i < curr.size(); i++) {
						child = curr;
						child.erase(child.begin() + i);
						childSet = VertexVectorToSet(child);
						if(duplicateQueueCheck.find(childSet) == duplicateQueueCheck.end()) {
							queue.push_back(child);
							duplicateQueueCheck.insert(childSet);
						}
					}
				}
			}
			return maximalCliques;

		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetLargestMaximalCliques() {
			vector<unsigned long long> vertexSet;
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				vertexSet.push_back(i);
			}

			return GetLargestMaximalCliques(vertexSet);
	
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetLargestMaximalCliques(vector<unsigned long long> vertexSet) {
			vector< set<unsigned long long> > maximalCliques;
			set< set<unsigned long long> > duplicateCheck;
			set< set<unsigned long long> > duplicateQueueCheck;
			vector< vector<unsigned long long> > queue;
			set<unsigned long long> currSet, childSet;
			
			vector<unsigned long long> curr, child;

			vector< set<unsigned long long> > connectedComponents = GetAllConnectedComponents(vertexSet);

			for(unsigned long long i = 0; i < connectedComponents.size(); i++) {
				queue.push_back(VertexSetToVector(connectedComponents[i]));
				duplicateQueueCheck.insert(connectedComponents[i]);
			}			

			int maximalCliqueSize = 0;

			while(queue.size() > 0) {
				curr = queue[0];
				queue.erase(queue.begin());
				if(curr.size() < maximalCliqueSize) {
					// disregard this node.
				} else if(IsClique(curr)) {
					currSet = VertexVectorToSet(curr);
					if(curr.size() > maximalCliqueSize) {
						maximalCliqueSize = curr.size();
						maximalCliques.clear();
						maximalCliques.push_back(currSet);
						duplicateCheck.clear();
						duplicateCheck.insert(currSet);
					} else if(duplicateCheck.find(currSet) == duplicateCheck.end()) {						
						bool notSubset = true;
						for(unsigned int j = 0; notSubset && (j < maximalCliques.size()); j++) {
							notSubset = notSubset && !IsSubset(maximalCliques[j], currSet);
						}
						if(notSubset) {
							maximalCliques.push_back(currSet);
							duplicateCheck.insert(currSet);
						}
					}
				} else {
					for(unsigned long long i = 0; i < curr.size(); i++) {
						child = curr;
						child.erase(child.begin() + i);
						childSet = VertexVectorToSet(child);
						if((child.size() >= maximalCliqueSize) && (duplicateQueueCheck.find(childSet) == duplicateQueueCheck.end())) {
							queue.push_back(child);
							duplicateQueueCheck.insert(childSet);
						}
					}
				}
			}
			return maximalCliques;
	
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetLargestMaximalCliques2() {
			vector<unsigned long long> vertexSet;
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				vertexSet.push_back(i);
			}

			return GetLargestMaximalCliques2(vertexSet);
	
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetLargestMaximalCliques2(vector<unsigned long long> vertexSet) {
			unsigned int maximalSize = 1;
			vector< set<unsigned long long> > maximalCliques;
			vector< vector<unsigned long long> > queue;
			vector<unsigned long long> curr, child;
			for(unsigned int i = 0; i < vertexSet.size(); i++) {
				curr.clear();
				curr.push_back(vertexSet[i]);
				queue.push_back(curr);
				maximalCliques.push_back(VertexVectorToSet(curr));
			}

			map< unsigned long long, vector<unsigned long long> > forwardNodes;
			
			curr.clear();
			for(int i = (int)(vertexSet.size())-1; i >= 0; i--) {
				forwardNodes[vertexSet[i]] = curr;
				curr.insert(curr.begin(), vertexSet[i]);
			}


			vector<unsigned long long> fnodes;

			while(queue.size() > 0) {
				curr = queue[0];
				queue.erase(queue.begin());
				fnodes = forwardNodes[curr[curr.size()-1]];

				for(unsigned int i = 0; i < fnodes.size(); i++) {
					child = curr;
					child.push_back(fnodes[i]);
					if(IsClique(child)) {
						queue.push_back(child);
						if(child.size() > maximalSize) {
							maximalSize = child.size();
							maximalCliques.clear();
							maximalCliques.push_back(VertexVectorToSet(child));
						} else if (child.size() == maximalSize) {
							maximalCliques.push_back(VertexVectorToSet(child));
						}
					}
				}
			}

			return maximalCliques;
	
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllConnectedComponents() {
			vector<unsigned long long> vertexSet;
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				vertexSet.push_back(i);
			}

			return GetAllConnectedComponents(vertexSet);
		}

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllConnectedComponents(vector<unsigned long long> vertexSet) {
			vector< set<unsigned long long> > components;
			vector<bool> added;

			for(unsigned long long i = 0; i < vertices.size(); i++) {
				added.push_back(true);
			}

			for(unsigned int i = 0; i < vertexSet.size(); i++) {
				added[vertexSet[i]] = false;
			}

			vector<unsigned long long> queue;
			unsigned long long curr;
			set<unsigned long long> currComponent;
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				if(!added[i]) {					
					currComponent.clear();
					queue.clear();
					queue.push_back(i);
					while(queue.size() > 0) {
						curr = queue[0];
						queue.erase(queue.begin());
						if(!added[curr]) {
							currComponent.insert(curr);
							added[curr] = true;							
							for(unsigned long long j = 0; j < vertices.size(); j++) {
								if(!added[j] && IsEdge(curr, j)) {
									queue.push_back(j);
								}
							}
						}
					}
					components.push_back(currComponent);
				}
			}
			return components;
		}

		template <class TVertexTag, class TEdgeTag> bool GraphBase<TVertexTag, TEdgeTag>::IsSubset(set<unsigned long long> set1, set<unsigned long long> set2) {
			bool isSubset = true;
			for(set<unsigned long long>::iterator i = set2.begin(); isSubset && (i!= set2.end()); i++) {
				isSubset = isSubset && (set1.find(*i) != set1.end());
			}
			return isSubset;
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::GetHighestValenceVertexIx() {
			int maxValence = -1;
			int maxValenceIx = -1;
			for(int i = 0; i < (int)vertices.size(); i++) {
				if((int)vertices[i].GetValence() > maxValence) {
					maxValence = (int)vertices[i].GetValence();
					maxValenceIx = i;
				}
			}
			return maxValenceIx;
		}

		template <class TVertexTag, class TEdgeTag> vector<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::GetOneRingNeighbors(unsigned long long vertexIx) {
			vector<unsigned long long> neighbors;
			unsigned int valence = vertices[vertexIx].GetValence();
			unsigned long long edgeHash, v1, v2;
			for(unsigned int i = 0; i < valence; i++) {
				edgeHash = vertices[vertexIx].GetEdge(i);
				GetEdgeVertices(v1, v2, edgeHash);
				if(v1 != vertexIx) {
					neighbors.push_back(v1);
				}
				if(v2 != vertexIx) {
					neighbors.push_back(v2);
				}
			}

			return neighbors;
		}

		template <class TVertexTag, class TEdgeTag> set<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::GetLowestCostCliqueInOneRing(unsigned long long vertexIx) {
			vector<unsigned long long> vertexSet = GetOneRingNeighbors(vertexIx);
			vertexSet.push_back(vertexIx);
			vector<set <unsigned long long> > cliques = GetLargestMaximalCliques2(vertexSet);
			float cost, minCost = MAX_FLOAT;
			int cliqueIx = -1;

			for(unsigned int i = 0; i < cliques.size(); i++) {
				if(cliques[i].find(vertexIx) != cliques[i].end()) {
					cost = GetCliqueEdgeCost(VertexSetToVector(cliques[i]));
					if(cost < minCost) {
						minCost = cost;
						cliqueIx = i;
					}
				}
			}
			if(cliqueIx == -1) {
				set <unsigned long long> singleton;
				singleton.insert(vertexIx);
				return singleton;
			} else {
				return cliques[cliqueIx];
			}
		}

		template <class TVertexTag, class TEdgeTag> set<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::GetLowestCostCliqueTriangleApprox(int smallestCliqueSize) {
			set<unsigned long long> clique;

			// Inputvalidation: Returns lowest cost vertex if no edges exist
			if(edges.size() == 0) { 
				if(smallestCliqueSize <= 1) {
					if(vertices.size() == 0) {
						return clique;
					}
					int minVertexIx = 0;
					float minVertexWeight = vertices[minVertexIx].GetWeight();
					

					for(unsigned int i = 0; i < vertices.size(); i++) {
						if(vertices[i].GetWeight() < minVertexWeight) {
							minVertexWeight = vertices[i].GetWeight();
							minVertexIx = i;
						}
					}
					clique.insert(minVertexIx);
				}
				return clique;
			}

			map<unsigned long long, unsigned long long> edgeTriangleCount = GetAllEdgeTriangleCount();
			
			unsigned long long v1, v2, hash, maxEdgeHash = edges.begin()->first;
			float maxEdgeWeight = edges[maxEdgeHash].GetWeight();
			int maxEdgeValence = edgeTriangleCount[maxEdgeHash];

			for(typename map< unsigned long long, GraphEdgeBase<TEdgeTag> >::iterator i = edges.begin(); i != edges.end(); i++) {
				hash = i->first;
				if (((edgeTriangleCount[hash] > maxEdgeValence)) || ((edgeTriangleCount[hash] == maxEdgeValence) && (edges[hash].GetWeight() < maxEdgeWeight))) {
					maxEdgeHash = hash;
					maxEdgeValence = (int)edgeTriangleCount[hash];
					maxEdgeWeight = edges[hash].GetWeight();
				}
			}


			// Inputvalidation: Returns lowest cost edge if no triangles exist
			if(maxEdgeValence == 0) { 
				if(smallestCliqueSize <= 2) {
					unsigned long long minEdgeHash = edges.begin()->first;
					float minEdgeWeight = edges[minEdgeHash].GetWeight();
					for(typename map< unsigned long long, GraphEdgeBase<TEdgeTag> >::iterator i = edges.begin(); i != edges.end(); i++) {
						hash = i->first;
						if(edges[hash].GetWeight() < minEdgeWeight) {
							minEdgeWeight = edges[hash].GetWeight();
							minEdgeHash = hash;
						}					
					}
					GetEdgeVertices(v1, v2, minEdgeHash);
					clique.insert(v1);
					clique.insert(v2);
				}
				return clique;
			}

			vector<unsigned long long> triangleVertices = VertexSetToVector(GetEdgeTriangles(maxEdgeHash));
			GetEdgeVertices(v1, v2, maxEdgeHash);

			GorgonPriorityQueue<float, unsigned int> triangleOrdering = GorgonPriorityQueue<float, unsigned int>(false);
			float triangleCost;
			for(unsigned int i = 0; i < triangleVertices.size(); i++) {
				triangleCost = GetEdge(v1, triangleVertices[i]).GetWeight() + GetEdge(v2, triangleVertices[i]).GetWeight();
				triangleOrdering.Add(triangleCost, i);
			}

			edgeTriangleCount.clear();
			
			vector<unsigned long long> currClique;
			currClique.push_back(v1);
			currClique.push_back(v2);

			int minIx;
			while(!triangleOrdering.IsEmpty()) {
				minIx = triangleOrdering.PopFirst();				
				currClique.push_back(triangleVertices[minIx]);
				if(!IsClique(currClique)) {
					currClique.erase(currClique.end()-1);
				}
			}
			if(currClique.size() < smallestCliqueSize) {
				currClique.clear();
			}

			return VertexVectorToSet(currClique);			
		}


		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllCliquesTriangleApprox(int smallestCliqueSize) {
			vector< set<unsigned long long> > allCliques;
			allCliques.clear();

			GraphBase<TVertexTag, TEdgeTag> tempGraph(*this);
			for(unsigned long long i = 0; i < tempGraph.GetVertexCount(); i++) {
				tempGraph.vertices[i].SetExternalIndex(i);
			}
			
			set<unsigned long long> maxClique;
			vector<unsigned long long> maxCliqueVector;
			set<unsigned long long> maxCliqueGlobalIndices;
			int minSize = smallestCliqueSize;
			vector<unsigned long long> subGraphVertices;

			bool found = true;
			while(found && (tempGraph.GetVertexCount() > 0)) {
				maxClique = tempGraph.GetLowestCostCliqueTriangleApprox(smallestCliqueSize);
				found = maxClique.size() >= smallestCliqueSize;
				if(found) {
					maxCliqueVector = tempGraph.VertexSetToVector(maxClique);
					if(maxClique.size() >= minSize) {
						maxCliqueGlobalIndices.clear();
						for(unsigned long long i = 0; i < maxCliqueVector.size(); i++) {
							maxCliqueGlobalIndices.insert(tempGraph.vertices[maxCliqueVector[i]].GetExternalIndex());
						}
						allCliques.push_back(maxCliqueGlobalIndices);
					}

					subGraphVertices.clear();
					for(unsigned long long i = 0; i < tempGraph.GetVertexCount(); i++) {
						if(maxClique.find(i) == maxClique.end()) {
							subGraphVertices.push_back(i);
						}
					}
					tempGraph = tempGraph.GetSubgraph(subGraphVertices);
				}
			}

			return allCliques;
			
		}

		template <class TVertexTag, class TEdgeTag> set<unsigned long long> GraphBase<TVertexTag, TEdgeTag>::GetEdgeTriangles(unsigned long long edgeHash) {
			set<unsigned long long> triangles, v1Neighbors, v2Neighbors;
			unsigned long long v1, v2;

			GetEdgeVertices(v1, v2, edgeHash);
			v1Neighbors = VertexVectorToSet(GetOneRingNeighbors(v1));
			v2Neighbors = VertexVectorToSet(GetOneRingNeighbors(v2));
			set_intersection(v1Neighbors.begin(), v1Neighbors.end(), v2Neighbors.begin(), v2Neighbors.end(), inserter(triangles, triangles.begin()));
			
			return triangles;
		}


		template <class TVertexTag, class TEdgeTag> map<unsigned long long, unsigned long long> GraphBase<TVertexTag, TEdgeTag>::GetAllEdgeTriangleCount() {
			map< unsigned long long, unsigned long long> edgeTriangleCount;
			for(typename map< unsigned long long, GraphEdgeBase<TEdgeTag> >::iterator i = edges.begin(); i != edges.end(); i++) {
				edgeTriangleCount[i->first] = GetEdgeTriangles(i->first).size();
			}
			return edgeTriangleCount;
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::GetEdgeVertices(unsigned long long & vertexIx1, unsigned long long & vertexIx2, unsigned long long hash) {
			vertexIx1 = hash / MAX_VERTEX_COUNT;
			vertexIx2 = hash % MAX_VERTEX_COUNT;
		}

		template <class TVertexTag, class TEdgeTag> void GraphBase<TVertexTag, TEdgeTag>::PrintMathematicaLists() {
			printf("v = {");
			for(unsigned int i = 0; i < vertices.size(); i++) {
				if(i != 0) {
					printf(", ");
				}
				printf("%d", i);
			}
			printf("};\n");

			bool first = true;
			printf("e = {");
			for(int i = 0; i < (int)vertices.size()-1; i++) {
				for(int j = i+1; j < (int)vertices.size(); j++) {				
					if(IsEdge(i, j)) {				
						if(!first) {
							printf(", "); 						
						}
						first = false;
						printf("{%d, %d}", i+1, j+1);
					}
				}
			}
			printf("};\n");
		}

		template <class TVertexTag, class TEdgeTag> GraphBase<TVertexTag, TEdgeTag> GraphBase<TVertexTag, TEdgeTag>::GetSubgraph(vector<unsigned long long> & childVertices) {
			GraphBase<TVertexTag, TEdgeTag> child;

			map<unsigned int, unsigned int> vertexIndices;

			unsigned int childIndex;

			for(unsigned int i = 0; i < childVertices.size(); i++) {
				childIndex = child.AddVertex(GetVertex(childVertices[i]).GetExternalIndex(), GetVertex(childVertices[i]).GetWeight(), GetVertex(childVertices[i]).GetTag());
				vertexIndices[childVertices[i]]  = childIndex;
			}	

			unsigned long long v1, v2;			

			for(typename map< unsigned long long, GraphEdgeBase<TEdgeTag> >::iterator i = edges.begin(); i != edges.end(); i++) {
				this->GetEdgeVertices(v1, v2, i->first);
				if((vertexIndices.find(v1) != vertexIndices.end()) && (vertexIndices.find(v2) != vertexIndices.end())) {
					child.AddEdge(vertexIndices[v1], vertexIndices[v2], i->second);
				}
			}


			return child;

		}
	}
}

#endif
