#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#include "GrayImageSkeletonizer.h"

using namespace WUSTL_MM;

void DisplayInputParams() {
	printf("The input parameters for GraySkeletonCPP is as follows\n");
	printf("\tGraySkeletonCPP.exe [inputfile] [outfile]\n\n");
	printf("\t[inputfile] : an input 2D image file in the BMP format\n");
	printf("\t[outfile]   : the filename to be used when generating the skeleton\n");
}

int main( int args, char * argv[] ) {
	switch(args){
		case 3: 
			GrayImageSkeletonizer::PerformSkeletonization(argv[1], argv[2]);
			break;
		default:
			DisplayInputParams();
	}
}

#endif 