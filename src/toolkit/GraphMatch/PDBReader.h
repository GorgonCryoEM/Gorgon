#ifndef TOOLKIT_GRAPHMATCH_PDBREADER_H
#define TOOLKIT_GRAPHMATCH_PDBREADER_H

#include <vector>
#include <map>
//#include "SecondaryStructure.h"
//#include "SheetStrand.h"
//#include "Core/GlobalConstants.h"
//#include <string>
//#include <MathTools/Vector3.h>
#include "PDBAtom.h"
#include "PDBHelix.h"
#include "Graph.h"
#include <MathTools/LinearSolver.h>

using namespace std;

//using namespace MathTools;

namespace GraphMatch {
    typedef Graph GraphType;

    class PDBReader {
    public:
        static Graph * ReadFile(string fname);
        static map<unsigned long long, PDBAtom> ReadAtomPositions(string fileName);
        static vector<PDBHelix> ReadHelixPositions(string fileName);
        static bool WriteAtomPositions(map<unsigned long long, PDBAtom> &atoms, string fileName);
        static string TrimString(string str);
        static int ToInt(string str);
    private:
        static string GetString(string str, int start, int length);
        static int GetInt(string str, int start, int length);
    };

    #ifdef GET_AMINO_SEQUENCE
    inline string GetSingleLetterFromThree(string aminoAcid) {
        string result;
        if(aminoAcid == "ALA") {
            result = 'A';
        } else if(aminoAcid == "ARG") {
            result = 'R';
        } else if(aminoAcid == "ASN") {
            result = 'N';
        } else if(aminoAcid == "ASP") {
            result = 'D';
        } else if(aminoAcid == "CYS") {
            result = 'C';
        } else if(aminoAcid == "GLN") {
            result = 'Q';
        } else if(aminoAcid == "GLU") {
            result = 'E';
        } else if(aminoAcid == "GLY") {
            result = 'G';
        } else if(aminoAcid == "HIS") {
            result = 'H';
        } else if(aminoAcid == "ILE") {
            result = 'I';
        } else if(aminoAcid == "LEU") {
            result = 'L';
        } else if(aminoAcid == "LYS") {
            result = 'K';
        } else if(aminoAcid == "MET") {
            result = 'M';
        } else if(aminoAcid == "PHE") {
            result = 'F';
        } else if(aminoAcid == "PRO") {
            result = 'P';
        } else if(aminoAcid == "SER") {
            result = 'S';
        } else if(aminoAcid == "THR") {
            result = 'T';
        } else if(aminoAcid == "TRP") {
            result = 'W';
        } else if(aminoAcid == "TYR") {
            result = 'Y';
        } else if(aminoAcid == "VAL") {
            result = 'V';
        } else {
            printf("/noops!!!/n");
            result = 'Z';
        }
        return result;
    }
    #endif

    inline Graph * PDBReader::ReadFile(string fname) {
        ifstream fin(fname.c_str());
        if (!fin)
        {
            cout<<"Error reading input file "<<fname<<".\n";
            exit(0) ;
        }

        string line;
        string sequence = "";
        string token;
        vector<SecStruct*> structures;
        SecStruct * currentStructure;
        bool add;
        int oldIndex = 0;
        int index;
        #ifdef GET_AMINO_SEQUENCE
        string acidString;
        string acidChar;
        int start = 10000;
        #endif

        while(getline(fin, line))
        {
            token = GetString(line, 0, 6);

            if(token == TOKEN_PDB_HELIX) {
                currentStructure = new SecStruct();
                currentStructure->serialNumber = GetInt(line, 7, 3);
                currentStructure->ID = GetString(line, 11, 3);
                currentStructure->startPosition = GetInt(line, 21, 4);
                currentStructure->endPosition = GetInt(line, 33, 4);
                currentStructure->sseType = GRAPHEDGE_HELIX;
                add = true;
                for(unsigned int i = 0; i < structures.size(); i++) {
                    add = add && !((currentStructure->startPosition == structures[i]->startPosition) &&
                        (currentStructure->endPosition == structures[i]->endPosition));
                }
                if(add) {
                    structures.push_back(currentStructure);
                } else {
                    delete currentStructure;
                }
            #ifdef GET_AMINO_SEQUENCE
            } else if(token == "ATOM") {
                index = GetInt(line, 22, 4);
                start = min(index, start);
                if(index != oldIndex){
                    acidString = GetString(line, 17,3);
                    acidChar = GetSingleLetterFromThree(acidString);
                    sequence += acidChar;
                    cout<<acidChar;
                }
                oldIndex = index;
            #endif
            #ifdef INCLUDE_SHEETS
            } else if (token == TOKEN_PDB_SHEET && INCLUDE_STRANDS == 1) {
                currentStructure = new SecStruct();
                currentStructure->serialNumber = GetInt(line, 7, 3);
                currentStructure->ID = GetString(line, 11, 3);
                currentStructure->startPosition = GetInt(line, 22, 4);
                currentStructure->endPosition = GetInt(line, 33, 4);
                currentStructure->sseType = GRAPHEDGE_SHEET;
                add = true;
                for(unsigned int i = 0; i < structures.size(); i++) {
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
        }

        #ifdef GET_AMINO_SEQUENCE
        printf("\n");
        #endif

        fin.close() ;

        // Sorting the structures by the start position
        int i,j;
        for(i = 0; i < (int)structures.size()-1; i++)   {
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
            if (currentStructure->sseType == GRAPHEDGE_HELIX) {
                numHelices++;
            } else  if (currentStructure->sseType == GRAPHEDGE_SHEET) {
                numSheets++;
            }
        }
#ifdef VERBOSE
        cout << "creating new graph for " << numHelices << " helices and " << numSheets << " strands." << endl;
#endif // VERBOSE

        Graph * graph = new Graph(2 * numHelices + numSheets);

        // Adding the rest of the structures into the graph
        int node = 0;
        for(i = 0; i < (int)structures.size(); i++) {
            if (structures[i]->sseType == GRAPHEDGE_HELIX) {
#ifdef VERBOSE
                cout << "adding helix " << i << endl;
#endif // VERBOSE
                graph->setCost(node+1, node+1, 0); // First helix node.
                graph->setType(node+1, node+1, GRAPHNODE_HELIX);
                graph->setCost(node+2, node+2, 0); // Second helix node.
                graph->setType(node+2, node+2, GRAPHNODE_HELIX);

                // An edge for the helix
                graph->setCost(node+1, node+2, structures[i]->GetLengthResidues());
                graph->setType(node+1, node+2, structures[i]->sseType);
                graph->setCost(node+2, node+1, structures[i]->GetLengthResidues());
                graph->setType(node+2, node+1, structures[i]->sseType);

                if(i != 0) {
                    // An edge for the loop before the helix
                    graph->setCost(node, node+1, max(0.5, (double)(structures[i]->startPosition - structures[i-1]->endPosition)));
                    graph->setType(node, node+1, GRAPHEDGE_LOOP);
                    graph->setCost(node+1, node, max(0.5, (double)(structures[i]->startPosition - structures[i-1]->endPosition)));
                    graph->setType(node+1, node, GRAPHEDGE_LOOP);
                }
                node += 2;
            }

            if (structures[i]->sseType == GRAPHEDGE_SHEET) {
#ifdef VERBOSE
                cout << "adding strand " << i << " with ID " << structures[i]->ID << endl;
#endif // VERBOSE
                graph->setCost(node+1, (structures[i]->endPosition - structures[i]->startPosition) );
#ifdef VERBOSE
                cout << "adding strand " << i << " at node " << node << " with length " << (structures[i]->endPosition - structures[i]->startPosition) * LOOP_C_ALPHA_TO_ANGSTROMS << endl;
#endif // VERBOSE
                graph->setCost(node+1, node+1, 0); // Strand node.
                graph->setType(node+1, node+1, GRAPHNODE_SHEET);

                if(i != 0) {
                    // An edge for the loop before the strand
                    graph->setCost(node, node+1, max(0.5, (double)structures[i]->startPosition - structures[i-1]->endPosition) );
                    graph->setType(node, node+1, GRAPHEDGE_LOOP);
                    graph->setCost(node+1, node, max(0.5, (double)structures[i]->startPosition - structures[i-1]->endPosition) );
                    graph->setType(node+1, node, GRAPHEDGE_LOOP);
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

        /* block below contains a bug (ex: BTV data set)
        oldNum = start;
        for(unsigned int i = 0; i < structures.size(); i++) {
            string dashLine = "";
            string sseLine = "";
            for(unsigned int j = 0; j <  structures[i]->startPosition - oldNum; j++) {
                dashLine = dashLine + "-";
            }
            for(unsigned int j = 0; j < structures[i]->endPosition - structures[i]->startPosition + 1; j++) {
                if (structures[i]->sseType == GRAPHEDGE_HELIX) {
                    sseLine =  sseLine + "H";
                } else if (structures[i]->sseType == GRAPHEDGE_SHEET) {
                    sseLine =  sseLine + "E";
                } else {
                    sseLine = sseLine + "*";
                }
            }
            printf("%s%s", dashLine.c_str(), sseLine.c_str());
            oldNum = structures[i]->endPosition + 1;
        }
        printf("\n");
        */
        #endif


        structures.clear();

        return graph;
    }


    inline map<unsigned long long, PDBAtom> PDBReader::ReadAtomPositions(string fileName) {
        map<unsigned long long, PDBAtom> atomPositions;

        ifstream fin(fileName.c_str());
        if (!fin)
        {
            cout<<"Error reading input file "<<fileName<<".\n";
            exit(0) ;
        }

        string line;
        string lineStr;
        string token;
        while(getline(fin, line))
        {
            lineStr = line;
            token = lineStr.substr(0, 6);

            if(token.compare("ATOM  ") == 0) {
                PDBAtom atom = PDBAtom(lineStr);
                atomPositions[atom.GetHashKey()] = atom;
            }
        }
        fin.close();

        return atomPositions;
    }

    inline string PDBReader::TrimString(string str) {
        int startPos = 0;
        int endPos = str.size() - 1;
        for(int i = 0; i < endPos; i++) {
            if(str[startPos] != ' ') {
                break;
            }
            startPos++;
        }
        for(int i = endPos - 1; i >= startPos; i--) {
            if(str[endPos] != ' ') {
                break;
            }
            endPos--;
        }
        int j = 0;
        string outString;
        for(int i = startPos; i <= endPos; i++) {
            outString[j] = str[i];
            j++;
        }
        outString[j] = 0;
        return outString;
    }

    inline string PDBReader::GetString(string str, int start, int length) {
        string out;
        string temp;
        for(int i = 0; i < length; i++) {
            out[i] = str[i+start];
        }
        out[length] = 0;
        temp = TrimString(out);

        // clean
        return temp;
    }

   inline vector<PDBHelix> PDBReader::ReadHelixPositions(string fileName) {
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

                vector<Vec3F> helixAtomLocs;
                if(token.compare("HELIX ") == 0) {
                    PDBHelix helix = PDBHelix(lineStr);
                    helixAtomLocs.clear();
                    for(int i = helix.GetInitialResidueSeqNo(); i <= helix.GetEndResidueSeqNo(); i++) {
                        helixAtomLocs.push_back(atomPositions[PDBAtom::ConstructHashKey("----", helix.GetInitialResidueChainId(), i, "CA")].GetPosition());
                    }

                    Vec3F pt1, pt2;
                    LinearSolver::FindBestFitLine(pt1, pt2, helixAtomLocs);

                    helix.SetEndPositions(pt1, pt2);
                    helices.push_back(helix);
                }
            }
            atomPositions.clear();
            fclose(fin);
            return helices;
        }

    inline int PDBReader::ToInt(string str) {
        int value = 0;
        int sign = 1;
        for(int i = 0; i < (int)str.size(); i++) {
            if(str[i] == '-') {
                sign = -1;
            } else {
                value += sign * (str[i]-48);
                value *= 10;
            }
        }
        value /= 10;
        return value;
    }
    inline int PDBReader::GetInt(string str, int start, int length) {
        string substring = GetString(str, start, length);
        int value = ToInt(substring);

        // clean
        return value;
    }
    inline bool PDBReader::WriteAtomPositions(map<unsigned long long, PDBAtom> &atoms, string fileName) {
        ofstream fout(fileName.c_str());
        if (!fout) {
            cout<<"Error reading output file "<<fileName<<".\n";
            return false;
        } else {
            fout<<StringUtils::RightPad("REMARK   5 Gorgon (C) 2005-2010", 70)<<endl;
            fout<<StringUtils::RightPad("REMARK   5 Pseudoatom export using SSEHunter", 70)<<endl;
            for(map<unsigned long long, PDBAtom>::iterator i = atoms.begin(); i != atoms.end(); i++) {
                fout<<i->second.GetPDBString()<<endl;
        }
            fout.close();
        }
        return true;
    }
}
//}
#endif
