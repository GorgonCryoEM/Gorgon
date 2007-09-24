#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#include "GrayImageSkeletonizer.h"
#include "VolumeReaderRAW.h"
#include "VolumeFormatConverter.h"
#include <sstream> 
#include <string>  
#include <time.h>
#include <math.h>
#include "DiscreteMesh.h"
#include "..\MatlabInterface\DataStructures.h"
#include "..\MatlabInterface\MathLib.h"


using namespace wustl_mm::GraySkeletonCPP;
using namespace std;


const int DO_SKELETONIZATION_AND_PRUNING = 0;
const int DO_SKELETONIZATION = 1;
const int DO_PRUNING = 2;
const int DO_CONVERSION = 3;

string DoubleToString(double number) {
	char * x = new char[20];
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
	printf("To get the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize]\n\t\t[minGrayValue] [maxGrayValue] [stepSize]\n\n");
	printf("\t[function]      : %i, Perform skeletonization\n", DO_SKELETONIZATION);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
	printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
	printf("\t[stepSize]      : The grayscale stepsize.\n");
	printf("\n");
	printf("To prune the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize]\n\t\t[pointThreshold] [curveThreshold] [surfaceThreshold] [minGrayValue] [maxGrayValue] [stepSize]\n\n");
	printf("\t[function]      : %i, Perform pruning\n", DO_PRUNING);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
	printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
	printf("\t[pointThreshold]: The cost threshold (between 0 (all) and 1(none))\n");
	printf("\t[curveThreshold]: The cost threshold (between 0 (all) and 1(none))\n");
	printf("\t[surfaceThreshold]: The cost threshold (between 0 (all) and 1(none)) (only used for 3D objects)\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
	printf("\t[stepSize]	  : The grayscale stepsize.\n");
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

	//VolumeSkeletonizer * vs = new VolumeSkeletonizer();
	//MRCReader * r = (MRCReader*)MRCReaderPicker::pick(argv[1]);
	//Volume * imgVol = r->getVolume();
	//vs->PerformPureJuSkeletonization(imgVol, argv[2], StringToDouble(argv[3]));
	//imgVol->toOFFCells2(argv[2]);
	////imgVol->toMRCFile(argv[2]);
	//return 0;

	clock_t start, finish;
	start = clock();
	int format, dimension, minGray, maxGray, minCurveSize, minSurfaceSize, stepSize;
	string inFile, skeletonFile, outFile, outPath;
	GrayImage * sourceImage, * sourceSkeleton, * outputImage;
	GrayImageSkeletonizer * skeletonizer2D;
	GrayImageList * imageList;
	MRCReader * reader;
	double pointThreshold, curveThreshold, surfaceThreshold;

	Volume * sourceVol, * outputVol;
	VolumeSkeletonizer * skeletonizer3D;


	switch(args){
		// Convert data types
		case 9:
			//GraySkeletonCPP.exe [function] [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]
			format = StringToInt(argv[1]);
			if(format == DO_CONVERSION) {  
				VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
			} else {
				DisplayInputParams();
			}
			break;
		// Perform skeletonization
		case 10:			
			//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
			format = StringToInt(argv[1]);
			if(format == DO_SKELETONIZATION) {
				dimension = StringToInt(argv[2]);
				inFile = argv[3];
				outFile = argv[4];
				minCurveSize = StringToInt(argv[5]);
				minSurfaceSize = StringToInt(argv[6]);
				minGray = StringToInt(argv[7]);
				maxGray = StringToInt(argv[8]);
				stepSize = StringToInt(argv[9]);
				outPath = outFile.substr(0, outFile.rfind("."));
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						outputImage = skeletonizer2D->PerformJuSkeletonization(sourceImage, outPath, minGray, maxGray);
						ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
						delete skeletonizer2D;
						delete sourceImage;
						delete outputImage;
						break;
					case 3:
						reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
						sourceVol = reader->getVolume();
						delete reader;
						skeletonizer3D = new VolumeSkeletonizer();
						skeletonizer3D->NormalizeVolume(sourceVol);
						outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, minGray, maxGray, stepSize, minCurveSize, minSurfaceSize, outPath, false, 1.0, 1.0, 1.0);
						outputVol->toOFFCells2((char *)(outFile + ".off").c_str());
						outputVol->toMRCFile((char *)outFile.c_str());
						delete sourceVol;
						delete skeletonizer3D;
						delete outputVol;
						break;
					default:
						DisplayInputParams();
						break;
				}		
			} else {
				DisplayInputParams();
			}
			break;
		// Do Pruning
		case 13:
			//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] 
			//                    [pointThreshold] [curveThreshold] [surfaceThreshold] [minGrayValue] [maxGrayValue] [stepSize]
			format = StringToInt(argv[1]);
			if(format == DO_PRUNING) {
				dimension = StringToInt(argv[2]);
				inFile = argv[3];
				outFile = argv[4];
				minCurveSize = StringToInt(argv[5]);
				minSurfaceSize = StringToInt(argv[6]);
				pointThreshold = StringToDouble(argv[7]);				
				curveThreshold = StringToDouble(argv[8]);
				surfaceThreshold = StringToDouble(argv[9]);
				minGray = StringToInt(argv[10]);
				maxGray = StringToInt(argv[11]);
				stepSize = StringToInt(argv[12]);
				outPath = outFile.substr(0, outFile.rfind("."));			
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						sourceSkeleton = ImageReaderBMP::LoadGrayscaleImage(skeletonFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						outputImage = skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, pointThreshold, curveThreshold, minGray, maxGray, outPath);
						ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
						delete skeletonizer2D;
						delete sourceSkeleton;
						delete sourceImage;
						delete outputImage;
						break;
					case 3:
						reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
						sourceVol = reader->getVolume();
						delete reader;
						skeletonizer3D = new VolumeSkeletonizer();
						skeletonizer3D->NormalizeVolume(sourceVol);
						skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
						outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, minGray, maxGray, stepSize, minCurveSize, minSurfaceSize, outPath, true, pointThreshold, curveThreshold, surfaceThreshold);
						outputVol->toMRCFile((char *)outFile.c_str());
						outputVol->toOFFCells2((char *)(outFile + ".off").c_str());
						delete sourceVol;
						delete skeletonizer3D;
						delete outputVol;
						break;
					default:
						DisplayInputParams();
						break;
				}
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