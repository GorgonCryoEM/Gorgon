#ifndef GRAYSKELETONCPP_GRAPH_3D_H
#define GRAYSKELETONCPP_GRAPH_3D_H

#include "GlobalDefinitions.h"
#include <MathTools/BasicDefines.h>

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace GraySkeletonCPP {

		const int NO_NODE_MARKER = -1;
		const double NO_EDGE_MARKER = -1.0;

		struct GraphNodeType {
			double edgeCosts[26];
		};

		class Graph3D {
		public:
			
			Graph3D(int sizeX, int sizeY, int sizeZ, int connectivity);
			~Graph3D();

			bool IsEdge(int index, int neighborIndex);
			bool IsEdge(int x, int y, int z, int neighborIndex);
			int GetIndex(int x, int y, int z);
			double GetEdgeCost(int index, int neighborIndex);
			double GetEdgeCost(int x, int y, int z, int neighborIndex);
			double MaxEdgeCost();
			double MinEdgeCost();
			void SetEdgeCost(double cost, int index, int neighborIndex);			
			void SetEdgeCost(double cost, int x, int y, int z, int neighborIndex);			
			void MultiplyAllEdgeCostsByConstant(double c);
			void Normalize();
			static Graph3D * MergeGraphs(Graph3D * graph1, Graph3D * graph2, double ratio1, double ratio2);
		protected:
			int sizeX;
			int sizeY;
			int sizeZ;
			int connectivity;
			int * graphPtrs;
			vector<GraphNodeType> nodeList;
		};

		Graph3D::Graph3D(int sizeX, int sizeY, int sizeZ, int connectivity) {
			this->sizeX = sizeX;
			this->sizeY = sizeY;
			this->sizeZ = sizeZ;
			this->connectivity = connectivity;
			
			graphPtrs = new int[sizeX * sizeY * sizeZ];
			for(int i = 0; i < sizeX * sizeY * sizeZ; i++) {
				graphPtrs[i] = NO_NODE_MARKER;
			}
		}	

		Graph3D::~Graph3D() {
			nodeList.clear();
			delete [] graphPtrs;
		}

		bool Graph3D::IsEdge(int index, int neighborIndex) {
			return ((graphPtrs[index] != NO_NODE_MARKER) && !isEqual(nodeList[graphPtrs[index]].edgeCosts[neighborIndex], NO_EDGE_MARKER));			
		}

		bool Graph3D::IsEdge(int x, int y, int z, int neighborIndex) {
			return IsEdge(GetIndex(x, y, z), neighborIndex);
		}

		int Graph3D::GetIndex(int x, int y, int z) {
			return (x * sizeY * sizeZ + y * sizeZ + z);
		}

		double Graph3D::GetEdgeCost(int index, int neighborIndex) {
			return nodeList[graphPtrs[index]].edgeCosts[neighborIndex];
		}

		double Graph3D::GetEdgeCost(int x, int y, int z, int neighborIndex) {
			return GetEdgeCost(GetIndex(x, y, z), neighborIndex);
		}

		double Graph3D::MaxEdgeCost() {
			double maxCost = MIN_DOUBLE;
			for(int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(!isEqual(nodeList[i].edgeCosts[j], NO_EDGE_MARKER)) {
						maxCost = max(maxCost, nodeList[i].edgeCosts[j]);
					}
				}
			}
			return maxCost;
		}

		double Graph3D::MinEdgeCost() {
			double minCost = MAX_DOUBLE;
			for(int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(!isEqual(nodeList[i].edgeCosts[j], NO_EDGE_MARKER)) {
						minCost = min(minCost, nodeList[i].edgeCosts[j]);
					}
				}
			}
			return minCost;
		}

		void Graph3D::SetEdgeCost(double cost, int index, int neighborIndex) {
			if(graphPtrs[index] == NO_NODE_MARKER) {
				graphPtrs[index] = nodeList.size();
				GraphNodeType node;
				for(int i = 0; i < connectivity; i++) {
					node.edgeCosts[i] = NO_EDGE_MARKER;
				}
				nodeList.push_back(node);
			}

			nodeList[graphPtrs[index]].edgeCosts[neighborIndex] = cost;
		}

		void Graph3D::SetEdgeCost(double cost, int x, int y, int z, int neighborIndex) {
			SetEdgeCost(cost, GetIndex(x, y, z), neighborIndex);
		}

		void Graph3D::MultiplyAllEdgeCostsByConstant(double c) {
			for(int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(!isEqual(nodeList[i].edgeCosts[j], NO_EDGE_MARKER)) {
						nodeList[i].edgeCosts[j] = nodeList[i].edgeCosts[j] * c;
					}
				}
			}
		}

		void Graph3D::Normalize() {
			double maxCost = MaxEdgeCost();
			double minCost = MinEdgeCost();
			for(int i = 0; i < nodeList.size(); i++) {
				for(int j = 0; j < connectivity; j++) {					
					if(!isEqual(nodeList[i].edgeCosts[j], NO_EDGE_MARKER)) {
						nodeList[i].edgeCosts[j] = (nodeList[i].edgeCosts[j] - minCost)/(maxCost - minCost);
					}
				}
			}
		}

		Graph3D * Graph3D::MergeGraphs(Graph3D * graph1, Graph3D * graph2, double ratio1, double ratio2) {
			if((graph1->sizeX != graph2->sizeX) || (graph1->sizeY != graph2->sizeY) || (graph1->sizeZ != graph2->sizeZ) || (graph1->connectivity != graph2->connectivity)) {
				printf("Graphs cannot be combined, they have mismatched sizes!\n");
				return NULL;
			}

			Graph3D * merged = new Graph3D(graph1->sizeX, graph1->sizeY, graph1->sizeZ, graph1->connectivity);
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