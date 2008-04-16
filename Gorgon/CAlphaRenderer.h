#ifndef GORGON_CALPHA_RENDERER_H
#define GORGON_CALPHA_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraphMatch/PDBReader.h>
#include "Renderer.h"

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {	
		struct CAlphaBackboneSegment {
			unsigned int a0;
			unsigned int a1;
		};

		class CAlphaRenderer : public Renderer{
		public:
			CAlphaRenderer();
			~CAlphaRenderer();

			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
		private:
			vector<PDBAtom> atoms;
			vector<GLUquadric *> atomQuadrics;
			vector<CAlphaBackboneSegment> backboneSegments;
		};


		CAlphaRenderer::CAlphaRenderer() {
			atoms.clear();
			backboneSegments.clear();
			atomQuadrics.clear();
		}

		CAlphaRenderer::~CAlphaRenderer() {
			atoms.clear();
			for(unsigned int i = 0; i < atomQuadrics.size(); i++) {
				gluDeleteQuadric(atomQuadrics[i]);
			}
			backboneSegments.clear();
			atomQuadrics.clear();
		}

		void CAlphaRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if(selectEnabled) {
					glPushName(0);
					glPushName(0);
				}
				for(unsigned int i=0; i < atoms.size(); i++) {
					glPushMatrix();
					if(selectEnabled){
						glLoadName(i);
					}
					glTranslatef(atoms[i].position.X(), atoms[i].position.Y(), atoms[i].position.Z());
					gluSphere(atomQuadrics[i], 0.3, 10, 10);
					glPopMatrix();
				}
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			} else if(subSceneIndex == 1) {
				if(selectEnabled) {
					glPushName(1);
					glPushName(0);
				}
				glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				glLineWidth(3);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		
				for(unsigned int i=0; i < backboneSegments.size(); i++) {
					if(selectEnabled){
						glLoadName(i);
					}
					glBegin(GL_LINES);
					glVertex3f(atoms[backboneSegments[i].a0].position.X(), atoms[backboneSegments[i].a0].position.Y(), atoms[backboneSegments[i].a0].position.Z());
					glVertex3f(atoms[backboneSegments[i].a1].position.X(), atoms[backboneSegments[i].a1].position.Y(), atoms[backboneSegments[i].a1].position.Z());
					glEnd();
				}
				glPopAttrib();
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			}
		}


		void CAlphaRenderer::LoadFile(string fileName) {
			atoms.clear();
			for(unsigned int i = 0; i < atomQuadrics.size(); i++) {
				gluDeleteQuadric(atomQuadrics[i]);
			}
			atomQuadrics.clear();
			backboneSegments.clear();
			atoms = PDBReader::ReadAtomPositions(fileName);
			for(unsigned int i = 0; i < atoms.size(); i++) {
				atomQuadrics.push_back(gluNewQuadric());				
			}
			CAlphaBackboneSegment segment;
			for(unsigned int i = 0; i < atoms.size()-1; i++) {
				segment.a0 = i;
				segment.a1 = i+1;
				backboneSegments.push_back(segment);				
			}
			UpdateBoundingBox();
			
		}

		void CAlphaRenderer::Unload() {
			atoms.clear();
			for(unsigned int i = 0; i < atomQuadrics.size(); i++) {
				gluDeleteQuadric(atomQuadrics[i]);
			}
			atomQuadrics.clear();
			backboneSegments.clear();
			UpdateBoundingBox();

		}

		void CAlphaRenderer::UpdateBoundingBox() {
			if(atoms.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = atoms[0].position.values[i];
					maxPts[i] = atoms[0].position.values[i];
				}
				for(unsigned int j = 1; j < atoms.size(); j++) {
					for(int i = 0; i < 3; i++) {
						minPts[i] = min(minPts[i], atoms[j].position.values[i]);
						maxPts[i] = max(maxPts[i], atoms[j].position.values[i]);
					}
				}
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = -0.5;
					maxPts[i] = 0.5;
				}
			}
		}

		string CAlphaRenderer::GetSupportedLoadFileFormats() {
			return "Atom Positions (*.pdb)";
		}

		string CAlphaRenderer::GetSupportedSaveFileFormats() {
			return "Atom Positions (*.atom)";
		}
	}
}


#endif