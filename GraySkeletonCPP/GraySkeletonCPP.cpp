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
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [inputSkeleton] [outfile] [threshold] [minGrayValue] [maxGrayValue]\n\n");
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

	//Volume * oldVolume = new Volume(15,15,15);
	//Volume * newVolume = new Volume(15,15,15);

	//for(int x = 3; x <= 5; x++) {
	//	for(int y = 3; y <= 5; y++) {
	//		newVolume->setDataAt(x, y, 5, 1);
	//		//newVolume->setDataAt(x, y, 7, 1);
	//	}
	//}

	//for(int x = 3; x <= 5; x++) {
	//	for(int z = 3; z <= 7; z++) {
	//		newVolume->setDataAt(x, 4, z, 1);
	//		//newVolume->setDataAt(x, y, 7, 1);
	//	}
	//}


	//VolumeDeltaAnalyzer * delta = new VolumeDeltaAnalyzer(oldVolume, newVolume);
	//int count;
	//SkeletalCurve * list;
	//delta->GetNewSurfaces(count, list);
	//for(int i = 0; i < count; i++) {
	//	for(int j=0; j < list[i].points.size(); j++) {
	//		printf("{%i %i %i} ", list[i].points[j].values[0], list[i].points[j].values[1], list[i].points[j].values[2]); 
	//	}
	//	printf("\n");
	//}

	//return 0;


	//VolumeSkeletonizer * s = new VolumeSkeletonizer();
	//string outputFile = "binarySkeleton-";
	//Volume * imageVol = (MRCReaderPicker::pick("C:\\_WashU\\ssa1\\data\\hand5\\hand5-volume.mrc"))->getVolume();
	//s->PerformPureJuSkeletonization(imageVol, outputFile+"0.0232-", 0.0232);
	//////s->PerformPureJuSkeletonization(imageVol, outputFile+"0.018-", 0.018);
	//////s->PerformPureJuSkeletonization(imageVol, outputFile+"0.0288-", 0.0288);
	//////s->PerformPureJuSkeletonization(imageVol, outputFile+"0.0303-", 0.0303);
	//////s->PerformPureJuSkeletonization(imageVol, outputFile+"0.0653-", 0.0653);

	//delete s;
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
						reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
						sourceVol = reader->getVolume();
						delete reader;
						skeletonizer3D = new VolumeSkeletonizer();
						outputVol = skeletonizer3D->PerformJuSkeletonization(sourceVol, outPath);
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
								imageList->AddImage(skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, t, t, 1, 255, outPath));
							}
							outputImage = imageList->GetCompositeImage(4);
							imageList->Clear(true);
							delete imageList;
						} else {
							outputImage = skeletonizer2D->PerformSkeletonPruning(sourceImage, sourceSkeleton, threshold, threshold, 1, 255, outPath);
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
		case 9:
			format = StringToInt(argv[1]);
			if(format == DO_CONVERSION) {  
				VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
			} else {
				DisplayInputParams();
			}
			break;
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
						outputVol = skeletonizer3D->PerformSkeletonPruning(sourceVol, sourceSkeletonVol, curveThreshold, surfaceThreshold, minGray, maxGray, outPath);
						//outputVol->toMRCFile((char *)outFile.c_str());
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