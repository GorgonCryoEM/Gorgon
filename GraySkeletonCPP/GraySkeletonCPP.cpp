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
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minGrayValue] [maxGrayValue]\n\n");
	printf("\t[function]      : %i, Perform skeletonization\n", DO_SKELETONIZATION);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
	printf("\n");
	printf("To prune the grayscale skeleton of an image\n");
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [inputSkeleton] [outfile] [threshold1] [threshold2] [minGrayValue] [maxGrayValue]\n\n");
	printf("\t[function]      : %i, Perform pruning\n", DO_PRUNING);
	printf("\t[dimensions]    : The number of dimensions\n");
	printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
	printf("\t[inputSkeleton] : an input image file of the skeleton (BMP if 2D, MRC if 3D)\n");	
	printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
	printf("\t[threshold1]     : The cost threshold (between 0 (all) and 1(none)) (2D: points, 3D: curves)\n");
	printf("\t[threshold2]     : The cost threshold (between 0 (all) and 1(none)) (2D: curves, 3D: surfaces)\n");
	printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
	printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
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
			//Vector3D x1 = VectorLib::Initialize(1.0, 0.0, 0.0);
			//Vector3D y1 = VectorLib::Initialize(0.0, 1.0, 0.0);
			//Vector3D u1 = VectorLib::Initialize(0.5, 0.5, 0.0);
			//Vector3D u2 = VectorLib::Initialize(0.0, 0.0, 1.0);
			//VectorLib::Normalize(u1);
			//VectorLib::Normalize(u2);
			//printf("U1: %f %f %f\n", u1.values[0], u1.values[1], u1.values[2]);

			//Vector3D a1 = VectorLib::CrossProduct(u1, x1);
			//VectorLib::Normalize(a1);
			//double t1 = acos(VectorLib::DotProduct(u1, x1));

			//printf("A1: %f %f %f by %f\n", a1.values[0], a1.values[1], a1.values[2], t1*180.0*7.0/ 22.0);

			//Vector3D x2 = VectorLib::Rotate(x1, a1, t1);
			//Vector3D y2 = VectorLib::Rotate(y1, a1, t1);

			//printf("X2: %f %f %f\n", x2.values[0], x2.values[1], x2.values[2]);
			//printf("Y2: %f %f %f\n", y2.values[0], y2.values[1], y2.values[2]);

			//Vector3D a2 = VectorLib::CrossProduct(u2, y2);
			//VectorLib::Normalize(a2);
			//double t2 = acos(VectorLib::DotProduct(u2, y2));

			//printf("A2: %f %f %f by %f\n", a2.values[0], a2.values[1], a2.values[2], t2*180.0*7.0/ 22.0);


			//return 0;


	clock_t start, finish;
	start = clock();
	int format, dimension, minGray, maxGray;
	string inFile, skeletonFile, outFile, outPath;
	GrayImage * sourceImage, * sourceSkeleton, * outputImage;
	GrayImageSkeletonizer * skeletonizer2D;
	GrayImageList * imageList;
	MRCReader * reader;
	double curveThreshold, surfaceThreshold, threshold;

	Volume * sourceVol, * sourceSkeletonVol, * outputVol;
	VolumeSkeletonizer * skeletonizer3D;


	switch(args){
		// Perform skeletonization
		case 7:			
			format = StringToInt(argv[1]);
			dimension = StringToInt(argv[2]);
			inFile = argv[3];
			outFile = argv[4];
			minGray = StringToInt(argv[5]);
			maxGray = StringToInt(argv[6]);
			outPath = outFile.substr(0, outFile.rfind("."));
			if(format == DO_SKELETONIZATION) {
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
						outputVol = skeletonizer3D->PerformJuSkeletonization(sourceVol, outPath, minGray, maxGray);
						outputVol->toMRCFile((char *)outFile.c_str());
						delete sourceVol;
						delete skeletonizer3D;
						delete outputVol;
						break;
				}			
			} else {
				DisplayInputParams();
			}
			break;
		// Convert data types
		case 9:
			format = StringToInt(argv[1]);
			if(format == DO_CONVERSION) {  
				VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
			} else {
				DisplayInputParams();
			}
			break;
		// Do Pruning
		case 10:
			format = StringToInt(argv[1]);
			if(format == DO_PRUNING) {
				dimension = StringToInt(argv[2]);
				inFile = argv[3];
				skeletonFile = argv[4];
				outFile = argv[5];
				curveThreshold = StringToDouble(argv[6]);
				surfaceThreshold = StringToDouble(argv[7]);
				minGray = StringToInt(argv[8]);
				maxGray = StringToInt(argv[9]);
				outPath = outFile.substr(0, outFile.rfind("."));			
				switch(dimension){
					case 2:
						sourceImage = ImageReaderBMP::LoadGrayscaleImage(inFile);
						sourceSkeleton = ImageReaderBMP::LoadGrayscaleImage(skeletonFile);
						skeletonizer2D = new GrayImageSkeletonizer();
						outputImage = skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, curveThreshold, surfaceThreshold, minGray, maxGray, outPath);
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
						reader = (MRCReader*)MRCReaderPicker::pick((char *)skeletonFile.c_str());
						sourceSkeletonVol = reader->getVolume();
						delete reader;
						skeletonizer3D = new VolumeSkeletonizer();
						skeletonizer3D->NormalizeVolume(sourceVol);
						skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
						outputVol = skeletonizer3D->PerformSkeletonPruning(sourceVol, sourceSkeletonVol, curveThreshold, surfaceThreshold, minGray, maxGray, outPath);
						outputVol->toMRCFile((char *)outFile.c_str());
						delete sourceVol;
						delete sourceSkeletonVol;
						delete skeletonizer3D;
						delete outputVol;
						break;
				}
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