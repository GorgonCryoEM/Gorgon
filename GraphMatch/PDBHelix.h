// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A data structure to store PDB helix information




#ifndef GRAPH_MATCH_PDB_HELIX_H
#define GRAPH_MATCH_PDB_HELIX_H

#include <MathTools/Vector3D.h>
#include <Foundation/StringUtils.h>
#include <string>

using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;
using namespace std;

namespace wustl_mm {
	namespace GraphMatch {
		class PDBHelix {			
		public:
			PDBHelix(string PDBLine);
			int GetSerialNo();
			string GetHelixId();
			string GetInitialResidueName();
			char GetInitialResidueChainId();
			int GetInitialResidueSeqNo();
			char GetInitialResidueICode();
			string GetEndResidueName();
			char GetEndResidueChainId();
			int GetEndResidueSeqNo();
			char GetEndResidueICode();
			int GetHelixClass();
			string GetComment();
			int GetLength();
			Vector3DFloat GetEndPosition(int ix);
			void SetEndPositions(Vector3DFloat p1, Vector3DFloat p2);
			void Print();
		private:
			int serialNo;
			string helixId;
			string initialResidueName;
			char initialResidueChainId;
			int initialResidueSeqNo;
			char initialResidueICode;
			string endResidueName;
			char endResidueChainId;
			int endResidueSeqNo;
			char endResidueICode;
			int helixClass;
			string comment;
			int length;
			Vector3DFloat endPositions[2];
		};

		PDBHelix::PDBHelix(string PDBLine) {			
			serialNo = atoi((char *)PDBLine.substr(7, 3).c_str());
			helixId = PDBLine.substr(11, 3);
			initialResidueName = PDBLine.substr(15, 3);
			initialResidueChainId = PDBLine[19];
			initialResidueSeqNo = atoi((char *)PDBLine.substr(21, 4).c_str());
			initialResidueICode = PDBLine[25];
			endResidueName = PDBLine.substr(27, 3);
			endResidueChainId = PDBLine[31];
			endResidueSeqNo = atoi((char *)PDBLine.substr(33, 4).c_str());
			endResidueICode = PDBLine[37];
			helixClass = atoi((char *)PDBLine.substr(38, 2).c_str());
			comment = PDBLine.substr(40, 30);
			length = atoi((char *)PDBLine.substr(71, 5).c_str());
		}

		int PDBHelix::GetSerialNo() {
			return serialNo;
		}

		string PDBHelix::GetHelixId() {
			return helixId;
		}

		string PDBHelix::GetInitialResidueName() {
			return initialResidueName;
		}

		char PDBHelix::GetInitialResidueChainId() {
			return initialResidueChainId;
		}

		int PDBHelix::GetInitialResidueSeqNo() {
			return initialResidueSeqNo;
		}

		char PDBHelix::GetInitialResidueICode() {
			return initialResidueICode;
		}

		string PDBHelix::GetEndResidueName() {
			return endResidueName;
		}

		char PDBHelix::GetEndResidueChainId() {
			return endResidueChainId;
		}

		int PDBHelix::GetEndResidueSeqNo() {
			return endResidueSeqNo;
		}

		char PDBHelix::GetEndResidueICode() {
			return endResidueICode;
		}

		int PDBHelix::GetHelixClass() {
			return helixClass;
		}

		string PDBHelix::GetComment() {
			return comment;
		}

		int PDBHelix::GetLength() {
			return length;
		}

		Vector3DFloat PDBHelix::GetEndPosition(int ix) {
			return endPositions[ix];
		}

		void PDBHelix::SetEndPositions(Vector3DFloat p1, Vector3DFloat p2) {
			endPositions[0] = p1;
			endPositions[1] = p2;
		}

		void PDBHelix::Print() {
			printf("Helix Information:\n");
			printf("\tSerial No: [%d]\n", serialNo);
			printf("\tHelix Id: [%s]\n", (char *)helixId.c_str());
			printf("\tInitial res Name: [%s]\n", (char *)initialResidueName.c_str());
			printf("\tinitialResidueChainId: [%c]\n", initialResidueChainId);
			printf("\tInitialResidueSeqNo: [%d]\n", initialResidueSeqNo);			
			printf("\tinitialResidueICode: [%c]\n", initialResidueICode);
			printf("\tEnd res Name: [%s]\n", (char *)endResidueName.c_str());
			printf("\tendResidueChainId: [%c]\n", endResidueChainId);
			printf("\tEndResidueSeqNo: [%d]\n", endResidueSeqNo);
			printf("\tendResidueICode: [%c]\n", endResidueICode);
			printf("\thelixClass: [%d]\n", helixClass);
			printf("\tComment: [%s]\n", (char *)comment.c_str());			
			printf("\tLength: [%d]\n", length);
			printf("\tEndPositions: {%f, %f, %f}, {%f, %f, %f}\n", endPositions[0].X(), endPositions[0].Y(), endPositions[0].Z(), endPositions[1].X(), endPositions[1].Y(), endPositions[1].Z());
		}
	}
}

#endif
