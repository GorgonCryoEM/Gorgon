// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Storage element for a Secondary structure element

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.5.2.1  2009/11/05 17:28:33  schuhs
//   Add a method to return the number of bonds in a structure
//
//   Revision 1.5  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.4  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//

#ifndef SECONDARYSTRUCTURE_H
#define SECONDARYSTRUCTURE_H

#include <string>
#include "GlobalConstants.h"

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class SecondaryStructure {
		public:
			int secondaryStructureType;
			int serialNumber;
			int startPosition;
			int endPosition;
			char * secondaryStructureID;

		public:
			SecondaryStructure();
			~SecondaryStructure();
			bool IsHelix();
			bool IsSheet();
			int GetLengthResidues();
			int GetLengthBonds();
			float GetLengthAngstroms();
			int GetSerialNumber();
			int GetStartPosition();
			int GetEndPosition();
			string GetSecondaryStructureID();
		};

		SecondaryStructure::SecondaryStructure() {
			secondaryStructureID = NULL;
		}

		SecondaryStructure::~SecondaryStructure() {
			if(secondaryStructureID != NULL) {
				delete secondaryStructureID;
			}
		}

		bool SecondaryStructure::IsHelix() {
			return (secondaryStructureType == GRAPHEDGE_HELIX);
		}

		bool SecondaryStructure::IsSheet() {
			return (secondaryStructureType == GRAPHEDGE_SHEET);
		}

		int SecondaryStructure::GetLengthResidues() {
			return (endPosition - startPosition + 1);
		}

		int SecondaryStructure::GetLengthBonds() {
			return (endPosition - startPosition);
		}

		float SecondaryStructure::GetLengthAngstroms() {
			if(IsHelix()) {
				return (float)(endPosition - startPosition + 1) * HELIX_C_ALPHA_TO_ANGSTROMS;
			} else {
				return (float)(endPosition - startPosition + 1) * LOOP_C_ALPHA_TO_ANGSTROMS;
			}
		}

		int SecondaryStructure::GetSerialNumber() {
			return serialNumber;
		}

		int SecondaryStructure::GetStartPosition() {
			return startPosition;
		}

		int SecondaryStructure::GetEndPosition() {
			return endPosition;
		}

		string SecondaryStructure::GetSecondaryStructureID() {
			return secondaryStructureID;
		}
	}
}

#endif