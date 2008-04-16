/*
A data structure to store atom information
Author: Sasakthi S. Abeysinghe
Date  : 01/28/2006
*/

#ifndef GRAPH_MATCH_PDB_ATOM_H
#define GRAPH_MATCH_PDB_ATOM_H

#include <MathTools/Vector3D.h>
#include <string>

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace GraphMatch {
		class PDBAtom {
		public:
			PDBAtom(string PDBLine);
			void Print();
		public:
			unsigned int	serial;
			string			name;
			char			altLoc;
			string			resName;
			char			chainId;
			unsigned int	resSeq;
			char			iCode;
			Vector3DFloat	position;
			float			occupancy;
			float			tempFactor;
			string			element;
			string			charge;
		};

		PDBAtom::PDBAtom(string PDBLine) {
			serial = atoi((char *)PDBLine.substr(6, 5).c_str());
			name = PDBLine.substr(12, 4);
			altLoc = PDBLine[16];
			resName = PDBLine.substr(17, 3);
			chainId = PDBLine[21];
			resSeq = atoi((char *)PDBLine.substr(22, 4).c_str());
			iCode = PDBLine[26];
			position = Vector3DFloat((float)atof((char *)PDBLine.substr(30, 8).c_str()),
									(float)atof((char *)PDBLine.substr(38, 8).c_str()),
									(float)atof((char *)PDBLine.substr(46, 8).c_str()));
			occupancy = (float)atof((char *)PDBLine.substr(54, 6).c_str());
			tempFactor = (float)atof((char *)PDBLine.substr(60, 6).c_str());
			element = PDBLine.substr(76, 2);
			charge = PDBLine.substr(78, 2);
		}

		void PDBAtom::Print() {
			printf("%d) %s - {%f, %f, %f}\n", serial, name.c_str(), position.X(), position.Y(), position.Z());
		}
	}
}

#endif