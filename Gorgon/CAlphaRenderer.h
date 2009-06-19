// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Rendering engine responsible for rendering C-Alpha atoms.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.33  2009/03/30 21:36:12  ssa1
//   Interactive loop building
//
//   Revision 1.32  2008/12/07 07:11:36  ssa1
//   Coloring bonds with red and blue if they exceed maximum or minimum length restrictions
//
//   Revision 1.31  2008/12/03 21:58:25  ssa1
//   Selection rotations for atoms and helices.
//
//   Revision 1.30  2008/12/02 23:55:43  colemanr
//   Fixed logic for GetBondIndex().
//
//   Revision 1.29  2008/12/02 21:25:44  ssa1
//   adding getBondIndex method to give access to bonds
//
//   Revision 1.28  2008/11/13 20:54:40  ssa1
//   Using the correct scale when loading volumes
//
//   Revision 1.27  2008/11/10 16:15:43  ssa1
//   Making python and C++ use the same PDBAtom objects
//
//   Revision 1.26  2008/11/07 21:32:21  ssa1
//   Fixing returning of the actual c++ pdbatom object instead of a copy
//
//   Revision 1.25  2008/10/10 14:25:55  ssa1
//   Setting the cost functions to scale with the edge length
//
//   Revision 1.24  2008/10/07 23:48:14  colemanr
//   added a function which returns the PDBAtom for a given hitStack
//
//   Revision 1.23  2008/09/29 20:36:35  ssa1
//   Drawing skeletal curves as cylinders and spheres
//
//   Revision 1.22  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//

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
#include <GraphMatch/VectorMath.h>

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
			void LoadSSEHunterFile(string fileName);
			int SelectionObjectCount();
			Vector3DFloat SelectionCenterOfMass();
			bool SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle);
			bool SelectionMove(Vector3DFloat moveDirection);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);

			// Controlling the atom vector
			PDBAtom * AddAtom(PDBAtom atom);
			PDBAtom * GetAtom(unsigned long long index);
			PDBAtom * GetAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4);
			void DeleteAtom(unsigned long long index);
			int GetAtomCount();
			
			//Controlling the bond vector
			void AddBond(PDBBond bond);
			PDBBond * GetBond(int index);
			int GetBondIndex(unsigned long long atom0, unsigned long long atom1);
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

		PDBAtom * CAlphaRenderer::AddAtom(PDBAtom atom) {
			atoms[atom.GetHashKey()] = atom;
			UpdateBoundingBox();
			return &atoms[atom.GetHashKey()];
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

					if(selectEnabled){
						glLoadName((long)&(i->second));
					}
					if(i->second.GetVisible()) {
						DrawSphere(i->second.GetPosition(), i->second.GetAtomRadius() * 0.3);
					}
					
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
				for(int i=0; i < (int)bonds.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(bonds[i].GetSelected()) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}

					if(selectEnabled){
						glLoadName(i);
					}
					float length = (atoms[bonds[i].GetAtom0Ix()].GetPosition() - atoms[bonds[i].GetAtom1Ix()].GetPosition()).Length();
					if(length > 4.2) {
						SetColor(1.0, 0, 0, 1.0);
					}

					if(length < 3.3) {
						SetColor(0, 0, 1.0, 1.0);
					}
			
					if(atoms[bonds[i].GetAtom0Ix()].GetVisible() && atoms[bonds[i].GetAtom1Ix()].GetVisible()) {
						DrawCylinder(atoms[bonds[i].GetAtom0Ix()].GetPosition(), atoms[bonds[i].GetAtom1Ix()].GetPosition(), 0.1);
					}
					glPopAttrib();
				}
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			}
		}


		PDBAtom * CAlphaRenderer::GetAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			if((subsceneIndex == 0) && (ix0 != NULL)) {
				PDBAtom * a = (PDBAtom*)ix0;
				return a;
			}  
			return NULL;
		}
			
		void CAlphaRenderer::LoadFile(string fileName) {
			Renderer::LoadFile(fileName);
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

		void CAlphaRenderer::LoadSSEHunterFile(string fileName) {
			Renderer::LoadFile(fileName);
			atoms.clear();
			bonds.clear();
			atoms = PDBReader::ReadAtomPositions(fileName);			
			float tempFactor;
			float r, g, b;

			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				i->second.SetAtomRadius(3.0);
				tempFactor = i->second.GetTempFactor();
				if(tempFactor < 0) {
					r = 1.0f + tempFactor;
					g = 1.0f + tempFactor;
					b = 1.0f;
				} else {
					r = 1.0f;
					g = 1 - tempFactor;
					b = 1 - tempFactor;
				}
					
				i->second.SetColor(r, g, b, 1.0f);
			}
			UpdateBoundingBox();
			
		}

		int CAlphaRenderer::SelectionObjectCount(){
			int count = 0;
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				if(i->second.GetSelected()) {
					count++;
				}
			}

			for(unsigned int i = 0; i < bonds.size(); i++) {
				if(bonds[i].GetSelected()) {
					count++;
				}
			}
			return count;
		}

		Vector3DFloat CAlphaRenderer::SelectionCenterOfMass() {
			int count = 0;
			Vector3DFloat centerOfMass = Vector3DFloat(0,0,0);
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				if(i->second.GetSelected()) {
					count++;
					centerOfMass = centerOfMass + i->second.GetPosition();
				}
			}

			for(unsigned int i = 0; i < bonds.size(); i++) {
				if(bonds[i].GetSelected()) {
					count++;
					centerOfMass = centerOfMass + (atoms[bonds[i].GetAtom0Ix()].GetPosition() + atoms[bonds[i].GetAtom1Ix()].GetPosition()) * 0.5;
				}
			}
			if(count == 0) {
				centerOfMass = Renderer::SelectionCenterOfMass();
			} else {
				centerOfMass = centerOfMass * (1.0f/(float)count);
			}
			return centerOfMass;
		}

		bool CAlphaRenderer::SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle) {
			bool rotated = false;
			Vector3 centerOfMassP3 = Vector3(centerOfMass.X(), centerOfMass.Y(), centerOfMass.Z());
			Vector3 rotationV3 = Vector3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z());
			
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				if(i->second.GetSelected()) {
					rotated = true;
					Vector3DFloat move = centerOfMass - i->second.GetPosition();
					Vector3 moveV3 = Vector3(move.X(), move.Y(), move.Z());
					Matrix4 rotMatrix = Matrix4::rotation(rotationV3, angle);
					Vector3 newMove = rotMatrix * moveV3; 
					newMove = centerOfMassP3 - newMove;
					i->second.SetPosition(Vector3DFloat(newMove[0], newMove[1], newMove[2]));
				}
			}
			return rotated;
		}

		bool CAlphaRenderer::SelectionMove(Vector3DFloat moveDirection) {
			bool moved = false;
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				if(i->second.GetSelected()) {
					i->second.SetPosition(i->second.GetPosition() + moveDirection);
					i->second.SetFlag(1);
					moved = true;
				} else {
					i->second.SetFlag(0);
				}
			}

			for(unsigned int i = 0; i < bonds.size(); i++) {
				if(bonds[i].GetSelected()) {
					PDBAtom a = atoms[bonds[i].GetAtom0Ix()];
					if(a.GetFlag() == 0) {
						a.SetPosition(a.GetPosition() + moveDirection);
						a.SetFlag(1);
						moved = true;
					}

					a = atoms[bonds[i].GetAtom1Ix()];
					if(a.GetFlag() == 0) {
						a.SetPosition(a.GetPosition() + moveDirection);
						a.SetFlag(1);
						moved = true;
					}
				}
			}
			UpdateBoundingBox();
			return moved;

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
		PDBAtom * CAlphaRenderer::GetAtom(unsigned long long index) {
			return &atoms[index];
		}

		PDBBond * CAlphaRenderer::GetBond(int index) {
			return &bonds[index];
		}

		int CAlphaRenderer::GetBondIndex(unsigned long long atom0, unsigned long long atom1) {
			for(unsigned int i = 0; i < bonds.size(); i++) {
				if(((bonds[i].GetAtom0Ix() == atom0) && (bonds[i].GetAtom1Ix() == atom1)) ||
							((bonds[i].GetAtom0Ix() == atom1) && (bonds[i].GetAtom1Ix() == atom0))) {
					return i;
				}
			}
			return -1;
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
