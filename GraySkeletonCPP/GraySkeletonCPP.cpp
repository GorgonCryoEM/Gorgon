#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#include "GrayImageSkeletonizer.h"
#include "VolumeReaderRAW.h"
#include "VolumeFormatConverter.h"
#include <sstream> 
#include <string>  
#include <time.h>
#include "DiscreteMesh.h"


using namespace wustl_mm::GraySkeletonCPP;
using namespace std;


const int DO_SKELETONIZATION_AND_PRUNING = 0;
const int DO_SKELETONIZATION = 1;
const int DO_PRUNING = 2;
const int DO_CONVERSION = 3;

string DoubleToString(double number) {
	char x[20];
	sprintf(x, "%f", number);
	string retVal = x;
	delete [] x;
	return retVal;
}

double StringToDouble(string s) {
	float retVal = 1;
	sscanf(s.c_str(), "%f", &retVal);
	return retVal;
}
					 

int StringToInt(const string &s)
{
  istringstream myStream(s);
  int i;
  
  myStream>>i;
  return i;
}

void DisplayInputParams() {
	printf("To make and prune the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [threshold]\n\n");
	printf("\t[function]      : %i, Perform skeletonization and pruning\n", DO_SKELETONIZATION_AND_PRUNING);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[threshold]     : The cost threshold (between 0 (all) and 1(none) and (-1 do many threhsolds))\n");
	printf("\n");
	printf("To get the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile]\n\n");
	printf("\t[function]      : %i, Perform skeletonization\n", DO_SKELETONIZATION);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\n");
	printf("To prune the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [inputSkeleton] [outfile] [threshold]\n\n");
	printf("\t[function]      : %i, Perform pruning\n", DO_PRUNING);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[inputSkeleton] : an input image file of the skeleton (BMP if 2D, MRC if 3D)\n");	
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[threshold]     : The cost threshold (between 0 (all) and 1(none))\n");
	printf("\n");
	printf("To convert file formats\n");
	printf("\tGraySkeletonCPP.exe [function] [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]\n\n");
	printf("\t[function]      : %i, Perform conversion\n", DO_CONVERSION);
	printf("\t[inputfile]     : The source file\n");
	printf("\t[inputformat]   : The source file format\n");
	printf("\t[outfile]       : The destination file\n");
	printf("\t[outformat]     : The destination file format\n");
	printf("\t[x-size]        : The size of the data set in the x-Dimension\n");
	printf("\t[y-size]        : The size of the data set in the y-Dimension\n");
	printf("\t[z-size]        : The size of the data set in the z-Dimension\n");
}

int main( int args, char * argv[] ) {

	VolumeSkeletonizer * vs = new VolumeSkeletonizer();
	Volume * v = new Volume(15,15,15);

	for(int x = 3; x<= 9; x++) {
		for(int y = 3; y <= 9; y++) {
			v->setDataAt(x, y, 5, 1);
		}
	}

	//Vector3D * dir = vs->GetSkeletonDirection(v);
	//int index = v->getIndex(5,5,5);
	//printf("{%f, %f, %f}", dir[index].values[0], dir[index].values[1], dir[index].values[2]);


	return 0;

	clock_t start, finish;
	start = clock();
	int format, dimension;
	string inFile, skeletonFile, outFile, outPath;
	GrayImage * sourceImage, * sourceSkeleton, * outputImage;
	GrayImageSkeletonizer * skeletonizer2D;
	GrayImageList * imageList;
	double threshold;

	switch(args){
		// Perform skeletonization
		case 5:			
			format = StringToInt(argv[1]);
			dimension = StringToInt(argv[2]);
			inFile = argv[3];
			outFile = argv[4];
			outPath = outFile.substr(0, outFile.rfind("."));
			if(format == DO_SKELETONIZATION) {
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						outputImage = skeletonizer2D->PerformJuSkeletonization(sourceImage, outPath);
						ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
						delete skeletonizer2D;
						delete sourceImage;
						delete outputImage;
						break;
					case 3:
						break;
				}			
			} else {
				DisplayInputParams();
			}
			break;
		// perform both
		case 6:
			format = StringToInt(argv[1]);
			dimension = StringToInt(argv[2]);
			inFile = argv[3];
			outFile = argv[4];
			threshold = StringToDouble(argv[5]);
			outPath = outFile.substr(0, outFile.rfind("."));

			if(format == DO_SKELETONIZATION_AND_PRUNING) {
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						sourceSkeleton = skeletonizer2D->PerformJuSkeletonization(sourceImage, outPath);						
						if(threshold < 0) {
							imageList = new GrayImageList();
							imageList->AddImage(sourceImage);
							imageList->AddImage(sourceSkeleton);					
							for(double t = 0.0; t <= 1.0; t+= 0.1) {
								imageList->AddImage(skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, t, outPath));
							}
							outputImage = imageList->GetCompositeImage(4);
							imageList->Clear(true);
							delete imageList;
						} else {
							outputImage = skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, threshold, outPath);
						}
						ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
						delete skeletonizer2D;
						delete outputImage;
						
						break;
					case 3:
						break;
				}
			}
			break;
		// Perform pruning
		case 7: 
			format = StringToInt(argv[1]);
			dimension = StringToInt(argv[2]);
			inFile = argv[3];
			skeletonFile = argv[4];
			outFile = argv[5];
			threshold = StringToDouble(argv[6]);
			outPath = outFile.substr(0, outFile.rfind("."));			
			if(format == DO_PRUNING) {
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						sourceSkeleton = ImageReaderBMP::LoadGrayscaleImage(skeletonFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						outputImage = skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, threshold, outPath);
						ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
						delete skeletonizer2D;
						delete sourceSkeleton;
						delete sourceImage;
						delete outputImage;
						break;
					case 3:
						break;
				}

			} else {
				DisplayInputParams();
			}	
			break;
		case 9:
			format = StringToInt(argv[1]);
			if(format == DO_CONVERSION) {
				VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
			} else {
				DisplayInputParams();
			}
			break;
		default:
			DisplayInputParams();
	}
	finish = clock();
	double timeTaken = ((double) (finish - start) / (double) CLOCKS_PER_SEC);
	printf("\n%f seconds taken!\n", timeTaken);
}

#endif 