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
			void StartSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			bool SetSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void EndSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio, float minSketchRatio, float maxSketchRatio);

			void Draw(int subscene);
			void FinalizeSkeleton();
			void ClearSkeleton();
			void ClearCurrentPath();
			void SelectEndSeed(float medialnessRatio, float smoothnessRatio);
			void BrowseStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, bool clearBrowsePoint);
			void SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio);
			void SelectRootRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio);
			void SetIsoValue(float isoValue);
		private:
			Volume * volume;
			NonManifoldMesh_Annotated * skeleton;
			InteractiveSkeletonizer * skeletonizer;
			bool started;
			bool analyzed;
			bool browseStarted;
			Vector3DInt browseStartPos;
			Vector3DInt startPos;
			vector<Vector3DInt> startPositions;
			Vector3DInt currentPos;
			vector<Vector3DInt> currentPositions;
			GLUquadric * quadricSphere;			
			float isoValue;
			bool startSeedIsolated;


			SketchMapType sketchPositions;
			bool sketchStarted;
			float rayStart[3], eyeStart[3], rayWidthStart;
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
			browseStarted = false;
			analyzed = false;
			startSeedIsolated = false;
			quadricSphere = gluNewQuadric();	
			sketchStarted = false;
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
		}

		void InteractiveSkeletonEngine::BrowseStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, bool clearBrowsePoint) {
			if(clearBrowsePoint) {
				browseStarted = false;
			} else {
				browseStarted = true;
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
		}


		void InteractiveSkeletonEngine::SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio) {
			analyzed = false;
			started = false;			
			browseStarted = false;
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

			if(startPositions.size() > 0) {
				skeletonizer->CalculateMinimalSpanningTree(startPositions, medialnessRatio, smoothnessRatio, false);
				startPos = startPositions[0];
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
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();

				vector<OctreeNode<octreeTagType> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);

				for(unsigned int i = 0; i < intersectingCells.size(); i++) {
					if(intersectingCells[i]->cellSize == 1) {
						currentPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					}
				}

				if(currentPositions.size() > 0) {

					int v1, v2;
					analyzed = true;
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

		void InteractiveSkeletonEngine::ClearSketchRay() {
			sketchPositions.clear();
			vector<Vector3DInt> sketchPts;
			skeletonizer->SetSketchPoints(sketchPts, 1.0f, 1.0f);
		}

		void InteractiveSkeletonEngine::StartSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			ClearSketchRay();
			rayStart[0] = rayX;
			rayStart[1] = rayY;
			rayStart[2] = rayZ;
			eyeStart[0] = eyeX;
			eyeStart[1] = eyeY;
			eyeStart[2] = eyeZ;
			rayWidthStart = rayWidth;
			sketchStarted = true;			
			SetSketchRay(rayX, rayY, rayZ, eyeX, eyeY, eyeZ, rayWidth);
		}

		bool InteractiveSkeletonEngine::SetSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			bool added = false;
			if(sketchStarted) {
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

		
		void InteractiveSkeletonEngine::EndSketchRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth, float medialnessRatio, float smoothnessRatio, float minSketchRatio, float maxSketchRatio) {
			if(sketchStarted) {
				vector<Vector3DInt> sketchPts;
				for(SketchMapType::iterator i = sketchPositions.begin(); i!= sketchPositions.end(); i++) {
					sketchPts.push_back(GetVector3DIntFromHash(i->first));
				}

				SetSketchRay(rayX, rayY, rayZ, eyeX, eyeY, eyeZ, rayWidth);
				skeletonizer->SetSketchPoints(sketchPts, minSketchRatio, maxSketchRatio);
				SelectStartSeedRay(rayStart[0], rayStart[1], rayStart[2], eyeStart[0], eyeStart[1], eyeStart[2], rayWidthStart, medialnessRatio, smoothnessRatio);
				AnalyzePathRay(rayX, rayY, rayZ, eyeX, eyeY, eyeZ, rayWidth);
				SelectEndSeed(medialnessRatio, smoothnessRatio);
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
		}

		void InteractiveSkeletonEngine::FinalizeSkeleton() {
			analyzed = false;
			started = false;	
			browseStarted = false;
			for(unsigned int i = 0; i < skeleton->vertices.size(); i++) {
				if(skeleton->vertices[i].edgeIds.size() == 0) {
					skeleton->RemoveVertex(i);
				}
			}
			skeleton->RemoveNullEntries();
		}
		void InteractiveSkeletonEngine::Draw(int subscene) {
			switch (subscene) {
				case(0) :
					if(started) {
						glPushMatrix();
						glTranslatef(startPos.X(), startPos.Y(), startPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					if(browseStarted) {
						glPushMatrix();
						glTranslatef(browseStartPos.X(), browseStartPos.Y(), browseStartPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					break;
				case(1) :
					if(analyzed && started) {
						glPushMatrix();
						glTranslatef(currentPos.X(), currentPos.Y(), currentPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
					}
					break;
				case(2):
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
				case(3):
					/*glPushAttrib(GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT);
										
					glPushName(2);
					glPushName(0);

					glPointSize(2);
					glEnable(GL_POINT_SMOOTH);
					glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);								
					for(unsigned int i = 0; i < skeleton->vertices.size(); i++) {										
						if((skeleton->vertices[i].edgeIds.size() == 0) && 
								(volume->getDataAt(skeleton->vertices[i].position.XInt(), skeleton->vertices[i].position.YInt(), skeleton->vertices[i].position.ZInt()) > isoValue)) {
						//if((skeleton->vertices[i].edgeIds.size() == 0)) {
							glLoadName(i);
							glBegin(GL_POINTS);
							glVertex3f(skeleton->vertices[i].position.X(), skeleton->vertices[i].position.Y(), skeleton->vertices[i].position.Z());
							glEnd();
						}
					}		
					glPopName();
					glPopName();
					glPopAttrib();

					glFlush();*/
					break;
			}
		}
	}
}


#endif
