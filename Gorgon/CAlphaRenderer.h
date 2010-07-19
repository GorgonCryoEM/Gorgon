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
//   Revision 1.51  2010/06/23 19:11:51  ssa1
//   Adding simple ribbon rendering and associated events for flexible fitting
//
//   Revision 1.50  2010/06/23 13:02:56  ssa1
//   Allowing users to reset a flexible fitting if need be.
//
//   Revision 1.49  2010/05/27 18:28:46  ssa1
//   Better color control for all atom visualization
//
//   Revision 1.48  2010/05/27 17:10:18  ssa1
//   Better color control for all atom visualization
//
//   Revision 1.47  2010/05/27 05:08:49  ssa1
//   Side chain visualization on Gorgon
//
//   Revision 1.46  2010/05/27 04:41:54  ssa1
//   Side chain visualization on Gorgon
//
//   Revision 1.45  2010/05/26 20:17:35  ssa1
//   Adding in display styles for atom rendering.
//
//   Revision 1.44  2010/05/21 15:45:16  ssa1
//   Flexible fitting implemented in Gorgon
//
//   Revision 1.43  2010/05/20 21:55:53  ssa1
//   Rigid body alignment based on largest flexible cluster
//
//   Revision 1.42  2010/02/11 23:19:11  ssa1
//   Allowing the ability to save pseudoatoms generated from SSEHunter
//
//   Revision 1.41  2010/01/10 05:31:43  colemanr
//   PDBAtoms now store their correlation, skeleton, and geometry scores. Changing the weighting for these three scores in the GUI now changes the total score for each pseudoatom.
//
//   Revision 1.40  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.39  2009/08/10 20:03:40  ssa1
//   SSEHunter interfaced into Gorgon
//
//   Revision 1.38  2009/08/10 13:54:38  ssa1
//   Adding initial ssehunter program
//
//   Revision 1.37  2009/07/01 21:25:13  ssa1
//   Centering the volume cropped using a radius around the point selected by the atom selection tool.
//
//   Revision 1.36  2009/06/30 21:23:24  ssa1
//   SSEHunter results have range between -3 and 3, not -1 and 1
//
//   Revision 1.35  2009/06/22 20:17:27  ssa1
//   Adding in SSEBuilder Functionality: Selection to Helix functionality
//
//   Revision 1.34  2009/06/19 18:51:05  ssa1
//   Adding in SSEBuilder Functionality
//
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
#include <ProteinMorph/SSEHunter.h>
#include <SkeletonMaker/volume.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/PDBBond.h>
#include "Renderer.h"
#include <map>
#include <list>
#include <GraphMatch/VectorMath.h>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::SkeletonMaker;
using namespace boost::tuples;

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

		const int CALPHA_DISPLAY_STYLE_BACKBONE = 3;
		const int CALPHA_DISPLAY_STYLE_RIBBON = 4;
		const int CALPHA_DISPLAY_STYLE_SIDE_CHAIN = 5;


		class CAlphaRenderer : public Renderer{
		public:
			struct Secel{
				vector<unsigned long long> atomHashes;
				bool selected;
			};

			CAlphaRenderer();
			~CAlphaRenderer();

			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void LoadSSEHunterFile(string fileName);
			bool SaveSSEHunterFile(string fileName);
			void GetSSEHunterAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff);
			void UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
			void ColorSSEHunterAtoms();
			int SelectionObjectCount();
			int SelectionAtomCount();
			Vector3DFloat SelectionCenterOfMass();
			bool SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle);
			bool SelectionMove(Vector3DFloat moveDirection);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void TransformAllAtomLocations(MatrixFloat transform);
			void UpdateBoundingBox();

			// Controlling the atom vector
			PDBAtom * AddAtom(PDBAtom atom);
			PDBAtom * GetAtom(unsigned long long index);
			PDBAtom * GetAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4);
			PDBAtom * GetSelectedAtom(unsigned int selectionId);
			void DeleteAtom(unsigned long long index);
			int GetAtomCount();
			
			//Controlling the bond vector
			void AddBond(PDBBond bond);
			PDBBond * GetBond(int index);
			int GetBondIndex(unsigned long long atom0, unsigned long long atom1);
			void DeleteBond(int index);
			int GetBondCount();

			//Controlling the bond vector
			void AddSideChainBond(PDBBond bond);
			PDBBond * GetSideChainBond(int index);
			int GetSideChainBondIndex(unsigned long long atom0, unsigned long long atom1);
			void DeleteSideChainBond(int index);
			int GetSideChainBondCount();

			int StartHelix(); //StartHelix creates a new helix element in aHelices and returns its index
			void AddHelixElement(int, unsigned long long); //adds a helix element to the helix indexed at param 1

			int StartStrand(); //StartStrand creates a new strand element in bStrands and returns its index
			void AddStrandElement(int, unsigned long long); //adds a strand element to the strand indexed at param 1

			int StartLoop(); //StartLoop creates a new strand element in loops and returns its index
			void AddLoopElement(int, unsigned long long); //adds a loop element to the loop indexed at param 1

			bool CleanSecondaryStructures(); //empties the aHelices, bStrands and loops variables
											//what should really happen is that the code should check if it is
											//trying to reload the same one, and then if it did return false


			vector<int> GetSelectedHelixIndices();
			void SetHelixCorrs( vector < int > flatCorrespondences);
			void SetSelectedSSEHelices(vector<int>);
			void ClearOtherHighlights();
			void SetFeatureVecs(vector<Vector3DFloat> flatFeatureVecs);
		private:
			void DrawBackboneModel(int subSceneIndex, bool selectEnabled);
			void DrawRibbonModel(int subSceneIndex, bool selectEnabled);
			void DrawSideChainModel(int subSceneIndex, bool selectEnabled);
		private:
			AtomMapType atoms;
			vector<PDBBond> bonds;
			vector<PDBBond> sidechainBonds;

			vector<Secel> aHelices;
			vector<Secel> bStrands;
			vector<Secel> loops;
			
			vector<int> selectedHelixIndices;
			vector < tuple<int, int> > corrs;
			vector<int> selectedSSEHelices;
			vector<tuple<Vector3DFloat, Vector3DFloat>> featureVecs;
		};


		CAlphaRenderer::CAlphaRenderer() {
			atoms.clear();
			bonds.clear();
			sidechainBonds.clear();
			selectedHelixIndices.clear();
			featureVecs.clear();
		}

		CAlphaRenderer::~CAlphaRenderer() {
			atoms.clear();
			bonds.clear();
			sidechainBonds.clear();
			selectedHelixIndices.clear();
			featureVecs.clear();
		}

		vector<int> CAlphaRenderer::GetSelectedHelixIndices(){
			return selectedHelixIndices;
		}

		PDBAtom * CAlphaRenderer::AddAtom(PDBAtom atom) {
			atoms[atom.GetHashKey()] = atom;
			UpdateBoundingBox();
			return &atoms[atom.GetHashKey()];
		}

		void CAlphaRenderer::AddBond(PDBBond bond) {
			bonds.push_back(bond);
		}

		void CAlphaRenderer::AddSideChainBond(PDBBond bond) {
			sidechainBonds.push_back(bond);
		}

		void CAlphaRenderer::DrawBackboneModel(int subSceneIndex, bool selectEnabled) {
			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

			if(subSceneIndex == 0) { // Drawing Atoms				
				if(selectEnabled) {
					glPushName(0);
					glPushName(0);
				}
				for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
					if(i->second.GetName() == "CA") {
						glPushAttrib(GL_LIGHTING_BIT);					
						if(i->second.GetSelected()) {
							glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
							glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
						} else {
							OpenGLUtils::SetColor(i->second.GetColorR(), i->second.GetColorG(), i->second.GetColorB(), i->second.GetColorA());
						}					

						if(selectEnabled){
							glLoadName((long)&(i->second));
						}
						if(i->second.GetVisible()) {
							DrawSphere(i->second.GetPosition(), i->second.GetAtomRadius() * 0.3);
						}
						
						glPopAttrib();
					}

				}
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			} else if(subSceneIndex == 1) { // Drawing Bonds
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
						OpenGLUtils::SetColor(1.0, 0, 0, 1.0);
					}

					if(length < 3.3) {
						OpenGLUtils::SetColor(0, 0, 1.0, 1.0);
					}
			
					if(atoms[bonds[i].GetAtom0Ix()].GetVisible() && atoms[bonds[i].GetAtom1Ix()].GetVisible()) {
						DrawCylinder(atoms[bonds[i].GetAtom0Ix()].GetPosition(), atoms[bonds[i].GetAtom1Ix()].GetPosition(), 0.1, 10, 2);
					}
					glPopAttrib();
				}
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			} else if(subSceneIndex == 2) { // Drawing spheres to cover up the cylinder edges				
				for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
					if(i->second.GetName() == "CA") {
						DrawSphere(i->second.GetPosition(), 0.1);
					}
				}
			}
		}

		void CAlphaRenderer::DrawRibbonModel(int subSceneIndex, bool selectEnabled) {
			if(selectEnabled) {
				glPushName(subSceneIndex);
				glPushName(0);
			}
			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
			GLfloat frontColor[4] = {1.0, 0.0, 0.0, 1.0};
			GLfloat backColor[4] = {0.0, 0.0, 1.0, 1.0};
			vector<int> PDBIndices;

			switch(subSceneIndex) {
				case 0: // Helices
					
					for(unsigned int i = 0; i < corrs.size(); ++i){
						int SSEIndex = get<1> (corrs[i]);
						for(unsigned int k = 0; k < selectedSSEHelices.size(); ++k){
							if(selectedSSEHelices[k] == SSEIndex){
								PDBIndices.push_back( get<0>( corrs[i]) );
							}
						}
					}

					for(int i = 0; i < aHelices.size(); i++) {
						cout << "Drawing helix # " << i << endl;
						if(selectEnabled){
							glLoadName(i);
						}
						glPushAttrib(GL_LIGHTING_BIT);
						if(aHelices[i].selected == true){
							cout << "This helix is selected" << endl;
							glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
							glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
						}
						for(int j = 0; j < aHelices[i].atomHashes.size()-1; j++) {
							DrawCylinder(atoms[aHelices[i].atomHashes[j]].GetPosition(), atoms[aHelices[i].atomHashes[j+1]].GetPosition(), 0.1, 10, 2);
						}
						glPopAttrib();

						if(aHelices[i].selected == true){
							cout << "About to draw end spheres size of featureVecs: " << featureVecs.size() << endl;
							glPushAttrib(GL_LIGHTING_BIT);
							
							if(featureVecs.size() > 0){
								OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
								DrawSphere(featureVecs[i].get<0>(), 1.0);
								OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
								DrawSphere(featureVecs[i].get<1>(), 1.0);
							}else{
								OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
								DrawSphere(atoms[aHelices[i].atomHashes[0]].GetPosition(), 1.0);
								OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
								DrawSphere(atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition(), 1.0);
							}
						
							glPopAttrib();
							Vector3DFloat pos1 = atoms[aHelices[i].atomHashes[0]].GetPosition();
							Vector3DFloat pos2 = atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition();
							printf("Drawing PDB Spheres at PDB ID %d with end #1 [%f, %f, %f] and #2 [%f, %f, %f]\n", i+1, pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z());
						
							fflush(stdout);
						}

						for(unsigned int j = 0; j < PDBIndices.size(); ++j){
							if(PDBIndices[j] == i){
								glPushAttrib(GL_LIGHTING_BIT);
								OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
								DrawSphere(atoms[aHelices[i].atomHashes[0]].GetPosition(), 1.0);
								OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
								DrawSphere(atoms[aHelices[i].atomHashes[aHelices[i].atomHashes.size()-1]].GetPosition(), 1.0);
								glPopAttrib();								
							}
						}
						


					}
					break;
				case 1: // Strands
					for(int i = 0; i < bStrands.size(); i++) {
						if(selectEnabled){
							glLoadName(i);
						}
						for(int j = 0; j < bStrands[i].atomHashes.size()-1; j++) {
							DrawCylinder(atoms[bStrands[i].atomHashes[j]].GetPosition(), atoms[bStrands[i].atomHashes[j+1]].GetPosition(), 0.1, 10, 2);
						}
					}
					break;
				case 2: // Loops
					for(int i = 0; i < loops.size(); i++) {
						if(selectEnabled){
							glLoadName(i);
						}
						for(int j = 0; j < loops[i].atomHashes.size()-1; j++) {
							DrawCylinder(atoms[loops[i].atomHashes[j]].GetPosition(), atoms[loops[i].atomHashes[j+1]].GetPosition(), 0.1, 10, 2);
						}
					}
					break;					
			}

			if(selectEnabled) {
				glPopName();
				glPopName();
			}
		}

		void CAlphaRenderer::DrawSideChainModel(int subSceneIndex, bool selectEnabled) {
			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
			float r,g,b,a;

			if(subSceneIndex == 0) { // Drawing Atoms				
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
						i->second.GetColor(r, g, b, a);
						OpenGLUtils::SetColor(r,g,b,a);
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
			} else if(subSceneIndex == 1) { // Drawing Bonds
				if(selectEnabled) {
					glPushName(1);
					glPushName(0);
				}
				Vector3DFloat v1, vc, v2;


				for(int i=0; i < (int)sidechainBonds.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(selectEnabled){
						glLoadName(i);
					}
			
					if(atoms[sidechainBonds[i].GetAtom0Ix()].GetVisible() && atoms[sidechainBonds[i].GetAtom1Ix()].GetVisible()) {
						v1 = atoms[sidechainBonds[i].GetAtom0Ix()].GetPosition();
						v2 = atoms[sidechainBonds[i].GetAtom1Ix()].GetPosition();
						if(sidechainBonds[i].GetSelected()) {
							glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
							glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
							DrawCylinder(v1, v2, 0.1, 6, 2);
						} else {						
							vc = (v1 + v2) * 0.5;
							atoms[sidechainBonds[i].GetAtom0Ix()].GetColor(r, g, b, a);
							OpenGLUtils::SetColor(r,g,b,a);
							DrawCylinder(v1, vc, 0.1, 6, 2);
							atoms[sidechainBonds[i].GetAtom1Ix()].GetColor(r, g, b, a);
							OpenGLUtils::SetColor(r,g,b,a);
							DrawCylinder(vc, v2, 0.1, 6, 2);
						}
					}
					glPopAttrib();
				}
				if(selectEnabled) {
					glPopName();
					glPopName();
				}
			} else if(subSceneIndex == 2) { // Drawing spheres to cover up the cylinder edges					
				for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);					
					i->second.GetColor(r, g, b, a);
					OpenGLUtils::SetColor(r,g,b,a);
					DrawSphere(i->second.GetPosition(), 0.1);										
					glPopAttrib();
				}
			}
		}

		void CAlphaRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			switch(displayStyle) {
				case CALPHA_DISPLAY_STYLE_BACKBONE: // Backbone only
					DrawBackboneModel(subSceneIndex, selectEnabled);
					break;
				case CALPHA_DISPLAY_STYLE_RIBBON: // Ribbon mode
					DrawRibbonModel(subSceneIndex, selectEnabled);
					break;
				case CALPHA_DISPLAY_STYLE_SIDE_CHAIN: // Side chains
					DrawSideChainModel(subSceneIndex, selectEnabled);
					break;
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

			float maxTempFactor = -10000.0f, minTempFactor = 10000.0f;
			float tempFactor;

			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				tempFactor = i->second.GetTempFactor();
				if(tempFactor > maxTempFactor) {
					maxTempFactor = tempFactor;
				}
				if(tempFactor < minTempFactor) {
					minTempFactor = tempFactor;
				}
			}
			float r, g, b;

			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				i->second.SetAtomRadius(3.0);
				tempFactor = i->second.GetTempFactor();
				if(tempFactor < 0) {
					tempFactor = (tempFactor / minTempFactor);
					r = 1.0f - tempFactor;
					g = 1.0f - tempFactor;
					b = 1.0f;
				} else {
					tempFactor = (tempFactor / maxTempFactor);
					r = 1.0f;
					g = 1.0f - tempFactor;
					b = 1.0f - tempFactor;
				}
					
				i->second.SetColor(r, g, b, 1.0f);
			}
			UpdateBoundingBox();
			
		}
		bool CAlphaRenderer::SaveSSEHunterFile(string fileName) {
			return PDBReader::WriteAtomPositions(atoms, fileName);			
		}

		void CAlphaRenderer::GetSSEHunterAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
			Renderer::LoadFile("");
			atoms.clear();
			bonds.clear();
			
			SSEHunter * hunter = new SSEHunter();
			atoms = hunter->GetScoredAtoms(vol, skeleton, resolution, threshold, correlationCoeff, skeletonCoeff, geometryCoeff);
			delete hunter;
			
			ColorSSEHunterAtoms();
		}
		
		void CAlphaRenderer::UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				i->second.SetTempFactor( i->second.GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
			}
			ColorSSEHunterAtoms();
		}

		void CAlphaRenderer::ColorSSEHunterAtoms() {
			float maxTempFactor = -10000.0f, minTempFactor = 10000.0f;
			float tempFactor;
			
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				tempFactor = i->second.GetTempFactor();
				if(tempFactor > maxTempFactor) {
					maxTempFactor = tempFactor;
				}
				if(tempFactor < minTempFactor) {
					minTempFactor = tempFactor;
				}
			}
			float r, g, b;
			
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
				i->second.SetAtomRadius(3.0);
				tempFactor = i->second.GetTempFactor();
				if(tempFactor < 0) {
					tempFactor = (tempFactor / minTempFactor);
					r = 1.0f - tempFactor;
					g = 1.0f - tempFactor;
					b = 1.0f;
				} else {
					tempFactor = (tempFactor / maxTempFactor);
					r = 1.0f;
					g = 1.0f - tempFactor;
					b = 1.0f - tempFactor;
				}
				
				i->second.SetColor(r, g, b, 1.0f);
			}
			
			UpdateBoundingBox();
		}

		int CAlphaRenderer::SelectionObjectCount(){
			int count = SelectionAtomCount();
			for(unsigned int i = 0; i < bonds.size(); i++) {
				if(bonds[i].GetSelected()) {
					count++;
				}
			}
			return count;
		}

		int CAlphaRenderer::SelectionAtomCount(){
			int count = 0;
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				if(i->second.GetSelected()) {
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
				for(unsigned int i = 0; i < aHelices.size(); i++) {
					aHelices[i].selected = false;
				}
				selectedHelixIndices.clear();
				selectedSSEHelices.clear();
				return true;
			}
			return false;
		}

		void CAlphaRenderer::ClearOtherHighlights(){
			selectedSSEHelices.clear();
		}

		void CAlphaRenderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Renderer::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
			PDBAtom * a;
			if((subsceneIndex == 0)) {
				switch(displayStyle) {
					case CALPHA_DISPLAY_STYLE_BACKBONE:
						if(ix0 != NULL){
							a = (PDBAtom*)ix0;
							a->SetSelected(forceTrue || !a->GetSelected());
						}
						break;
					case CALPHA_DISPLAY_STYLE_RIBBON:
						
						if(aHelices[ix0].selected == true && !forceTrue){
							aHelices[ix0].selected = false;
						}
						else{
							cout << "Updating selectedHelix" << " ix0=" << ix0 << " forceTrue=" << forceTrue << endl;
							aHelices[ix0].selected = true;
							selectedHelixIndices.push_back(ix0);
						}
						break;
					case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
						if(ix0 != NULL){
							a = (PDBAtom*)ix0;
							a->SetSelected(forceTrue || !a->GetSelected());
						}
						break;
				} 
			} else if((subsceneIndex == 1) && (ix0 >= 0) && (ix0 <= (int)bonds.size())) {
				switch(displayStyle) {
					case CALPHA_DISPLAY_STYLE_BACKBONE:
						bonds[ix0].SetSelected(forceTrue || !bonds[ix0].GetSelected());
						break;
					case CALPHA_DISPLAY_STYLE_RIBBON:
						cout << "A Ribbon was selected and subscene is 1" << endl;
						break;
					case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
						sidechainBonds[ix0].SetSelected(forceTrue || !sidechainBonds[ix0].GetSelected());
						break;
				} 
			} else if((subsceneIndex == 2) && (ix0 != NULL)) {			
				switch(displayStyle) {
					case CALPHA_DISPLAY_STYLE_BACKBONE:
						break;
					case CALPHA_DISPLAY_STYLE_RIBBON:
						cout << "A Ribbon was selected and subscene is 2" << endl;
						break;
					case CALPHA_DISPLAY_STYLE_SIDE_CHAIN:
						break;
				} 			
			}
			cout << "Finished updating selected calpha helix" << endl;
		}

		void CAlphaRenderer::Unload() {
			atoms.clear();
			bonds.clear();
			sidechainBonds.clear();
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

		PDBBond * CAlphaRenderer::GetSideChainBond(int index) {
			return &sidechainBonds[index];
		}

		PDBAtom * CAlphaRenderer::GetSelectedAtom(unsigned int selectionId) {
			int count = 0;
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				if(i->second.GetSelected()) {
					if(count == selectionId) {
						return &i->second;
					}
					count++;
				}
			}
			return NULL;
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

		int CAlphaRenderer::GetSideChainBondIndex(unsigned long long atom0, unsigned long long atom1) {
			for(unsigned int i = 0; i < sidechainBonds.size(); i++) {
				if(((sidechainBonds[i].GetAtom0Ix() == atom0) && (sidechainBonds[i].GetAtom1Ix() == atom1)) ||
							((sidechainBonds[i].GetAtom0Ix() == atom1) && (sidechainBonds[i].GetAtom1Ix() == atom0))) {
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

		int CAlphaRenderer::GetSideChainBondCount() {
			return sidechainBonds.size();
		}

		void CAlphaRenderer::DeleteAtom(unsigned long long index) {
			atoms.erase(atoms.find(index));
		}

		void CAlphaRenderer::DeleteBond(int index) {
			bonds.erase(bonds.begin() + index);
		}

		void CAlphaRenderer::DeleteSideChainBond(int index) {
			sidechainBonds.erase(sidechainBonds.begin() + index);
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

		void CAlphaRenderer::TransformAllAtomLocations(MatrixFloat transform) {
			for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {					
				i->second.Transform(transform);
			}	
		}

		//the following code is used to render ribbon diagrams
		int CAlphaRenderer::StartHelix() {
			aHelices.push_back(Secel());
			return aHelices.size() - 1;
		}

		void CAlphaRenderer::AddHelixElement(int index, unsigned long long hashKey){
			aHelices[index].atomHashes.push_back(hashKey);
		}

		int CAlphaRenderer::StartStrand() {
			bStrands.push_back(Secel());
			return bStrands.size() - 1;
		}

		void CAlphaRenderer::AddStrandElement(int index, unsigned long long hashKey){
			bStrands[index].atomHashes.push_back(hashKey);
		}
		
		int CAlphaRenderer::StartLoop() {
			loops.push_back(Secel());
			return loops.size() - 1;
		}

		void CAlphaRenderer::AddLoopElement(int index, unsigned long long hashKey){
			loops[index].atomHashes.push_back(hashKey);
		}

		bool CAlphaRenderer::CleanSecondaryStructures(){
			aHelices.clear();
			bStrands.clear();
			loops.clear();
			return true;
		}

		//end ribbon diagram code


		void CAlphaRenderer::SetHelixCorrs(  vector < int > flatCorrespondences){
			if(flatCorrespondences.size() %2 != 0)
				return;
			else
				corrs.clear();
			for(int i=0; i < flatCorrespondences.size(); i = i+2){
				corrs.push_back(tuple<int, int>(flatCorrespondences[i], flatCorrespondences[i+1]));
			}
		}

		void CAlphaRenderer::SetFeatureVecs(vector<Vector3DFloat> flatFeatureVecs){
			if(flatFeatureVecs.size() %2 != 0)
				return;
			else
				featureVecs.clear();
			for(int i=0; i < flatFeatureVecs.size(); i = i+2){
				featureVecs.push_back(tuple<Vector3DFloat, Vector3DFloat>(flatFeatureVecs[i], flatFeatureVecs[i+1]));
			}

		}
		void CAlphaRenderer::SetSelectedSSEHelices(vector<int> indices){
			selectedSSEHelices.clear();
			selectedSSEHelices = indices;
		}

		
	}
}


#endif
