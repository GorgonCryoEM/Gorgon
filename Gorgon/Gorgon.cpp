// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the GORGON module.


#ifndef GORGON_GORGON_CPP
#define GORGON_GORGON_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include "Renderer.h"
#include "VolumeRenderer.h"
#include "MeshRenderer.h"
#include "SSERenderer.h"
#include "InteractiveSkeletonEngine.h"
#include "CAlphaRenderer.h"
#include "FlexibleFittingEngine.h"


unsigned char neighbors[8][3] = {
	{1,4,3},
	{0,2,5},
	{1,3,6},
	{0,7,2},
	{0,5,7},
	{1,6,4},
	{2,7,5},
	{3,4,6}
};

unsigned char edges[8][3] = {
	{0,8,3},
	{0,1,9},
	{1,2,10},
	{3,11,2},
	{8,4,7},
	{9,5,4},
	{10,6,5},
	{11,7,6}
};



void GenerateTriangleTable() {
	char triangleTable[256][15];
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 15; j++) {
			triangleTable[i][j] = -1;
		}
	}

	int triCount;
	bool validCorner;
	for(unsigned char edgeFlags = 0; edgeFlags < 255; edgeFlags++) {
		triCount = 0;
		for(unsigned char corner = 0; corner < 8; corner++) {
			if(edgeFlags & (1<<corner)) {		// Corner is in the outside
				validCorner = true;
				for(int neighborIx = 0; neighborIx < 3; neighborIx++) {		
					validCorner = validCorner && !(edgeFlags & (1<<neighbors[corner][neighborIx]));
				}
				if(validCorner) {// All neighbors are inside
					for(int i = 0; i < 3; i++) {
						triangleTable[edgeFlags][triCount*3+i] = edges[corner][2-i];						
					}
					triCount++;
				}
			} else {			// Corner is in the inside
				validCorner = true;
				for(int neighborIx = 0; neighborIx < 3; neighborIx++) {		
					validCorner = validCorner && (edgeFlags & (1<<neighbors[corner][neighborIx]));
				}
				if(validCorner) {// All neighbors are outside
					for(int i = 0; i < 3; i++) {
						triangleTable[edgeFlags][triCount*3+i] = edges[corner][i];						
					}
					triCount++;
				}
			}

		}
	}

	for(int i = 0; i < 256; i++) {
		printf("{");
		for(int j = 0; j < 14; j++) {
			printf("%d, ", triangleTable[i][j]);
		}
		printf("%d },\n", triangleTable[i][14]);
	}

}

int main( int args, char * argv[] ) {
	GenerateTriangleTable();
}

#endif
