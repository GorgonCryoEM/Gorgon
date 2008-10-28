// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying skeletons

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.23  2008/10/16 19:50:44  ssa1
//   Supporting line deletion
//
//   Revision 1.22  2008/10/16 02:39:57  ssa1
//   Modifying the sketch behavior to supplement line drawing instead of replace it.
//
//   Revision 1.21  2008/10/15 19:41:30  ssa1
//   Esc to cancel path, Clear Button and Tracking of start seed point
//
//   Revision 1.20  2008/10/15 16:34:18  colemanr
//   includes grant's fix for a gcc compile error.
//
//   Revision 1.19  2008/10/15 12:23:50  ssa1
//   Modifying the cost function for sketch interraction, and changing mousebehavior to trigger different interaction modes
//
//   Revision 1.18  2008/10/14 17:52:07  ssa1
//   Fixing SketchRatio error
//
//   Revision 1.17  2008/10/14 14:59:33  ssa1
//   Adding in sketching mode for interactive skeletonization
//
//   Revision 1.16  2008/10/10 14:25:55  ssa1
//   Setting the cost functions to scale with the edge length
//
//   Revision 1.15  2008/10/08 16:43:19  ssa1
//   Interactive skeletonization changes
//
//   Revision 1.14  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//

#ifndef GORGON_INTERACTIVE_SKELETON_ENGINE_H
#define GORGON_INTERACTIVE_SKELETON_ENGINE_H

#include "MeshRenderer.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraySkeletonCPP/InteractiveSkeletonizer.h>
#include <MathTools/Vector3D.h>
#include <Foundation/Octree.h>
#include <vector>
#include <map>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace std;


namespace wustl_mm {
	namespace Visualization {	
		typedef map<unsigned long long, bool> SketchMapType;
		class InteractiveSkeletonEngine {
		public:
			InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction);
			~InteractiveSkeletonEngine();			
			void AnalyzePathRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void ClearSketchRay();
			bool SetSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void EndSketchRay(float medialnessRatio, float smoothnessRatio, float minSketchRatio, float maxSketchRatio);

			void Draw(int subscene);
			void FinalizeSkeleton();
			void ClearSkeleton();
			void ClearCurrentPath();
			void SelectEndSeed(float medialnessRatio, float smoothnessRatio);
			void StartEndPolyLineMode(bool start);
			void StartEndSingleRootMode(bool start);
			void BrowseStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio);
			void SelectRootRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio);
			void SetIsoValue(float isoValue);
			
			void AddSelectionPoint(int subsceneIndex, int ix0);
			void SelectSelection();
			void DeleteSelection();
			void CancelSelection();

		private:
			void SelectStartSeed(vector<Vector3DInt> & startPts, float medialnessRatio, float smoothnessRatio);
			void AnalyzePath(vector<Vector3DInt> & endPts);
			Volume * volume;
			NonManifoldMesh_Annotated * skeleton;
			InteractiveSkeletonizer * skeletonizer;
			bool started;
			bool analyzed;
			bool polyLineMode;
			bool singleRootMode;
			Vector3DInt browseStartPos;
			Vector3DInt startPos;
			vector<Vector3DInt> startPositions;
			Vector3DInt currentPos;
			vector<Vector3DInt> currentPositions;
			GLUquadric * quadricSphere;			
			float isoValue;
			bool startSeedIsolated;			
			SketchMapType sketchPositions;

			vector<unsigned int> selectedEdges;
			vector<unsigned int> removableEdges;
		};

		InteractiveSkeletonEngine::InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction) {
			this->volume = volume;
			this->skeleton = skeleton;
			float minVal = minGray;
			float maxVal = volume->getMax();
			float stepSize = (maxVal - minVal)/(float)stepCount;
			this->isoValue = 0.0f;

			skeletonizer = new InteractiveSkeletonizer(volume, minVal, maxVal, stepSize, curveRadius, minCurveSize, false, medialnessScoringFunction);
			wustl_mm::GraySkeletonCPP::GraphType * graph = skeletonizer->GetGraph();
			OctreeNode<octreeTagType> * node;
			unsigned int ix;
			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				ix = skeleton->AddVertex(graph->vertices[i].position, false);
				node = graph->vertices[i].tag.octreeNode;	
				node->tag.tag2 = ix;
			}
			started = false;
			polyLineMode = true;
			singleRootMode = false;
			analyzed = false;
			startSeedIsolated = false;
			quadricSphere = gluNewQuadric();	
		}

		InteractiveSkeletonEngine::~InteractiveSkeletonEngine() {
			delete skeletonizer;
			gluDeleteQuadric(quadricSphere);			
		}

		void InteractiveSkeletonEngine::SelectEndSeed(float medialnessRatio, float smoothnessRatio) {
			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				skeleton->edges[i].tag = true;
			}
			if(!startSeedIsolated) {
				skeletonizer->IsolateStartSeed(startPos, medialnessRatio, smoothnessRatio, false);
				startSeedIsolated = true;
			}
			ClearSketchRay();
		}

		void InteractiveSkeletonEngine::BrowseStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			vector<Vector3DInt> newStartPositions;

			vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);

			bool snapOn = false;
			for(unsigned int i = 0; !snapOn && (i < intersectingCells.size()); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					snapOn = snapOn || (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0);					
				}
			}			

			for(unsigned int i = 0; i < intersectingCells.size(); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					if(!snapOn || (snapOn && (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0))) {
						newStartPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					}
				}
			}			

			if(newStartPositions.size() > 0) {
				browseStartPos = newStartPositions[0];
			}		
		}


		void InteractiveSkeletonEngine::StartEndPolyLineMode(bool start) {
			polyLineMode = start;
		}
		
		void InteractiveSkeletonEngine::StartEndSingleRootMode(bool start) {
			singleRootMode = start;
		}


		void InteractiveSkeletonEngine::SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio) {
			analyzed = false;
			started = false;			
			startPositions.clear();				

			vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);


			bool snapOn = false;
			for(unsigned int i = 0; !snapOn && (i < intersectingCells.size()); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					snapOn = snapOn || (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0);					
				}
			}			

			for(unsigned int i = 0; i < intersectingCells.size(); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					if(!snapOn || (snapOn && (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0))) {
						startPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					}
				}
			}			
			
			if(startPositions.size() > 0) {
				SelectStartSeed(startPositions, medialnessRatio, smoothnessRatio);				
				startSeedIsolated = false;
				started = true;
			}					
		}

		void InteractiveSkeletonEngine::SelectRootRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio) {
			analyzed = false;
			started = false;			
			startPositions.clear();				
			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				if(!skeleton->edges[i].tag) {
					skeleton->RemoveEdge(i);
				}
			}
			skeleton->RemoveNullEntries();

			vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);


			bool snapOn = false;
			for(unsigned int i = 0; !snapOn && (i < intersectingCells.size()); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					snapOn = snapOn || (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0);					
				}
			}			

			for(unsigned int i = 0; i < intersectingCells.size(); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					if(!snapOn || (snapOn && (skeleton->vertices[intersectingCells[i]->tag.tag2].edgeIds.size() > 0))) {
						startPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					}
				}
			}

			float grayValue, maxGrayValue = MIN_FLOAT;
			int maxIndex = -1;
			for(unsigned int i = 0; i < startPositions.size(); i++) {
				grayValue = volume->getDataAt(startPositions[i].X(), startPositions[i].Y(), startPositions[i].Z());
				if(grayValue > maxGrayValue) {
					grayValue = maxGrayValue;
					maxIndex = i;
				}
			}

			if(maxIndex >= 0) {
				startPos = startPositions[maxIndex];				
				skeletonizer->IsolateStartSeed(startPos, medialnessRatio, smoothnessRatio, true);
				startSeedIsolated = true;
				//skeletonizer->DrawTree(startPos);
			}
		}

		void InteractiveSkeletonEngine::SetIsoValue(float isoValue) {
			this->isoValue = isoValue;
		}

		void InteractiveSkeletonEngine::AnalyzePathRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			if(started) {
				currentPositions.clear();			

				vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);

				for(unsigned int i = 0; i < intersectingCells.size(); i++) {
					if(intersectingCells[i]->cellSize == 1) {
						currentPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					}
				}				

				if(currentPositions.size() > 0) {
					analyzed = true;
					AnalyzePath(currentPositions);
				}
			}
		}



		void InteractiveSkeletonEngine::ClearSketchRay() {
			if(sketchPositions.size() > 0) {
				sketchPositions.clear();
				vector<Vector3DInt> sketchPts;
				skeletonizer->SetSketchPoints(sketchPts, 1.0f, 1.0f);
			}
		}

		bool InteractiveSkeletonEngine::SetSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			bool added = false;
			if(started && (startPositions.size() > 0) && (currentPositions.size() > 0)) {
				vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);
				unsigned long long hash;
				
				for(unsigned int i = 0; i < intersectingCells.size(); i++) {
					if(intersectingCells[i]->cellSize == 1) {
						Vector3DInt tmpv = Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]);
						hash = GetHashFromVector3DInt(tmpv);
						if(sketchPositions.find(hash) == sketchPositions.end()) {
							sketchPositions[hash] = true;
							added = true;
						}
					}
				}
			}

			return added;
		}		
		void InteractiveSkeletonEngine::EndSketchRay(float medialnessRatio, float smoothnessRatio, float minSketchRatio, float maxSketchRatio) {
			if(started && (startPositions.size() > 0) && (currentPositions.size() > 0)) {
				vector<Vector3DInt> sketchPts;
				for(SketchMapType::iterator i = sketchPositions.begin(); i!= sketchPositions.end(); i++) {
					sketchPts.push_back(GetVector3DIntFromHash(i->first));
				}

				skeletonizer->SetSketchPoints(sketchPts, minSketchRatio, maxSketchRatio);
				SelectStartSeed(startPositions, medialnessRatio, smoothnessRatio);
				AnalyzePath(currentPositions);
			}
		}

		void InteractiveSkeletonEngine::ClearCurrentPath() {
			if(started) {
				analyzed = false;
				started = false;			
				startPositions.clear();		
				currentPositions.clear();

				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				ClearSketchRay();
				CancelSelection();
			}
		}
		void InteractiveSkeletonEngine::ClearSkeleton() {
			analyzed = false;
			started = false;			
			startPositions.clear();		
			currentPositions.clear();

			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				skeleton->RemoveEdge(i);
			}
			skeleton->RemoveNullEntries();
			ClearSketchRay();
		}

		void InteractiveSkeletonEngine::FinalizeSkeleton() {
			analyzed = false;
			started = false;	
			polyLineMode = false;
			singleRootMode = false;
			for(unsigned int i = 0; i < skeleton->vertices.size(); i++) {
				if(skeleton->vertices[i].edgeIds.size() == 0) {
					skeleton->RemoveVertex(i);
				}
			}
			skeleton->RemoveNullEntries();
		}
		void InteractiveSkeletonEngine::Draw(int subscene) {
			switch (subscene) {
				case(0) :	// Start Points
					if(started) {
						glPushMatrix();
						glTranslatef(startPos.X(), startPos.Y(), startPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					if(polyLineMode) {
						glPushMatrix();
						glTranslatef(browseStartPos.X(), browseStartPos.Y(), browseStartPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					break;
				case(1) :	// End Points
					if(analyzed && started && (singleRootMode || !polyLineMode)) {
						glPushMatrix();
						glTranslatef(currentPos.X(), currentPos.Y(), currentPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					break;
				case(2):	// Sketch Points
					{
						Vector3DInt v;
						for(SketchMapType::iterator i = sketchPositions.begin(); i != sketchPositions.end(); i++) {
							v = GetVector3DIntFromHash(i->first);
							glPushMatrix();
							glTranslatef(v.X(), v.Y(), v.Z());
							gluSphere(quadricSphere, 1.0, 10, 10);  
							glPopMatrix();						
						}
					}
					break;
				case(3):	// Temporary skeletal paths
					for(unsigned int i = 0; i <  skeleton->edges.size(); i++) {					
						if((skeleton->edges[i].faceIds.size() == 0) && !skeleton->edges[i].tag) {
							NonManifoldMeshVertex<bool> v0 = skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[i].vertexIds[0])];
							NonManifoldMeshVertex<bool> v1 = skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[i].vertexIds[1])];
							Renderer::DrawCylinder(v0.position, v1.position, 0.3);
						}
					}	
					break;
				case(4):	// Removable Edges
					for(unsigned int i = 0; i < removableEdges.size(); i++) {					
						NonManifoldMeshVertex<bool> v0 = skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[removableEdges[i]].vertexIds[0])];
						NonManifoldMeshVertex<bool> v1 = skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[removableEdges[i]].vertexIds[1])];
						Renderer::DrawCylinder(v0.position, v1.position, 0.3);
					}	
					break;
			}
		}

		void InteractiveSkeletonEngine::SelectStartSeed(vector<Vector3DInt> & startPts, float medialnessRatio, float smoothnessRatio) {
			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				if(!skeleton->edges[i].tag) {
					skeleton->RemoveEdge(i);
				}
			}
			skeleton->RemoveNullEntries();

			if(startPts.size() > 0) {
				skeletonizer->CalculateMinimalSpanningTree(startPts, medialnessRatio, smoothnessRatio, false);
				startPos = startPts[0];
			}
		}

		void InteractiveSkeletonEngine::AnalyzePath(vector<Vector3DInt> & endPts) {
			if(started) {

				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();


				if(endPts.size() > 0) {
					int v1, v2;
					vector<OctreeNode< octreeTagType > *> path = skeletonizer->GetPath(currentPositions);
					currentPos = Vector3DInt(path[0]->pos[0], path[0]->pos[1], path[0]->pos[2]);
					startPos = Vector3DInt(path[path.size()-1]->pos[0], path[path.size()-1]->pos[1], path[path.size()-1]->pos[2]); 
					for(unsigned int i = 1; i < path.size(); i++) {
						v1 = path[i-1]->tag.tag2;
						v2 = path[i]->tag.tag2;
						if(!skeleton->IsEdgePresent(v1, v2)) {
							skeleton->AddEdge(v1, v2, false);
						}
					}
				}
			}
		}


		void InteractiveSkeletonEngine::AddSelectionPoint(int subsceneIndex, int ix0) {			
			if((subsceneIndex >= 0) && (ix0 >= 0)) {
				switch(subsceneIndex){
					case 1:						
						selectedEdges.push_back(ix0);
						break;
				}
			}
			if(selectedEdges.size() > 2) {
				selectedEdges.erase(selectedEdges.begin());
			}
			SelectSelection();
		}

		void InteractiveSkeletonEngine::SelectSelection() {
			removableEdges.clear();
			switch(selectedEdges.size()) {
				case(1):
					removableEdges.push_back(selectedEdges[0]);
					break;
				case(2):
					vector<unsigned int> path = skeleton->GetPath(selectedEdges[0], selectedEdges[1]);
					for(unsigned int i = 0; i < path.size(); i++) {
						removableEdges.push_back(path[i]);
					}
					break;
			}					
		}

		void InteractiveSkeletonEngine::CancelSelection() {
			selectedEdges.clear();
			removableEdges.clear();
			SelectSelection();
		}

		void InteractiveSkeletonEngine::DeleteSelection() {
			for(unsigned int i = 0; i < removableEdges.size(); i++) {
				skeleton->RemoveEdge(removableEdges[i]);
			}
			skeleton->RemoveNullEntries();
			removableEdges.clear();
			selectedEdges.clear();

		}

	}
}


#endif
