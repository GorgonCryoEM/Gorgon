#ifndef GORGON_CALPHA_RENDERER_H
#define GORGON_CALPHA_RENDERER_H


#include <glut.h>
#include <cstdlib>
#include <cstdio>
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/PDBBond.h>
#include "Renderer.h"
#include <map>
#include <list>

using namespace std;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {	
		typedef map<unsigned long long, PDBAtom> AtomMapType;
		struct SerialAndHashType {
			unsigned int serial;
			unsigned long long hashKey;
		};

		class SerialAndHashTypePredicate {
		public:
			bool operator() (const SerialAndHashType& lhs, const SerialAndHashType& rhs) {
				return lhs.serial < rhs.serial;
			}
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
			virtual Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);

			// Controlling the atom vector
			int AddAtom(PDBAtom atom);
			PDBAtom GetAtom(unsigned long long index);
			void DeleteAtom(unsigned long long index);
			int GetAtomCount();
			
			//Controlling the bond vector
			void AddBond(PDBBond bond);
			PDBBond GetBond(int index);
			void DeleteBond(int index);
			int GetBondCount();

		private:
			void UpdateBoundingBox();
		private:
			AtomMapType atoms;
			vector<PDBBond> bonds;
		};


		CAlphaRenderer::CAlphaRenderer() {
			atoms.clear();
			bonds.clear();
		}

		CAlphaRenderer::~CAlphaRenderer() {
			atoms.clear();
			bonds.clear();
		}

		int CAlphaRenderer::AddAtom(PDBAtom atom) {
		  	int index;
			index = atoms.size();
			atom.SetSerial(index);
			atoms[atom.GetHashKey()] = atom;
			UpdateBoundingBox();
			return index;
		}

		void CAlphaRenderer::AddBond(PDBBond bond) {
			bonds.push_back(bond);
		}

		void CAlphaRenderer::Draw(int subSceneIndex, bool selectEnabled) {

			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

			if(subSceneIndex == 0) {				
				if(selectEnabled) {
					glPushName(0);
					glPushName(0);
				}
				for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(i->second.GetSelected()) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					} else {
						SetColor(i->second.GetColorR(), i->second.GetColorG(), i->second.GetColorB(), i->second.GetColorA());
					}					

					glPushMatrix();
					if(selectEnabled){
						glLoadName((GLint)&(i->second));
					}
					glTranslatef(i->second.GetPosition().X(), i->second.GetPosition().Y(), i->second.GetPosition().Z());
					GLUquadric * quadricSphere = gluNewQuadric();
					gluSphere(quadricSphere, i->second.GetAtomRadius() * 0.3, 10, 10);
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
				for(int i=0; i < (int)bonds.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(bonds[i].GetSelected()) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}

					if(selectEnabled){
						glLoadName(i);
					}
					DrawCylinder(atoms[bonds[i].GetAtom0Ix()].GetPosition(), atoms[bonds[i].GetAtom1Ix()].GetPosition(), 0.1);
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
			bonds.clear();
			atoms = PDBReader::ReadAtomPositions(fileName);			
						
			// Keeping only C-Alpha atoms
			vector<unsigned long long> eraseKeys;
			eraseKeys.clear();

			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {				
				if(i->second.GetName().compare("CA") != 0) {					
					eraseKeys.push_back(i->first);
				} 
			}

			for(unsigned int i = 0; i < eraseKeys.size(); i++) {
				atoms.erase(atoms.find(eraseKeys[i]));
			}

			eraseKeys.clear();

			list<SerialAndHashType> sortedSerials;
			SerialAndHashType elem;
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				elem.hashKey = i->first;
				elem.serial = i->second.GetSerial();

				sortedSerials.push_back(elem);				
			}
			sortedSerials.sort(SerialAndHashTypePredicate());


			list<SerialAndHashType>::iterator oldAtom = sortedSerials.begin();
			list<SerialAndHashType>::iterator startAtom = sortedSerials.begin();

			startAtom++;
			for(list<SerialAndHashType>::iterator i = startAtom; i != sortedSerials.end(); i++) {
				bonds.push_back(PDBBond(oldAtom->hashKey, i->hashKey, false));	
				oldAtom = i;
			}
			sortedSerials.clear();
			UpdateBoundingBox();
			
		}

		bool CAlphaRenderer::SelectionClear() {
			if(Renderer::SelectionClear()) {
				for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
					i->second.SetSelected(false);
				}

				for(unsigned int i = 0; i < bonds.size(); i++) {
					bonds[i].SetSelected(false);
				}
				return true;
			}
			return false;
		}

		void CAlphaRenderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Renderer::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
			if((subsceneIndex == 0) && (ix0 != NULL)) {
				PDBAtom * a = (PDBAtom*)ix0;
				a->SetSelected(forceTrue || !a->GetSelected());
			} else if((subsceneIndex == 1) && (ix0 >= 0) && (ix0 <= (int)bonds.size())) {
				bonds[ix0].SetSelected(forceTrue || !bonds[ix0].GetSelected());
			}			
		}

		void CAlphaRenderer::Unload() {
			atoms.clear();
			bonds.clear();
			UpdateBoundingBox();
		}

		void CAlphaRenderer::UpdateBoundingBox() {
			if(atoms.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = atoms.begin()->second.GetPosition().values[i];
					maxPts[i] = atoms.begin()->second.GetPosition().values[i];
				}

				for(AtomMapType::iterator j = atoms.begin(); j != atoms.end(); j++) {
					for(int i = 0; i < 3; i++) {
						minPts[i] = min(minPts[i], j->second.GetPosition().values[i]);
						maxPts[i] = max(maxPts[i], j->second.GetPosition().values[i]);
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
		PDBAtom CAlphaRenderer::GetAtom(unsigned long long index) {
			return atoms[index];
		}

		PDBBond CAlphaRenderer::GetBond(int index) {
			return bonds[index];
		}

		int CAlphaRenderer::GetAtomCount() {
			return atoms.size();
		}

		int CAlphaRenderer::GetBondCount() {
			return bonds.size();
		}

		void CAlphaRenderer::DeleteAtom(unsigned long long index) {
			atoms.erase(atoms.find(index));
		}

		void CAlphaRenderer::DeleteBond(int index) {
			bonds.erase(bonds.begin() + index);
		}

		Vector3DFloat CAlphaRenderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Vector3DFloat position;
			switch(subsceneIndex) {
				case(0):
					if((ix0 >= 0) && (ix0 <= (int)atoms.size())) {
						PDBAtom * a = (PDBAtom*)ix0;
						position = a->GetPosition();
					}
					break;
				case(1):
					if((ix0 >= 0) && (ix0 <= (int)bonds.size())) {
						position = (atoms[bonds[ix0].GetAtom0Ix()].GetPosition() + atoms[bonds[ix0].GetAtom1Ix()].GetPosition()) * 0.5;
					}
					break;
				default:
					position = Vector3DFloat(0,0,0);
					break;
			}
			return position;
		}
	}
}


#endif
