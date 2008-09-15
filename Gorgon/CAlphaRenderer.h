#ifndef GORGON_CALPHA_RENDERER_H
#define GORGON_CALPHA_RENDERER_H


#include <glut.h>
#include <cstdlib>
#include <cstdio>
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBAtom.h>
#include "Renderer.h"

using namespace std;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {	
		struct CAlphaBackboneSegment {
			unsigned int a0;
			unsigned int a1;
			bool selected;
		};

		class CAlphaRenderer : public Renderer{
		public:
			CAlphaRenderer();
			~CAlphaRenderer();

			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();

			// Controlling the atom vector
			void AddAtom(PDBAtom atom);
			PDBAtom GetAtom(int index);
			void DeleteAtom(int index);
			int GetAtomCount();

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

			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

			if(subSceneIndex == 0) {				
				if(selectEnabled) {
					glPushName(0);
					glPushName(0);
				}
				for(int i=0; i < (int)atoms.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(atoms[i].GetSelected()) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					} else {
						SetColor(atoms[i].GetColorR(), atoms[i].GetColorG(), atoms[i].GetColorB(), atoms[i].GetColorA());
					}					

					glPushMatrix();
					if(selectEnabled){
						glLoadName(i);
					}
					glTranslatef(atoms[i].GetPosition().X(), atoms[i].GetPosition().Y(), atoms[i].GetPosition().Z());
					GLUquadric * quadricSphere = gluNewQuadric();
					gluSphere(quadricSphere, atoms[i].GetAtomRadius() * 0.3, 10, 10);
					gluDeleteQuadric(quadricSphere);
					glPopMatrix();
					glPopAttrib();

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
					glPushAttrib(GL_LIGHTING_BIT);
					if(backboneSegments[i].selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}

					if(selectEnabled){
						glLoadName(i);
					}
					DrawCylinder(atoms[backboneSegments[i].a0].GetPosition(), atoms[backboneSegments[i].a1].GetPosition(), 0.1);
					glPopAttrib();
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
			for(int i = (int)atoms.size()-1; i >= 0; i--) {
				if(atoms[i].GetName().compare(" CA ") != 0) {
					atoms.erase(atoms.begin() + i);
				}
			}

			CAlphaBackboneSegment segment;
			for(int i = 0; i < (int)atoms.size()-1; i++) {
				segment.a0 = i;
				segment.a1 = i+1;
				segment.selected = false;
				backboneSegments.push_back(segment);				
			}
			UpdateBoundingBox();
			
		}

		bool CAlphaRenderer::SelectionClear() {
			if(Renderer::SelectionClear()) {
				for(unsigned int i = 0; i < atoms.size(); i++) {					
					atoms[i].SetSelected(false);
				}

				for(unsigned int i = 0; i < backboneSegments.size(); i++) {
					backboneSegments[i].selected = false;
				}
				return true;
			}
			return false;
		}

		void CAlphaRenderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Renderer::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
			if((subsceneIndex == 0) && (ix0 >= 0) && (ix0 <= (int)atoms.size())) {
				atoms[ix0].SetSelected(forceTrue || !atoms[ix0].GetSelected());
			} else if((subsceneIndex == 1) && (ix0 >= 0) && (ix0 <= (int)backboneSegments.size())) {
				backboneSegments[ix0].selected = (forceTrue || !backboneSegments[ix0].selected);
			}			
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
		PDBAtom CAlphaRenderer::GetAtom(int index) {
			return atoms[index];
		}

		int CAlphaRenderer::GetAtomCount() {
			return atoms.size();
		}

		void CAlphaRenderer::DeleteAtom(int index) {
			atoms.erase(atoms.begin() + index);
		}
	}
}


#endif