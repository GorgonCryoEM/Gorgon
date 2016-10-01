#ifndef CORE_GRAPHMATCH_GLOBALCONSTANTS_H
#define CORE_GRAPHMATCH_GLOBALCONSTANTS_H

#define INCLUDE_SHEETS
//#define GET_STATS
#define VERBOSE
//#define MAKE_FINAL_MRC
#define GET_AMINO_SEQUENCE

#ifdef _WIN32
	#include <string>
#else
	#include <cstring>
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

namespace GraphMatch {

    const int NOPATHPENALTY = 1;
    const int TYPEMISMATCHPENALTY = 1;

    const int PRIORITYQUEUESIZE = 50000000;
    const int RESULT_COUNT = 35;
    const int MAX_NODES = 60;
    const int MAXINT = 2147483647;
    const unsigned int MAXUNSIGNEDINT = 4294967295;
    const double MAXDOUBLE = 1.7E308;
    const int GRAPHEDGE_HELIX = 0;
    const int GRAPHEDGE_LOOP = 1;
    const int GRAPHEDGE_SHEET = 2;
    const int GRAPHEDGE_OTHER = 3;
    const int GRAPHEDGE_LOOP_EUCLIDEAN = 4;
    const int GRAPHNODE_HELIX = 5;
    const int GRAPHNODE_SHEET = 6;
    const string TOKEN_PDB_HELIX = "HELIX";
    const string TOKEN_PDB_ATOM = "ATOM";
    const string TOKEN_PDB_SHEET = "SHEET";
    const string TOKEN_VRML_TRANSLATION = "translation";
    const string TOKEN_VRML_ROTATION = "rotation";
    const string TOKEN_VRML_HEIGHT = "height";
    const string TOKEN_VRML_RADIUS = "radius";
    const string TOKEN_VRML_SHAPE = "Shape";
    const string TOKEN_VRML_POINT = "point";
    const string TOKEN_VRML_COORDINDEX = "coordIndex";
    const string TOKEN_SSE_ALPHA = "ALPHA";
    const string TOKEN_SSE_FILE_NAME = "SSE_FILE_NAME";
    const string TOKEN_VRML_HELIX_FILE_NAME = "VRML_HELIX_FILE_NAME";
    const string TOKEN_VRML_SHEET_FILE_NAME = "VRML_SHEET_FILE_NAME";
    const string TOKEN_SEQUENCE_FILE_NAME = "SEQUENCE_FILE_NAME";
    const string TOKEN_SEQUENCE_FILE_TYPE = "SEQUENCE_FILE_TYPE";
    const string TOKEN_PDB_FILE_NAME = "PDB_FILE_NAME";
    const string TOKEN_MRC_FILE_NAME = "MRC_FILE_NAME";
    const string TOKEN_MAXIMUM_DISTANCE_SHEET_SKELETON = "MAXIMUM_DISTANCE_SHEET_SKELETON";
    const string TOKEN_MINIMUM_SHEET_SIZE = "MINIMUM_SHEET_SIZE";
    const string TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD = "EUCLIDEAN_DISTANCE_THRESHOLD";
    const string TOKEN_BORDER_MARGIN_THRESHOLD = "BORDER_MARGIN_THRESHOLD";
    const string TOKEN_NORMALIZE_GRAPHS = "NORMALIZE_GRAPHS";
    const string TOKEN_MISSING_HELIX_PENALTY = "MISSING_HELIX_PENALTY";
    const string TOKEN_MISSING_SHEET_PENALTY = "MISSING_SHEET_PENALTY";
    const string TOKEN_MISSING_HELIX_PENALTY_SCALED = "MISSING_HELIX_PENALTY_SCALED";
    const string TOKEN_MISSING_SHEET_PENALTY_SCALED = "MISSING_SHEET_PENALTY_SCALED";
    const string TOKEN_EUCLIDEAN_LOOP_PENALTY = "EUCLIDEAN_LOOP_PENALTY";
    const string TOKEN_START_END_MISSING_HELIX_PENALTY = "START_END_MISSING_HELIX_PENALTY";
    const string TOKEN_HELIX_WEIGHT_COEFFICIENT = "HELIX_WEIGHT_COEFFICIENT";
    const string TOKEN_LOOP_WEIGHT_COEFFICIENT = "LOOP_WEIGHT_COEFFICIENT";
    const string TOKEN_SHEET_CAPACITY_COEFFICIENT = "SHEET_CAPACITY_COEFFICIENT";
    const string TOKEN_SHEET_MERGE_THRESHOLD = "SHEET_MERGE_THRESHOLD";
    const string TOKEN_MISSING_HELIX_LENGTH = "MISSING_HELIX_LENGTH";
    const string TOKEN_MISSING_SHEET_LENGTH = "MISSING_SHEET_LENGTH";
    const string TOKEN_SHEET_SELF_LOOP_LENGTH = "SHEET_SELF_LOOP_LENGTH";
    const string TOKEN_SHEET_WEIGHT_COEFFICIENT = "SHEET_WEIGHT_COEFFICIENT";
    const string TOKEN_COST_FUNCTION = "COST_FUNCTION";
    const string TOKEN_INCLUDE_STRANDS = "INCLUDE_STRANDS";
    const string TOKEN_VOXEL_SIZE = "VOXEL_SIZE";
    const string TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES = "TRANSLATE_VOLUMETRIC_COORDINATES";
    const string TOKEN_MISSING_HELIX_COUNT = "MISSING_HELIX_COUNT";
    const string TOKEN_MISSING_SHEET_COUNT = "MISSING_SHEET_COUNT";
    const string TOKEN_NODE_CONSTRAINT = "NODE_CONSTRAINT";
    const string TOKEN_HELIX_CONSTRAINT = "HELIX_CONSTRAINT";
    const string TOKEN_NODE_MISMATCH = "NODE_MISMATCH";
    const string TOKEN_HELIX_MISMATCH = "HELIX_MISMATCH";
    const string TOKEN_EUCLIDEAN_VOXEL_TO_PDB_RATIO = "EUCLIDEAN_VOXEL_TO_PDB_RATIO";
    const string TOKEN_SOLUTION = "SOLUTION";
    const int MAX_RANDOM_HELIX_SIZE = 30;
    const int MAX_RANDOM_LOOP_SIZE = 30;
    const int WONG_HASH_TABLE_SIZE = 1024;

    const float HELIX_C_ALPHA_TO_ANGSTROMS = 1.5f;
    const float LOOP_C_ALPHA_TO_ANGSTROMS = 3.8f / 1.5f; // 1.5 works well on test data

    string SSE_FILE_NAME;
    string VRML_HELIX_FILE_NAME;
    string VRML_SHEET_FILE_NAME;
    string SEQUENCE_FILE_NAME;
    string SEQUENCE_FILE_TYPE;
    string MRC_FILE_NAME;
    double MAXIMUM_DISTANCE_SHEET_SKELETON = 0.0;
    int MINIMUM_SHEET_SIZE = 10;
    double EUCLIDEAN_DISTANCE_THRESHOLD = 15;
    int BORDER_MARGIN_THRESHOLD = 3;
    bool NORMALIZE_GRAPHS = true;
    double EUCLIDEAN_VOXEL_TO_PDB_RATIO = 2.0;
    double MISSING_HELIX_PENALTY = 2;
    double MISSING_SHEET_PENALTY = 2;
    double MISSING_HELIX_PENALTY_SCALED = 0;
    double MISSING_SHEET_PENALTY_SCALED = 1;
    double EUCLIDEAN_LOOP_PENALTY = 5;
    double START_END_MISSING_HELIX_PENALTY = 5;
    double HELIX_WEIGHT_COEFFICIENT = 1.0;
    double LOOP_WEIGHT_COEFFICIENT = 0.25;
    double SHEET_CAPACITY_COEFFICIENT = 10000000.0;
    double SHEET_MERGE_THRESHOLD = 3.0;
    double MISSING_HELIX_LENGTH = 8;
    double MISSING_SHEET_LENGTH = 8;
    double SHEET_SELF_LOOP_LENGTH = 4.0 * LOOP_C_ALPHA_TO_ANGSTROMS;
    double SHEET_WEIGHT_COEFFICIENT = 1.0;
    int COST_FUNCTION = 1;   // 1 : |a-b|		2 : |a-b|/(a+b)		3:|a-b|^2
    int INCLUDE_STRANDS = 0;   // 0 : no		1 : yes
    double VOXEL_SIZE = 1;
    bool TRANSLATE_VOLUMETRIC_COORDINATES = false;
    int MISSING_HELIX_COUNT = -1;
    int MISSING_SHEET_COUNT = -1;
    bool PERFORMANCE_COMPARISON_MODE = false;
    int SOLUTION[MAX_NODES];
    int D26[26][3] = {
        {-1,-1,-1}, {-1, -1, 0}, {-1, -1, 1},
        {-1,0,-1},  {-1, 0, 0},  {-1, 0, 1},
        {-1,1,-1},  {-1, 1, 0},  {-1, 1, 1},
        {0,-1,-1},  {0, -1, 0},  {0, -1, 1},
        {0,0,-1},                {0, 0, 1},
        {0,1,-1},   {0, 1, 0},   {0, 1, 1},
        {1,-1,-1},  {1, -1, 0},  {1, -1, 1},
        {1,0,-1},   {1, 0, 0},   {1, 0, 1},
        {1,1,-1},   {1, 1, 0},   {1, 1, 1}};
    int BACK26[26] = {
        25, 24, 23,
        22, 21, 20,
        19, 18, 17,
        16, 15, 14,
        13,     12,
        11, 10, 9,
        8,  7,  6,
        5,  4,  3,
        2,  1,  0};



    // Private fields.. not to be used!!!
    int allowedConstraintCollection[MAX_NODES][MAX_NODES];
    unsigned int allowedConstraintCount[MAX_NODES];
    int notAllowedConstraintCollection[MAX_NODES][MAX_NODES];
    unsigned int notAllowedConstraintCount[MAX_NODES];

    void ClearAllowedConstraints() {
        for(int i = 0; i < MAX_NODES; i++) {
            allowedConstraintCount[i] = 0;
        }
    }

    void ClearNotAllowedConstraints() {
        for(int i = 0; i < MAX_NODES; i++) {
            notAllowedConstraintCount[i] = 0;
        }
    }

    void AddNodeConstraint(int patternNode, int baseNode) {
        allowedConstraintCollection[patternNode-1][allowedConstraintCount[patternNode-1]] = baseNode;
        allowedConstraintCount[patternNode-1]++;
    }

    int GetNodeConstraint(int patternNode, int constraintNum) {
        if (constraintNum < 0 || constraintNum >= (int)allowedConstraintCount[patternNode-1]) {
            return 0;
        } else {
            return allowedConstraintCollection[patternNode-1][constraintNum];
        }
    }

    void AddNodeMismatch(int patternNode, int baseNode) {
        notAllowedConstraintCollection[patternNode-1][notAllowedConstraintCount[patternNode-1]] = baseNode;
        notAllowedConstraintCount[patternNode-1]++;
    }

    void AddHelixConstraint(int patternHelix, int baseHelix) {
        int patternNode1 = patternHelix;
        int patternNode2 = patternHelix+1;

        if(baseHelix == -1) {
            allowedConstraintCollection[patternNode1-1][allowedConstraintCount[patternNode1-1]] = -1;		allowedConstraintCount[patternNode1-1]++;
            allowedConstraintCollection[patternNode2-1][allowedConstraintCount[patternNode2-1]] = -1;		allowedConstraintCount[patternNode2-1]++;
        } else {
            int baseNode1 = baseHelix;
            int baseNode2 = baseHelix+1;
            allowedConstraintCollection[patternNode1-1][allowedConstraintCount[patternNode1-1]] = baseNode1;		allowedConstraintCount[patternNode1-1]++;
            allowedConstraintCollection[patternNode1-1][allowedConstraintCount[patternNode1-1]] = baseNode2;		allowedConstraintCount[patternNode1-1]++;
            allowedConstraintCollection[patternNode2-1][allowedConstraintCount[patternNode2-1]] = baseNode1;		allowedConstraintCount[patternNode2-1]++;
            allowedConstraintCollection[patternNode2-1][allowedConstraintCount[patternNode2-1]] = baseNode2;		allowedConstraintCount[patternNode2-1]++;
        }
    }

    void AddHelixMismatch(int patternHelix, int baseHelix) {
        int patternNode1 = patternHelix*2 - 1;
        int patternNode2 = patternHelix*2;

        if(baseHelix == -1) {
            notAllowedConstraintCollection[patternNode1-1][notAllowedConstraintCount[patternNode1-1]] = -1;		notAllowedConstraintCount[patternNode1-1]++;
            notAllowedConstraintCollection[patternNode2-1][notAllowedConstraintCount[patternNode2-1]] = -1;		notAllowedConstraintCount[patternNode2-1]++;
        } else {
            int baseNode1 = baseHelix*2 - 1;
            int baseNode2 = baseHelix*2;
            notAllowedConstraintCollection[patternNode1-1][notAllowedConstraintCount[patternNode1-1]] = baseNode1;		notAllowedConstraintCount[patternNode1-1]++;
            notAllowedConstraintCollection[patternNode1-1][notAllowedConstraintCount[patternNode1-1]] = baseNode2;		notAllowedConstraintCount[patternNode1-1]++;
            notAllowedConstraintCollection[patternNode2-1][notAllowedConstraintCount[patternNode2-1]] = baseNode1;		notAllowedConstraintCount[patternNode2-1]++;
            notAllowedConstraintCollection[patternNode2-1][notAllowedConstraintCount[patternNode2-1]] = baseNode2;		notAllowedConstraintCount[patternNode2-1]++;
        }
    }


    bool SetConstantFromToken(string token, string stringValue, double doubleValue, int intValue, bool boolValue) {
        if(token == TOKEN_SSE_FILE_NAME) {
            SSE_FILE_NAME = stringValue;
        } else if(token == TOKEN_VRML_HELIX_FILE_NAME) {
            VRML_HELIX_FILE_NAME = stringValue;
        } else if(token == TOKEN_VRML_SHEET_FILE_NAME) {
            VRML_SHEET_FILE_NAME = stringValue;
        } else if(token == TOKEN_SEQUENCE_FILE_NAME) {
            SEQUENCE_FILE_NAME = stringValue;
        } else if(token == TOKEN_SEQUENCE_FILE_TYPE) {
            SEQUENCE_FILE_TYPE = stringValue;
        } else if(token == TOKEN_MRC_FILE_NAME) {
            MRC_FILE_NAME = stringValue;
        } else if(token == TOKEN_MAXIMUM_DISTANCE_SHEET_SKELETON) {
            MAXIMUM_DISTANCE_SHEET_SKELETON = doubleValue;
        } else if(token == TOKEN_MINIMUM_SHEET_SIZE) {
            MINIMUM_SHEET_SIZE = intValue;
        } else if(token == TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD) {
            EUCLIDEAN_DISTANCE_THRESHOLD = doubleValue;
        } else if(token == TOKEN_BORDER_MARGIN_THRESHOLD) {
            BORDER_MARGIN_THRESHOLD = intValue;
        } else if(token == TOKEN_NORMALIZE_GRAPHS) {
            NORMALIZE_GRAPHS = boolValue;
        } else if(token == TOKEN_MISSING_HELIX_PENALTY) {
            MISSING_HELIX_PENALTY = doubleValue;
        } else if(token == TOKEN_MISSING_SHEET_PENALTY) {
            MISSING_SHEET_PENALTY = doubleValue;
        } else if(token == TOKEN_MISSING_HELIX_PENALTY_SCALED) {
            MISSING_HELIX_PENALTY_SCALED = doubleValue;
        } else if(token == TOKEN_MISSING_SHEET_PENALTY_SCALED) {
            MISSING_SHEET_PENALTY_SCALED = doubleValue;
        } else if(token == TOKEN_EUCLIDEAN_LOOP_PENALTY) {
            EUCLIDEAN_LOOP_PENALTY = doubleValue;
        } else if(token == TOKEN_START_END_MISSING_HELIX_PENALTY) {
            START_END_MISSING_HELIX_PENALTY = doubleValue;
        } else if(token == TOKEN_HELIX_WEIGHT_COEFFICIENT) {
            HELIX_WEIGHT_COEFFICIENT = doubleValue;
        } else if(token == TOKEN_LOOP_WEIGHT_COEFFICIENT) {
            LOOP_WEIGHT_COEFFICIENT = doubleValue;
        } else if(token == TOKEN_SHEET_CAPACITY_COEFFICIENT) {
            SHEET_CAPACITY_COEFFICIENT = doubleValue;
        } else if(token == TOKEN_SHEET_MERGE_THRESHOLD) {
            SHEET_MERGE_THRESHOLD = doubleValue;
        } else if(token == TOKEN_MISSING_HELIX_LENGTH) {
            MISSING_HELIX_LENGTH = doubleValue;
        } else if(token == TOKEN_MISSING_SHEET_LENGTH) {
            MISSING_SHEET_LENGTH = doubleValue;
        } else if(token == TOKEN_SHEET_SELF_LOOP_LENGTH) {
            SHEET_SELF_LOOP_LENGTH = doubleValue;
        } else if(token == TOKEN_SHEET_WEIGHT_COEFFICIENT) {
            SHEET_WEIGHT_COEFFICIENT = doubleValue;
        } else if(token == TOKEN_COST_FUNCTION) {
            COST_FUNCTION = intValue;
        } else if(token == TOKEN_INCLUDE_STRANDS) {
            INCLUDE_STRANDS = intValue;
        } else if(token == TOKEN_VOXEL_SIZE) {
            VOXEL_SIZE = doubleValue;
        } else if(token == TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES) {
            TRANSLATE_VOLUMETRIC_COORDINATES = boolValue;
        } else if(token == TOKEN_MISSING_HELIX_COUNT) {
            MISSING_HELIX_COUNT = intValue;
        } else if(token == TOKEN_MISSING_SHEET_COUNT) {
            MISSING_SHEET_COUNT = intValue;
        } else if(token == TOKEN_EUCLIDEAN_VOXEL_TO_PDB_RATIO) {
            EUCLIDEAN_VOXEL_TO_PDB_RATIO = doubleValue;
        } else {
            return false;
        }
        return true;
    }


    bool GetConstantFromToken(string token, string stringValue, double &doubleValue, int &intValue, bool &boolValue) {
        if(token == TOKEN_SSE_FILE_NAME) {
            stringValue = SSE_FILE_NAME;
        } else if(token == TOKEN_VRML_HELIX_FILE_NAME) {
            stringValue = VRML_HELIX_FILE_NAME;
        } else if(token == TOKEN_VRML_SHEET_FILE_NAME) {
            stringValue = VRML_SHEET_FILE_NAME;
        } else if(token == TOKEN_SEQUENCE_FILE_NAME) {
            stringValue = SEQUENCE_FILE_NAME;
        } else if(token == TOKEN_SEQUENCE_FILE_TYPE) {
            stringValue = SEQUENCE_FILE_TYPE;
        } else if(token == TOKEN_MRC_FILE_NAME) {
            stringValue = MRC_FILE_NAME;
        } else if(token == TOKEN_MAXIMUM_DISTANCE_SHEET_SKELETON) {
            doubleValue = MAXIMUM_DISTANCE_SHEET_SKELETON;
        } else if(token == TOKEN_MINIMUM_SHEET_SIZE) {
            intValue = MINIMUM_SHEET_SIZE;
        } else if(token == TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD) {
            doubleValue = EUCLIDEAN_DISTANCE_THRESHOLD;
        } else if(token == TOKEN_BORDER_MARGIN_THRESHOLD) {
            intValue = BORDER_MARGIN_THRESHOLD;
        } else if(token == TOKEN_NORMALIZE_GRAPHS) {
            boolValue = NORMALIZE_GRAPHS;
        } else if(token == TOKEN_MISSING_HELIX_PENALTY) {
            doubleValue = MISSING_HELIX_PENALTY;
        } else if(token == TOKEN_MISSING_SHEET_PENALTY) {
            doubleValue = MISSING_SHEET_PENALTY;
        } else if(token == TOKEN_MISSING_HELIX_PENALTY_SCALED) {
            doubleValue = MISSING_HELIX_PENALTY_SCALED;
        } else if(token == TOKEN_MISSING_SHEET_PENALTY_SCALED) {
            doubleValue = MISSING_SHEET_PENALTY_SCALED;
        } else if(token == TOKEN_EUCLIDEAN_LOOP_PENALTY) {
            doubleValue = EUCLIDEAN_LOOP_PENALTY;
        } else if(token == TOKEN_START_END_MISSING_HELIX_PENALTY) {
            doubleValue = START_END_MISSING_HELIX_PENALTY;
        } else if(token == TOKEN_HELIX_WEIGHT_COEFFICIENT) {
            doubleValue = HELIX_WEIGHT_COEFFICIENT;
        } else if(token == TOKEN_LOOP_WEIGHT_COEFFICIENT) {
            doubleValue = LOOP_WEIGHT_COEFFICIENT;
        } else if(token == TOKEN_SHEET_CAPACITY_COEFFICIENT) {
            doubleValue = SHEET_CAPACITY_COEFFICIENT;
        } else if(token == TOKEN_SHEET_MERGE_THRESHOLD) {
            doubleValue = SHEET_MERGE_THRESHOLD;
        } else if(token == TOKEN_MISSING_HELIX_LENGTH) {
            doubleValue = MISSING_HELIX_LENGTH;
        } else if(token == TOKEN_MISSING_SHEET_LENGTH) {
            doubleValue = MISSING_SHEET_LENGTH;
        } else if(token == TOKEN_SHEET_SELF_LOOP_LENGTH) {
            doubleValue = SHEET_SELF_LOOP_LENGTH;
        } else if(token == TOKEN_SHEET_WEIGHT_COEFFICIENT) {
            doubleValue = SHEET_WEIGHT_COEFFICIENT;
        } else if(token == TOKEN_COST_FUNCTION) {
            intValue = COST_FUNCTION;
        } else if(token == TOKEN_INCLUDE_STRANDS) {
            intValue = INCLUDE_STRANDS;
        } else if(token == TOKEN_VOXEL_SIZE) {
            doubleValue = VOXEL_SIZE;
        } else if(token == TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES) {
            boolValue = TRANSLATE_VOLUMETRIC_COORDINATES;
        } else if(token == TOKEN_MISSING_HELIX_COUNT) {
            intValue = MISSING_HELIX_COUNT;
        } else if(token == TOKEN_MISSING_SHEET_COUNT) {
            intValue = MISSING_SHEET_COUNT;
        } else if(token == TOKEN_EUCLIDEAN_VOXEL_TO_PDB_RATIO) {
            doubleValue = EUCLIDEAN_VOXEL_TO_PDB_RATIO;
        } else {
            return false;
        }
        return true;
    }


    void LoadConstantsFromFile(string settingsFile) {
        for(int i = 0; i < MAX_NODES; i++) {
            SOLUTION[i] = -2;
            allowedConstraintCount[i] = 0;
            notAllowedConstraintCount[i] = 0;
        }

        int tempInt1, tempInt2;

        ifstream fin(settingsFile.c_str());
        if (!fin) {
            cout<<"Error reading input file "<<settingsFile<<".\n" ;
            exit(0) ;
        }

        string token;
        while (fin>>token) {
            if(token == TOKEN_SSE_FILE_NAME) {
                // Stop the usage of the SSE_FILE_NAME parameter
                //fin>>SSE_FILE_NAME;
            } else if(token == TOKEN_VRML_HELIX_FILE_NAME) {
                fin>>VRML_HELIX_FILE_NAME;
            } else if(token == TOKEN_VRML_SHEET_FILE_NAME) {
                fin>>VRML_SHEET_FILE_NAME;
            } else if(token == TOKEN_SEQUENCE_FILE_NAME) {
                fin>>SEQUENCE_FILE_NAME;
                SEQUENCE_FILE_TYPE = "SEQ";
            } else if(token == TOKEN_PDB_FILE_NAME) {
                fin>>SEQUENCE_FILE_NAME;
                SEQUENCE_FILE_TYPE = "PDB";
            } else if(token == TOKEN_SEQUENCE_FILE_TYPE) {
                fin>>SEQUENCE_FILE_TYPE;
            } else if(token == TOKEN_MRC_FILE_NAME) {
                fin>>MRC_FILE_NAME;
            } else if(token == TOKEN_MAXIMUM_DISTANCE_SHEET_SKELETON) {
                fin>>MAXIMUM_DISTANCE_SHEET_SKELETON;
            } else if(token == TOKEN_MINIMUM_SHEET_SIZE) {
                fin>>MINIMUM_SHEET_SIZE;
            } else if(token == TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD) {
                fin>>EUCLIDEAN_DISTANCE_THRESHOLD;
            } else if(token == TOKEN_BORDER_MARGIN_THRESHOLD) {
                fin>>BORDER_MARGIN_THRESHOLD;
            } else if(token == TOKEN_NORMALIZE_GRAPHS) {
                fin>>NORMALIZE_GRAPHS;
            } else if(token == TOKEN_MISSING_HELIX_PENALTY) {
                fin>>MISSING_HELIX_PENALTY;
            } else if(token == TOKEN_MISSING_SHEET_PENALTY) {
                fin>>MISSING_SHEET_PENALTY;
            } else if(token == TOKEN_MISSING_HELIX_PENALTY_SCALED) {
                fin>>MISSING_HELIX_PENALTY_SCALED;
            } else if(token == TOKEN_MISSING_SHEET_PENALTY_SCALED) {
                fin>>MISSING_SHEET_PENALTY_SCALED;
            } else if(token == TOKEN_EUCLIDEAN_LOOP_PENALTY) {
                fin>>EUCLIDEAN_LOOP_PENALTY;
            } else if(token == TOKEN_START_END_MISSING_HELIX_PENALTY) {
                fin>>START_END_MISSING_HELIX_PENALTY;
            } else if(token == TOKEN_HELIX_WEIGHT_COEFFICIENT) {
                fin>>HELIX_WEIGHT_COEFFICIENT;
            } else if(token == TOKEN_LOOP_WEIGHT_COEFFICIENT) {
                fin>>LOOP_WEIGHT_COEFFICIENT;
            } else if(token == TOKEN_SHEET_CAPACITY_COEFFICIENT) {
                fin>>SHEET_CAPACITY_COEFFICIENT;
            } else if(token == TOKEN_SHEET_MERGE_THRESHOLD) {
                fin>>SHEET_MERGE_THRESHOLD;
            } else if(token == TOKEN_MISSING_HELIX_LENGTH) {
                fin>>MISSING_HELIX_LENGTH;
            } else if(token == TOKEN_MISSING_SHEET_LENGTH) {
                fin>>MISSING_SHEET_LENGTH;
            } else if(token == TOKEN_SHEET_SELF_LOOP_LENGTH) {
                fin>>SHEET_SELF_LOOP_LENGTH;
            } else if(token == TOKEN_SHEET_WEIGHT_COEFFICIENT) {
                fin>>SHEET_WEIGHT_COEFFICIENT;
            } else if(token == TOKEN_COST_FUNCTION) {
                fin>>COST_FUNCTION;
            } else if(token == TOKEN_INCLUDE_STRANDS) {
                fin>>INCLUDE_STRANDS;
            } else if(token == TOKEN_VOXEL_SIZE) {
                fin>>VOXEL_SIZE;
            } else if(token == TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES) {
                fin>>TRANSLATE_VOLUMETRIC_COORDINATES;
            } else if(token == TOKEN_MISSING_HELIX_COUNT) {
                fin>>MISSING_HELIX_COUNT;
            } else if(token == TOKEN_MISSING_SHEET_COUNT) {
                fin>>MISSING_SHEET_COUNT;
            } else if(token == TOKEN_EUCLIDEAN_VOXEL_TO_PDB_RATIO) {
                fin>>EUCLIDEAN_VOXEL_TO_PDB_RATIO;
            } else if(token == TOKEN_NODE_CONSTRAINT) {
                fin>>tempInt1>>tempInt2;
                AddNodeConstraint(tempInt1, tempInt2);
            } else if(token == TOKEN_HELIX_CONSTRAINT) {
                fin>>tempInt1>>tempInt2;
                AddHelixConstraint(tempInt1, tempInt2);
            } else if(token == TOKEN_NODE_MISMATCH) {
                fin>>tempInt1>>tempInt2;
                AddNodeMismatch(tempInt1, tempInt2);
            } else if(token == TOKEN_HELIX_MISMATCH) {
                fin>>tempInt1>>tempInt2;
                AddHelixMismatch(tempInt1, tempInt2);
            } else if(token == TOKEN_SOLUTION) {
                fin>>tempInt1>>tempInt2;
                SOLUTION[tempInt1-1] = tempInt2;
            }
        }
        fin.close();
    }


    void DisplayConstants()
    {
    #ifdef VERBOSE
        std::cout << "Constants...\n";
        std::cout << "\tMAXIMUM_DISTANCE_SHEET_SKELETON  = " << MAXIMUM_DISTANCE_SHEET_SKELETON                                         <<std::endl;
        std::cout << "\tMINIMUM_SHEET_SIZE               = " << MINIMUM_SHEET_SIZE                                                      <<std::endl;
        std::cout << "\tEUCLIDEAN_DISTANCE_THRESHOLD     = " << EUCLIDEAN_DISTANCE_THRESHOLD                                            <<std::endl;
        std::cout << "\tBORDER_MARGIN_THRESHOLD          = " << BORDER_MARGIN_THRESHOLD                                                 <<std::endl;
        std::cout << "\tNORMALIZE_GRAPHS                 = " << NORMALIZE_GRAPHS                                                        <<std::endl;
        std::cout << "\tTRANSLATE_VOLUMETRIC_COORDINATES = " << TRANSLATE_VOLUMETRIC_COORDINATES                                        <<std::endl;
        std::cout << "\tMISSING_HELIX_PENALTY            = " << MISSING_HELIX_PENALTY                                                   <<std::endl;
        std::cout << "\tMISSING_SHEET_PENALTY            = " << MISSING_SHEET_PENALTY                                                   <<std::endl;
        std::cout << "\tMISSING_HELIX_PENALTY_SCALED     = " << MISSING_HELIX_PENALTY_SCALED                                            <<std::endl;
        std::cout << "\tMISSING_SHEET_PENALTY_SCALED     = " << MISSING_SHEET_PENALTY_SCALED                                            <<std::endl;
        std::cout << "\tEUCLIDEAN_LOOP_PENALTY           = " << EUCLIDEAN_LOOP_PENALTY                                                  <<std::endl;
        std::cout << "\tSTART_END_MISSING_HELIX_PENALTY  = " << START_END_MISSING_HELIX_PENALTY                                         <<std::endl;
        std::cout << "\tHELIX_WEIGHT_COEFFICIENT         = " << HELIX_WEIGHT_COEFFICIENT                                                <<std::endl;
        std::cout << "\tLOOP_WEIGHT_COEFFICIENT          = " << LOOP_WEIGHT_COEFFICIENT                                                 <<std::endl;
        std::cout << "\tSHEET_WEIGHT_COEFFICIENT         = " << SHEET_WEIGHT_COEFFICIENT                                                <<std::endl;
        std::cout << "\tSHEET_CAPACITY_COEFFICIENT       = " << SHEET_CAPACITY_COEFFICIENT                                              <<std::endl;
        std::cout << "\tSHEET_MERGE_THRESHOLD            = " << SHEET_MERGE_THRESHOLD                                                   <<std::endl;
        std::cout << "\tMISSING_HELIX_LENGTH             = " << MISSING_HELIX_LENGTH                                                    <<std::endl;
        std::cout << "\tMISSING_SHEET_LENGTH             = " << MISSING_SHEET_LENGTH                                                    <<std::endl;
        std::cout << "\tSHEET_SELF_LOOP_LENGTH           = " << SHEET_SELF_LOOP_LENGTH                                                  <<std::endl;
        std::cout << "\tCOST_FUNCTION                    = " << COST_FUNCTION    <<" -- 1 : |a-b|       2 : |a-b|/(a+b)      3:|a-b|^2" <<std::endl;
        std::cout << "\tINCLUDE_STRANDS                  = " << INCLUDE_STRANDS  <<" -- 0 : no       1 : yes"                           <<std::endl;
        std::cout << "\tVOXEL_SIZE                       = " << VOXEL_SIZE                                                              <<std::endl;
        std::cout << "\tMISSING_HELIX_COUNT              = " << MISSING_HELIX_COUNT                                                     <<std::endl;
        std::cout << "\tMISSING_SHEET_COUNT              = " << MISSING_SHEET_COUNT                                                     <<std::endl;
        std::cout << "\tEUCLIDEAN_VOXEL_TO_PDB_RATIO     = " << EUCLIDEAN_VOXEL_TO_PDB_RATIO                                            <<std::endl;
        std::cout << "\tNODE_CONSTRAINTS                 = ";
        for(int i = 0 ; i < MAX_NODES; i++) {
            if(allowedConstraintCount[i] > 0) {
                std::cout<<"("<<i+1<<" -";
                for(unsigned int j = 0; j < allowedConstraintCount[i]; j++) {
                    std::cout<<" "<< allowedConstraintCollection[i][j];
                }
                std::cout<<") ";
            }
        }
        std::cout<<"\n\tNODE_MISMATCHES                  = ";
        for(int i = 0 ; i < MAX_NODES; i++) {
            int x = notAllowedConstraintCount[i];
            if(x > 0) {
                std::cout<<"("<<i+1<<" -";
                for(int j = 0; j < x; j++) {
                    std::cout<<" "<<notAllowedConstraintCollection[i][j];
                }
                std::cout<<") ";
            }
        }
        std::cout<<"\n\tSOLUTION                         = ";
        for(int i = 0 ; i < MAX_NODES; i++) {
            if(SOLUTION[i] != -2) {
                std::cout<<SOLUTION[i]<<" ";
            } else {
                std::cout<<".. ";
            }
        }

        std::cout<<"\n\n";
    #endif //VERBOSE
    }

    bool IsNodeAssignmentAllowed(int patternNode, int baseNode) {
        bool isAllowed;

        // Returning true if no constraints are specified.
        if(allowedConstraintCount[patternNode-1] == 0) {
            isAllowed = true;
        } else {
            isAllowed = false;
            for(unsigned int i = 0; i < allowedConstraintCount[patternNode-1]; i++) {
                isAllowed = isAllowed || (baseNode == allowedConstraintCollection[patternNode-1][i]);
            }
        }

        for(unsigned int i = 0; i < notAllowedConstraintCount[patternNode-1]; i++) {
            isAllowed = isAllowed && (baseNode != notAllowedConstraintCollection[patternNode-1][i]);
        }
        return isAllowed;
    }

    bool IsNodeConstrained(int patternNode) {
        return (allowedConstraintCount[patternNode-1] != 0) || (notAllowedConstraintCount[patternNode-1] != 0);
    }
}
#endif
