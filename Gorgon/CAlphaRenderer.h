#ifndef GORGON_CALPHA_RENDERER_H
#define GORGON_CALPHA_RENDERER_H


#include <glut.h>
#include <cstdlib>
#include <cstdio>
#include "ProteinMorph/NonManifoldMesh.h"
#include "GraphMatch/PDBReader.h"
#include "GraphMatch/PDBAtom.h"
#include "Renderer.h"

using namespace std;
using namespace wustl_mm::Protein_Morph;
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

			void AddAtom(PDBAtom atom);
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
		private:
			vector<PDBAtom> atoms;
			vector<CAlphaBackboneSegment> backboneSegments;
		};


		CAlphaRenderer::CAlphaRenderer() {
			atoms.clear();
			backboneSegments.clear();
		}

		CAlphaRenderer::~CAlphaRenderer() {
			atoms.clear();
			backboneSegments.clear();
		}

		void CAlphaRenderer::AddAtom(PDBAtom atom) {
			atoms.push_back(atom);
		}
		void CAlphaRenderer::Draw(int subSceneIndex, bool selectEnabled) {

			bool selected;
			GLfloat frontMaterial[4];
			GLfloat backMaterial[4];
			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

			if(subSceneIndex == 0) {
				if(selectEnabled) {
					glPushName(0);
					glPushName(0);
				}
				for(int i=0; i < (int)atoms.size(); i++) {
					selected = ((subSceneIndex == selectedSubSceneIndex) && (i == selectedIx[0]));
					if(selected) {
						glGetMaterialfv(GL_FRONT, GL_EMISSION, frontMaterial);
						glGetMaterialfv(GL_BACK, GL_EMISSION, backMaterial);
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}

					glPushMatrix();
					if(selectEnabled){
						glLoadName(i);
					}
					glTranslatef(atoms[i].GetPosition().X(), atoms[i].GetPosition().Y(), atoms[i].GetPosition().Z());
					GLUquadric * quadricSphere = gluNewQuadric();
					gluSphere(quadricSphere, 0.3, 10, 10);
					gluDeleteQuadric(quadricSphere);
					glPopMatrix();
					if(selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, frontMaterial);
						glMaterialfv(GL_BACK, GL_EMISSION, backMaterial);
					}
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
				for(int i=0; i < (int)backboneSegments.size(); i++) {
					selected = ((subSceneIndex == selectedSubSceneIndex) && (i == selectedIx[0]));
					if(selected) {
						glGetMaterialfv(GL_FRONT, GL_EMISSION, frontMaterial);
						glGetMaterialfv(GL_BACK, GL_EMISSION, backMaterial);
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}

					if(selectEnabled){
						glLoadName(i);
					}
					DrawCylinder(atoms[backboneSegments[i].a0].GetPosition(), atoms[backboneSegments[i].a1].GetPosition(), 0.1);
					if(selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, frontMaterial);
						glMaterialfv(GL_BACK, GL_EMISSION, backMaterial);
					}
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
			backboneSegments.clear();
			atoms = PDBReader::ReadAtomPositions(fileName);

			// Keeping only C-Alpha atoms
			for(int i = atoms.size()-1; i >= 0; i--) {
				if(atoms[i].GetName().compare(" CA ") != 0) {
					atoms.erase(atoms.begin() + i);
				}
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
			backboneSegments.clear();
			UpdateBoundingBox();

		}

		void CAlphaRenderer::UpdateBoundingBox() {
			if(atoms.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = atoms[0].GetPosition().values[i];
					maxPts[i] = atoms[0].GetPosition().values[i];
				}
				for(unsigned int j = 1; j < atoms.size(); j++) {
					for(int i = 0; i < 3; i++) {
						minPts[i] = min(minPts[i], atoms[j].GetPosition().values[i]);
						maxPts[i] = max(maxPts[i], atoms[j].GetPosition().values[i]);
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