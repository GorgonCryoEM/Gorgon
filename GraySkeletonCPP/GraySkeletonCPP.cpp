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


const int DO_SKELETONIZATION = 1;
const int DO_SKELETONIZATION_AND_PRUNING = 2;

const int DO_BINARY_THINNING_JU2007 = 50;
const int DO_TOPOLOGICAL_WATERSHED_JU2007  = 60;

const int DO_CONVERSION = 999;

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
	printf("\t[stepSize]      : The grayscale stepsize.\n\n");

	printf("To skeletonize and prune the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]\n\t\t[pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius]\n\t\t[minGrayValue] [maxGrayValue] [stepSize]\n\n");
	printf("\t[function]      : %i, Perform pruning\n", DO_SKELETONIZATION_AND_PRUNING);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
	printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
	printf("\t[maxCurveHole]  : The length of the largest curve hole segment to be repaired.\n");
	printf("\t[maxSurfaceHole]: The radius of the largest surface hole segment to be repaired (only used for 3D objects).\n");
	printf("\t[pointThreshold]: The cost threshold (between 0 (all) and 1(none))\n");
	printf("\t[curveThreshold]: The cost threshold (between 0 (all) and 1(none))\n");
	printf("\t[surfaceThreshold]: The cost threshold (between 0 (all) and 1(none)) (only used for 3D objects)\n");
	printf("\t[pointRadius]   : The gaussian filter radius to use when pruning points\n");
	printf("\t[curveRadius]   : The gaussian filter radius to use when pruning curves\n");
	printf("\t[surfaceRadius] : The gaussian filter radius to use when pruning surfaces (only used for 3D objects)\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
	printf("\t[stepSize]	  : The grayscale stepsize.\n\n");

	printf("Other tasks:\n\n");

	printf("To perform Binary Thinning (Ju2007)\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [threshold]\n\n");
	printf("\t[function]      : %i, Perform Binary Thinning (Ju 2007)\n", DO_BINARY_THINNING_JU2007);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
	printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
	printf("\t[pointThreshold]: The cost threshold of the input file\n\n");

	printf("To perform Topological Watershed (Ju2007 iteratively)\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]\n\n");
	printf("\t[function]      : %i, Perform Topological Watershed (Ju2007 iteratively\n", DO_TOPOLOGICAL_WATERSHED_JU2007);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
	printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
	printf("\t[stepSize]	  : The grayscale stepsize.\n\n");

	printf("To convert file formats\n");
	printf("\tGraySkeletonCPP.exe [function] [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]\n\n");
	printf("\t[function]      : %i, Perform conversion\n", DO_CONVERSION);
	printf("\t[inputfile]     : The source file\n");
	printf("\t[inputformat]   : The source file format\n");
	printf("\t[outfile]       : The destination file\n");
	printf("\t[outformat]     : The destination file format\n");
	printf("\t[x-size]        : The size of the data set in the x-Dimension\n");
	printf("\t[y-size]        : The size of the data set in the y-Dimension\n");
	printf("\t[z-size]        : The size of the data set in the z-Dimension\n\n");
}

// Performs binary thinning using the method described by Ju et. al. 2007 ("Computing a family of skeletons of volumetric models for shape description")
void DoBinaryThinningJu2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, double generalThreshold) {
	string outPath = outFile.substr(0, outFile.rfind("."));
	switch(dimensions){
		case 2:
			printf("Not implemented yet! \n");
			break;
		case 3: {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			delete reader;

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0);
			Volume * outputVol = skeletonizer3D->PerformPureJuSkeletonization(sourceVol, outFile, generalThreshold, minCurveSize, minSurfaceSize);
			outputVol->toOFFCells2((char *)(outPath + ".off").c_str());
			outputVol->toMRCFile((char *)(outPath + ".mrc").c_str());
			delete outputVol;
			delete sourceVol;
			delete skeletonizer3D;
			break;
		}
		default:
			DisplayInputParams();
			break;
	}
}


// Performs topological watershed by performing the method of Ju et. al 2007 for every grayscale level.
void DoTopologicalWatershedJu2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int minGray, int maxGray, int stepSize) {
	string outPath = outFile.substr(0, outFile.rfind("."));	
	switch(dimensions){
		case 2: {
			GrayImage * sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
			GrayImageSkeletonizer * skeletonizer2D = new GrayImageSkeletonizer(0,0);
			GrayImage * outputImage = skeletonizer2D->PerformJuSkeletonization(sourceImage, outPath, minGray, maxGray, stepSize);
			ImageReaderBMP::SaveGrayscaleImage(outputImage, outFile);
			delete skeletonizer2D;
			delete sourceImage;
			delete outputImage;
			break;
		}
		case 3: {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			delete reader;

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0);
			skeletonizer3D->NormalizeVolume(sourceVol);
			skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
			Volume * outputVol = skeletonizer3D->PerformJuSkeletonization(sourceVol, outPath, minGray, maxGray, stepSize);				
			outputVol->toOFFCells2((char *)(outPath + ".off").c_str());
			outputVol->toMRCFile((char *)(outPath + ".mrc").c_str());
			delete outputVol;
			delete sourceVol;
			delete skeletonizer3D;			
			break;
		}
		default:
			DisplayInputParams();
			break;
	}
}

// Performs skeletonization of a grayscale volume directly using an adaptation of thinning which does not use binarization.
void DoSkeletonizationAbeysinghe2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int minGray, int maxGray, int stepSize) {
	string outPath = outFile.substr(0, outFile.rfind("."));	
	switch(dimensions){
		case 2: {
			printf("Not implemented yet! \n");
			break;
		}
		case 3: {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			delete reader;

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0);
			skeletonizer3D->NormalizeVolume(sourceVol);
			skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
			Volume * outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, minGray, maxGray, stepSize, minCurveSize, minSurfaceSize, 0.0, 0.0, outPath, false, 1.0, 1.0, 1.0);
			outputVol->toOFFCells2((char *)(outPath + ".off").c_str());
			outputVol->toMRCFile((char *)(outPath + ".mrc").c_str());
			delete outputVol;
			delete sourceVol;
			delete skeletonizer3D;			
			break;
		}
		default:
			DisplayInputParams();
			break;
	}
}

// Performs skeletonization of a grayscale volume directly using an adaptation of thinning which does not use binarization, and then uses a pruning step to remove wrong branches
void DoSkeletonizationAndPruningAbeysinghe2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int maxCurveHole, int maxSurfaceHole, double pointThreshold, double curveThreshold, double surfaceThreshold, int pointRadius, int curveRadius, int surfaceRadius, int minGray, int maxGray, int stepSize) {
	string outPath = outFile.substr(0, outFile.rfind("."));	
	switch(dimensions){
		case 2: {
			printf("Not implemented yet! \n");
			break;
		}
		case 3: {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			delete reader;
			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(pointRadius, curveRadius, surfaceRadius);
			skeletonizer3D->NormalizeVolume(sourceVol);
			skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
			Volume * outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, minGray, maxGray, stepSize, minCurveSize, minSurfaceSize, maxCurveHole, maxSurfaceHole, outPath, true, pointThreshold, curveThreshold, surfaceThreshold);
			outputVol->toMRCFile((char *)outFile.c_str());
			outputVol->toOFFCells2((char *)(outPath + ".off").c_str());
			delete sourceVol;
			delete skeletonizer3D;
			delete outputVol;
			break;
		}
		default:
			DisplayInputParams();
			break;
	}
}


int main( int args, char * argv[] ) {

	clock_t start, finish;
	start = clock();
	int function;


	switch(args){		
		case 8: 
			function = StringToInt(argv[1]);
			if(function == DO_BINARY_THINNING_JU2007) {
				// GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [threshold]
				DoBinaryThinningJu2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), StringToDouble(argv[7]));
			} else {
				DisplayInputParams();
			}
			break;		
		case 9:			
			function = StringToInt(argv[1]);
			if(function == DO_CONVERSION) {  
				// GraySkeletonCPP.exe [function] [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]
				VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
			} else {
				DisplayInputParams();
			}
			break;		
		case 10:			
			function = StringToInt(argv[1]);
			switch(function) {
				case DO_TOPOLOGICAL_WATERSHED_JU2007 :
					//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
					DoTopologicalWatershedJu2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), 
						StringToInt(argv[7]), StringToInt(argv[8]), StringToInt(argv[9]));
					break;
				case DO_SKELETONIZATION :
					//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
					DoSkeletonizationAbeysinghe2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), 
						StringToInt(argv[7]), StringToInt(argv[8]), StringToInt(argv[9]));
					break;
				default:
					DisplayInputParams();
					break;
			}
			break;
		case 18:
			function = StringToInt(argv[1]);
			switch(function) {
				//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]
				//                    [pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius]
				//                    [minGrayValue] [maxGrayValue] [stepSize]
				case DO_SKELETONIZATION_AND_PRUNING :
					DoSkeletonizationAndPruningAbeysinghe2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]),
						StringToDouble(argv[9]), StringToDouble(argv[10]), StringToDouble(argv[11]), StringToInt(argv[12]), StringToInt(argv[13]),
						StringToInt(argv[14]), StringToInt(argv[15]), StringToInt(argv[16]), StringToInt(argv[17]));
					break;				
				default:
					DisplayInputParams();
					break;
			}
			break;
		default:
			DisplayInputParams();
			break;
	}
	finish = clock();
	double timeTaken = ((double) (finish - start) / (double) CLOCKS_PER_SEC);
	printf("\n%f seconds taken!\n", timeTaken);
}

#endif 