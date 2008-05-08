#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#ifndef FOUNDATION_SWITCHBOARD_CPP
#define FOUNDATION_SWITCHBOARD_CPP

#include "TimeManager.h"
#include "StringUtils.h"
#include "Octree.h"


using namespace wustl_mm::Foundation;

int main( int args, char * argv[] ) {

	Octree * tree = new Octree(64, 64, 64);
	tree->PrintStructure();
	tree->AddNewLeaf(18, 26, 16, 1);
	printf("\n\n");
	tree->PrintStructure();
	return 0;

}



#endif