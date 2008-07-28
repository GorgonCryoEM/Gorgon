#ifndef SECONDARYSTRUCTURE_H
#define SECONDARYSTRUCTURE_H

#include <string>

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
			int GetLength();
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

		int SecondaryStructure::GetLength() {
			return endPosition - startPosition + 1;
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