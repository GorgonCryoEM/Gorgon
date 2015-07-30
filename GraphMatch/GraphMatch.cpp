// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the GRAPHMATCH module.



#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <cstdlib>
#include <cstdio>
#include "StandardGraph.h"
#include "WongMatch15Constrained.h"
#include "WongMatch15ConstrainedNoFuture.h"
#include "WongMatch15ConstrainedOnlyA.h"
#include "PDBReader.h"
#include "SkeletonReader.h"
#include "GlobalConstants.h"
#include "GraphGenerator.h"
#include "BackEndInterface.h"
#include "PDBHelix.h"
#include <ctime>

#define DllExport   __declspec( dllexport )



using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::SkeletonMaker;

void DisplayInputFormat()
{
	printf("Usage: GraphMatch <settings file>\n\n") ;
	printf("    <settings file> \t The file containing the settings \n") ;
}

//void DoPerformanceComparison(StandardGraph * patternGraph, StandardGraph * baseGraph, double & ourTime, double & wongTime, double & wongTime15, double & wongTime2, double & vfTime, double & vf2Time, double & inexactTime) 
//{
	// Things to undo: Normalize Matcher7, WongMatch, Printfs in Matcher7

	//clock_t start, finish;




	//// Wong Matching Inexact
	//WongMatchMissing * wongMatchMissing;
	//wongMatchMissing = new WongMatchMissing(patternGraph, baseGraph);
	//start = clock();
	//wongMatchMissing->RunMatching(start);
	//finish = clock();
	//wongMatchMissing->SaveResults();
	//inexactTime += ((double) (finish - start) / (double) CLOCKS_PER_SEC);
	//delete wongMatchMissing;

	//// VF Matching Algorithm
	//VFMatch * matcher2;
	//if(MISSING_HELIX_COUNT == -1) {
	//	matcher2 = new VFMatch(patternGraph, baseGraph);
	//} else {
	//	matcher2 = new VFMatch(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	//}

	//start = clock();
	//matcher2->RunMatchingVF();
	//finish = clock();
	//vfTime += ((double) (finish - start) / (double) CLOCKS_PER_SEC);

	//start = clock();
	//matcher2->RunMatchingVF2();
	//finish = clock();
	//vf2Time += ((double) (finish - start) / (double) CLOCKS_PER_SEC);

	//delete(matcher2);

	//VFInexact * matcherInexact = new VFInexact(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	//start = clock();
	////matcherInexact->PerformMatching();
	//finish = clock();
	//inexactTime += ((double) (finish - start) / (double) CLOCKS_PER_SEC);

	//delete(matcherInexact);


//}



int main( int args, char * argv[] ) {


	//StandardGraph * patternGraph;
	//StandardGraph * baseGraph;
	//clock_t start, finish;

	// Input Validation and graph loading.
	//if(args == 1) {		
	//	PERFORMANCE_COMPARISON_MODE = true;
	//	srand(800);
	//	FILE * outFile = fopen("PerformanceResults.txt", "w");
	//	fprintf(outFile, "NodeCount, Density, OurAlgo, Wong, Wong2, VF, VF2, Inexact Matching\n");
	//	printf("NodeCount\tDensity\t\tGraph Num\tWong\t\tWong1.5\t\tWong2\t\tInexact\n");
	//	double ourTime, wongTime, wongTime15, wongTime2, vfTime, vf2Time, inexactTime;

	//	for(int density = 0; density <= 100; density+= 5) {
	//		for(int i = 2; i <= MAX_NODES-10; i+=2) {
	//			//printf("%d: ", i);
	//			ourTime = 0;
	//			wongTime = 0;
	//			wongTime15 = 0;
	//			wongTime2 = 0;
	//			vfTime = 0;
	//			vf2Time = 0;
	//			inexactTime = 0;
	//			for(int j = 0; j < 5; j++) {
	//				patternGraph = GraphGenerator::GeneratePDBGraph(i);
	//				baseGraph = GraphGenerator::GenerateSkeletonGraph(patternGraph, density);
	//				//printf("%d\t\t%d\t\t%d\t\t", density, i, j+1);
	//				DoPerformanceComparison(patternGraph, baseGraph, ourTime, wongTime, wongTime15, wongTime2, vfTime, vf2Time, inexactTime);
	//				//printf("\n");
	//				delete(baseGraph);
	//				delete(patternGraph);
	//			}
	//			fprintf(outFile, "%d, %d, %f, %f, %f, %f, %f\n", i, density, ourTime, wongTime, wongTime15, wongTime2, inexactTime);
	//			fflush(outFile);
	//			printf("%d\t\t%d\t\t%f\t%f\t%f\t%f\t%f\n", i, density, ourTime, wongTime, wongTime15, wongTime2, inexactTime);
	//		}
	//	}
	//	fclose(outFile);
	//} else 
	if(args == 2) {
		BackEndInterface i;
		i.SetConstantsFromFile(argv[1]);		
		DisplayConstants();
		// -------------
		//patternGraph = i.LoadSequenceGraph();
		//baseGraph = i.LoadSkeletonGraph();
		//i.ExecuteQuery(patternGraph, baseGraph);
		//delete(patternGraph);
		//delete(baseGraph);
		// Had to change BackEndInterface for python interface
		//
		i.LoadSequenceGraph();
		i.LoadSkeletonGraph();
		i.ExecuteQuery();
		// -------------

		i.CleanupMemory();
	} else if((args == 3) && (strcmp(argv[1], "Mathematica") == 0)) {
		Volume * vol = (MRCReaderPicker::pick(argv[2]))->getVolume();
		vol->toMathematicaFile("myVolume.nb");
		delete(vol);
	} 
	//else if((args == 5) && (strcmp(argv[1], "1") == 0)) {
	//	DisplayConstants();

	//	start = clock();
	//	patternGraph = new StandardGraph(argv[2]);
	//	finish = clock();
	//	printf("\tReading Pattern file Took %f seconds.\n\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;

	//	start = clock();
	//	baseGraph = new StandardGraph(argv[3]);		
	//	finish = clock();
	//	printf("\tReading Base file Took %f seconds.\n\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
	//}
	else {
		DisplayInputFormat();
		exit(0);
	}
}
