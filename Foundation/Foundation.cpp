// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy cpp file for the FOUNDATION module.



#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#ifndef FOUNDATION_SWITCHBOARD_CPP
#define FOUNDATION_SWITCHBOARD_CPP

#include "TimeManager.h"
#include "StringUtils.h"
#include "Octree.h"
#include "GraphBase.h"
#include <vector>
#include <set>
#include "GorgonPriorityQueue.h"
#include "GorgonMergeSort.h"
#include "GorgonBubbleSort.h"
#include "GorgonHeapSort.h"
#include "GorgonQuickSort.h"
#include "GorgonBinarySearchTree.h"
#include <SkeletonMaker/PriorityQueue.h>

using namespace wustl_mm::Foundation;

using namespace wustl_mm::SkeletonMaker;

int main( int args, char * argv[] ) {
	TimeManager myT;

	int qCount = 100000;
	{
		myT.PushCurrentTime();
		PriorityQueue<int, int> t = PriorityQueue<int, int>(qCount);
		srand(0);
		int * v = NULL;
		int k;
		for(unsigned int i = 0; i < qCount; i++) {
			k = rand(); 
			t.add(v, k);
		}

		for(unsigned int i = 0; i < qCount; i++) {
			t.remove(v, k);
		}
		myT.PopAndDisplayTime("Taos %f\n");
	}

	{
		myT.PushCurrentTime();
		GorgonPriorityQueue<int, int *> t = GorgonPriorityQueue<int, int *>(false);
		srand(0);
		int * v = NULL;
		int k;
		for(unsigned int i = 0; i < qCount; i++) {
			k = rand(); 
			t.Add(k, v);
		}

		for(unsigned int i = 0; i < qCount; i++) {
			v = t.PopFirst();
		}
		myT.PopAndDisplayTime("Mine %f\n");
	}

	
	int size = 10;
	bool print = false;

	vector<int> a;
	srand(0);
	for(unsigned int i = 0; i < size; i++) {
		a.push_back(rand());
	}

	TimeManager m;

	m.PushCurrentTime();
	//GorgonBubbleSort<int>::Sort(a, true);
	m.PopAndDisplayTime("\nBubble Sort : %f\n");
	if(print) {
		for(unsigned int i =0; i < a.size(); i++) {
			printf("%d ", a[i]);
		}
		printf("\n");
	}


	a.clear();
	srand(0);
	for(unsigned int i = 0; i < size; i++) {
		a.push_back(rand());
	}

	m.PushCurrentTime();
	GorgonHeapSort<int>::Sort(a, true);
	m.PopAndDisplayTime("\nHeap Sort : %f\n");
	if(print) {
		for(unsigned int i =0; i < a.size(); i++) {
			printf("%d ", a[i]);
		}
		printf("\n");
	}


	a.clear();
	srand(0);
	for(unsigned int i = 0; i < size; i++) {
		a.push_back(rand());
	}

	m.PushCurrentTime();
	GorgonMergeSort<int>::Sort(a, true);
	m.PopAndDisplayTime("\nMerge Sort : %f\n");
	if(print) {
		for(unsigned int i =0; i < a.size(); i++) {
			printf("%d ", a[i]);
		}
		printf("\n");
	}


	a.clear();
	srand(0);
	for(unsigned int i = 0; i < size; i++) {
		a.push_back(rand());
	}

	m.PushCurrentTime();
	GorgonQuickSort<int>::Sort(a, true);
	m.PopAndDisplayTime("\nQuick Sort : %f\n");
	if(print) {
		for(unsigned int i =0; i < a.size(); i++) {
			printf("%d ", a[i]);
		}
		printf("\n");
	}





	/*GorgonBinarySearchTree<int> s;
	s.AddValue(2);	s.Print();
	s.AddValue(4);	s.Print();
	s.AddValue(6);	s.Print();
	s.AddValue(3);	s.Print();
	s.AddValue(5);	s.Print();
	s.AddValue(3);	s.Print();
	s.AddValue(1);	s.Print();

	printf("Finding %d \n", s.GetElement(3));

	s.RemoveValue(5); printf("removed 5 :"); s.Print();	
	s.RemoveValue(1); printf("removed 1 :"); s.Print();
	s.RemoveValue(2); printf("removed 2 :"); s.Print();
	s.RemoveValue(4); printf("removed 4 :"); s.Print();
	s.RemoveValue(6); printf("removed 6 :"); s.Print();
	s.RemoveValue(3); printf("removed 3 :"); s.Print();
	s.RemoveValue(3); printf("removed 3 :"); s.Print();*/
	
	




	/*Octree<bool> * tree = new Octree<bool>(8, 8, 8);
	tree->PrintStructure();
	tree->AddNewLeaf(4, 0, 0, 1);z
	printf("\n\n");
	tree->PrintStructure();
	vector<OctreeNode<bool> *> intersects = tree->IntersectRay(Vector3DFloat(0,0,1), Vector3DFloat(5, 1, -10), 0.01);
	printf("Intersections: \n");
	for(unsigned int i = 0; i < intersects.size(); i++) {
		printf("%d %d %d - %d \n", intersects[i]->pos[0], intersects[i]->pos[1], intersects[i]->pos[2], intersects[i]->cellSize); 
	}
	return 0; */

	//GraphBase<bool, bool> graph;
	//vector<unsigned long long> vertices;

	//for(unsigned int i = 0; i < 10; i++) {		
	//	vertices.push_back(graph.AddVertex(3, false));
	//}
	//printf("Try1 \n");
	//graph.PrintAllCliques(graph.GetAllMaximalCliques());

	//if(graph.IsClique(vertices)) {
	//	printf("clique \n");
	//} else {
	//	printf("not clique \n");
	//}

	//for(unsigned int i = 0; i < 5; i++) {
	//	for(unsigned int j = 0; j < 5; j++) {
	//		graph.AddEdge(i, j, 1.0f, false);
	//	}
	//}

	//printf("Try2 \n");
	//graph.PrintAllCliques(graph.GetAllMaximalCliques());

	//graph.AddEdge(6, 8, 1.0f, false);
	//graph.AddEdge(2, 5, 1.0f, false);
	//printf("ConnComponents\n");
	//graph.PrintAllCliques(graph.GetAllConnectedComponents());

	//printf("Try3 \n");
	//graph.PrintAllCliques(graph.GetAllMaximalCliques());

	//if(graph.IsClique(vertices)) {
	//	printf("clique \n");
	//} else {
	//	printf("not clique \n");
	//}

	//set<unsigned long long> vert;
	//vert.insert(10);
	//vert.insert(2);
	//vert.insert(15);
	//vert.insert(11);


	//set<unsigned long long> vert2;
	//vert2.insert(10);
	//vert2.insert(2);
	//vert2.insert(11);
	//vert2.insert(15);
	//

	//if(vert == vert2) {
	//	printf("Same\n");
	//} else {
	//	printf("not same\n");
	//}


	//TimeManager m;
	//for(int i = 1; i <= 30; i++) {
	//	GraphBase<bool, bool> g;
	//	for(int j = 0; j < i; j++) {
	//		g.AddVertex(0, false);
	//	}

	//	/*m.PushCurrentTime();
	//	g.GetLargestMaximalCliques2();
	//	printf("%d ", i);
	//	m.PopAndDisplayTime(" nodes (empty graph): time = %f \n");*/

	//	for(int j = 0; j < i; j++) {
	//		for(int k = 0; k < i; k++) {
	//			g.AddEdge(j, k, 0, false);
	//		}
	//	}

	//	m.PushCurrentTime();
	//	g.GetLargestMaximalCliques2();
	//	printf("%d ", i);
	//	m.PopAndDisplayTime(" nodes (full graph): time = %f \n");
	//}

	return 0;
}



#endif
