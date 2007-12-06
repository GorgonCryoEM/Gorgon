#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#include "GrayImageSkeletonizer.h"
#include "VolumeReaderRAW.h"
#include "VolumeFormatConverter.h"
#include <sstream> 
#include <string>  
#include <time.h>
#include <math.h>
#include "NormalFinder.h"
#include "DiscreteMesh.h"
#include <MatlabInterface\DataStructures.h>
#include <MatlabInterface\MathLib.h>
#include "GlobalDefinitions.h"
#include "TimeManager.h"


using namespace wustl_mm::GraySkeletonCPP;
using namespace std;


const int DO_SKELETONIZATION = 1;
const int DO_SKELETONIZATION_AND_PRUNING = 2;

const int DO_BINARY_THINNING_JU2007 = 50;
const int DO_TOPOLOGICAL_WATERSHED_JU2007  = 60;

const int DO_TEXT_TO_VOLUME = 996;
const int DO_CROPPING = 997;
const int DO_DOWNSAMPLING = 998;
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
	printf("\tGraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]\n\t\t[pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius] [skeletonDirectionRadius]\n\t\t[minGrayValue] [maxGrayValue] [stepSize]\n\n");
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
	printf("\t[skeletonDirectionRadius] : The radius used when calculating the direction of the skeleton\n");
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

	printf("To convert a text file into a volume\n");
	printf("\tGraySkeletonCPP.exe [function] [inputfile] [outfile]\n\n");
	printf("\t[function]      : %i, create a volume\n", DO_TEXT_TO_VOLUME);
	printf("\t[inputfile]     : The source file\n");
	printf("\t[outfile]       : The destination file\n\n");

	printf("To Crop a volume\n");
	printf("\tGraySkeletonCPP.exe [function] [inputfile] [outfile] [startx] [endx] [starty] [endy] [startz] [endz]\n\n");
	printf("\t[function]      : %i, Crop a volume\n", DO_CROPPING);
	printf("\t[inputfile]     : The source file\n");
	printf("\t[outfile]       : The destination file\n");
	printf("\t[startd]        : The starting coordinate in the d dimension\n");
	printf("\t[endd]          : The ending coordinate in the d dimension\n\n");

	printf("To Downsample a volume\n");
	printf("\tGraySkeletonCPP.exe [function] [inputfile] [outfile]\n\n");
	printf("\t[function]      : %i, Downsample a volume\n", DO_DOWNSAMPLING);
	printf("\t[inputfile]     : The source file\n");
	printf("\t[outfile]       : The destination file\n\n");

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

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
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

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
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

			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
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
void DoSkeletonizationAndPruningAbeysinghe2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int maxCurveHole, int maxSurfaceHole, double pointThreshold, double curveThreshold, double surfaceThreshold, int pointRadius, int curveRadius, int surfaceRadius, int skeletonDirectionRadius, int minGray, int maxGray, int stepSize) {
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
			VolumeSkeletonizer * skeletonizer3D = new VolumeSkeletonizer(pointRadius, curveRadius, surfaceRadius, skeletonDirectionRadius);
			skeletonizer3D->NormalizeVolume(sourceVol);
			skeletonizer3D->CleanupVolume(sourceVol, minGray, maxGray);
			Volume * outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, minGray, maxGray, stepSize, minCurveSize, minSurfaceSize, maxCurveHole, maxSurfaceHole, outPath, true, pointThreshold, curveThreshold, surfaceThreshold);
			delete sourceVol;
			delete skeletonizer3D;
			outputVol->toMRCFile((char *)outFile.c_str());		
			delete outputVol;
			break;
		}
		default:
			DisplayInputParams();
			break;
	}
}



void DoCropping(string inFile, string outFile, int startX, int endX, int startY, int endY, int startZ, int endZ) {
	MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
	Volume * sourceVol = reader->getVolume();
	Volume * destVol = new Volume(endX-startX+1, endY-startY+1, endZ-startZ+1, startX, startY, startZ, sourceVol);
	destVol->toMRCFile((char *)outFile.c_str());
	delete reader;
	delete sourceVol;
	delete destVol;	
}

void DoDownsampling(string inFile, string outFile) {
	MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
	Volume * sourceVol = reader->getVolume();
	Volume * destVol = new Volume(sourceVol->getSizeX()/2, sourceVol->getSizeY()/2, sourceVol->getSizeZ()/2);
	double val;

	int radius = 1;
	MathLib * math = new MathLib();

	ProbabilityDistribution3D gaussianFilter;
	gaussianFilter.radius = radius;
	math->GetBinomialDistribution(gaussianFilter);

	for(int x = radius; x < destVol->getSizeX()-radius; x++) {
		for(int y = radius; y < destVol->getSizeY()-radius; y++) {
			for(int z = radius; z < destVol->getSizeZ()-radius; z++) {
				val = 0;
				for(int xx = -radius; xx <= radius; xx++) {
					for(int yy = -radius; yy <= radius; yy++) {
						for(int zz = -radius; zz <= radius; zz++) {
							val += sourceVol->getDataAt(2*x+xx, 2*y+yy, 2*z+zz) * gaussianFilter.values[xx+radius][yy+radius][zz+radius] ;
						}
					}
				}
				destVol->setDataAt(x, y, z, val);					
			}
		}
	}
	destVol->toMRCFile((char *)outFile.c_str());
	delete math;
	delete reader;
	delete sourceVol;
	delete destVol;
}

void DoTextToVolume(string inFile, string outFile) {
	FILE* fin = fopen((char *)inFile.c_str(), "rt");
	if (fin == NULL)
	{
		printf("Error reading input file %s\n", inFile) ;
		exit(0) ;
	}
	int layers, rows, cols, radius;
	const int margin = 5;
	fscanf(fin, "%d %d %d %d\n", &layers, &rows, &cols, &radius);
	Volume * test = new Volume(cols+ margin*2, rows + margin*2, layers + margin*2);
	for(int z = 0; z < layers; z++) {
		for(int y = 0; y < rows; y++) {
			for(int x = 0; x < cols; x++) {
				char inp = fgetc(fin);
				if(inp == 'X') {
					test->setDataAt(x+margin , y + margin, z+margin, 1.0);		
					printf("X");
				} else {
					printf(" ");
				}
			}
			fscanf(fin, "\n");
			printf("\n");			
		}
		fscanf(fin, "\n");
		printf("\n");
	}
	fclose(fin);
	Volume * distanceField = test->getDistanceField(radius, 0);
	distanceField->toMRCFile((char *)outFile.c_str());
	delete test;
	delete distanceField;
}

double GetGaussianContribution(int x, int y, int z, double px, double py, double pz, int radius) {
	return max(0.0, 1.0 - sqrt(pow(x-px, 2) + pow(y-py, 2) + pow(z-pz, 2))/((double)radius));
}

Volume * GetDistanceField(Vector3D * points, int pointCount, int xSize, int ySize, int zSize, int gaussianBlur) {
	Volume * field = new Volume(xSize, ySize, zSize);
	int x1,x2,y1,y2,z1,z2;

	for(int i = 0; i < pointCount; i++) {
		x1 = max((int)floor(points[i].X()-gaussianBlur), 0);
		x2 = min((int)floor(points[i].X()+gaussianBlur), xSize-1);
		y1 = max((int)floor(points[i].Y()-gaussianBlur), 0);
		y2 = min((int)floor(points[i].Y()+gaussianBlur), ySize-1);
		z1 = max((int)floor(points[i].Z()-gaussianBlur), 0);
		z2 = min((int)floor(points[i].Z()+gaussianBlur), zSize-1);

		for(int x = x1; x <= x2; x++) {
			for(int y = y1; y <= y2; y++) {
				for(int z = z1; z <= z2; z++) {
					field->setDataAt(x, y, z, max(field->getDataAt(x,y,z), GetGaussianContribution(x, y, z, points[i].X(), points[i].Y(), points[i].Z(), gaussianBlur)));
				}
			}
		}
	}
	return field;
}

void CreateHemisphere(string outFile, int radius, int gaussianBlur, double samplingRateI, double samplingRateJ) {
	int margin = 5;
	int maxPointCount = 2*PI*PI/(samplingRateI*samplingRateJ) + 10;
	int pointCount = 0;
	int offset = margin+radius + gaussianBlur;
	Vector3D * points = new Vector3D[maxPointCount];
	
	double step = 1;
	for(double i = 0; i < 2.0 * PI; i+= samplingRateI) {
		step = 1.0;
		for(double j = 0; j < PI/2.0; j+= (samplingRateJ*step) ) {
			points[pointCount] = Vector3D(offset + radius* cos(i) * sin(j), offset + radius* sin(i)*sin(j), offset + radius*cos(j));
			pointCount++;
			step += 0.3;
		}
	}

	

	Volume * hemisphere = GetDistanceField(points, pointCount, 2*offset, 2*offset, 2*offset, gaussianBlur);

	hemisphere->toMRCFile((char *)outFile.c_str());
	delete [] points;
	delete hemisphere;
}


void CreateSheet(string outFile, double startX, double startY, double xIncrement, double yIncrement, double xDiff, double yDiff, int xCount, int yCount) {	
	double xDiffCumu = 0;
	double yDiffCumu = 0;
	int margin = 5;
	Vector3D * points = new Vector3D[10000];
	int pointCount = 0;
	for (int x = 0; x < xCount; x++) {		
		for(int y = 0; y < yCount; y++) {
			points[pointCount] = Vector3D(startX + xIncrement*x + xDiffCumu, startY + yIncrement*y + yDiffCumu, margin);
			pointCount++;
			yDiffCumu += yDiff * y;
		}
		yDiffCumu = 0;
		xDiffCumu += xDiff * x ;
	}

	delete [] points;
	Volume * sheet = GetDistanceField(points, pointCount, 100, 100, 20, 10);
	sheet->toMRCFile((char *)outFile.c_str());
	delete sheet;
}

int main( int args, char * argv[] ) {
	appTimeManager.PushCurrentTime();
	int function;
	bool error = true;

	if(args > 2) {
		function = StringToInt(argv[1]);
		switch(function) {		
			case DO_BINARY_THINNING_JU2007:
				// GraySkeletonCPP.exe DO_BINARY_THINNING_JU2007 [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [threshold]
				if(args == 8) {					
					DoBinaryThinningJu2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), StringToDouble(argv[7]));
					error = false;
				} 
				break;
			case DO_TOPOLOGICAL_WATERSHED_JU2007:
				//GraySkeletonCPP.exe DO_TOPOLOGICAL_WATERSHED_JU2007 [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
				if(args == 10) {					
					DoTopologicalWatershedJu2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), 
						StringToInt(argv[7]), StringToInt(argv[8]), StringToInt(argv[9]));
					error = false;
				} 
				break;
			case DO_SKELETONIZATION:
				//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
				if(args == 10) {					
					DoSkeletonizationAbeysinghe2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), 
						StringToInt(argv[7]), StringToInt(argv[8]), StringToInt(argv[9]));
					error = false;
				} 
				break;
			case DO_SKELETONIZATION_AND_PRUNING:
				//GraySkeletonCPP.exe DO_SKELETONIZATION_AND_PRUNING [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]
				//                    [pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius] [skeletonDirectionRadius]
				//                    [minGrayValue] [maxGrayValue] [stepSize]
				if(args == 19) {					
					DoSkeletonizationAndPruningAbeysinghe2007(StringToInt(argv[2]), argv[3], argv[4], StringToInt(argv[5]), StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]),
						StringToDouble(argv[9]), StringToDouble(argv[10]), StringToDouble(argv[11]), StringToInt(argv[12]), StringToInt(argv[13]),
						StringToInt(argv[14]), StringToInt(argv[15]), StringToInt(argv[16]), StringToInt(argv[17]), StringToInt(argv[18]));
					error = false;
				} 
				break;
			case DO_TEXT_TO_VOLUME:
				// GraySkeletonCPP.exe DO_TEXT_TO_VOLUME [inputfile] [outfile]
				if(args == 4) {					
					DoTextToVolume(argv[2], argv[3]);
					error = false;
				} 
				break;
			case DO_CROPPING:
				// GraySkeletonCPP.exe DO_CROPPING [inputfile] [outfile] [startx] [endx] [starty] [endy] [startz] [endz]
				if(args == 10) {					
					DoCropping(argv[2], argv[3], StringToInt(argv[4]), StringToInt(argv[5]), StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]), StringToInt(argv[9]));
					error = false;
				} 
				break;
			case DO_DOWNSAMPLING:
				// GraySkeletonCPP.exe DO_DOWNSAMPLING [inputfile] [outfile]
				if(args == 4) {					
					DoDownsampling(argv[2], argv[3]);
					error = false;
				} 
				break;
			case DO_CONVERSION:
				// GraySkeletonCPP.exe DO_CONVERSION [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]
				if(args == 9) {					
					VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringToInt(argv[6]), StringToInt(argv[7]), StringToInt(argv[8]));
					error = false;
				} 
				break;
					
		}		
	} 

	if(error) {
		DisplayInputParams();
	}

	appTimeManager.PopAndDisplayTime("\nTotal : %f seconds!\n");
}

#endif 