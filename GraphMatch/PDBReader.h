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
#include <string>
#include <MathTools/Vector3D.h>
#include "PDBAtom.h"
#include "StandardGraph.h"

using namespace std;
using namespace wustl_mm::MathTools;


namespace wustl_mm {
	namespace GraphMatch {
		typedef StandardGraph GraphType;
		
		class PDBReader {
		public:
			static StandardGraph * ReadFile(char * fname);
			static vector<PDBAtom> ReadAtomPositions(string fileName);
			static char * TrimString(char * string);
			static int ToInt(char * string);
		private:
			static char * GetString(char * string, int start, int length);
			static int GetInt(char * string, int start, int length);
		};

		#ifdef GET_AMINO_SEQUENCE
		char GetSingleLetterFromThree(char * aminoAcid) {
			char result;
			if(strcmp(aminoAcid, "ALA") == 0) {
				result = 'A';
			} else if(strcmp(aminoAcid, "ARG") == 0) {
				result = 'R';
			} else if(strcmp(aminoAcid, "ASN") == 0) {
				result = 'N';
			} else if(strcmp(aminoAcid, "ASP") == 0) {
				result = 'D';
			} else if(strcmp(aminoAcid, "CYS") == 0) {
				result = 'C';
			} else if(strcmp(aminoAcid, "GLN") == 0) {
				result = 'Q';
			} else if(strcmp(aminoAcid, "GLU") == 0) {
				result = 'E';
			} else if(strcmp(aminoAcid, "GLY") == 0) {
				result = 'G';
			} else if(strcmp(aminoAcid, "HIS") == 0) {
				result = 'H';
			} else if(strcmp(aminoAcid, "ILE") == 0) {
				result = 'I';
			} else if(strcmp(aminoAcid, "LEU") == 0) {
				result = 'L';
			} else if(strcmp(aminoAcid, "LYS") == 0) {
				result = 'K';
			} else if(strcmp(aminoAcid, "MET") == 0) {
				result = 'M';
			} else if(strcmp(aminoAcid, "PHE") == 0) {
				result = 'F';
			} else if(strcmp(aminoAcid, "PRO") == 0) {
				result = 'P';
			} else if(strcmp(aminoAcid, "SER") == 0) {
				result = 'S';
			} else if(strcmp(aminoAcid, "THR") == 0) {
				result = 'T';
			} else if(strcmp(aminoAcid, "TRP") == 0) {
				result = 'W';
			} else if(strcmp(aminoAcid, "TYR") == 0) {
				result = 'Y';
			} else if(strcmp(aminoAcid, "VAL") == 0) {
				result = 'V';
			} else {
				printf("/noops!!!/n");
				result = 'Z';
			}
			return result;
		}
		#endif

		StandardGraph * PDBReader::ReadFile(char* fname) {
			FILE* fin = fopen(fname, "rt");
			if (fin == NULL)
			{
				printf("Error reading input file %s.\n", fname) ;
				exit(0) ;
			}

			char line[100];
			string sequence = "";
			char * token;
			vector<SecondaryStructure*> structures;
			SecondaryStructure * currentStructure;
			bool add;
			int oldIndex = 0;
			int index;
			#ifdef GET_AMINO_SEQUENCE
			char * acidString;
			char acidChar;
			int start = 10000;
			#endif

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
					for(unsigned int i = 0; i < structures.size(); i++)	{
						add = add && !((currentStructure->startPosition == structures[i]->startPosition) && 
							(currentStructure->endPosition == structures[i]->endPosition));
					}
					if(add) {
						structures.push_back(currentStructure);		
					} else {
						delete currentStructure;
					}
				#ifdef GET_AMINO_SEQUENCE
				} else if(strcmp(token, "ATOM") == 0) {
					index = GetInt(line, 22, 4);
					start = min(index, start);
					if(index != oldIndex){
						acidString = GetString(line, 17,3);				
						acidChar = GetSingleLetterFromThree(acidString);
						sequence += acidChar;
						printf("%c", acidChar);
						delete acidString;
					}
					oldIndex = index;
				#endif
				#ifdef INCLUDE_SHEETS
				} else if (strcmp(token, TOKEN_PDB_SHEET)== 0) {
					currentStructure = new SecondaryStructure();
					currentStructure->serialNumber = GetInt(line, 7, 3);
					currentStructure->secondaryStructureID = GetString(line, 11, 3);
					currentStructure->startPosition = GetInt(line, 22, 4);
					currentStructure->endPosition = GetInt(line, 33, 4);
					currentStructure->secondaryStructureType = GRAPHEDGE_SHEET;
					add = true;
					for(unsigned int i = 0; i < structures.size(); i++)	{
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
				delete [] token;
				token = NULL;
			}

			#ifdef GET_AMINO_SEQUENCE
			printf("\n");
			#endif

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


			#ifdef GET_AMINO_SEQUENCE			
			int oldNum = start;
			for(unsigned int i = 0; i < structures.size(); i++) {
				printf("%s [%s] ", 
					sequence.substr(oldNum - start, structures[i]->startPosition - oldNum).c_str(),
					sequence.substr(structures[i]->startPosition - start, structures[i]->endPosition - structures[i]->startPosition + 1).c_str());
				oldNum = structures[i]->endPosition + 1;
			}
			printf("\n%s \n", sequence.c_str());
			#endif


			structures.clear();

			return graph;
		}


		vector<PDBAtom> PDBReader::ReadAtomPositions(string fileName) {
			vector<PDBAtom> atomPositions;

			FILE* fin = fopen((char *)fileName.c_str(), "rt");
			if (fin == NULL)
			{
				printf("Error reading input file %s.\n", fileName.c_str()) ;
				exit(0) ;
			}

			char line[100];
			string lineStr;
			string token;
			while(!feof(fin))
			{
				fgets(line, 100, fin);
				lineStr = line;
				token = lineStr.substr(0, 6);

				if(token.compare("ATOM  ") == 0) {
					atomPositions.push_back(PDBAtom(lineStr));
				}
			}

			return atomPositions;
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
			delete [] out;
			out = NULL;
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
			delete [] substring;
			substring = NULL;
			return value;
		}
	}
}
#endif