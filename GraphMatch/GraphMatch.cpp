/*


Author: Sasakthi S. Abeysinghe
Date  : 01/23/2006
*/

#include <stdlib.h>
#include <stdio.h>
#include "StandardGraph.h"
#include "Matcher7.h"
#include "WongMatchMissing.h"
#include "WongMatchMissing15.h"
#include "WongMatchMissing15Linked.h"
#include "PDBReader.h"
#include "SkeletonReader.h"
#include "GlobalConstants.h"
#include "GraphGenerator.h"
#include <time.h>



void DisplayInputFormat()
{
	printf("Usage: GraphMatch <settings file>\n\n") ;
	printf("    <settings file> \t The file containing the settings \n") ;
}

void DoPerformanceComparison(StandardGraph * patternGraph, StandardGraph * baseGraph, double & ourTime, double & wongTime, double & wongTime15, double & wongTime2, double & vfTime, double & vf2Time, double & inexactTime) 
{
	// Things to undo: Normalize Matcher7, WongMatch, Printfs in Matcher7

	clock_t start, finish;




	// Wong Matching Inexact
	WongMatchMissing * wongMatchMissing;
	wongMatchMissing = new WongMatchMissing(patternGraph, baseGraph);
	start = clock();
	wongMatchMissing->RunMatching(start);
	finish = clock();
	wongMatchMissing->SaveResults();
	inexactTime += ((double) (finish - start) / (double) CLOCKS_PER_SEC);
	delete wongMatchMissing;

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


}

void DoGraphMatching(StandardGraph * patternGraph, StandardGraph * baseGraph) 
{
	clock_t start, finish;


	//// Match Graphs
	//Matcher7 * matcher7;
	//if(MISSING_HELIX_COUNT == -1) {
	//	matcher7 = new Matcher7(patternGraph, baseGraph);
	//} else {
	//	matcher7 = new Matcher7(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	//}
	//start = clock();
	//matcher7->RunMatching();
	//finish = clock();
	//matcher7->SaveResults();
	//printf("\n\tDP Matching process Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
	//
	//// Clean
	//delete(matcher7);


	//// Match Graphs
	//WongMatchMissing * matcher;
	//if(MISSING_HELIX_COUNT == -1) {
	//	matcher = new WongMatchMissing(patternGraph, baseGraph);
	//} else {
	//	matcher = new WongMatchMissing(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	//}
	//start = clock();
	//matcher->RunMatching(start);
	//matcher->SaveResults();
	//
	//// Clean
	//delete(matcher);

	//// Match Graphs
	//WongMatchMissing15 * matcher15;
	//if(MISSING_HELIX_COUNT == -1) {
	//	matcher15 = new WongMatchMissing15(patternGraph, baseGraph);
	//} else {
	//	matcher15 = new WongMatchMissing15(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	//}
	//start = clock();
	//matcher15->RunMatching(start);
	//matcher15->SaveResults();
	//
	//// Clean
	//delete(matcher15);

	// Match Graphs
	//char ch = getchar();
	WongMatchMissing15Linked * matcher15linked;
	if(MISSING_HELIX_COUNT == -1) {
		matcher15linked = new WongMatchMissing15Linked(patternGraph, baseGraph);
	} else {
		matcher15linked = new WongMatchMissing15Linked(patternGraph, baseGraph, MISSING_HELIX_COUNT, MISSING_SHEET_COUNT);
	}
	//ch = getchar();
	start = clock();
	matcher15linked->RunMatching(start);
	//ch = getchar();
	matcher15linked->SaveResults();
	//ch = getchar();
	
	// Clean
	delete matcher15linked;
	//ch = getchar();
}



int main( int args, char * argv[] ) {

	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
	clock_t start, finish;

	// Input Validation and graph loading.
	if(args == 1) {		
		PERFORMANCE_COMPARISON_MODE = true;
		srand(800);
		FILE * outFile = fopen("PerformanceResults.txt", "w");
		fprintf(outFile, "NodeCount, Density, OurAlgo, Wong, Wong2, VF, VF2, Inexact Matching\n");
		printf("NodeCount\tDensity\t\tGraph Num\tWong\t\tWong1.5\t\tWong2\t\tInexact\n");
		double ourTime, wongTime, wongTime15, wongTime2, vfTime, vf2Time, inexactTime;

		for(int density = 0; density <= 100; density+= 5) {
			for(int i = 2; i <= MAX_NODES-10; i+=2) {
				//printf("%d: ", i);
				ourTime = 0;
				wongTime = 0;
				wongTime15 = 0;
				wongTime2 = 0;
				vfTime = 0;
				vf2Time = 0;
				inexactTime = 0;
				for(int j = 0; j < 5; j++) {
					patternGraph = GraphGenerator::GeneratePDBGraph(i);
					baseGraph = GraphGenerator::GenerateSkeletonGraph(patternGraph, density);
					//printf("%d\t\t%d\t\t%d\t\t", density, i, j+1);
					DoPerformanceComparison(patternGraph, baseGraph, ourTime, wongTime, wongTime15, wongTime2, vfTime, vf2Time, inexactTime);
					//printf("\n");
					delete(baseGraph);
					delete(patternGraph);
				}
				fprintf(outFile, "%d, %d, %f, %f, %f, %f, %f\n", i, density, ourTime, wongTime, wongTime15, wongTime2, inexactTime);
				fflush(outFile);
				printf("%d\t\t%d\t\t%f\t%f\t%f\t%f\t%f\n", i, density, ourTime, wongTime, wongTime15, wongTime2, inexactTime);
			}
		}
		fclose(outFile);
	} else if(args == 2) {
		PERFORMANCE_COMPARISON_MODE = false;
		LoadConstantsFromFile(argv[1]);
		DisplayConstants();
		printf("Pattern Graph \n");
		start = clock();
		patternGraph = PDBReader::ReadFile(PDB_FILE_NAME);
		finish = clock();
		printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
		patternGraph->PrintGraph();

		printf("\n\nBase Graph \n");		
		start = clock();
		baseGraph = SkeletonReader::ReadFile(MRC_FILE_NAME, VRML_HELIX_FILE_NAME, SSE_FILE_NAME, VRML_SHEET_FILE_NAME);
		finish = clock();
		printf("\tReading Base file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
		baseGraph->PrintGraph();
		DoGraphMatching(patternGraph, baseGraph);
		delete(baseGraph);
		delete(patternGraph);
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




