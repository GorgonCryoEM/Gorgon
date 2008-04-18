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
			PDBAtom();
			PDBAtom(string PDBLine);
			void Print();

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
		private:
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

		PDBAtom::PDBAtom() {
			serial = 1;
			name = "    ";
			altLoc = ' ';
			resName = "   ";
			chainId = ' ';
			resSeq = 1;
			iCode = ' ';
			position = Vector3DFloat(0,0,0);
			occupancy = 0;
			tempFactor = 0;
			element = "  ";
			charge = "  ";
		}

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

	}
}

#endif