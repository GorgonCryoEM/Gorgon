#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#include "GrayImageSkeletonizer.h"
#include "VolumeReaderRAW.h"
#include "VolumeFormatConverter.h"
#include <sstream> 
#include <string>  


using namespace wustl_mm::GraySkeletonCPP;
using namespace std;

int StringToInt(const string &s)
{
  istringstream myStream(s);
  int i;
  
  myStream>>i;
  return i;
}

void DisplayInputParams() {
	printf("The input parameters for GraySkeletonCPP is as follows\n");
	printf("\tGraySkeletonCPP.exe [dimensions] [inputfile] [outfile]\n\n");
	printf("\t[dimensions]: The number of dimensions\n");
	printf("\t[inputfile] : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]   : the filename to be used when generating the skeleton\n");
	printf("\n");
	printf("To convert file formats\n");
	printf("\tGraySkeletonCPP.exe [inputfile] [inputformat] [outfile] [outputformat] [x-size] [y-size] [z-size]\n\n");
	printf("\t[inputfile] : The source file\n");
	printf("\t[outfile]   : The destination file\n");
	printf("\t[x-size]    : The size of the data set in the x-Dimension\n");
	printf("\t[y-size]    : The size of the data set in the y-Dimension\n");
	printf("\t[z-size]    : The size of the data set in the z-Dimension\n");
}

int main( int args, char * argv[] ) {
	switch(args){
		case 4: 
			if(strcmp(argv[1], "2") == 0) {
				GrayImageSkeletonizer * skeletonizer2D = new GrayImageSkeletonizer();
				skeletonizer2D->PerformJuSkeletonization(argv[2], argv[3]);
				delete skeletonizer2D;
			} else if(strcmp(argv[1], "3") == 0) {
				VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer();
				skeletonizer3D->PerformJuSkeletonization(argv[2], argv[3]);
				delete skeletonizer3D;
			} else {
				DisplayInputParams();
			}		
			break;
		case 8:
			VolumeFormatConverter::ConvertVolume(argv[1], argv[2], argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), StringToInt(argv[7]));
			break;
		default:
			DisplayInputParams();
	}
}

#endif 