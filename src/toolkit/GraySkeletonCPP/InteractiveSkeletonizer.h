// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs interactive skeletonization of a volume


#ifndef GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H
#define GRAYSKELETONCPP_INTERACTIVE_SKELETONIZER_H

#include <queue>
#include "GlobalDefinitions.h"
#include "VolumeSkeletonizer.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <Foundation/Octree.h>
#include <Foundation/Rasterizer.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		struct octreeTagType {
			unsigned int tag1;
			unsigned int tag2;
		};

		struct nodeAttrib{
			float sketchRatio;
			float mstCost;
			NonManifoldMeshVertex<nodeAttrib> * returnNode;
			OctreeNode<octreeTagType> * octreeNode;
		};

		struct edgeAttrib {
			float medialnessCost;
			float smoothCost;
		};


		typedef NonManifoldMesh<nodeAttrib, edgeAttrib, bool> GraphType;

		const unsigned int MEDIALNESS_SCORING_FUNCTION_BINARY = 0;
		const unsigned int MEDIALNESS_SCORING_FUNCTION_GLOBAL_RANK = 1;
		const unsigned int MEDIALNESS_SCORING_FUNCTION_LOCAL_RANK = 2;
		const float MAX_ALLOWED_NODE_COST = 0.9f;
		const float SKETCH_RADIUS = 5.0f;

		class InteractiveSkeletonizer : public VolumeSkeletonizer {
		public:
			InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo, unsigned int medialnessScoringFunction);
			~InteractiveSkeletonizer();
			GraphType * GetGraph();
			Octree<octreeTagType> * GetOctree();
			vector<OctreeNode< octreeTagType > *> GetPath(Vector3DInt endPoint);			
			vector<OctreeNode< octreeTagType > *> GetPath(vector<Vector3DInt> endPoints);
			Vector3DInt FindClosestSkeletalPoint(Vector3DInt point);
			void CalculateMinimalSpanningTree(Vector3DInt seedPoint, float medialnessRatio, float smoothnessRatio, bool terminate);
			void CalculateMinimalSpanningTree(vector<Vector3DInt> seedPoints, float medialnessRatio, float smoothnessRatio, bool terminate);
			void IsolateStartSeed(Vector3DInt startPos, float medialnessRatio, float smoothnessRatio, bool terminate);
			void SetSketchPoints(vector<Vector3DInt> sketchPoints, float minSketchRatio, float maxSketchRatio);

		protected:
			double GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type);			
			GraphType * graph;
			Octree<octreeTagType> * octree;
			Vector3DInt seedPoint;
			EigenResults3D * volumeEigens;
			unsigned int medialnessScoringFunction;
			
			float maxEdgeSegmentSmoothness;
			float minEdgeSegmentSmoothness;

		private:
			float GetSketchRatio(float oldY, float xValue, float minSketchRatio, float maxSketchRatio);
			Volume * GetAutomaticSkeleton(Volume * sourceVol, float minGray, float maxGray, float stepSize, int minCurveSize);
			Octree<octreeTagType> * GetOctreeFromSkeleton(Volume * skeleton);
			void CreateAndAnnotateVertices(GraphType * graph, Octree<octreeTagType> * octree, Volume * sourceVol, Volume * skeleton, unsigned int medialnessScoringFunction, int curveRadius);
			void CreateAndAnnotateEdges(GraphType * graph, Octree<octreeTagType> * octree, Volume * sourceVol, Volume * skeleton, unsigned int medialnessScoringFunction, int curveRadius);
			float GetVertexMedialnessCost(unsigned int medialnessScoringFunction, Volume * sourceVol, Volume * skeleton, int x, int y, int z, unsigned int curveRadius, float minGray, float maxGray);
			float GetVertexMedialnessCost(vector<Vector3DInt> vertices, unsigned int medialnessScoringFunction, Volume * sourceVol, Volume * skeleton, unsigned int curveRadius, float minGray, float maxGray);
			float GetEdgeSmoothnessCost(GraphType * graph, Vector3DFloat * volumeGradient, Volume * sourceVol, Vector3DInt p1, Vector3DInt p2, Vector3DFloat direction, int curveRadius);
			float GetEdgeSmoothnessCost(vector<Vector3DInt> vertices, GraphType * graph, Vector3DFloat * volumeGradient, Volume * sourceVol, Vector3DFloat direction, int curveRadius);
		};



		InteractiveSkeletonizer::InteractiveSkeletonizer(Volume * sourceVol, float minGray, float maxGray, float stepSize, int curveRadius, int minCurveSize, bool storeEigenInfo, unsigned int medialnessScoringFunction) : VolumeSkeletonizer(0, curveRadius, 0,0) {			
			appTimeManager.PushCurrentTime();
			this->medialnessScoringFunction = medialnessScoringFunction;
			graph = new GraphType();		
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			Volume * skeleton = GetAutomaticSkeleton(sourceVol, minGray, maxGray, stepSize, minCurveSize);					
			octree = GetOctreeFromSkeleton(skeleton);
			CreateAndAnnotateVertices(graph, octree, sourceVol, skeleton, medialnessScoringFunction, curveRadius);
			CreateAndAnnotateEdges(graph, octree, sourceVol, skeleton, medialnessScoringFunction, curveRadius);

			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			delete skeleton;

			appTimeManager.PopAndDisplayTime("Creating graphs: %f seconds!\n"); 
			printf("Graph size: %d nodes, %d edges\n", (int)graph->vertices.size(), (int)graph->edges.size());
		}


		InteractiveSkeletonizer::~InteractiveSkeletonizer() {
			delete graph;
			delete octree;
			if(volumeEigens != NULL) {
				delete [] volumeEigens;
			}
		}


		Volume * InteractiveSkeletonizer::GetAutomaticSkeleton(Volume * sourceVol, float minGray, float maxGray, float stepSize, int minCurveSize) {
			Volume * nullVol = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
			appTimeManager.PushCurrentTime();
			Volume * skeleton = GetImmersionThinning(sourceVol, nullVol, minGray, maxGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			delete nullVol;
			PruneCurves(skeleton, minCurveSize);
			#ifdef SAVE_INTERMEDIATE_RESULTS
				skeleton->toMRCFile("CurveSkeleton.mrc");				
			#endif			
			appTimeManager.PopAndDisplayTime("	Thinning and pruning completed: %f seconds\n");
			return skeleton;
		}

		Octree<octreeTagType> * InteractiveSkeletonizer::GetOctreeFromSkeleton(Volume * skeleton) {
			Octree<octreeTagType> * octree = new Octree<octreeTagType>(skeleton->getSizeX() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeY() - 2*MAX_GAUSSIAN_FILTER_RADIUS, skeleton->getSizeZ()- 2*MAX_GAUSSIAN_FILTER_RADIUS);
			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < skeleton->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < skeleton->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < skeleton->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							octree->AddNewLeaf(x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS, 1);
						}
					}
				}
			}
			return octree;
		}			
		float InteractiveSkeletonizer::GetVertexMedialnessCost(unsigned int medialnessScoringFunction, Volume * sourceVol, Volume * skeleton, int x, int y, int z, unsigned int curveRadius, float minGray, float maxGray) {
			float lMaxGray, lMinGray;
			float cost = 1.0f;
			if(isZero(skeleton->getDataAt(x, y, z))) {
				cost = 1.0f;
			} else {
				switch(medialnessScoringFunction) {
					case MEDIALNESS_SCORING_FUNCTION_BINARY :
						cost = 0.0f;
						break;
					case MEDIALNESS_SCORING_FUNCTION_GLOBAL_RANK :
						cost = 1.0f - (sourceVol->getDataAt(x, y, z) - minGray)/(maxGray-minGray);
						break;
					case MEDIALNESS_SCORING_FUNCTION_LOCAL_RANK:
						lMaxGray = sourceVol->getLocalMax(x, y, z, curveRadius);
						lMinGray = sourceVol->getLocalMin(x, y, z, curveRadius);
						cost = 1.0f - (sourceVol->getDataAt(x, y, z) - lMinGray)/(lMaxGray-lMinGray);
						break;
				}
			}
			return cost;
		}


		float InteractiveSkeletonizer::GetVertexMedialnessCost(vector<Vector3DInt> vertices, unsigned int medialnessScoringFunction, Volume * sourceVol, Volume * skeleton, unsigned int curveRadius, float minGray, float maxGray) {
			float cost = 0.0f;
			if(vertices.size() > 1) {
				// dividing the first and last vertices by 2 since they get added in twice when traversing the graph.
				cost += GetVertexMedialnessCost(medialnessScoringFunction, sourceVol, skeleton, vertices[0].X(), vertices[0].Y(), vertices[0].Z(), curveRadius, minGray, maxGray);
				cost += GetVertexMedialnessCost(medialnessScoringFunction, sourceVol, skeleton, vertices[vertices.size()-1].X(), vertices[vertices.size()-1].Y(), vertices[vertices.size()-1].Z(), curveRadius, minGray, maxGray);
				cost = cost / 2.0f;	
				for(unsigned int i = 1; i < vertices.size()-1; i++) {
					cost += GetVertexMedialnessCost(medialnessScoringFunction, sourceVol, skeleton, vertices[i].X(), vertices[i].Y(), vertices[i].Z(), curveRadius, minGray, maxGray);
				}
			} else if (vertices.size() == 1) {
				cost = GetVertexMedialnessCost(medialnessScoringFunction, sourceVol, skeleton, vertices[0].X(), vertices[0].Y(), vertices[0].Z(), curveRadius, minGray, maxGray);
			} else {
				cost = 1.0f;
			}

			return cost;
		}

		float InteractiveSkeletonizer::GetEdgeSmoothnessCost(GraphType * graph, Vector3DFloat * volumeGradient, Volume * sourceVol, Vector3DInt p1, Vector3DInt p2, Vector3DFloat direction, int curveRadius) {
			EigenResults3D eig1, eig2;
			GetEigenResult(eig1, volumeGradient, gaussianFilterCurveRadius, p1.X(), p1.Y(), p1.Z(), sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ(), curveRadius, false);
			GetEigenResult(eig2, volumeGradient, gaussianFilterCurveRadius, p2.X(), p2.Y(), p2.Z(), sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ(), curveRadius, false);

			float segmentCost = (GetStructureTensorProjectedScore(eig1, direction, 2, PRUNING_CLASS_PRUNE_CURVES) + 
								 GetStructureTensorProjectedScore(eig2, direction, 2, PRUNING_CLASS_PRUNE_CURVES)) * (p1 - p2).Length() / 2.0f;

			maxEdgeSegmentSmoothness = max(segmentCost, maxEdgeSegmentSmoothness);
			minEdgeSegmentSmoothness = min(segmentCost, minEdgeSegmentSmoothness);

			return  segmentCost;
		}

		float InteractiveSkeletonizer::GetEdgeSmoothnessCost(vector<Vector3DInt> vertices, GraphType * graph, Vector3DFloat * volumeGradient, Volume * sourceVol, Vector3DFloat direction, int curveRadius) {
			float cost = 0.0f;

			for(unsigned int i = 0; i < vertices.size() - 1; i++) {
				cost += GetEdgeSmoothnessCost(graph, volumeGradient, sourceVol, vertices[i], vertices[i+1], direction, curveRadius);
			}
			return cost;			
		}

		void InteractiveSkeletonizer::CreateAndAnnotateVertices(GraphType * graph, Octree<octreeTagType> * octree, Volume * sourceVol, Volume * skeleton, unsigned int medialnessScoringFunction, int curveRadius) {
			//Creating
			vector<OctreeNode<octreeTagType> *> cells = octree->GetCells();
			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					cells[i]->tag.tag1 = graph->AddVertex(Vector3DFloat(cells[i]->pos[0], cells[i]->pos[1], cells[i]->pos[2]), nodeAttrib());
					graph->vertices[cells[i]->tag.tag1].tag.octreeNode = cells[i];
				}
			}

			
			//Annotating			

			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				graph->vertices[i].tag.sketchRatio = 1.0f;
			}
		}


		void InteractiveSkeletonizer::CreateAndAnnotateEdges(GraphType * graph, Octree<octreeTagType> * octree, Volume * sourceVol, Volume * skeleton, unsigned int medialnessScoringFunction, int curveRadius) {
			// Creating
			vector<OctreeNode<octreeTagType> *> cells = octree->GetCells();
			vector<OctreeNode<octreeTagType> *> neighbors;
			for(unsigned int i = 0; i < cells.size(); i++) {
				if(cells[i]->isLeaf) {
					neighbors = octree->GetNeighbors(cells[i]);
					for(unsigned int j = 0; j < neighbors.size(); j++) {
						if(!graph->IsEdgePresent(cells[i]->tag.tag1, neighbors[j]->tag.tag1)) {
							graph->AddEdge(cells[i]->tag.tag1, neighbors[j]->tag.tag1, edgeAttrib());
						}
					}				
				}
			}


			// Annotating
			Vector3DFloat * volumeGradient = GetVolumeGradient(sourceVol);					

			float maxGray = sourceVol->getMax();
			float minGray = sourceVol->getMin();

			OctreeNode<octreeTagType> * n1, * n2;
			vector<Vector3DInt> pseudoVertices;
			
			minEdgeSegmentSmoothness = MAX_FLOAT;		// These two values will be set in the GetEdgeSmoothnessCost method.
			maxEdgeSegmentSmoothness = MIN_FLOAT;

			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				n1 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].tag.octreeNode;
				n2 = graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].tag.octreeNode;
				Vector3DInt p1 = Vector3DInt(n1->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n1->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS);
				Vector3DInt p2 = Vector3DInt(n2->pos[0]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[1]+MAX_GAUSSIAN_FILTER_RADIUS, n2->pos[2]+MAX_GAUSSIAN_FILTER_RADIUS);

				pseudoVertices = Rasterizer::ScanConvertLine(p1, p2);
				graph->edges[i].tag.medialnessCost = GetVertexMedialnessCost(pseudoVertices, medialnessScoringFunction, sourceVol, skeleton, curveRadius, minGray, maxGray);
				graph->edges[i].tag.smoothCost = GetEdgeSmoothnessCost(pseudoVertices, graph, volumeGradient, sourceVol, 
					graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[0])].position - graph->vertices[graph->GetVertexIndex(graph->edges[i].vertexIds[1])].position,
					curveRadius); 
			}


			// Re-normalizing the smoothness cost so each edge segment cost is between 0 and 1.
			for(unsigned int i = 0; i < graph->edges.size(); i++) {
				graph->edges[i].tag.smoothCost = (graph->edges[i].tag.smoothCost - minEdgeSegmentSmoothness) / (maxEdgeSegmentSmoothness - minEdgeSegmentSmoothness);
			}

			delete [] volumeGradient;

		}

		double InteractiveSkeletonizer::GetStructureTensorProjectedScore(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, float power, int type) {						
			skeletonDirection.Normalize();

			double score = 0.0;
			switch(type) {
				case PRUNING_CLASS_PRUNE_CURVES:
					if(isZero(imageEigen.values[0])) {
						score = 1.0;
					} else {
						for(int i = 0 ; i < 3; i++) {
							score += (imageEigen.values[i] / imageEigen.values[0])  * pow(skeletonDirection * imageEigen.vectors[i], power);
						}				
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
			//appTimeManager.PushCurrentTime();
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
			//appTimeManager.PopAndDisplayTime("Analyzed path %f seconds!\n");
			return GetPath(endPoints[minCostIndex]); 
		}


		Vector3DInt InteractiveSkeletonizer::FindClosestSkeletalPoint(Vector3DInt point) {
			appTimeManager.PushCurrentTime();
			OctreeNode<octreeTagType> * node = octree->GetLeaf(point.X(), point.Y(), point.Z());
			appTimeManager.PopAndDisplayTime("Found Closest skeletal point: %f seconds!\n");
			return Vector3DInt(node->pos[0], node->pos[1], node->pos[2]);							
		}


		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(Vector3DInt seedPoint, float medialnessRatio, float smoothnessRatio, bool terminate) {
			appTimeManager.PushCurrentTime();
			vector<Vector3DInt> seedPoints;
			seedPoints.push_back(seedPoint);
			CalculateMinimalSpanningTree(seedPoints, medialnessRatio, smoothnessRatio, terminate);
			appTimeManager.PopAndDisplayTime("Initializing seed point: %f seconds!\n");
		}

		void InteractiveSkeletonizer::CalculateMinimalSpanningTree(vector<Vector3DInt> seedPoints, float medialnessRatio, float smoothnessRatio, bool terminate) {
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
				currentNode->tag.mstCost = 0;
				currentNode->tag.returnNode = NULL;

				pointList.push(currentNode);
			}


			float cost, vertexCost, avgSketchRatio;
			unsigned int edgeIx;
			unsigned int vertexIx;


			while(!pointList.empty()) {	
				currentNode = pointList.front();
				pointList.pop();
				for(unsigned int i = 0; i < currentNode->edgeIds.size(); i++) {
					edgeIx = graph->GetEdgeIndex(currentNode->edgeIds[i]);

					avgSketchRatio = (graph->vertices[graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[0])].tag.sketchRatio + 
								      graph->vertices[graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[1])].tag.sketchRatio) / 2.0f;


					cost = currentNode->tag.mstCost 
							+ (smoothnessRatio * graph->edges[edgeIx].tag.smoothCost +
							   medialnessRatio * graph->edges[edgeIx].tag.medialnessCost) * avgSketchRatio;

					for(unsigned int j = 0; j < 2; j++) {
						vertexIx = graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[j]);
						vertexCost = graph->vertices[vertexIx].tag.mstCost;
						if((vertexCost < 0) || (vertexCost > cost)) {
							graph->vertices[vertexIx].tag.mstCost = cost;
							graph->vertices[vertexIx].tag.returnNode = currentNode;
							pointList.push(&graph->vertices[vertexIx]);
						}
					}
				}				
			}
			appTimeManager.PopAndDisplayTime("Initializing seed points: %f seconds!\n");

		}

		void InteractiveSkeletonizer::IsolateStartSeed(Vector3DInt startPos, float medialnessRatio, float smoothnessRatio, bool terminate) {
			CalculateMinimalSpanningTree(startPos, medialnessRatio, smoothnessRatio, terminate);
		}


		float InteractiveSkeletonizer::GetSketchRatio(float oldY, float xValue, float minSketchRatio, float maxSketchRatio) {
			float p = 4.0f;
			float pp = pow(p, p);
			float a = maxSketchRatio;
			float b = minSketchRatio;
			float oldX = pow((pp * (a-b) / (a - oldY) - pp), 1.0f/p);
			float newX = oldX + xValue;

			return (1.0f - 1.0f / (pow(newX, p) / pp + 1.0f)) * (a-b)+ b;
		}

		void InteractiveSkeletonizer::SetSketchPoints(vector<Vector3DInt> sketchPoints, float minSketchRatio, float maxSketchRatio) {
			appTimeManager.PushCurrentTime();
			
			OctreeNode<octreeTagType> * node;
			NonManifoldMeshVertex<nodeAttrib> * currentNode;
			queue<NonManifoldMeshVertex<nodeAttrib> *> pointList;

			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				graph->vertices[i].tag.sketchRatio = maxSketchRatio;
			}

			for(unsigned int i = 0; i < sketchPoints.size(); i++) {
				node = octree->GetLeaf(sketchPoints[i].X(), sketchPoints[i].Y(), sketchPoints[i].Z());
		
				currentNode = &graph->vertices[node->tag.tag1];
				currentNode->tag.sketchRatio = minSketchRatio;
				pointList.push(currentNode);
			}


			float newRatio, edgeLength;
			unsigned int edgeIx;
			unsigned int vertexIx;


			while(!pointList.empty()) {	
				currentNode = pointList.front();
				pointList.pop();
				for(unsigned int i = 0; i < currentNode->edgeIds.size(); i++) {
					edgeIx = graph->GetEdgeIndex(currentNode->edgeIds[i]);
					edgeLength = (graph->vertices[graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[0])].position - 
								  graph->vertices[graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[1])].position).Length();

					newRatio = GetSketchRatio(currentNode->tag.sketchRatio, edgeLength, minSketchRatio, maxSketchRatio);
							

					for(unsigned int j = 0; j < 2; j++) {
						vertexIx = graph->GetVertexIndex(graph->edges[edgeIx].vertexIds[j]);
						if((graph->vertices[vertexIx].tag.sketchRatio > newRatio)) {
							graph->vertices[vertexIx].tag.sketchRatio = newRatio;
							pointList.push(&graph->vertices[vertexIx]);
						}
					}
				}				
			}
			appTimeManager.PopAndDisplayTime("Resetting sketch ratios: %f seconds!\n");


		}
	}
}


#endif
