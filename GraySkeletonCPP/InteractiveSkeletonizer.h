#ifndef GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H
#define GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H

#include <queue>
#include "GlobalDefinitions.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <Foundation/Octree.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		struct nodeAttrib{
			float cost;
			NonManifoldMeshVertex<nodeAttrib> * returnNode;
			OctreeNode<unsigned int> * octreeNode;
		};

		struct edgeAttrib {
			float medialCost;
			float smoothCost;
			float totalCost;
		};

		typedef NonManifoldMesh<nodeAttrib, edgeAttrib, bool> GraphType;

		class InteractiveSkeletonizer : public VolumeSkeletonizer {
		public:
			InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo = false);
			~InteractiveSkeletonizer();
			GraphType * GetGraph();
			vector<Vector3DInt> GetPath(Vector3DInt endPoint);			
			Vector3DInt FindClosestSkeletalPoint(Vector3DInt point);
			void SetGraphWeights(double skeletonRatio, double structureTensorRatio);
			void CalculateMinimalSpanningTree(Vector3DInt seedPoint);
									
		protected:
			double GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type);			
			GraphType * graph;
			Octree<unsigned int> * octree;
			Vector3DInt seedPoint;
			Vector3DInt offset;
			EigenResults3D * volumeEigens;

			static const int CONNECTIVITY = 26;
			static const char SEED_POINT_FLAG = 50;
		};

		InteractiveSkeletonizer::InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo) : VolumeSkeletonizer(0, curveRadius, 0,0) {
			appTimeManager.PushCurrentTime();
			graph = new GraphType();		
			offset = Vector3DInt(MAX_GAUSSIAN_FILTER_RADIUS, MAX_GAUSSIAN_FILTER_RADIUS, MAX_GAUSSIAN_FILTER_RADIUS);
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
					
			Volume * nullVol = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
			appTimeManager.PushCurrentTime();
			Volume * skeleton = GetImmersionThinning(sourceVol, nullVol, minGray, maxGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			appTimeManager.PopAndDisplayTime("	Thinning completed: %f seconds\n");
			delete nullVol;
			PruneCurves(skeleton, minCurveSize);
			//#ifdef SAVE_INTERMEDIATE_RESULTS
				skeleton->toMRCFile("CurveSkeleton.mrc");				
			//#endif
			
			octree = new Octree<unsigned int>(skeleton->getSizeX() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeY() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeZ()- 2*MAX_GAUSSIAN_FILTER_RADIUS);
			//octree->PrintStructure();
			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < sourceVol->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < sourceVol->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < sourceVol->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							octree->AddNewLeaf(x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS, 1);
						}
					}
				}
			}

			Volume * maskVol = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ());
			vector<OctreeNode<unsigned int> *> cells = octree->GetCells();
			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					cells[i]->tag = graph->AddVertex(Vector3DFloat(cells[i]->pos[0], cells[i]->pos[1], cells[i]->pos[2]), nodeAttrib());
					graph->vertices[cells[i]->tag].tag.octreeNode = cells[i];
					maskVol->setDataAt(cells[i]->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, cells[i]->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, cells[i]->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, 1.0);
				}
			}

			vector<OctreeNode<unsigned int> *> neighbors;

			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					neighbors = octree->GetNeighbors(cells[i]);
					for(unsigned int j = 0; j < neighbors.size(); j++) {
						graph->AddEdge(cells[i]->tag, neighbors[j]->tag, edgeAttrib());
					}				
				}
			}

			//graph->ToOffCells("Graph.off");


			Vector3DFloat * volumeGradient = GetVolumeGradient2(sourceVol);					
			volumeEigens = GetEigenResults2(maskVol, volumeGradient, gaussianFilterCurveRadius, curveRadius, true);
			delete maskVol;

			float maxProjectedScore = MIN_FLOAT;
			float minProjectedScore = MAX_FLOAT;
			OctreeNode<unsigned int> * n1, * n2;
			float s1, s2;

			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				n1 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].tag.octreeNode;
				n2 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].tag.octreeNode;
				if( (graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position - graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position).Length() < 2.0) {						
					graph->edges[i].tag.medialCost = 1.0 - 
						(skeleton->getDataAt(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS) +  
						 skeleton->getDataAt(n2->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS)) / 2.0;
				} else {
					graph->edges[i].tag.medialCost = 1.0;
				}
				s1 = GetStructureTensorProjectedScore(
						volumeEigens[skeleton->getIndex(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS)], 
						graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position - graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position,
						2, PRUNING_CLASS_PRUNE_CURVES);
				s2 = GetStructureTensorProjectedScore(
						volumeEigens[skeleton->getIndex(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS)], 
						graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position - graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position,
						2, PRUNING_CLASS_PRUNE_CURVES);
				graph->edges[i].tag.smoothCost = s1 + s2;
				minProjectedScore = min(graph->edges[i].tag.smoothCost, minProjectedScore);
				maxProjectedScore = max(graph->edges[i].tag.smoothCost, maxProjectedScore);								
			}

			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				graph->edges[i].tag.smoothCost = (graph->edges[i].tag.smoothCost - minProjectedScore) / (maxProjectedScore - minProjectedScore);
			}


			delete [] volumeGradient;
			if(!storeEigenInfo) {
				delete [] volumeEigens; 
				volumeEigens = NULL;
			}
			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			delete skeleton;

			appTimeManager.PopAndDisplayTime("Creating graphs: %f seconds!\n"); 
		}


		InteractiveSkeletonizer::~InteractiveSkeletonizer() {
			delete graph;
			delete octree;
			if(volumeEigens != NULL) {
				delete [] volumeEigens;
			}
		}

		double InteractiveSkeletonizer::GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type) {
			skeletonDirection.Normalize();
			float score = 0.0;
			switch(type) {
				case PRUNING_CLASS_PRUNE_CURVES:
					for(int i = 0 ; i < 3; i++) {
						score +=  pow(abs(skeletonDirection * imageEigen.vectors[i]) * sqrt(imageEigen.values[i]), power);
					}
					break;
				default:
					score = 1.0;
			}
			return score;
		}
		GraphType * InteractiveSkeletonizer::GetGraph() {
			return graph;
		}
		vector<Vector3DInt> InteractiveSkeletonizer::GetPath(Vector3DInt endPoint) {
			
			appTimeManager.PushCurrentTime();
			//printf("Getting path at {%ld %ld %ld}\n", endPoint.X(), endPoint.Y(), endPoint.Z()); flushall();

			vector<Vector3DInt> outPath;
			
			OctreeNode<unsigned int> * node = octree->GetLeaf(endPoint.X(), endPoint.Y(), endPoint.Z());
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			currentNode = &graph->vertices[node->tag];
			outPath.push_back(Vector3DInt(currentNode->tag.octreeNode->pos[0], currentNode->tag.octreeNode->pos[1], currentNode->tag.octreeNode->pos[2]));

			while(currentNode->tag.returnNode != NULL) {
				currentNode = currentNode->tag.returnNode;
				outPath.push_back(Vector3DInt(currentNode->tag.octreeNode->pos[0], currentNode->tag.octreeNode->pos[1], currentNode->tag.octreeNode->pos[2]));
			}

			appTimeManager.PopAndDisplayTime("Finding path: %f seconds!\n");
			return outPath; 


		}
		Vector3DInt InteractiveSkeletonizer::FindClosestSkeletalPoint(Vector3DInt point) {
			appTimeManager.PushCurrentTime();
			//printf("Starting finding closest skeletal point at {%ld %ld %ld}\n", point.X(), point.Y(), point.Z()); flushall();
			OctreeNode<unsigned int> * node = octree->GetLeaf(point.X(), point.Y(), point.Z());
			appTimeManager.PopAndDisplayTime("Done: %f seconds!\n");
			return Vector3DInt(node->pos[0], node->pos[1], node->pos[2]);	
			
			
		}

		void InteractiveSkeletonizer::SetGraphWeights(double skeletonRatio, double structureTensorRatio){
			appTimeManager.PushCurrentTime();
			Vector3DFloat p1, p2;
			float length;
			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				p1 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position;
				p2 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position;
				length = (p1-p2).Length();
				graph->edges[i].tag.totalCost = (skeletonRatio * graph->edges[i].tag.medialCost + structureTensorRatio * graph->edges[i].tag.smoothCost) * length;
			}

			appTimeManager.PopAndDisplayTime("Merging graphs: %f seconds!\n");
			
		}
		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(Vector3DInt seedPoint) {
			appTimeManager.PushCurrentTime();
			//printf("Calculating MST {%ld %ld %ld}\n", seedPoint.X(), seedPoint.Y(), seedPoint.Z()); flushall();
			OctreeNode<unsigned int> * node = octree->GetLeaf(seedPoint.X(), seedPoint.Y(), seedPoint.Z());
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			Volume * costVol = new Volume(octree->size[0], octree->size[1], octree->size[2]);

			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				graph->vertices[i].tag.cost = -1;
			}

			currentNode = &graph->vertices[node->tag];
			currentNode->tag.cost = 0;
			costVol->setDataAt(currentNode->tag.octreeNode->pos[0], currentNode->tag.octreeNode->pos[1], currentNode->tag.octreeNode->pos[2], currentNode->tag.cost);
			currentNode->tag.returnNode = NULL;

			queue<NonManifoldMeshVertex<nodeAttrib> *> pointList;
			pointList.push(currentNode);
			float cost, vertexCost;
			unsigned int edgeIx;
			unsigned int vertexIx;


			while(!pointList.empty()) {	
				currentNode = pointList.front();
				pointList.pop();
				for(unsigned int i = 0; i < currentNode->edgeIds.size(); i++) {
					edgeIx = graph->GetEdgeIndex(currentNode->edgeIds[i]);
					cost = currentNode->tag.cost + graph->edges[edgeIx].tag.totalCost;
					for(unsigned int j = 0; j < 2; j++) {
						vertexIx = graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[j]);
						vertexCost = graph->vertices[vertexIx].tag.cost;
						if((vertexCost < 0) || (vertexCost > cost)) {
							costVol->setDataAt(graph->vertices[vertexIx].tag.octreeNode->pos[0], graph->vertices[vertexIx].tag.octreeNode->pos[1], graph->vertices[vertexIx].tag.octreeNode->pos[2], graph->vertices[vertexIx].tag.cost);
							graph->vertices[vertexIx].tag.cost = cost;
							graph->vertices[vertexIx].tag.returnNode = currentNode;
							pointList.push(&graph->vertices[vertexIx]);
						}
					}
				}				
			}
			costVol->toMRCFile("Cost.mrc");
			delete costVol;
			appTimeManager.PopAndDisplayTime("Initializing seed point: %f seconds!\n");

		}

	}
}


#endif