/*
A data structure to store atom information
Author: Sasakthi S. Abeysinghe
Date  : 01/28/2006
*/

#ifndef GRAPH_MATCH_PDB_ATOM_H
#define GRAPH_MATCH_PDB_ATOM_H

#include <MathTools/Vector3D.h>
#include <Foundation/StringUtils.h>
#include <string>

using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace GraphMatch {
		static const int ATOM_ROLE_COUNT = 44;
		static const string ATOM_ROLE_NAMES[ATOM_ROLE_COUNT] = {
			"C",    "CA",   "CB",   "CD",   "CD1",  "CD2",  "CE",   "CE1",  "CE2",  "CE3",
			"CH",   "CH1",  "CH2",  "CG",   "CG1",  "CG2",  "CZ",   "CZ1",  "CZ2",  "CZ3",  
			"N",    "ND1",  "ND2",  "NE",   "NE1",  "NE2",  "NH",   "NH1",  "NH2",  "NZ",   
			"O",    "OD",   "OD1",  "OD2",  "OE",   "OE1",  "OE2",  "OG",   "OG1",  "OG2",  
			"OH",   "OXT",  "SD",   "SG"
		};

		class PDBAtom {
		public:
			PDBAtom();
			PDBAtom(string pdbId, char chainId, unsigned int resSeq, string name);
			PDBAtom(string PDBLine);
			void Print();

			string			GetPDBId();
			unsigned int	GetSerial();
			string			GetName();
			char			GetAltLoc();
			string			GetResName();
			char			GetChainId();
			unsigned int	GetResSeq();
			char			GetICode();
			Vector3DFloat	GetPosition();
			float			GetOccupancy();
			float			GetTempFactor();
			string			GetElement();
			string			GetCharge();		
			float			GetColorA();
			float			GetColorR();
			float			GetColorG();
			float			GetColorB();
			float			GetAtomRadius();
			bool			GetSelected();
			unsigned long long	GetHashKey();
			static unsigned long long ConstructHashKey(string pdbId, char chainId, unsigned int resSeq, string name);

			void SetSerial(unsigned int serial);
			void SetName(string	name);
			void SetAltLoc(char altLoc);
			void SetResName(string resName);
			void SetChainId(char chainId);
			void SetResSeq(unsigned int resSeq);
			void SetICode(char iCode);
			void SetPosition(Vector3DFloat position);
			void SetOccupancy(float occupancy);
			void SetTempFactor(float tempFactor);
			void SetElement(string element);
			void SetCharge(string charge);
			void SetColor(float r, float g, float b, float a);
			void SetAtomRadius(float radius);
			void SetSelected(bool selected);
		private:
			static unsigned long long GetCharIndex(char c);
			static unsigned long long GetPDBIdIndex(string pdbId);
			static unsigned long long GetChainIdIndex(char chainId);
			static unsigned long long GetAtomTypeIndex(string atomType);

		private:
			string			pdbId;
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
			float			atomRadius;
			float			colorR;
			float			colorG;
			float			colorB;
			float			colorA;
			bool			selected;
		};

		PDBAtom::PDBAtom() {
			this->pdbId = "";
			this->serial = 0;
			this->name = "";
			altLoc = ' ';
			resName = "   ";
			this->chainId = '0';
			this->resSeq = 0;
			iCode = ' ';
			position = Vector3DFloat(0,0,0);
			occupancy = 0;
			tempFactor = 0;
			element = "  ";
			charge = "  ";
			atomRadius = 1;
			colorR = 0.66f;
			colorG = 0.66f;
			colorB = 0.0f;
			colorA = 1.0f;
			selected = false;
		}

		PDBAtom::PDBAtom(string pdbId, char chainId, unsigned int resSeq, string name) {
			this->pdbId = pdbId;
			this->serial = 0;
			this->name = name;
			altLoc = ' ';
			resName = "   ";
			this->chainId = chainId;
			this->resSeq = resSeq;
			iCode = ' ';
			position = Vector3DFloat(0,0,0);
			occupancy = 0;
			tempFactor = 0;
			element = "  ";
			charge = "  ";
			atomRadius = 1;
			colorR = 0.66f;
			colorG = 0.66f;
			colorB = 0.0f;
			colorA = 1.0f;
			selected = false;
		}

		PDBAtom::PDBAtom(string PDBLine) {
			pdbId = "----";
			serial = atoi((char *)PDBLine.substr(6, 5).c_str());
			name = PDBLine.substr(12, 4);
			LeftTrim(name, string(" \t\f\v\n\r"));
			RightTrim(name, string(" \t\f\v\n\r"));
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
			atomRadius = 1;
			colorR = 0.66f;
			colorG = 0.66f;
			colorB = 0.0f;
			colorA = 1.0f;
			selected = false;
		}

		void PDBAtom::Print() {
			printf("%d) %s - {%f, %f, %f}\n", serial, name.c_str(), position.X(), position.Y(), position.Z());
		}


		string PDBAtom::GetPDBId() {
			return pdbId;
		}

		unsigned int PDBAtom::GetSerial() {
			return serial;
		}

		string PDBAtom::GetName() {
			return name;
		}

		char PDBAtom::GetAltLoc() {
			return altLoc;
		}

		string PDBAtom::GetResName() {
			return resName;
		}

		char PDBAtom::GetChainId() {
			return chainId;
		}

		unsigned int PDBAtom::GetResSeq() {
			return resSeq;
		}

		char PDBAtom::GetICode() {
			return iCode;
		}

		Vector3DFloat PDBAtom::GetPosition(){
			return position;
		}

		float PDBAtom::GetOccupancy() {
			return occupancy;
		}

		float PDBAtom::GetTempFactor() {
			return tempFactor;
		}

		string PDBAtom::GetElement() {
			return element;
		}

		string PDBAtom::GetCharge() {
			return charge;
		}

		float PDBAtom::GetColorA(){
			return colorA;
		}

		float PDBAtom::GetColorR(){
			return colorR;
		}

		float PDBAtom::GetColorG(){
			return colorG;
		}

		float PDBAtom::GetColorB(){
			return colorB;
		}

		float PDBAtom::GetAtomRadius(){
			return atomRadius;
		}

		bool PDBAtom::GetSelected() {
			return selected;
		}

		unsigned long long PDBAtom::GetCharIndex(char c) {
			unsigned long long value;
			if((c >= 48) && (c <= 57)) {		// 0..9
				value = (unsigned long long)c - 48;
			} else if ((c >= 65) && (c <= 90))  {  // A..Z
				value = (unsigned long long)c - 55;
			} else if ((c >= 97) && (c <= 122))  {  // a..z  (same as A..Z)
				value = (unsigned long long)c - 87;
			} else {
				value = 36;
			}
			return value;
		}

		unsigned long long PDBAtom::GetPDBIdIndex(string pdbId) {
			return GetCharIndex(pdbId.c_str()[0]) * 37*37*37 + 
				GetCharIndex(pdbId.c_str()[1]) * 37*37 + 
				GetCharIndex(pdbId.c_str()[2]) * 37 + 
				GetCharIndex(pdbId.c_str()[3]);
		}

		unsigned long long PDBAtom::GetChainIdIndex(char chainId) {
			return GetCharIndex(chainId);
		}

		unsigned long long PDBAtom::GetAtomTypeIndex(string atomType) {
			unsigned long long ix = ATOM_ROLE_COUNT;
			for(unsigned long long i = 0; i < ATOM_ROLE_COUNT; i++) {
				if(atomType.compare(ATOM_ROLE_NAMES[i]) == 0) {
					ix = i;
					break;
				}
			}

			if(ix == ATOM_ROLE_COUNT) {
				printf("Atom role [%s] not defined.. inaccurate hashing will occur!\n", (char *)atomType.c_str());
			}
			return ix;
		}	
		unsigned long long PDBAtom::GetHashKey() {
			return ConstructHashKey(pdbId, chainId, resSeq, name);
		}

		unsigned long long PDBAtom::ConstructHashKey(string pdbId, char chainId, unsigned int resSeq, string name) {
			unsigned long long chainIDCount = 37;
			unsigned long long residueNumCount = 10000;
			unsigned long long atomTypeCount = ATOM_ROLE_COUNT + 1;

			return GetPDBIdIndex(pdbId) * chainIDCount * residueNumCount * atomTypeCount + 
				GetChainIdIndex(chainId) * residueNumCount * atomTypeCount +
				(unsigned long long)resSeq * atomTypeCount +
				GetAtomTypeIndex(name);
		}

		void PDBAtom::SetSerial(unsigned int serial){
			this->serial = serial;
		}

		void PDBAtom::SetName(string name){
			this->name = name;
		}

		void PDBAtom::SetAltLoc(char altLoc){
			this->altLoc = altLoc;
		}

		void PDBAtom::SetResName(string resName){
			this->resName = resName;
		}

		void PDBAtom::SetChainId(char chainId){
			this->chainId = chainId;
		}

		void PDBAtom::SetResSeq(unsigned int resSeq){
			this->resSeq = resSeq;
		}

		void PDBAtom::SetICode(char iCode){
			this->iCode = iCode;
		}

		void PDBAtom::SetPosition(Vector3DFloat position){
			this->position = position;
		}

		void PDBAtom::SetOccupancy(float occupancy){
			this->occupancy = occupancy;
		}

		void PDBAtom::SetTempFactor(float tempFactor){
			this->tempFactor = tempFactor;
		}

		void PDBAtom::SetElement(string element){
			this->element = element;
		}

		void PDBAtom::SetCharge(string charge){
			this->charge = charge;
		}

		void PDBAtom::SetColor(float r, float g, float b, float a) {
			colorR = r;
			colorG = g;
			colorB = b;
			colorA = a;
		}

		void PDBAtom::SetAtomRadius(float radius){
			atomRadius = radius;
		}

		void PDBAtom::SetSelected(bool selected) {
			this->selected = selected;
		}

	}
}

#endif