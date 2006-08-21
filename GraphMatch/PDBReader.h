/*
Reads PDB files and creates Graphs from them.

Author: Sasakthi S. Abeysinghe
Date  : 01/28/2006
*/

#ifndef PDBREADER_H
#define PDBREADER_H

#include <vector>
#include "SecondaryStructure.h"
#include "SheetStrand.h"
#include "GlobalConstants.h"


using namespace std;

class PDBReader {
public:
	static StandardGraph * ReadFile(char * fname);
	static char * TrimString(char * string);
	static int ToInt(char * string);
private:
	static char * GetString(char * string, int start, int length);
	static int GetInt(char * string, int start, int length);
};

StandardGraph * PDBReader::ReadFile(char* fname) {
	FILE* fin = fopen(fname, "rt");
	if (fin == NULL)
	{
		printf("Error reading input file %s.\n", fname) ;
		exit(0) ;
	}

	char line[100];
	char * token;
	vector<SecondaryStructure*> structures;
	SecondaryStructure * currentStructure;
	bool add;

	while(!feof(fin))
	{
		fgets(line, 100, fin);
		token = GetString(line, 0, 6);

		if(strcmp(token, TOKEN_PDB_HELIX)== 0) {
			currentStructure = new SecondaryStructure();
			currentStructure->serialNumber = GetInt(line, 7, 3);
			currentStructure->secondaryStructureID = GetString(line, 11, 3);
			currentStructure->startPosition = GetInt(line, 21, 4);
			currentStructure->endPosition = GetInt(line, 33, 4);
			currentStructure->secondaryStructureType = GRAPHEDGE_HELIX;
			add = true;
			for(int i = 0; i < (int)structures.size(); i++)	{
				add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
					(currentStructure->endPosition == structures[i]->endPosition));
			}
			if(add) {
				structures.push_back(currentStructure);		
			} else {
				delete currentStructure;
			}
#ifdef INCLUDE_SHEETS
		} else if (strcmp(token, TOKEN_PDB_SHEET)== 0) {
			currentStructure = new SecondaryStructure();
			currentStructure->serialNumber = GetInt(line, 7, 3);
			currentStructure->secondaryStructureID = GetString(line, 11, 3);
			currentStructure->startPosition = GetInt(line, 22, 4);
			currentStructure->endPosition = GetInt(line, 33, 4);
			currentStructure->secondaryStructureType = GRAPHEDGE_SHEET;
			add = true;
			for(int i = 0; i < (int)structures.size(); i++)	{
				add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
					(currentStructure->endPosition == structures[i]->endPosition));
			}
			if(add) {
				structures.push_back(currentStructure);	
			} else {
				delete currentStructure;
			}
#endif
		}
		delete token;
	}
	fclose( fin ) ;

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
		exit(0);
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

char * PDBReader::TrimString(char * string) {
	int startPos = 0;
	int endPos = strlen(string) - 1;
	for(int i = 0; i < endPos; i++) {
		if(string[startPos] != ' ') {
			break;
		}
		startPos++;
	}
	for(int i = endPos - 1; i >= startPos; i--) {
		if(string[endPos] != ' ') {
			break;
		}
		endPos--;
	}
	int j = 0;
	char * outString = new char[100];
	for(int i = startPos; i <= endPos; i++) {
		outString[j] = string[i];
		j++;
	}
	outString[j] = 0;
	return outString;
}

char * PDBReader::GetString(char * string, int start, int length) {
	char * out = new char[100];
	char * temp;
	for(int i = 0; i < length; i++) {
		out[i] = string[i+start];
	}
	out[length] = 0;
	temp = TrimString(out);
	
	// clean
	delete out;
	return temp;
}

int PDBReader::ToInt(char * string) {
	int value = 0;
	int sign = 1;
	for(int i = 0; i < (int)strlen(string); i++) {
		if(string[i] == '-') {
			sign = -1;
		} else {
			value += sign * (string[i]-48);
			value *= 10;
		}
	}
	value /= 10;
	return value;
}
int PDBReader::GetInt(char * string, int start, int length) {
	char * substring = GetString(string, start, length);
	int value = ToInt(substring);
	
	// clean
	delete substring;
	return value;
}
#endif