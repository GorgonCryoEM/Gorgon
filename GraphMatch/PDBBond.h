/*
A data structure to store bond information
Author: Sasakthi S. Abeysinghe
Date  : 10/16/2008
*/

#ifndef GRAPH_MATCH_PDB_BOND_H
#define GRAPH_MATCH_PDB_BOND_H

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace GraphMatch {
		class PDBBond {
		public:
			PDBBond();
			PDBBond(int atom0Index, int atom1Index, bool selected=false);
		public:	
			int GetAtom0Ix();
			int GetAtom1Ix();
			bool GetSelected();
			void SetAtom0Ix(int ix);
			void SetAtom1Ix(int ix);
			void SetSelected(bool selected);
		private:
			int atom0Ix;
			int atom1Ix;
			bool selected;
		};

		PDBBond::PDBBond() {
			this->atom0Ix = -1;
			this->atom1Ix = -1;
			this->selected = false;
		}

		PDBBond::PDBBond(int atom0Index, int atom1Index, bool selected) {
			this->atom0Ix = atom0Index;
			this->atom1Ix = atom1Index;
			this->selected = selected;
		}

		int PDBBond::GetAtom0Ix() {
			return atom0Ix;
		}

		int PDBBond::GetAtom1Ix() {
			return atom1Ix;
		}

		bool PDBBond::GetSelected() {
			return selected;
		}

		void PDBBond::SetAtom0Ix(int ix) {
			atom0Ix = ix;
		}

		void PDBBond::SetAtom1Ix(int ix){
			atom1Ix = ix;
		}

		void PDBBond::SetSelected(bool selected) {
			this->selected = selected;
		}

	}
}
#endif