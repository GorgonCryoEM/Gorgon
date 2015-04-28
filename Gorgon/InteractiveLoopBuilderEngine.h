// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying c-alpha loops



#ifndef GORGON_INTERACTIVE_LOOP_BUILDER_ENGINE_H
#define GORGON_INTERACTIVE_LOOP_BUILDER_ENGINE_H

#include "InteractiveSkeletonEngine.h"
#include "CAlphaRenderer.h"
#include <vector>
#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/PDBBond.h>
#include <MathTools/Vector3D.h>

using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace std;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace Visualization {	
		typedef map<unsigned long long, bool> SketchMapType;
		class InteractiveLoopBuilderEngine : public InteractiveSkeletonEngine {
		public:
			InteractiveLoopBuilderEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, CAlphaRenderer * calphaRenderer, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction);
			void FinishLoopBuilding();
			void SetTranslation(float tx, float ty, float tz);
			void SetScaling(float sx, float sy, float sz);
			void AddAtom(unsigned long long index);
			void CommitAtomList();
			void ClearAtomList();
			void Draw(int subscene);
			void ClearCurrentPath();

		protected:
			void AnalyzePath(vector<Vector3DInt> & endPts);
			void DistributeAtoms(vector<Vector3DFloat> pathPositions);

		private:			
			Vector3DFloat origin;
			Vector3DFloat scale;
			CAlphaRenderer * calphaRenderer;
			vector<PDBAtom *> atoms;
		};

		InteractiveLoopBuilderEngine::InteractiveLoopBuilderEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, CAlphaRenderer * calphaRenderer, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction) : InteractiveSkeletonEngine(volume, skeleton, minGray, stepCount, curveRadius, minCurveSize, medialnessScoringFunction){
			skeleton->drawingDisabled = true;
			this->calphaRenderer = calphaRenderer;
			SetTranslation(0,0,0);
			SetScaling(1,1,1);
		}

		void InteractiveLoopBuilderEngine::FinishLoopBuilding() {
			skeleton->drawingDisabled = false;
		}

		void InteractiveLoopBuilderEngine::SetTranslation(float tx, float ty, float tz) {
			origin = Vector3DFloat(tx, ty, tz);
		}

		void InteractiveLoopBuilderEngine::SetScaling(float sx, float sy, float sz) {
			scale = Vector3DFloat(sx, sy, sz);
		}

		void InteractiveLoopBuilderEngine::AddAtom(unsigned long long index) {
			PDBAtom * atom = calphaRenderer->GetAtom(index); 
			atoms.push_back(atom);
		}

		void InteractiveLoopBuilderEngine::CommitAtomList() {
			atoms.clear();
		}

		void InteractiveLoopBuilderEngine::ClearAtomList() {
			atoms.clear();
		}

		void InteractiveLoopBuilderEngine::AnalyzePath(vector<Vector3DInt> & endPts) {
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
					vector<Vector3DFloat> pathPositions;

					pathPositions.push_back(Vector3DFloat(path[0]->pos[0], path[0]->pos[1], path[0]->pos[2]));

					currentPos = Vector3DInt(path[0]->pos[0], path[0]->pos[1], path[0]->pos[2]);
					startPos = Vector3DInt(path[path.size()-1]->pos[0], path[path.size()-1]->pos[1], path[path.size()-1]->pos[2]); 
					for(unsigned int i = 1; i < path.size(); i++) {
						pathPositions.push_back(Vector3DFloat(path[i]->pos[0], path[i]->pos[1], path[i]->pos[2]));
						v1 = path[i-1]->tag.tag2;
						v2 = path[i]->tag.tag2;
						if(!skeleton->IsEdgePresent(v1, v2)) {
							skeleton->AddEdge(v1, v2, false);
						}
					}					
					DistributeAtoms(pathPositions);
				}
			}
		}

		void InteractiveLoopBuilderEngine::Draw(int subscene) {
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
						if(sketchDrawPositions.size() > 0) {
							glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT );
							glDisable(GL_LIGHTING);
							glLineWidth(lineThickness*3);
							glEnable(GL_LINE_SMOOTH);
							glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	
														
							glMatrixMode(GL_PROJECTION);
							glPushMatrix();
							glLoadIdentity();

							glMatrixMode(GL_MODELVIEW);
							glPushMatrix();
							glLoadIdentity();
							
							float x, y;
							bool sketchStarted = false;
							for(unsigned int i = 0; i < sketchDrawPositions.size(); i++) {
								if(!sketchStarted) {
									glBegin(GL_LINE_STRIP);
									sketchStarted = true;
								}
								if(sketchDrawPositions[i].X() == -1) {
									sketchStarted = false;
									glEnd();
								} 
								else {
									x = 2.0f * (float)sketchDrawPositions[i].X() / (float)screenWidth  - 1.0f;
									y = 2.0f * ((float) screenHeight - (float)sketchDrawPositions[i].Y()) / (float)screenHeight  - 1.0f;
									glVertex2f(x, y);
								}
							}
							if(sketchStarted) {
								glEnd();
							}

							glMatrixMode(GL_PROJECTION);
							glPopMatrix();

							glMatrixMode(GL_MODELVIEW);
							glPopMatrix();

							glPopAttrib();
						}
					}
					break;
			}
		}

		void InteractiveLoopBuilderEngine::DistributeAtoms(vector<Vector3DFloat> pathPositions) {
			for(unsigned int i = 0; i < pathPositions.size(); i++) {
				pathPositions[i] = Vector3DFloat(
					pathPositions[i].X() * scale.X() + origin.X(),
					pathPositions[i].Y() * scale.Y() + origin.Y(),
					pathPositions[i].Z() * scale.Z() + origin.Z());
			}

			if(atoms.size() > 1){ 
				double pathLength = 0;
				for(unsigned int i = 0; i < pathPositions.size()-1; i++) {				
					pathLength += (pathPositions[i] - pathPositions[i+1]).Length();
				}
				double atomGap = pathLength / (atoms.size() - 1);

				double residualDistance = 0.0;
				double segmentLength, param;
				int atomCtr = atoms.size() - 1;
				for(unsigned int i = 0; i < pathPositions.size()-1; i++) {
					if(isZero(residualDistance)) {
						atoms[atomCtr]->SetPosition(pathPositions[i]);
						atoms[atomCtr]->SetVisible(true);
						atomCtr--;
						residualDistance = atomGap;
					}

					Vector3DFloat p0 = pathPositions[i];
					Vector3DFloat p1 = pathPositions[i+1];

					segmentLength = (p0-p1).Length();
					while ((atomCtr >= 0) && (segmentLength >= residualDistance)) {
						param = residualDistance/segmentLength;
						atoms[atomCtr]->SetPosition(p0 * (1.0 - param) + p1 * param);
						atoms[atomCtr]->SetVisible(true);
						p0 = atoms[atomCtr]->GetPosition();
						atomCtr--;
						segmentLength = (p0-p1).Length();
						residualDistance = atomGap;
					}			
					residualDistance -= segmentLength;
				}
			}
		}

		void InteractiveLoopBuilderEngine::ClearCurrentPath() {
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

				for(unsigned int i = 0; i < atoms.size(); i++) {
					atoms[i]->SetVisible(false);
				}
			}
		}
	}
}

#endif
