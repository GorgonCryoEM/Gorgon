// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy cpp file for the FOUNDATION module.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$


#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#ifndef FOUNDATION_SWITCHBOARD_CPP
#define FOUNDATION_SWITCHBOARD_CPP

#include "TimeManager.h"
#include "StringUtils.h"
#include "Octree.h"
#include <vector>


using namespace wustl_mm::Foundation;

int main( int args, char * argv[] ) {
	Octree<bool> * tree = new Octree<bool>(8, 8, 8);
	tree->PrintStructure();
	tree->AddNewLeaf(4, 0, 0, 1);
	printf("\n\n");
	tree->PrintStructure();
	vector<OctreeNode<bool> *> intersects = tree->IntersectRay(Vector3DFloat(0,0,1), Vector3DFloat(5, 1, -10), 0.01);
	printf("Intersections: \n");
	for(unsigned int i = 0; i < intersects.size(); i++) {
		printf("%d %d %d - %d \n", intersects[i]->pos[0], intersects[i]->pos[1], intersects[i]->pos[2], intersects[i]->cellSize); 
	}
	return 0;

}



#endif