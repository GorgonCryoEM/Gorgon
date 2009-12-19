// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads PDB files.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.14.2.7  2009/11/05 17:28:04  schuhs
//   Added code (commented out now) to set helix length as number of bonds and not the number of residues
//
//   Revision 1.14.2.6  2009/09/10 16:25:00  schuhs
//   Set graph weights in units of residues instead of Angstroms, as was done in the SMI paper code.
//
//   Revision 1.14.2.5  2009/09/08 14:54:14  schuhs
//   More changes to reproduce results from SMI paper when SMIPAPER_MODE flag is set.
//
//   Revision 1.14.2.4  2009/08/26 20:50:15  schuhs
//   Add code to reproduce results from SMI paper when SMIPAPER_MODE flag is set.
//
//   Revision 1.14.2.3  2009/08/18 17:55:11  schuhs
//   Adding ability to ignore strands when building the sequence graph
//
//   Revision 1.14.2.2  2009/07/03 16:31:43  schuhs
//   Storing strand length as node cost
//
//   Revision 1.14.2.1  2009/06/18 20:36:16  schuhs
//   Adding sheets to sequence graph
//
//   Revision 1.14  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.13  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//


#ifndef PDBREADER_H
#define PDBREADER_H

#include <vector>
#include <map>
#include "SecondaryStructure.h"
#include "SheetStrand.h"
#include "GlobalConstants.h"
#include <string>
#include <MathTools/Vector3D.h>
#include "PDBAtom.h"
#include "PDBHelix.h"
#include "StandardGraph.h"
#include <MathTools/LinearSolver.h>

using namespace std;
using namespace wustl_mm::MathTools;


namespace wustl_mm {
	namespace GraphMatch {
		typedef StandardGraph GraphType;
		
		class PDBReader {
		public:
			static StandardGraph * ReadFile(char * fname);
			static map<unsigned long long, PDBAtom> ReadAtomPositions(string fileName);
			static vector<PDBHelix> ReadHelixPositions(string fileName);
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
				} else if (strcmp(token, TOKEN_PDB_SHEET)== 0 && INCLUDE_STRANDS == 1) {
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
			cout << "creating new graph for " << numHelices << " helices and " << numSheets << " strands." << endl;

			//StandardGraph * graph = new StandardGraph(2 * structures.size());
			StandardGraph * graph = new StandardGraph(2 * numHelices + numSheets);

			// Adding the rest of the structures into the graph
			int node = 0;
			for(i = 0; i < (int)structures.size(); i++) {
				if (structures[i]->secondaryStructureType == GRAPHEDGE_HELIX) {
					cout << "adding helix " << i << endl;
					graph->SetCost(node+1, node+1, 0); // First helix node.
					graph->SetType(node+1, node+1, GRAPHNODE_HELIX); 
					graph->SetCost(node+2, node+2, 0); // Second helix node.
					graph->SetType(node+2, node+2, GRAPHNODE_HELIX); 

					// An edge for the helix
					if (SMIPAPER_MODE == 1) {
						graph->SetCost(node+1, node+2, structures[i]->GetLengthResidues()); 
					} else {
						//graph->SetCost(node+1, node+2, structures[i]->GetLengthAngstroms()); 
						graph->SetCost(node+1, node+2, structures[i]->GetLengthResidues()); 
						// should be shorter by 1 since # bonds = # residues - 1
						//graph->SetCost(node+1, node+2, structures[i]->GetLengthBonds()); 
					}
					graph->SetType(node+1, node+2, structures[i]->secondaryStructureType); 

					if (SMIPAPER_MODE == 1) {
						graph->SetCost(node+2, node+1, structures[i]->GetLengthResidues()); 
					} else {
						//graph->SetCost(node+2, node+1, structures[i]->GetLengthAngstroms()); 
						graph->SetCost(node+2, node+1, structures[i]->GetLengthResidues()); 
						// should be shorter by 1 since # bonds = # residues - 1
						//graph->SetCost(node+2, node+1, structures[i]->GetLengthBonds()); 
					}
					graph->SetType(node+2, node+1, structures[i]->secondaryStructureType); 

					if(i != 0) {
						// An edge for the loop before the helix
						if (SMIPAPER_MODE == 1) {
							graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition));
						} else {
							//graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
							graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition));
						}
						graph->SetType(node, node+1, GRAPHEDGE_LOOP);

						if (SMIPAPER_MODE == 1) {
							graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition));
						} else {
							//graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
							graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition));
						}
						graph->SetType(node+1, node, GRAPHEDGE_LOOP);
					}
					node += 2;
				}

				if (structures[i]->secondaryStructureType == GRAPHEDGE_SHEET) {
					cout << "adding strand " << i << " with ID " << structures[i]->secondaryStructureID << endl;
					// TODO: Add some cost for the node itself
					if (SMIPAPER_MODE == 1) {
						graph->SetCost(node+1, (structures[i]->endPosition - structures[i]->startPosition) );
					} else {
						//graph->SetCost(node+1, (structures[i]->endPosition - structures[i]->startPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
						graph->SetCost(node+1, (structures[i]->endPosition - structures[i]->startPosition) );
					}
					cout << "adding strand " << i << " at node " << node << " with length " << (structures[i]->endPosition - structures[i]->startPosition) * LOOP_C_ALPHA_TO_ANGSTROMS << endl;
					graph->SetCost(node+1, node+1, 0); // Strand node.
					graph->SetType(node+1, node+1, GRAPHNODE_SHEET); 

					if(i != 0) {
						// An edge for the loop before the strand
						// TODO: fix length calculation -- do startPosition and endPosition hold the right values for strands?
						if (SMIPAPER_MODE == 1) {
							graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition) );
						} else {
							//graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
							graph->SetCost(node, node+1, (structures[i]->startPosition - structures[i-1]->endPosition) );
						}
						graph->SetType(node, node+1, GRAPHEDGE_LOOP);

						if (SMIPAPER_MODE == 1) {
							graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition) );
						} else {
							//graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition) * LOOP_C_ALPHA_TO_ANGSTROMS);
							graph->SetCost(node+1, node, (structures[i]->startPosition - structures[i-1]->endPosition) );
						}

						graph->SetType(node+1, node, GRAPHEDGE_LOOP);
					}
					node += 1;
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


		map<unsigned long long, PDBAtom> PDBReader::ReadAtomPositions(string fileName) {
			map<unsigned long long, PDBAtom> atomPositions;

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
					PDBAtom atom = PDBAtom(lineStr);
					atomPositions[atom.GetHashKey()] = atom;
				}
			}
			fclose(fin);

			return atomPositions;
		}

		vector<PDBHelix> PDBReader::ReadHelixPositions(string fileName) {
			map<unsigned long long, PDBAtom> atomPositions = ReadAtomPositions(fileName);
			vector<PDBHelix> helices;

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

				vector<Vector3DFloat> helixAtomLocs;
				if(token.compare("HELIX ") == 0) {
					PDBHelix helix = PDBHelix(lineStr);
					helixAtomLocs.clear();
					for(int i = helix.GetInitialResidueSeqNo(); i <= helix.GetEndResidueSeqNo(); i++) {
						helixAtomLocs.push_back(atomPositions[PDBAtom::ConstructHashKey("----", helix.GetInitialResidueChainId(), i, "CA")].GetPosition());
					}

					Vector3DFloat pt1, pt2;
					LinearSolver::FindBestFitLine(pt1, pt2, helixAtomLocs);

					helix.SetEndPositions(pt1, pt2);
					helices.push_back(helix);
				}
			}
			atomPositions.clear();
			fclose(fin);
			return helices;
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