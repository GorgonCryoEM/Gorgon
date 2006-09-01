/*
A static store of global constants

Author: Sasakthi S. Abeysinghe
Date  : 02/06/2006
*/

#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

//#define INCLUDE_SHEETS
//#define GET_STATS

const int NOPATHPENALTY = 1;
const int TYPEMISMATCHPENALTY = 1;

const int PRIORITYQUEUESIZE = 50000000;
const int RESULT_COUNT = 20;
const int MAX_NODES = 40;
const int MAXINT = 2147483647;
const unsigned int MAXUNSIGNEDINT = 4294967295;
const double MAXDOUBLE = 1.7E308;
const int GRAPHEDGE_HELIX = 0;
const int GRAPHEDGE_LOOP = 1;
const int GRAPHEDGE_SHEET = 2;
const int GRAPHEDGE_OTHER = 3;
const int GRAPHEDGE_LOOP_EUCLIDEAN = 4;
const char * TOKEN_PDB_HELIX = "HELIX";
const char * TOKEN_PDB_ATOM = "ATOM";
const char * TOKEN_PDB_SHEET = "SHEET";
const char * TOKEN_VRML_TRANSLATION = "translation";
const char * TOKEN_VRML_ROTATION = "rotation";
const char * TOKEN_VRML_HEIGHT = "height";
const char * TOKEN_VRML_RADIUS = "radius";
const char * TOKEN_VRML_SHAPE = "Shape";
const char * TOKEN_VRML_POINT = "point";
const char * TOKEN_VRML_COORDINDEX = "coordIndex";
const char * TOKEN_SSE_ALPHA = "ALPHA";
const char * TOKEN_SSE_FILE_NAME = "SSE_FILE_NAME";
const char * TOKEN_VRML_HELIX_FILE_NAME = "VRML_HELIX_FILE_NAME";
const char * TOKEN_VRML_SHEET_FILE_NAME = "VRML_SHEET_FILE_NAME";
const char * TOKEN_PDB_FILE_NAME = "PDB_FILE_NAME";
const char * TOKEN_MRC_FILE_NAME = "MRC_FILE_NAME";
const char * TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD = "EUCLIDEAN_DISTANCE_THRESHOLD";
const char * TOKEN_BORDER_MARGIN_THRESHOLD = "BORDER_MARGIN_THRESHOLD";
const char * TOKEN_NORMALIZE_GRAPHS = "NORMALIZE_GRAPHS";
const char * TOKEN_MISSING_HELIX_PENALTY = "MISSING_HELIX_PENALTY";
const char * TOKEN_EUCLIDEAN_LOOP_PENALTY = "EUCLIDEAN_LOOP_PENALTY";
const char * TOKEN_START_END_MISSING_HELIX_PENALTY = "START_END_MISSING_HELIX_PENALTY";
const char * TOKEN_HELIX_WEIGHT_COEFFICIENT = "HELIX_WEIGHT_COEFFICIENT";
const char * TOKEN_LOOP_WEIGHT_COEFFICIENT = "LOOP_WEIGHT_COEFFICIENT";
const char * TOKEN_MISSING_HELIX_LENGTH = "MISSING_HELIX_LENGTH";
const char * TOKEN_SHEET_WEIGHT_COEFFICIENT = "SHEET_WEIGHT_COEFFICIENT";
const char * TOKEN_COST_FUNCTION = "COST_FUNCTION";
const char * TOKEN_VOXEL_SIZE = "VOXEL_SIZE";
const char * TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES = "TRANSLATE_VOLUMETRIC_COORDINATES";
const char * TOKEN_MISSING_HELIX_COUNT = "MISSING_HELIX_COUNT";
const char * TOKEN_MISSING_SHEET_COUNT = "MISSING_SHEET_COUNT";
const int MAX_RANDOM_HELIX_SIZE = 30;
const int MAX_RANDOM_LOOP_SIZE = 30;
const int WONG_HASH_TABLE_SIZE = 1024;

char SSE_FILE_NAME[100];
char VRML_HELIX_FILE_NAME[100];
char VRML_SHEET_FILE_NAME[100];
char PDB_FILE_NAME[100];
char MRC_FILE_NAME[100];
double EUCLIDEAN_DISTANCE_THRESHOLD = 15;
int BORDER_MARGIN_THRESHOLD = 3;
bool NORMALIZE_GRAPHS = true;
double MISSING_HELIX_PENALTY = 2;
double EUCLIDEAN_LOOP_PENALTY = 5;
double START_END_MISSING_HELIX_PENALTY = 5;
double HELIX_WEIGHT_COEFFICIENT = 1.0;
double LOOP_WEIGHT_COEFFICIENT = 0.25;
double MISSING_HELIX_LENGTH = 8;
double SHEET_WEIGHT_COEFFICIENT = 1.0;
int COST_FUNCTION = 1;   // 1 : |a-b|		2 : |a-b|/(a+b)		3:|a-b|^2
double VOXEL_SIZE = 1;
bool TRANSLATE_VOLUMETRIC_COORDINATES = false;
int MISSING_HELIX_COUNT = -1;
int MISSING_SHEET_COUNT = -1;
bool PERFORMANCE_COMPARISON_MODE = false;

void LoadConstantsFromFile(char * settingsFile) {
	FILE* fin = fopen(settingsFile, "rt");
	if (fin == NULL) {
		printf("Error reading input file %s.\n", settingsFile) ;
		exit(0) ;
	}

	char token[80];
	while (!feof(fin)) {
		fscanf(fin, "%s", token);
		if(strcmp(token, TOKEN_SSE_FILE_NAME) == 0) {
			fscanf(fin, "%s", &SSE_FILE_NAME);
		} else if(strcmp(token, TOKEN_VRML_HELIX_FILE_NAME) == 0) {
			fscanf(fin, "%s", &VRML_HELIX_FILE_NAME);
		} else if(strcmp(token, TOKEN_VRML_SHEET_FILE_NAME) == 0) {
			fscanf(fin, "%s", &VRML_SHEET_FILE_NAME);
		} else if(strcmp(token, TOKEN_PDB_FILE_NAME) == 0) {
			fscanf(fin, "%s", &PDB_FILE_NAME);
		} else if(strcmp(token, TOKEN_MRC_FILE_NAME) == 0) {
			fscanf(fin, "%s", &MRC_FILE_NAME);
		} else if(strcmp(token, TOKEN_EUCLIDEAN_DISTANCE_THRESHOLD) == 0) {
			fscanf(fin, "%lf", &EUCLIDEAN_DISTANCE_THRESHOLD);
		} else if(strcmp(token, TOKEN_BORDER_MARGIN_THRESHOLD) == 0) {
			fscanf(fin, "%d", &BORDER_MARGIN_THRESHOLD);
		} else if(strcmp(token, TOKEN_NORMALIZE_GRAPHS) == 0) {
			fscanf(fin, "%d", &NORMALIZE_GRAPHS);
		} else if(strcmp(token, TOKEN_MISSING_HELIX_PENALTY) == 0) {
			fscanf(fin, "%lf", &MISSING_HELIX_PENALTY);
		} else if(strcmp(token, TOKEN_EUCLIDEAN_LOOP_PENALTY) == 0) {
			fscanf(fin, "%lf", &EUCLIDEAN_LOOP_PENALTY);
		} else if(strcmp(token, TOKEN_START_END_MISSING_HELIX_PENALTY) == 0) {
			fscanf(fin, "%lf", &START_END_MISSING_HELIX_PENALTY);
		} else if(strcmp(token, TOKEN_HELIX_WEIGHT_COEFFICIENT) == 0) {
			fscanf(fin, "%lf", &HELIX_WEIGHT_COEFFICIENT);
		} else if(strcmp(token, TOKEN_LOOP_WEIGHT_COEFFICIENT) == 0) {
			fscanf(fin, "%lf", &LOOP_WEIGHT_COEFFICIENT);
		} else if(strcmp(token, TOKEN_MISSING_HELIX_LENGTH) == 0) {
			fscanf(fin, "%lf", &MISSING_HELIX_LENGTH);
		} else if(strcmp(token, TOKEN_SHEET_WEIGHT_COEFFICIENT) == 0) {
			fscanf(fin, "%lf", &SHEET_WEIGHT_COEFFICIENT);
		} else if(strcmp(token, TOKEN_COST_FUNCTION) == 0) {
			fscanf(fin, "%d", &COST_FUNCTION);
		} else if(strcmp(token, TOKEN_VOXEL_SIZE) == 0) {
			fscanf(fin, "%lf", &VOXEL_SIZE);
		} else if(strcmp(token, TOKEN_TRANSLATE_VOLUMETRIC_COORDINATES) == 0) {
			fscanf(fin, "%d", &TRANSLATE_VOLUMETRIC_COORDINATES);
		} else if(strcmp(token, TOKEN_MISSING_HELIX_COUNT) == 0) {
			fscanf(fin, "%d", &MISSING_HELIX_COUNT);
		} else if(strcmp(token, TOKEN_MISSING_SHEET_COUNT) == 0) {
			fscanf(fin, "%d", &MISSING_SHEET_COUNT);
		}
	}
	fclose(fin);
}


void DisplayConstants()
{
	printf("Constants...\n");
	printf("\tEUCLIDEAN_DISTANCE_THRESHOLD     = %f\n", EUCLIDEAN_DISTANCE_THRESHOLD);
	printf("\tBORDER_MARGIN_THRESHOLD          = %d\n", BORDER_MARGIN_THRESHOLD);
	printf("\tNORMALIZE_GRAPHS                 = %d\n", NORMALIZE_GRAPHS);
	printf("\tTRANSLATE_VOLUMETRIC_COORDINATES = %d\n", TRANSLATE_VOLUMETRIC_COORDINATES);
	printf("\tMISSING_HELIX_PENALTY            = %f\n", MISSING_HELIX_PENALTY);
	printf("\tEUCLIDEAN_LOOP_PENALTY           = %f\n", EUCLIDEAN_LOOP_PENALTY);
	printf("\tSTART_END_MISSING_HELIX_PENALTY  = %f\n", START_END_MISSING_HELIX_PENALTY);
	printf("\tHELIX_WEIGHT_COEFFICIENT         = %f\n", HELIX_WEIGHT_COEFFICIENT);
	printf("\tLOOP_WEIGHT_COEFFICIENT          = %f\n", LOOP_WEIGHT_COEFFICIENT);
	printf("\tSHEET_WEIGHT_COEFFICIENT         = %f\n", SHEET_WEIGHT_COEFFICIENT);
	printf("\tMISSING_HELIX_LENGTH             = %f\n", MISSING_HELIX_LENGTH);
	printf("\tCOST_FUNCTION                    = %d -- 1 : |a-b|       2 : |a-b|/(a+b)      3:|a-b|^2\n", COST_FUNCTION);
	printf("\tVOXEL_SIZE                       = %f\n", VOXEL_SIZE);
	printf("\tMISSING_HELIX_COUNT              = %d\n", MISSING_HELIX_COUNT);
	printf("\tMISSING_SHEET_COUNT              = %d\n\n", MISSING_SHEET_COUNT);
}







#endif



