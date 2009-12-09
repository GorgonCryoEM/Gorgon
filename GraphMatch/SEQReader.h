// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Ross Coleman (racolema@bcm.edu)
// Description:   Reads SEQ Files

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.11  2008/11/20 16:02:21  colemanr
//   Fixed the logic on whether there is a START or start line at the
//   beginning, and implemented handling of NULL pointers in error
//   conditions.
//
//   Revision 1.10  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.9  2008/11/17 19:35:50  colemanr
//   defined SEQFileData class, and used it in the functions
//
//   Revision 1.8  2008/11/14 22:18:30  colemanr
//   ReadFile() logic is now broken down into two seperate functions
//
//   Revision 1.7  2008/11/13 00:48:02  colemanr
//   Ignores helices that are shorter than minHelixLength = 6
//
//   Revision 1.6  2008/11/13 00:40:33  colemanr
//   Fixed a memory error.  string::c_str() returns a reference to a string, not a copy, so I am
//   now making a copy using string::copy().  Also, switched from using Foundation/StringUtils.h to
//   using <sstream> to convert integers to strings.
//
//   Revision 1.5  2008/11/12 20:40:10  colemanr
//   fixed some indexing errors
//
//   Revision 1.4  2008/11/10 21:31:44  ssa1
//   Fixing compilation errors
//
//   Revision 1.3  2008/11/10 21:10:28  colemanr
//   updated debugging code to show the line of a runtime error that was
//   occurring
//
//   Revision 1.2  2008/11/10 16:15:22  ssa1
//   Adding in ifdef statements to support compilation
//


#ifndef SEQREADER_H
#define SEQREADER_H

#include <vector>
#include "GlobalConstants.h"
#include "SecondaryStructure.h"
#include "StandardGraph.h"
#include "SheetStrand.h"
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {
		typedef StandardGraph GraphType;
		
		class SEQFileData {
		
		public:
			unsigned int GetStartResNo() { return startResNo; }
			void SetStartResNo(unsigned int Ix) { startResNo = Ix; }
			string GetSequenceString() { return sequenceString; }
			void SetSequenceString(char * str) { sequenceString = str; }
			void SetSequenceString(string str) { sequenceString = str; }
			string GetStructureString() { return structureString; }
			void SetStructureString(char * str) { structureString = str; }
			void SetStructureString(string str) { structureString = str; }
			unsigned int GetNumberOfStructures() { return pStructures->size(); }
			SecondaryStructure* GetStructure(unsigned int Ix) { 
				vector<SecondaryStructure*> & structures = *pStructures;
				return structures[Ix];
			}
			vector<SecondaryStructure*> * GetStructuresVectorPointer() { return pStructures; }
			void SetStructuresPointer(vector<SecondaryStructure*> * pNewStructures) { pStructures = pNewStructures; }
			SEQFileData() {
				startResNo = 1;
				sequenceString = "";
				structureString = "";
				pStructures = NULL;
			}
			SEQFileData(unsigned int stRes, string seqStr, string structStr, vector<SecondaryStructure*> * pStructuresVect) {
				startResNo = stRes;
				sequenceString = seqStr;
				structureString = structStr;
				pStructures = pStructuresVect;
			}
			
		private:
			unsigned int startResNo;
			string sequenceString;
			string structureString;
			vector<SecondaryStructure*> * pStructures;
		
		};
		
		class SEQReader {
		public:
			static StandardGraph * ReadFile(string fileName);
			static SEQFileData ReadSeqFileData(string fileName);
			static StandardGraph * GetGraphFromSeqFileData(SEQFileData seqFData);
		};
		
		
		StandardGraph * SEQReader::ReadFile(string fileName)
		{
			#ifdef DEBUG
				cout << "In SEQReader::ReadFile" << endl;
			#endif
			SEQFileData seqFData = ReadSeqFileData(fileName);
			StandardGraph * pGraph = GetGraphFromSeqFileData(seqFData);
			if (!pGraph) return NULL;
			return pGraph;
		}
		
		
		SEQFileData SEQReader::ReadSeqFileData(string fileName)
		{
			//Reading the file for start residue # (possibly), sequence, and predicted SSEs
			ifstream fin(fileName.c_str());
			string str;
			string tempStr;
			getline(fin, str);
			int startResNum = 1;	
			if (!str.compare(0,5,"START") || !str.compare(0,5, "start"))	{
				#ifdef DEBUG
					cout << "There is a START line at the beginning" << endl;
				#endif
				size_t found;
				found = str.find(" ");
				string startResNumString;
				startResNumString = str.substr(int(found) + 1);
				startResNum = atoi(startResNumString.c_str());
				str.clear();
			}
			
			while (!fin.eof())
			{
				getline(fin, tempStr);
				str.append(tempStr);
			}
			#ifdef DEBUG
				cout << "str: " << str << endl;
			#endif
			int strLength = str.length();
			if (strLength % 2 == 1) 
			{
				cout << "Odd number of characters!" << endl;
				SEQFileData empty;
				return empty;
			}
			string sequence = str.substr(0,strLength/2);
			string predictedSSEs = str.substr(strLength/2);
			
			#ifdef DEBUG
				cout << "\nStart residue: " << startResNum << endl;
				cout << "\nSequence:\n" << sequence << endl;
				cout << "\nPredicted SSEs:\n" << predictedSSEs << endl;
			#endif
			if (sequence.length() != predictedSSEs.length())
			{
				cout << "\nSequence and structure string lengths differ!" << endl;
				SEQFileData empty;
				return empty;
			}
			fin.close();
		
			
			//Interpreting strings or sequence and predicted SSEs
			//const char coilChar = '-';
			const char helixChar = 'H';
			const unsigned short minHelixLength = 6;
			//const char strandChar = 'E';
			char currentChar = predictedSSEs[0];
			unsigned int startCharNum = 0;
			unsigned int stopCharNum = 0;
			unsigned int length = 0;
			char ch;
			string substring;
			string sseID;
			vector<SecondaryStructure*> * pStructures = new vector<SecondaryStructure*>;
			vector<SecondaryStructure*> & structures = *pStructures;
			SecondaryStructure * currentStructure;
			bool add;
			unsigned int idNum = 1;
			int strLen;
		
			for (unsigned int i = 1; i < predictedSSEs.length(); i++)
			{
				ch = predictedSSEs[i];
				if (ch != currentChar)
				{
					stopCharNum = i - 1;
					length = stopCharNum - startCharNum + 1;
					substring = sequence.substr(startCharNum, length);
					
					if (currentChar == helixChar && length >= minHelixLength)
					{
						currentStructure = new SecondaryStructure();			
						currentStructure->serialNumber = idNum;
						stringstream ssOut;
						ssOut << idNum;
						sseID = "H" + ssOut.str();
						unsigned int sseIDSize = sseID.size();
						char * cSseID = new char [sseIDSize + 1];
						strLen = sseID.copy(cSseID, sseIDSize);
						cSseID[strLen] = '\0';
						currentStructure->secondaryStructureID = cSseID;
						idNum++;
						currentStructure->startPosition = startCharNum + startResNum;
						currentStructure->endPosition = stopCharNum + startResNum;
						currentStructure->secondaryStructureType = GRAPHEDGE_HELIX;
						add = true;
						for(unsigned int i = 0; i < structures.size(); i++) {
							add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
								(currentStructure->endPosition == structures[i]->endPosition));
						}
						
						#ifdef DEBUG
							cout << "\nHelix(" << startCharNum+startResNum << ',' << stopCharNum + startResNum << "):" << substring << endl;
							cout << "Structure(" << currentStructure->GetStartPosition() << ',';
							cout << currentStructure->GetEndPosition() << "):";
							cout << " Serial=" << currentStructure->GetSerialNumber(); 
							cout << " ID=" << currentStructure->GetSecondaryStructureID();
							cout << " Type=" << currentStructure->secondaryStructureType << endl;
						#endif
						
						if(add) {
							structures.push_back(currentStructure);		
						}
						else {
							delete currentStructure;
						}
					}		
					startCharNum = i;
					currentChar = ch;
				}
			}
			substring = sequence.substr(startCharNum);
			stopCharNum = predictedSSEs.length() - 1;
			if (currentChar == helixChar) 
			{
				currentStructure = new SecondaryStructure();
				currentStructure->serialNumber = idNum;
				stringstream ssOut;
				ssOut << idNum;
				sseID = "H" + ssOut.str();
				unsigned int sseIDSize = sseID.size();
				char * cSseID = new char [sseIDSize + 1];
				strLen = sseID.copy(cSseID, sseIDSize);
				cSseID[strLen] = '\0';
				currentStructure->secondaryStructureID = cSseID;
				idNum++;
				currentStructure->startPosition = startCharNum + startResNum;
				currentStructure->endPosition = stopCharNum + startResNum;
				currentStructure->secondaryStructureType = GRAPHEDGE_HELIX;
				add = true;
				for(unsigned int i = 0; i < structures.size(); i++) {
					add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
						(currentStructure->endPosition == structures[i]->endPosition));
				}
				
				#ifdef DEBUG
					cout << "\nHelix(" << startCharNum+startResNum << ',' << stopCharNum + startResNum << "):" << substring << endl;
					cout << "Structure(" << currentStructure->GetStartPosition() << ',';
					cout << currentStructure->GetEndPosition() << "):";
					cout << " Serial=" << currentStructure->GetSerialNumber(); 
					cout << " ID=" << currentStructure->GetSecondaryStructureID();
					cout << " Type=" << currentStructure->secondaryStructureType << endl;
				#endif
				
				if(add) {
					structures.push_back(currentStructure);		
				}
				else {
					delete currentStructure;
				}
			}
			
			return SEQFileData(startResNum, sequence, predictedSSEs, pStructures);
		}
		
		
		StandardGraph * SEQReader::GetGraphFromSeqFileData(SEQFileData seqFData)
		{
			vector<SecondaryStructure*> * pStructures = seqFData.GetStructuresVectorPointer();
			if (!pStructures) {
				cout << "Pointer to vector<SecondaryStructure*> object is NULL!" << endl;
				return NULL;
			}
			vector<SecondaryStructure*> & structures = *pStructures;
			int i;
			//**********************************************************************************************
			//Code below is copied and pasted from PDBReader.h
			//**********************************************************************************************
		
			
		
			if(structures.size() == 0){
				printf("No helixes or sheets found... Unable to perform matching");
				return NULL;
			}
		
			StandardGraph * graph = new StandardGraph(2 * structures.size());
		
			// Adding the rest of the structures into the graph
			for(i = 0; i < (int)structures.size(); i++) {
				graph->SetCost(i*2+1, i*2+1, 0); // First helix node.
				graph->SetCost(i*2+2, i*2+2, 0); // Second helix node.
		
				// An edge for the helix
				graph->SetCost(i*2+1, i*2+2, structures[i]->GetLengthAngstroms()); 
				graph->SetType(i*2+1, i*2+2, structures[i]->secondaryStructureType); 
				graph->SetCost(i*2+2, i*2+1, structures[i]->GetLengthAngstroms()); 
				graph->SetType(i*2+2, i*2+1, structures[i]->secondaryStructureType); 
		
				if(i != 0) {
					// An edge for the loop before the helix
					graph->SetCost(i*2, i*2+1, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
					graph->SetType(i*2, i*2+1, GRAPHEDGE_LOOP);
					graph->SetCost(i*2+1, i*2, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
					graph->SetType(i*2+1, i*2, GRAPHEDGE_LOOP);
				}
			}
		
			for(i = 0; i < (int)structures.size(); i++) {
				graph->pdbStructures.push_back(structures[i]);
			}
		
			structures.clear();
		
			return graph;
		
		}
		
		
	}
}

#endif
