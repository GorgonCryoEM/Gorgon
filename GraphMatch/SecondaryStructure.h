// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Storage element for a Secondary structure element

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraphMatch/SecondaryStructure.h,v $
//   $Revision: 1.7 $
//   $Date: 2009/12/22 01:03:06 $
//   $Author: schuhs $
//   $State: Exp $
//
// History Log: 
//   $Log: SecondaryStructure.h,v $
//   Revision 1.7  2009/12/22 01:03:06  schuhs
//   Adding support for beta sheets to the SSE correspondence search algorithm
//
//   Revision 1.6  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
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