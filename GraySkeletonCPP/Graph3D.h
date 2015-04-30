// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An implementation of a 3D Spatial graph


#ifndef GRAYSKELETONCPP_GRAPH_3D_H
#define GRAYSKELETONCPP_GRAPH_3D_H

#include "GlobalDefinitions.h"
#include <MathTools/BasicDefines.h>

using namespace wustl_mm::MathTools;


// All edge costs have to be positive 
namespace wustl_mm {
	namespace GraySkeletonCPP {
	
		const int NO_NODE_MARKER = -1;

		template <class T>	struct GraphNodeType {			
			T edgeCosts[26];
		};

		template <class T>	class Graph3D {
		public:
			
			Graph3D(int sizeX, int sizeY, int sizeZ, int connectivity);
			~Graph3D();

			bool IsEdge(int index, int neighborIndex);
			bool IsEdge(int x, int y, int z, int neighborIndex);
			int GetIndex(int x, int y, int z);
			T GetEdgeCost(int index, int neighborIndex);
			T GetEdgeCost(int x, int y, int z, int neighborIndex);
			T MaxEdgeCost();
			T MinEdgeCost();
			void SetEdgeCost(T cost, int index, int neighborIndex);			
			void SetEdgeCost(T cost, int x, int y, int z, int neighborIndex);			
			void MultiplyAllEdgeCostsByConstant(T c);
			void Normalize();
			static Graph3D<T> * MergeGraphs(Graph3D<T> * graph1, Graph3D<T> * graph2, T ratio1, T ratio2);
		protected:
			int sizeX;
			int sizeY;
			int sizeZ;
			int connectivity;
			int * graphPtrs;
			vector< GraphNodeType<T> > nodeList;
		};

		template <class T>	Graph3D<T>::Graph3D(int sizeX, int sizeY, int sizeZ, int connectivity) {
			this->sizeX = sizeX;
			this->sizeY = sizeY;
			this->sizeZ = sizeZ;
			this->connectivity = connectivity;
			
			graphPtrs = new int[sizeX * sizeY * sizeZ];
			for(int i = 0; i < sizeX * sizeY * sizeZ; i++) {
				graphPtrs[i] = NO_NODE_MARKER;
			}
		}	

		template <class T>	Graph3D<T>::~Graph3D() {
			
			nodeList.clear();
			delete [] graphPtrs;
		}

		template <class T>	bool Graph3D<T>::IsEdge(int index, int neighborIndex) {
			return ((graphPtrs[index] != NO_NODE_MARKER) && (nodeList[graphPtrs[index]].edgeCosts[neighborIndex] >= 0));			
		}

		template <class T>	bool Graph3D<T>::IsEdge(int x, int y, int z, int neighborIndex) {
			return IsEdge(GetIndex(x, y, z), neighborIndex);
		}

		template <class T>	int Graph3D<T>::GetIndex(int x, int y, int z) {
			return (x * sizeY * sizeZ + y * sizeZ + z);
		}

		template <class T>	T Graph3D<T>::GetEdgeCost(int index, int neighborIndex) {
			return nodeList[graphPtrs[index]].edgeCosts[neighborIndex];
		}

		template <class T>	T Graph3D<T>::GetEdgeCost(int x, int y, int z, int neighborIndex) {
			return GetEdgeCost(GetIndex(x, y, z), neighborIndex);
		}

		template <class T>	T Graph3D<T>::MaxEdgeCost() {
			double maxCost = MIN_DOUBLE;
			for(unsigned int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(nodeList[i].edgeCosts[j] >= 0) {
						maxCost = max(maxCost, (double)nodeList[i].edgeCosts[j]);
					}
				}
			}
			return (T)maxCost;
		}

		template <class T>	T Graph3D<T>::MinEdgeCost() {
			double minCost = MAX_DOUBLE;
			for(unsigned int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(nodeList[i].edgeCosts[j] >= 0) {
						minCost = min(minCost, (double)nodeList[i].edgeCosts[j]);
					}
				}
			}
			return (T)minCost;
		}

		template <class T>	void Graph3D<T>::SetEdgeCost(T cost, int index, int neighborIndex) {
			if(graphPtrs[index] == NO_NODE_MARKER) {
				graphPtrs[index] = nodeList.size();
				GraphNodeType<T> node;
				for(int i = 0; i < connectivity; i++) {
					node.edgeCosts[i] = NO_NODE_MARKER;
				}
				nodeList.push_back(node);
			}

			nodeList[graphPtrs[index]].edgeCosts[neighborIndex] = cost;
		}

		template <class T>	void Graph3D<T>::SetEdgeCost(T cost, int x, int y, int z, int neighborIndex) {
			SetEdgeCost(cost, GetIndex(x, y, z), neighborIndex);
		}

		template <class T>	void Graph3D<T>::MultiplyAllEdgeCostsByConstant(T c) {
			for(int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(nodeList[i].edgeCosts[j] >= 0) {
						nodeList[i].edgeCosts[j] = nodeList[i].edgeCosts[j] * c;
					}
				}
			}
		}

		template <class T>	void Graph3D<T>::Normalize() {
			T maxCost = MaxEdgeCost();
			T minCost = MinEdgeCost();
			for(unsigned int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(nodeList[i].edgeCosts[j] >= 0) {
						nodeList[i].edgeCosts[j] = (nodeList[i].edgeCosts[j] - minCost)/(maxCost - minCost);
					}
				}
			}
		}

		template <class T>	Graph3D<T> * Graph3D<T>::MergeGraphs(Graph3D<T> * graph1, Graph3D<T> * graph2, T ratio1, T ratio2) {
			if((graph1->sizeX != graph2->sizeX) || (graph1->sizeY != graph2->sizeY) || (graph1->sizeZ != graph2->sizeZ) || (graph1->connectivity != graph2->connectivity)) {
				printf("Graphs cannot be combined, they have mismatched sizes!\n");
				return NULL;
			}

			Graph3D<T> * merged = new Graph3D(graph1->sizeX, graph1->sizeY, graph1->sizeZ, graph1->connectivity);
			for(int i = 0; i < graph1->sizeX * graph1->sizeY * graph1->sizeZ; i++) {
				for(int j = 0; j < graph1->connectivity; j++) {
					if(graph1->IsEdge(i,j) && graph2->IsEdge(i,j)) {
						merged->SetEdgeCost(graph1->GetEdgeCost(i, j) * ratio1 + graph2->GetEdgeCost(i, j) * ratio2, i, j);
					} else if(graph1->IsEdge(i,j)) {
						merged->SetEdgeCost(graph1->GetEdgeCost(i, j) * ratio1, i, j);
					} else if(graph2->IsEdge(i,j)) {
						merged->SetEdgeCost(graph2->GetEdgeCost(i, j) * ratio2, i, j);
					}					
				}
			}
			return merged;

		}
	}
}


#endif
