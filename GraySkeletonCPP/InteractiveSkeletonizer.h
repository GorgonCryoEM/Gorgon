// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs interactive skeletonization of a volume

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.19  2008/09/29 16:30:15  ssa1
//   Adding in CVS meta information
//

#ifndef GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H
#define GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H

#include <queue>
#include "GlobalDefinitions.h"
#include "VolumeSkeletonizer.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <Foundation/Octree.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		struct octreeTagType {
			unsigned int tag1;
			unsigned int tag2;
		};

		struct nodeAttrib{
			float medialnessCost;
			float sketchCost;
			float mstCost;
			NonManifoldMeshVertex<nodeAttrib> * returnNode;
			OctreeNode<octreeTagType> * octreeNode;
		};

		struct edgeAttrib {
			float smoothCost;
			float lengthCost;
		};


		typedef NonManifoldMesh<nodeAttrib, edgeAttrib, bool> GraphType;

		const unsigned int MEDIALNESS_SCORING_FUNCTION_BINARY = 0;
		const unsigned int MEDIALNESS_SCORING_FUNCTION_GLOBAL_RANK = 1;
		const unsigned int MEDIALNESS_SCORING_FUNCTION_LOCAL_RANK = 2;
		const float MAX_ALLOWED_NODE_COST = 0.9f;

		class InteractiveSkeletonizer : public VolumeSkeletonizer {
		public:
			InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo, unsigned int medialnessScoringFunction);
			~InteractiveSkeletonizer();
			GraphType * GetGraph();
			Octree<octreeTagType> * GetOctree();
			vector<OctreeNode< octreeTagType > *> GetPath(Vector3DInt endPoint);			
			vector<OctreeNode< octreeTagType > *> GetPath(vector<Vector3DInt> endPoints);
			Vector3DInt FindClosestSkeletalPoint(Vector3DInt point);
			void CalculateMinimalSpanningTree(Vector3DInt seedPoint, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate);
			void CalculateMinimalSpanningTree(vector<Vector3DInt> seedPoints, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate);
			void IsolateStartSeed(Vector3DInt startPos, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate);
									
		protected:
			double GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type);			
			GraphType * graph;
			Octree<octreeTagType> * octree;
			Vector3DInt seedPoint;
			EigenResults3D * volumeEigens;
			unsigned int medialnessScoringFunction;
		private:
			//Volume * GetAutomaticSkeleton(Volume * sourceVol, float minGray, float maxGray, float stepSize, int minCurveSize);
		};


		//Volume * InteractiveSkeletonizer::GetAutomaticSkeleton(Volume * sourceVol, float minGray, float maxGray, float stepSize, int minCurveSize) {

		//}


		InteractiveSkeletonizer::InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo, unsigned int medialnessScoringFunction) : VolumeSkeletonizer(0, curveRadius, 0,0) {			
			appTimeManager.PushCurrentTime();
			this->medialnessScoringFunction = medialnessScoringFunction;
			graph = new GraphType();		
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
					
			// Generating the Skeleton
			Volume * nullVol = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
			appTimeManager.PushCurrentTime();
			Volume * skeleton = GetImmersionThinning(sourceVol, nullVol, minGray, maxGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			appTimeManager.PopAndDisplayTime("	Thinning completed: %f seconds\n");
			delete nullVol;
			PruneCurves(skeleton, minCurveSize);
			#ifdef SAVE_INTERMEDIATE_RESULTS
				skeleton->toMRCFile("CurveSkeleton.mrc");				
			#endif
		
			// Creating the octree
			octree = new Octree<octreeTagType>(skeleton->getSizeX() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeY() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeZ()- 2*MAX_GAUSSIAN_FILTER_RADIUS);
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

			// Creating graph vertices and also a mask for finding eigen values
			Volume * maskVol = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ());
			vector<OctreeNode<octreeTagType> *> cells = octree->GetCells();
			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					cells[i]->tag.tag1 = graph->AddVertex(Vector3DFloat(cells[i]->pos[0], cells[i]->pos[1], cells[i]->pos[2]), nodeAttrib());
					graph->vertices[cells[i]->tag.tag1].tag.octreeNode = cells[i];
					maskVol->setDataAt(cells[i]->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, cells[i]->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, cells[i]->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, 1.0);
				}
			}

			// Creating graph edges
			vector<OctreeNode<octreeTagType> *> neighbors;
			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					neighbors = octree->GetNeighbors(cells[i]);
					for(unsigned int j = 0; j < neighbors.size(); j++) {
						graph->AddEdge(cells[i]->tag.tag1, neighbors[j]->tag.tag1, edgeAttrib());
					}				
				}
			}


			// Annnotating the medial cost, and sketch cost in the graph.
			float skeletonValue;
			maxGray = sourceVol->getMax();
			minGray = sourceVol->getMin();
			float lMaxGray, lMinGray;
			OctreeNode<octreeTagType> * n1, * n2;

			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				n1 = graph->vertices[i].tag.octreeNode;
				skeletonValue = skeleton->getDataAt(n1->pos[0] + MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1] + MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2] + MAX_GAUSSIAN_FILTER_RADIUS);
				if(isZero(skeletonValue)) {
					graph->vertices[i].tag.medialnessCost = 1.0;
				} else {
					switch(medialnessScoringFunction) {
						case MEDIALNESS_SCORING_FUNCTION_BINARY :
							graph->vertices[i].tag.medialnessCost = 0.0;
							break;
						case MEDIALNESS_SCORING_FUNCTION_GLOBAL_RANK :
							graph->vertices[i].tag.medialnessCost = 1.0 - (skeletonValue - minGray)/(maxGray-minGray);
							break;
						case MEDIALNESS_SCORING_FUNCTION_LOCAL_RANK:
							lMaxGray = sourceVol->getLocalMax(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, curveRadius);
							lMinGray = sourceVol->getLocalMin(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, curveRadius);
							graph->vertices[i].tag.medialnessCost = 1.0 - (skeletonValue - lMinGray)/(lMaxGray-lMinGray);
							break;
					}
				}
				graph->vertices[i].tag.sketchCost = 0.0;
			}

			// Annotating the smoothness cost in the graph.
			Vector3DFloat * volumeGradient = GetVolumeGradient2(sourceVol);					
			delete maskVol;
			float maxProjectedScore = MIN_FLOAT;
			float minProjectedScore = MAX_FLOAT;
			
			float s1, s2;

			EigenResults3D eig1;
			EigenResults3D eig2;

			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				n1 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].tag.octreeNode;
				n2 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].tag.octreeNode;
				graph->edges[i].tag.lengthCost = (Vector3DInt(n1->pos[0], n1->pos[1], n1->pos[2]) - Vector3DInt(n2->pos[0], n2->pos[1], n2->pos[2])).Length();


				GetEigenResult2(eig1, volumeGradient, gaussianFilterCurveRadius, n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), curveRadius, false);
				GetEigenResult2(eig2, volumeGradient, gaussianFilterCurveRadius, n2->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), curveRadius, false);


				s1 = GetStructureTensorProjectedScore(
						eig1,
						graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position - graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position,
						2, PRUNING_CLASS_PRUNE_CURVES);
				s2 = GetStructureTensorProjectedScore(
						eig2,
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
			//if(!storeEigenInfo) {
			//	delete [] volumeEigens; 
				volumeEigens = NULL;
			//}
			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			delete skeleton;

			appTimeManager.PopAndDisplayTime("Creating graphs: %f seconds!\n"); 
			printf("Graph size: %d nodes, %d edges\n", graph->vertices.size(), graph->edges.size());
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
			//return GetVoxelCost(imageEigen, skeletonDirection, PRUNING_CLASS_PRUNE_CURVES);
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
		Octree<octreeTagType> * InteractiveSkeletonizer::GetOctree() {
			return octree;
		}

		vector<OctreeNode< octreeTagType > *> InteractiveSkeletonizer::GetPath(Vector3DInt endPoint) {
			vector<OctreeNode< octreeTagType > *> outPath;
			
			OctreeNode<octreeTagType> * node = octree->GetLeaf(endPoint.X(), endPoint.Y(), endPoint.Z());
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			currentNode = &graph->vertices[node->tag.tag1];
			outPath.push_back(currentNode->tag.octreeNode);

			while(currentNode->tag.returnNode != NULL) {
				currentNode = currentNode->tag.returnNode;
				outPath.push_back(currentNode->tag.octreeNode);
			}
			return outPath; 
		}

		vector<OctreeNode< octreeTagType > *> InteractiveSkeletonizer::GetPath(vector<Vector3DInt> endPoints) {
			
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			float minCost = MAX_FLOAT;
			unsigned int minCostIndex = 0;
			OctreeNode<octreeTagType> * node; 

			for(unsigned int i = 0; i < endPoints.size(); i++) {
				node = octree->GetLeaf(endPoints[i].X(), endPoints[i].Y(), endPoints[i].Z());
				currentNode = &graph->vertices[node->tag.tag1];
				if(currentNode->tag.mstCost <= minCost) {
					minCost = currentNode->tag.mstCost;
					minCostIndex = i;
				}
			}	
			return GetPath(endPoints[minCostIndex]); 
		}


		Vector3DInt InteractiveSkeletonizer::FindClosestSkeletalPoint(Vector3DInt point) {
			appTimeManager.PushCurrentTime();
			OctreeNode<octreeTagType> * node = octree->GetLeaf(point.X(), point.Y(), point.Z());
			appTimeManager.PopAndDisplayTime("Found Closest skeletal point: %f seconds!\n");
			return Vector3DInt(node->pos[0], node->pos[1], node->pos[2]);							
		}


		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(Vector3DInt seedPoint, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate) {
			appTimeManager.PushCurrentTime();
			vector<Vector3DInt> seedPoints;
			seedPoints.push_back(seedPoint);
			CalculateMinimalSpanningTree(seedPoints, medialnessRatio, smoothnessRatio, sketchRatio, lengthRatio, terminate);
			appTimeManager.PopAndDisplayTime("Initializing seed point: %f seconds!\n");
		}

		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(vector<Vector3DInt> seedPoints, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate) {
			appTimeManager.PushCurrentTime();
			
			OctreeNode<octreeTagType> * node;
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			queue<NonManifoldMeshVertex<nodeAttrib> *> pointList;

			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				graph->vertices[i].tag.mstCost = -1;
			}

			for(unsigned int i = 0; i < seedPoints.size(); i++) {
				node = octree->GetLeaf(seedPoints[i].X(), seedPoints[i].Y(), seedPoints[i].Z());
		
				currentNode = &graph->vertices[node->tag.tag1];
				currentNode->tag.mstCost = medialnessRatio * currentNode->tag.medialnessCost + sketchRatio * currentNode->tag.sketchCost;
				currentNode->tag.returnNode = NULL;

				pointList.push(currentNode);
			}


			float cost, vertexCost, newCost;
			unsigned int edgeIx;
			unsigned int vertexIx;


			while(!pointList.empty()) {	
				currentNode = pointList.front();
				pointList.pop();
				for(unsigned int i = 0; i < currentNode->edgeIds.size(); i++) {
					edgeIx = graph->GetEdgeIndex(currentNode->edgeIds[i]);
					cost = currentNode->tag.mstCost 
							+ smoothnessRatio * graph->edges[edgeIx].tag.smoothCost * graph->edges[edgeIx].tag.lengthCost // Scale the edge weight by the length
							+ medialnessRatio * max(graph->edges[edgeIx].tag.lengthCost - sqrt(2.0), 0.0)				  // Add cost for all the vertices we skipped because of long edge
							+ lengthRatio * pow(max(graph->edges[edgeIx].tag.lengthCost - sqrt(2.0), 0.0), 2.0);		  // Add edge length so we minimize edge length

					for(unsigned int j = 0; j < 2; j++) {
						vertexIx = graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[j]);
						vertexCost = graph->vertices[vertexIx].tag.mstCost;
						if(!terminate || graph->vertices[vertexIx].tag.medialnessCost <= MAX_ALLOWED_NODE_COST) {
							newCost = cost + medialnessRatio * graph->vertices[vertexIx].tag.medialnessCost + sketchRatio * graph->vertices[vertexIx].tag.sketchCost;
							if((vertexCost < 0) || (vertexCost > newCost)) {
								graph->vertices[vertexIx].tag.mstCost = newCost;
								graph->vertices[vertexIx].tag.returnNode = currentNode;
								pointList.push(&graph->vertices[vertexIx]);
							}
						}
					}
				}				
			}
			appTimeManager.PopAndDisplayTime("Initializing seed points: %f seconds!\n");

		}

		void InteractiveSkeletonizer::IsolateStartSeed(Vector3DInt startPos, float medialnessRatio, float smoothnessRatio, float sketchRatio, float lengthRatio, bool terminate) {
			CalculateMinimalSpanningTree(startPos, medialnessRatio, smoothnessRatio, sketchRatio, lengthRatio, terminate);
		}
	}
}


#endif
