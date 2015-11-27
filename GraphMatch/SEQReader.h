// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Ross Coleman (racolema@bcm.edu)
// Description:   Reads SEQ Files



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
			//const unsigned short minHelixLength = 6;
			const unsigned short minHelixLength = 0;
			const char strandChar = 'E';
			const unsigned short minStrandLength = 0;
			char currentChar = predictedSSEs[0];
			int startCharNum = 0;
			int stopCharNum = 0;
			int length = 0;
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
					else if (currentChar == strandChar && length >= minStrandLength)
					{
						currentStructure = new SecondaryStructure();			
						currentStructure->serialNumber = idNum;
						stringstream ssOut;
						ssOut << idNum;
						sseID = "S" + ssOut.str();
						unsigned int sseIDSize = sseID.size();
						char * cSseID = new char [sseIDSize + 1];
						strLen = sseID.copy(cSseID, sseIDSize);
						cSseID[strLen] = '\0';
						currentStructure->secondaryStructureID = cSseID;
						idNum++;
						currentStructure->startPosition = startCharNum + startResNum;
						currentStructure->endPosition = stopCharNum + startResNum;
						currentStructure->secondaryStructureType = GRAPHEDGE_SHEET;
						add = true;
						for(unsigned int i = 0; i < structures.size(); i++) {
							add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
								(currentStructure->endPosition == structures[i]->endPosition));
						}
						
						#ifdef DEBUG
							cout << "\nStrand(" << startCharNum+startResNum << ',' << stopCharNum + startResNum << "):" << substring << endl;
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
			else if (currentChar == strandChar) 
			{
				currentStructure = new SecondaryStructure();
				currentStructure->serialNumber = idNum;
				stringstream ssOut;
				ssOut << idNum;
				sseID = "S" + ssOut.str();
				unsigned int sseIDSize = sseID.size();
				char * cSseID = new char [sseIDSize + 1];
				strLen = sseID.copy(cSseID, sseIDSize);
				cSseID[strLen] = '\0';
				currentStructure->secondaryStructureID = cSseID;
				idNum++;
				currentStructure->startPosition = startCharNum + startResNum;
				currentStructure->endPosition = stopCharNum + startResNum;
				currentStructure->secondaryStructureType = GRAPHEDGE_SHEET;
				add = true;
				for(unsigned int i = 0; i < structures.size(); i++) {
					add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
						(currentStructure->endPosition == structures[i]->endPosition));
				}
				
				#ifdef DEBUG
					cout << "\nStrand(" << startCharNum+startResNum << ',' << stopCharNum + startResNum << "):" << substring << endl;
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
		
			
			SecondaryStructure * currentStructure;
		
			if(structures.size() == 0){
				printf("No helixes or sheets found... Unable to perform matching");
				return NULL;
			}

			// count number of helices
			int numHelices = 0;
			int numSheets = 0;
			vector<string> strandLabels;
			for(int i = 0; i < (int)structures.size(); i++) {
				currentStructure = structures[i];
				if (currentStructure->secondaryStructureType == GRAPHEDGE_HELIX) {
					numHelices++;
				} else	if (currentStructure->secondaryStructureType == GRAPHEDGE_SHEET) {
					numSheets++;
				}
			}
#ifdef VERBOSE
			cout << "creating new graph for " << numHelices << " helices and " << numSheets << " strands." << endl;
#endif // VERBOSE

			StandardGraph * graph = new StandardGraph(2 * numHelices + numSheets);

			// Adding the rest of the structures into the graph
			int node = 0;
			for(i = 0; i < (int)structures.size(); i++) {
				if (structures[i]->secondaryStructureType == GRAPHEDGE_HELIX) {
#ifdef VERBOSE
					cout << "adding helix " << i << endl;
#endif // VERBOSE
					graph->SetCost(node+1, node+1, 0); // First helix node.
					graph->SetType(node+1, node+1, GRAPHNODE_HELIX); 
					graph->SetCost(node+2, node+2, 0); // Second helix node.
					graph->SetType(node+2, node+2, GRAPHNODE_HELIX); 

					// An edge for the helix
					graph->SetCost(node+1, node+2, structures[i]->GetLengthResidues()); 
					graph->SetType(node+1, node+2, structures[i]->secondaryStructureType); 
					graph->SetCost(node+2, node+1, structures[i]->GetLengthResidues()); 
					graph->SetType(node+2, node+1, structures[i]->secondaryStructureType); 

					if(i != 0) {
						// An edge for the loop before the helix
						graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition));
						graph->SetType(node, node+1, GRAPHEDGE_LOOP);
						graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition));
						graph->SetType(node+1, node, GRAPHEDGE_LOOP);
					}
					node += 2;
				}

				if (structures[i]->secondaryStructureType == GRAPHEDGE_SHEET) {
#ifdef VERBOSE
					cout << "adding strand " << i << " with ID " << structures[i]->secondaryStructureID << endl;
#endif // VERBOSE
					graph->SetCost(node+1, (structures[i]->endPosition - structures[i]->startPosition) );
#ifdef VERBOSE
					cout << "adding strand " << i << " at node " << node << " with length " << (structures[i]->endPosition - structures[i]->startPosition) * LOOP_C_ALPHA_TO_ANGSTROMS << endl;
#endif // VERBOSE
					graph->SetCost(node+1, node+1, 0); // Strand node.
					graph->SetType(node+1, node+1, GRAPHNODE_SHEET); 

					if(i != 0) {
						// An edge for the loop before the strand
						graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition) );
						graph->SetType(node, node+1, GRAPHEDGE_LOOP);
						graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition) );
						graph->SetType(node+1, node, GRAPHEDGE_LOOP);
					}
					node += 1;
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
