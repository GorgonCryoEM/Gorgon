// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.9  2008/10/28 18:46:52  ssa1
//   Fixing octree neighbor search, and changing the structure tensor cost function
//
//   Revision 1.8  2008/09/29 15:42:54  ssa1
//   Adding in CVS meta information
//

#ifndef FOUNDATION_GRAPH_BASE_H
#define FOUNDATION_GRAPH_BASE_H

#include "GraphVertexBase.h"
#include "GraphEdgeBase.h"
#include <vector>
#include <map>
#include <set>

using namespace std;

namespace wustl_mm {
	namespace Foundation {

		template <class TVertexTag, class TEdgeTag> class GraphBase {
		public:
			GraphBase();
			unsigned long long AddVertex(float weight, TVertexTag tag);
			unsigned long long AddVertex(GraphVertexBase<TVertexTag> vertex);
			GraphVertexBase<TVertexTag> GetVertex(unsigned long long vertexIx);
			bool IsEdge(unsigned long long vertexIx1, unsigned long long vertexIx2);			
			void AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, float weight, TEdgeTag tag);
			void AddEdge(unsigned long long vertexIx1, unsigned long long vertexIx2, GraphEdgeBase<TEdgeTag> edge);
			GraphEdgeBase<TEdgeTag> GetEdge(unsigned long long vertexIx1, unsigned long long vertexIx2);
			// Graph functions
			bool IsClique(set<unsigned long long> vertexSet);
			bool IsClique(vector<unsigned long long> vertexIndices);
			vector< set<unsigned long long> > GetAllConnectedComponents();
			vector< set<unsigned long long> > GetAllCliques();
			vector< set<unsigned long long> > GetAllMaximalCliques();
			void PrintAllCliques(vector< set<unsigned long long> > allCliques);

		private:
			void PrintClique(set<unsigned long long> vertexSet);
			vector<unsigned long long> VertexSetToVector(set<unsigned long long> vertexSet);
			set<unsigned long long> VertexVectorToSet(vector<unsigned long long> vertexVector);				
			set<unsigned long long> SetUnion(set<unsigned long long> set1, set<unsigned long long> set2);
			bool IsSubset(set<unsigned long long> set1, set<unsigned long long> set2); 
			unsigned long long GetEdgeHash(unsigned long long vertexIx1, unsigned long long vertexIx2);
		private:
			vector< GraphVertexBase<TVertexTag> > vertices;
			map< unsigned long long, GraphEdgeBase<TEdgeTag> > edges;
			static const unsigned long long MAX_VERTEX_COUNT = 4294967296;
		};

		template <class TVertexTag, class TEdgeTag> GraphBase<TVertexTag, TEdgeTag>::GraphBase() {
			vertices.clear();
			edges.clear();
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::AddVertex(float weight, TVertexTag tag) {
			GraphVertexBase<TVertexTag> vertex = GraphVertexBase<TVertexTag>(0, weight, tag);
			return AddVertex(vertex);
		}

		template <class TVertexTag, class TEdgeTag> unsigned long long GraphBase<TVertexTag, TEdgeTag>::AddVertex(GraphVertexBase<TVertexTag> vertex) {
			unsigned long long ix = vertices.size();
			vertex.SetIndex(ix);
			vertices.push_back(vertex);
			return ix;
		}

		template <class TVertexTag, class TEdgeTag> GraphVertexBase<TVertexTag> GraphBase<TVertexTag, TEdgeTag>::GetVertex(unsigned long long vertexIx) {
			return vertices[vertexIx];
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

		template <class TVertexTag, class TEdgeTag> vector< set<unsigned long long> > GraphBase<TVertexTag, TEdgeTag>::GetAllConnectedComponents() {
			vector< set<unsigned long long> > components;
			vector<bool> added;
			for(unsigned long long i = 0; i < vertices.size(); i++) {
				added.push_back(false);
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
	}
}

#endif