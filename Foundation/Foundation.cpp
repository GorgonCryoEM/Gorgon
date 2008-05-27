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
	printf("MS VC version: %d\n", _MSC_VER);
	Octree<bool> * tree = new Octree<bool>(8, 8, 8);
	tree->PrintStructure();
	tree->AddNewLeaf(4, 0, 0, 1);
	printf("\n\n");
	tree->PrintStructure();
	vector<OctreeNode<bool> * > neigh = tree->GetNeighbors(tree->GetLeaf(0,0,0));
	for(int i = 0; i < neigh.size(); i++) {
		printf("%ld - %ld %ld %ld\n", neigh[i]->cellSize, neigh[i]->pos[0], neigh[i]->pos[1], neigh[i]->pos[2]);
	}
	return 0;

}



#endif