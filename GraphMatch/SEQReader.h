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
#include <fstream>
#include <cstdlib>
#include <Foundation/StringUtils.h>
#define DEBUG
#ifdef DEBUG
	#include <iostream>
#endif


using namespace std;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace GraphMatch {
		typedef StandardGraph GraphType;
		
		class SEQReader {
		public:
			static StandardGraph * ReadFile(char* fileName);
		};
		
		
		StandardGraph * SEQReader::ReadFile(char* fileName)
		{
			#ifdef DEBUG
			cout << "In SEQReader::ReadFile" << endl;
			#endif
			//Reading the file for start residue # (possibly), sequence, and predicted SSEs
			ifstream fin(fileName);
			string str;
			getline(fin, str);
			int startResNum = 1;	
			if (str.compare(0,5,"START") || str.compare(0,5, "start"))	{
				size_t found;
				found = str.find(" ");
				string startResNumString;
				startResNumString = str.substr(int(found) + 1);
				startResNum = atoi(startResNumString.c_str());
				str.clear();
			}
			
			string tempStr	;
			while (!fin.eof())
			{
				getline(fin, tempStr);
				str.append(tempStr);
			}
			int strLength = str.length();
			if (strLength % 2 == 1) 
			{
				cout << "Odd number of characters!" << endl;
				return NULL;
			}
			string sequence = str.substr(0,strLength/2);
			string predictedSSEs = str.substr(strLength/2);
			
			#ifdef DEBUG
				cout << "\nStart residue: " << startResNum << endl;
				cout << "\nSequence:\n" << sequence << endl;
				cout << "\nPredicted SSEs:\n" << predictedSSEs << endl;
			#endif
			if (sequence.length() != predictedSSEs.length()) return NULL;
			fin.close();
		
			
			//Interpreting strings or sequence and predicted SSEs
			const char coilChar = '-';
			const char helixChar = 'H';
			const char strandChar = 'E';
			char currentChar = predictedSSEs[0];
			int startIndex, stopIndex, length;
			char ch;
			string substring;
			vector<SecondaryStructure*> structures;
			SecondaryStructure * currentStructure;
			bool add;
			unsigned int idNum = 1;
		
			if (currentChar == helixChar || currentChar == strandChar)
				startIndex = 0;
			for (unsigned int i = 1; i < predictedSSEs.length(); i++)
			{
				ch = predictedSSEs[i];
				if (ch != currentChar)
				{
					stopIndex = i - 1;
					length = stopIndex - startIndex + 1;
#ifdef DEBUG 
cout << "error here?" << endl;
#endif
					substring = sequence.substr(startIndex, length);
#ifdef DEBUG 
cout << "error here?" << endl;
#endif
					if (currentChar == helixChar)
					{
						currentStructure = new SecondaryStructure();			
						currentStructure->serialNumber = idNum;
						currentStructure->secondaryStructureID = (char * )(StringUtils::IntToString(idNum)).c_str();
						idNum++;
						currentStructure->startPosition = startIndex+1;
						currentStructure->endPosition = stopIndex+1;
						currentStructure->secondaryStructureType = GRAPHEDGE_HELIX;
						add = true;
#ifdef DEBUG 
cout << "error here?" << endl;
#endif
						for(unsigned int i = 0; i < structures.size(); i++) {
							add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
								(currentStructure->endPosition == structures[i]->endPosition));
						}
						
						#ifdef DEBUG
							cout << "\nHelix(" << startIndex+1 << ',' << stopIndex+1 << "):" << substring << endl;
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
					startIndex = i;
					currentChar = ch;
				}
			}
			substring = sequence.substr(startIndex);
			if (currentChar == helixChar) 
			{
				currentStructure = new SecondaryStructure();
				currentStructure->serialNumber = idNum;
				currentStructure->secondaryStructureID = (char * )(StringUtils::IntToString(idNum)).c_str();
				idNum++;
				currentStructure->startPosition = startIndex+1;
				currentStructure->endPosition = stopIndex+1;
				currentStructure->secondaryStructureType = GRAPHEDGE_HELIX;
				add = true;
				for(unsigned int i = 0; i < structures.size(); i++) {
					add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
						(currentStructure->endPosition == structures[i]->endPosition));
				}
				
				#ifdef DEBUG
					cout << "\nHelix(" << startIndex+1 << ',' << predictedSSEs.length() << "):" << substring << endl;
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
			
		
			//**********************************************************************************************
			//Now it's copying and pasting from SEQReader.h
			//**********************************************************************************************
		
		
			// Sorting the structures by the start position
			int i,j;
			for(i = 0; i < (int)structures.size()-1; i++)	{
				for(j = i+1; j < (int)structures.size(); j++) {
					if(structures[i]->startPosition > structures[j]->startPosition) {
						currentStructure = structures[i];
						structures[i] = structures[j];
						structures[j] = currentStructure;
					}
				}
			}
		
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
				graph->SetCost(i*2+1, i*2+2, structures[i]->GetLength()); 
				graph->SetType(i*2+1, i*2+2, structures[i]->secondaryStructureType); 
				graph->SetCost(i*2+2, i*2+1, structures[i]->GetLength()); 
				graph->SetType(i*2+2, i*2+1, structures[i]->secondaryStructureType); 
		
				if(i != 0) {
					// An edge for the loop before the helix
					graph->SetCost(i*2, i*2+1, structures[i]->startPosition - structures[i-1]->endPosition);
					graph->SetType(i*2, i*2+1, GRAPHEDGE_LOOP);
					graph->SetCost(i*2+1, i*2, structures[i]->startPosition - structures[i-1]->endPosition);
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