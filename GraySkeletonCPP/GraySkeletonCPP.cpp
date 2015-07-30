// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the GRAYSKELETONCPP Module


#ifndef GRAY_SKELETON_CPP_CPP
#define GRAY_SKELETON_CPP_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include "GrayImageSkeletonizer.h"
#include "VolumeReaderRAW.h"
#include "VolumeFormatConverter.h"
#include <cmath>
#include "NormalFinder.h"
#include "DiscreteMesh.h"
#include <MathTools/DataStructures.h>
#include <MathTools/MathLib.h>
#include "GlobalDefinitions.h"
#include <Foundation/TimeManager.h>
#include <Foundation/StringUtils.h>
#include "InteractiveSkeletonizer.h"
//#include "TreeSkeletonizer.h"

using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::SkeletonMaker;
using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		const int DO_NOTHING = -1;
		const int DO_SKELETONIZATION = 1;
		const int DO_SKELETONIZATION_AND_PRUNING = 2;			

		const int DO_BINARY_THINNING_JU2007 = 50;
		const int DO_TOPOLOGICAL_WATERSHED_JU2007  = 60;
		const int DO_INTERACTIVE_SKELETONIZATION = 70;
		/*const int DO_TREE_SKELETONIZATION = 80;*/

		const int DO_DISPLAY_VOXEL_COUNT = 800;

		const int DO_TEXT_TO_VOLUME = 900;
		const int DO_RESIZE = 901;
		const int DO_CROPPING = 902;
		const int DO_DOWNSAMPLING = 903;
		const int DO_SMOOTHING = 904;
		const int DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED = 905;
		const int DO_THRESHOLD_GRAY_RANGE = 906;
		const int DO_CONVERSION = 950;

		void DisplayInputParams(int process) {
			switch(process) {
				case DO_NOTHING:
					printf("Performs various functions on grayscale volumes... \n");
					printf("Usage: \n");
					printf("\tGraySkeletonCPP.exe [function] [{args}*]\n\n");
					printf(" %i  \t- Perform skeletonization (Step 1/3 of SMI 08 paper) \n", DO_SKELETONIZATION);
					printf(" %i  \t- Perform and pruning (Step 1-4 of SMI 08 paper) \n", DO_SKELETONIZATION_AND_PRUNING);
					printf(" %i  \t- Perform binary thinning (Ju 07) \n", DO_BINARY_THINNING_JU2007);
					printf(" %i  \t- Perform topology watershed by iterating binary thinning (Ju 07) \n", DO_TOPOLOGICAL_WATERSHED_JU2007);
					printf("\n");
					printf(" %i  \t- Display the non-zero voxel count \n", DO_DISPLAY_VOXEL_COUNT);
					printf(" %i  \t- Create a binary volume from a text file \n", DO_TEXT_TO_VOLUME);
					printf(" %i  \t- Resize a volume \n", DO_RESIZE);
					printf(" %i  \t- Crop a volume \n", DO_CROPPING);
					printf(" %i  \t- Downsample a volume by a scale of 2 in all dimensions\n", DO_DOWNSAMPLING);
					printf(" %i  \t- Smoothen a volume \n", DO_SMOOTHING);
					printf(" %i  \t- Smoothen a volume (Anisotropic axis aligned)\n", DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED);
					printf(" %i  \t- Threshold the gray-range of a volume (Results in those out of range set to zero)\n", DO_THRESHOLD_GRAY_RANGE);
					printf(" %i  \t- Convert the data type \n", DO_CONVERSION);			
					break;
				case DO_SKELETONIZATION:
					printf("To get the grayscale skeleton of an image\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize]\n\t\t[minGrayValue] [maxGrayValue] [stepSize]\n\n", DO_SKELETONIZATION);
					printf("\t[dimensions]    : The number of dimensions\n");
					printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
					printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
					printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
					printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
					printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
					printf("\t[stepSize]      : The grayscale stepsize.\n");
					printf("\t[smoothingIterations]: The number of smoothing steps.\n");
					printf("\t[smoothingRadius]: The radius for the structure tensor used in smoothing.\n\n");
					break;
				case DO_SKELETONIZATION_AND_PRUNING:
					printf("To skeletonize and prune the grayscale skeleton of an image\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [curveRadius] [surfaceRadius] [minGrayValue]\n\n", DO_SKELETONIZATION_AND_PRUNING);			
					printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
					printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
					printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
					printf("\t[curveRadius]   : The gaussian filter radius to use when pruning curves\n");
					printf("\t[surfaceRadius] : The gaussian filter radius to use when pruning surfaces (only used for 3D objects)\n");
					printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n\n");
					printf("Advanced Mode:\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]\n\t\t[pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius] [skeletonDirectionRadius]\n\t\t[minGrayValue] [maxGrayValue] [stepSize] [smoothingIterations]\n\n", DO_SKELETONIZATION_AND_PRUNING);
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
					printf("\t[stepSize]	  : The grayscale stepsize.\n");
					printf("\t[smoothingIterations]: The number of smoothing steps.\n");
					printf("\t[smoothingRadius]: The radius for the structure tensor used in smoothing.\n\n");
					break;

				case DO_INTERACTIVE_SKELETONIZATION:
					printf("To perform Interactive skeletonization (debug only.. dosent load a UI)\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [curveRadius] [minGray] [maxGray] [stepSize] [startX] [startY] [startZ] [endX] [endY] [endZ] [skeletonRatio] [structureTensorRatio]\n\n", DO_INTERACTIVE_SKELETONIZATION);
					printf("\t[dimensions]    : The number of dimensions\n");
					printf("\t[inputfile]     : an input image file (MRC)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton (MRC)\n");
					printf("\t[minCurveSize]  : The minimum curve size in the skeleton graph\n");
					printf("\t[curveRadius]   : The gaussian filter radius to use when pruning curves\n");
					printf("\t[minGray]       : The minimum grayscale value to consider.\n");
					printf("\t[maxGray]       : The maximum grayscale value to consider.\n");
					printf("\t[stepSize]	  : The grayscale stepsize.\n");
					printf("\t[start[D]]	  : The starting seed point (dimension d).\n");
					printf("\t[end[D]]        : The ending seed point (dimension d).\n");
					printf("\t[skeletonRatio] : The ratio for the skeleton component in the graph.\n");
					printf("\t[structureTensorRatio] : The ratio for the structure tensor component in the graph.\n\n");
					break;

				/*case DO_TREE_SKELETONIZATION:
					printf("To perform automatic skeletonization of trees\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [curveRadius] [minGray] [maxGray] [stepSize] [rootX] [rootY] [rootZ] [skeletonRatio] [structureTensorRatio]\n\n", DO_INTERACTIVE_SKELETONIZATION);
					printf("\t[dimensions]    : The number of dimensions\n");
					printf("\t[inputfile]     : an input image file (MRC)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton (OFF)\n");
					printf("\t[minCurveSize]  : The minimum curve size in the skeleton graph\n");
					printf("\t[curveRadius]   : The gaussian filter radius to use when pruning curves\n");
					printf("\t[minGray]       : The minimum grayscale value to consider.\n");
					printf("\t[maxGray]       : The maximum grayscale value to consider.\n");
					printf("\t[stepSize]	  : The grayscale stepsize.\n");
					printf("\t[root[D]]       : The root seed point (dimension d).\n");
					printf("\t[skeletonRatio] : The ratio for the skeleton component in the graph.\n");
					printf("\t[structureTensorRatio] : The ratio for the structure tensor component in the graph.\n\n");
					break; */

				case DO_BINARY_THINNING_JU2007:
					printf("To perform Binary Thinning (Ju2007)\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [threshold]\n\n", DO_BINARY_THINNING_JU2007);
					printf("\t[dimensions]    : The number of dimensions\n");
					printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
					printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
					printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
					printf("\t[pointThreshold]: The cost threshold of the input file\n\n");
					break;

				case DO_TOPOLOGICAL_WATERSHED_JU2007:
					printf("To perform Topological Watershed (Ju2007 iteratively)\n");
					printf("\tGraySkeletonCPP.exe %i [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]\n\n", DO_TOPOLOGICAL_WATERSHED_JU2007);
					printf("\t[dimensions]    : The number of dimensions\n");
					printf("\t[inputfile]     : an input image file (BMP if 2D, MRC if 3D)\n");
					printf("\t[outfile]       : the filename to be used when generating the skeleton\n");
					printf("\t[minCurveSize]  : The minimum length of curve segments.\n");
					printf("\t[minSurfaceSize]: The minimum radius of surface segments (only used for 3D objects).\n");
					printf("\t[minGrayValue]  : The minimum grayscale value to consider.\n");
					printf("\t[maxGrayValue]  : The maximum grayscale value to consider.\n");
					printf("\t[stepSize]	  : The grayscale stepsize.\n\n");
					break;

				case DO_RESIZE:
					printf("To resize a volume\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [newsizeX] [newsizeY] [newsizeZ]\n\n", DO_RESIZE);
					printf("\t[inputfile]     : The source file (MRC)\n");
					printf("\t[outfile]       : The destination file (MRC)\n");
					printf("\t[newsizeX]      : The new size of the data set in the x-Dimension\n");
					printf("\t[newsizeY]      : The new size of the data set in the y-Dimension\n");
					printf("\t[newsizeZ]      : The new size of the data set in the z-Dimension\n\n");
					break;

				case DO_DISPLAY_VOXEL_COUNT:
					printf("To display the non-zero voxel count of a volume \n");
					printf("\tGraySkeletonCPP.exe %i [inputfile]\n\n", DO_DISPLAY_VOXEL_COUNT);
					printf("\t[inputfile]     : The source file\n\n");
					break;

				case DO_TEXT_TO_VOLUME:
					printf("To convert a text file into a volume\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile]\n\n", DO_TEXT_TO_VOLUME);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n\n");
					break;

				case DO_CROPPING:
					printf("To Crop a volume\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [startx] [endx] [starty] [endy] [startz] [endz]\n\n", DO_CROPPING);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n");
					printf("\t[startd]        : The starting coordinate in the d dimension\n");
					printf("\t[endd]          : The ending coordinate in the d dimension\n\n");
					break;

				case DO_DOWNSAMPLING:
					printf("To Downsample a volume\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile]\n\n", DO_DOWNSAMPLING);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n\n");
					break;

				case DO_SMOOTHING:
					printf("To Smoothen a volume\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [minGrayscale] [maxGrayscale] [stRadius] [iterations]\n\n", DO_SMOOTHING);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n");
					printf("\t[minGrayscale]  : The minimum grayscale to consider (non-normalized)\n");
					printf("\t[maxGrayscale]  : The maximum grayscale to consider (non-normalized)\n");
					printf("\t[stRadius]      : The radius to be used for the structure tensor when smoothing\n");
					printf("\t[iterations]    : The number of smoothing iterations\n\n");
					break;

				case DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED:
					printf("To Smoothen a volume in an anisotropic fashion\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [xRadius] [yRadius] [zRadius] [iterations]\n\n", DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n");
					printf("\t[dRadius]       : The radius in the d dimension (0 - 10)\n");
					printf("\t[iterations]    : The number of smoothing iterations\n\n");
					break;


				case DO_THRESHOLD_GRAY_RANGE:
					printf("To threshold the grayrange of a volume. (Results in those out of range set to zero)\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [outfile] [minGrayscale] [maxGrayscale]\n\n", DO_THRESHOLD_GRAY_RANGE);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[outfile]       : The destination file\n");
					printf("\t[minGrayscale]  : The minimum grayscale to consider (non-normalized)\n");
					printf("\t[maxGrayscale]  : The maximum grayscale to consider (non-normalized)\n\n");
					break;

				case DO_CONVERSION:
					printf("To convert file formats\n");
					printf("\tGraySkeletonCPP.exe %i [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]\n\n", DO_CONVERSION);
					printf("\t[inputfile]     : The source file\n");
					printf("\t[inputformat]   : The source file format\n");
					printf("\t[outfile]       : The destination file\n");
					printf("\t[outformat]     : The destination file format\n");
					printf("\t[x-size]        : The size of the data set in the x-Dimension\n");
					printf("\t[y-size]        : The size of the data set in the y-Dimension\n");
					printf("\t[z-size]        : The size of the data set in the z-Dimension\n\n");
					break;
			}
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
					DisplayInputParams(DO_BINARY_THINNING_JU2007);
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
					DisplayInputParams(DO_TOPOLOGICAL_WATERSHED_JU2007);
					break;
			}
		}

		// Performs skeletonization of a grayscale volume directly using an adaptation of thinning which does not use binarization.
		void DoSkeletonizationAbeysinghe2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int minGray, int maxGray, int stepSize, int smoothingIterations, int smoothingRadius) {
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
					Volume * outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, NULL, minGray, maxGray, stepSize, smoothingIterations, smoothingRadius, minCurveSize, minSurfaceSize, 0.0, 0.0, outPath, false, 1.0, 1.0, 1.0);
					outputVol->toOFFCells2((char *)(outPath + ".off").c_str());
					outputVol->toMRCFile((char *)(outPath + ".mrc").c_str());
					delete outputVol;
					delete sourceVol;
					delete skeletonizer3D;			
					break;
				}
				default:
					DisplayInputParams(DO_SKELETONIZATION);
					break;
			}
		}

		// Performs skeletonization of a grayscale volume directly using an adaptation of thinning which does not use binarization, and then uses a pruning step to remove wrong branches
		void DoSkeletonizationAndPruningAbeysinghe2007(int dimensions, string inFile, string outFile, int minCurveSize, int minSurfaceSize, int maxCurveHole, int maxSurfaceHole, double pointThreshold, double curveThreshold, double surfaceThreshold, int pointRadius, int curveRadius, int surfaceRadius, int skeletonDirectionRadius, int minGray, int maxGray, int stepSize, int smoothingIterations, int smoothingRadius) {
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
					Volume * outputVol = skeletonizer3D->PerformImmersionSkeletonizationAndPruning(sourceVol, NULL, minGray, maxGray, stepSize, smoothingIterations, smoothingRadius, minCurveSize, minSurfaceSize, maxCurveHole, maxSurfaceHole, outPath, true, pointThreshold, curveThreshold, surfaceThreshold);
					delete sourceVol;
					delete skeletonizer3D;
					outputVol->toMRCFile((char *)outFile.c_str());		
					delete outputVol;
					break;
				}
				default:
					DisplayInputParams(DO_SKELETONIZATION_AND_PRUNING);
					break;
			}
		}




		void DoInteractiveSkeletonization(int dimensions, string inFile, string outFile, int minCurveSize, int curveRadius, int minGray, int maxGray, int stepSize, int startX, int startY, int startZ, int endX, int endY, int endZ, double skeletonRatio, double stRatio) {
			string outPath = outFile.substr(0, outFile.rfind("."));
			switch(dimensions){
				case 2:
					printf("Not implemented yet! \n");
					break;
				case 3: {
					printf("Not implemented yet! \n");
					//MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
					//Volume * sourceVol = reader->getVolume();
					//delete reader;

					//InteractiveSkeletonizer * skel = new InteractiveSkeletonizer(sourceVol, minGray, maxGray, stepSize, curveRadius, minCurveSize);
					//skel->SetGraphWeights(skeletonRatio, stRatio);
					//skel->CalculateMinimalSpanningTree(skel->FindClosestSkeletalPoint(Vector3DInt(startX, startY, startZ)));
					//vector<Vector3DInt> path = skel->GetPath(skel->FindClosestSkeletalPoint(Vector3DInt(endX, endY, endZ)));

					//Volume * skeletonPath = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
					//for(unsigned int i = 0; i < path.size(); i++) {
					//	skeletonPath->setDataAt(path[i].X(), path[i].Y(), path[i].Z(), 1);
					//}
					//skeletonPath->toMRCFile((char *)outFile.c_str());					

					//delete skeletonPath;
					//delete sourceVol;
					//delete skel;
					break;
				}
				default:
					DisplayInputParams(DO_INTERACTIVE_SKELETONIZATION);
					break;
			}
			
		}

/*		void DoTreeSkeletonization(int dimensions, string inFile, string outFile, int minCurveSize, int curveRadius, int minGray, int maxGray, int stepSize, int rootX, int rootY, int rootZ, double skeletonRatio, double stRatio) {
			string outPath = outFile.substr(0, outFile.rfind("."));
			switch(dimensions){
				case 2:
					printf("Not implemented yet! \n");
					break;
				case 3: {
					MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
					Volume * sourceVol = reader->getVolume();
					delete reader;

					TreeSkeletonizer * skel = new TreeSkeletonizer(sourceVol, minGray, maxGray, stepSize, curveRadius, minCurveSize);
					NonManifoldMesh_NoTags * mesh = skel->BuildTree(skel->FindClosestSkeletalPoint(Vector3DInt(rootX, rootY, rootZ)), skeletonRatio, stRatio);
					mesh->ToOffCells(outFile);
					
					delete sourceVol;
					delete skel;
					delete mesh;
					break;
				}
				default:
					DisplayInputParams(DO_INTERACTIVE_SKELETONIZATION);
					break;
			}
			
		}*/


		void Do_Resize(string inFile, string outFile, int newX, int newY, int newZ) {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			Volume * destVol = new Volume(newX, newY, newZ);
			double ox, oy, oz;	// The actual position of the old grid
			double dx, dy, dz;  // the delta from the integral position
			double fx, fy, fz;  // the floor
			double cx, cy, cz;  // the ceiling
			double i1, i2, j1, j2, w1, w2, iv;

			for(int x = 0; x < newX; x++) {
				ox = (double)(x * sourceVol->getSizeX()) / ((double)newX);
				fx = floor(ox);
				cx = ceil(ox);
				dx = ox - fx;
				for(int y = 0; y < newY; y++) {
					oy = (double)(y * sourceVol->getSizeY()) / ((double)newY);
					fy = floor(oy);
					cy = ceil(oy);
					dy = oy - fy;
					for(int z = 0; z < newZ; z++) {
						oz = (double)(z * sourceVol->getSizeZ()) / ((double)newZ);
						fz = floor(oz);
						cz = ceil(oz);
						dz = oz - fz;

						i1 = sourceVol->getDataAt(fx, fy, fz) * (1 - dz) + sourceVol->getDataAt(fx, fy, cz) * dz;
						i2 = sourceVol->getDataAt(fx, cy, fz) * (1 - dz) + sourceVol->getDataAt(fx, cy, cz) * dz;
						j1 = sourceVol->getDataAt(cx, fy, fz) * (1 - dz) + sourceVol->getDataAt(cx, fy, cz) * dz;
						j2 = sourceVol->getDataAt(cx, cy, fz) * (1 - dz) + sourceVol->getDataAt(cx, cy, cz) * dz;
						w1 = i1 * (1 - dy) + i2 * dy;
						w2 = j1 * (1 - dy) + j2 * dy;

						iv = w1 * (1- dx) + w2 * dx;

						destVol->setDataAt(x, y, z, iv);
					}
				}
			}


			destVol->toMRCFile((char *)outFile.c_str());
			delete destVol;
			delete reader;
			delete sourceVol;

		}
		void DoDisplayVoxelCount(string inFile) {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();

			printf("%i voxels\n" , sourceVol->getNonZeroVoxelCount());

			delete reader;
			delete sourceVol;

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

		void DoSmoothing(string inFile, string outFile, double minGrayscale, double maxGrayscale, int stRadius, int iterations) {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(stRadius, stRadius, stRadius, stRadius);
			for(int i = 0; i < iterations; i++) {
				skeletonizer->SmoothenVolume(sourceVol, minGrayscale, maxGrayscale, stRadius);
			}
			sourceVol->toMRCFile((char *)outFile.c_str());
			delete skeletonizer;
			delete reader;
			delete sourceVol;
		}

		void DoSmoothingAnisotropicAxisAligned(string inFile, string outFile, int xRadius, int yRadius, int zRadius, int iterations) {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			Volume * tempVol;
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(1,1,1,1);
			for(int i = 0; i < iterations; i++) {
				tempVol = VolumeSkeletonizer::PerformAnisotropicSmoothingAxisAligned(sourceVol, xRadius, yRadius, zRadius);
				delete sourceVol;
				sourceVol = tempVol;
			}
			sourceVol->toMRCFile((char *)outFile.c_str());
			delete reader;
			delete sourceVol;
		}

		void DoThresholdGrayRange(string inFile, string outFile, double minGrayscale, double maxGrayscale) {
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)inFile.c_str());
			Volume * sourceVol = reader->getVolume();
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, 0, 0, 0);
			skeletonizer->ThresholdGrayValueRange(sourceVol, minGrayscale, maxGrayscale);
			sourceVol->toMRCFile((char *)outFile.c_str());
			delete skeletonizer;
			delete reader;
			delete sourceVol;
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

		Volume * GetDistanceField(Vector3DDouble * points, int pointCount, int xSize, int ySize, int zSize, int gaussianBlur) {
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
			Vector3DDouble * points = new Vector3DDouble[maxPointCount];
			
			double step = 1;
			for(double i = 0; i < 2.0 * PI; i+= samplingRateI) {
				step = 1.0;
				for(double j = 0; j < PI/2.0; j+= (samplingRateJ*step) ) {
					points[pointCount] = Vector3DDouble(offset + radius* cos(i) * sin(j), offset + radius* sin(i)*sin(j), offset + radius*cos(j));
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
			Vector3DDouble * points = new Vector3DDouble[10000];
			int pointCount = 0;
			for (int x = 0; x < xCount; x++) {		
				for(int y = 0; y < yCount; y++) {
					points[pointCount] = Vector3DDouble(startX + xIncrement*x + xDiffCumu, startY + yIncrement*y + yDiffCumu, margin);
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
	}
}

int main( int args, char * argv[] ) {
	appTimeManager.PushCurrentTime();
	int function;
	bool error = true;

	if(args >= 2) {
		function = StringUtils::StringToInt(argv[1]);
		switch(function) {		
			case DO_BINARY_THINNING_JU2007:
				// GraySkeletonCPP.exe DO_BINARY_THINNING_JU2007 [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [threshold]
				if(args == 8) {					
					DoBinaryThinningJu2007(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), StringUtils::StringToDouble(argv[7]));
					error = false;
				} 
				break;
			case DO_TOPOLOGICAL_WATERSHED_JU2007:
				//GraySkeletonCPP.exe DO_TOPOLOGICAL_WATERSHED_JU2007 [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize]
				if(args == 10) {					
					DoTopologicalWatershedJu2007(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), 
						StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]), StringUtils::StringToInt(argv[9]));
					error = false;
				} 
				break;
			case DO_SKELETONIZATION:
				//GraySkeletonCPP.exe [function] [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [minGrayValue] [maxGrayValue] [stepSize] [smoothingIterations] [smoothingRadius]
				if(args == 12) {					
					DoSkeletonizationAbeysinghe2007(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), 
						StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]), StringUtils::StringToInt(argv[9]), StringUtils::StringToInt(argv[10]), StringUtils::StringToInt(argv[11]));
					error = false;
				} 
				break;
			case DO_SKELETONIZATION_AND_PRUNING:
				//GraySkeletonCPP.exe DO_SKELETONIZATION_AND_PRUNING [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [curveRadius] [surfaceRadius] [minGrayValue]
				if(args == 9) {										
					DoSkeletonizationAndPruningAbeysinghe2007(3, argv[2], argv[3], StringUtils::StringToInt(argv[4]), StringUtils::StringToInt(argv[5]), 0, 0, 1.0, 0.57, 0.57, 1, StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), 3, StringUtils::StringToInt(argv[8]), 255, 1, 0, 2);
					error = false;
				} 
				//GraySkeletonCPP.exe DO_SKELETONIZATION_AND_PRUNING [dimensions] [inputfile] [outfile] [minCurveSize] [minSurfaceSize] [maxCurveHole] [maxSurfaceHole]
				//                    [pointThreshold] [curveThreshold] [surfaceThreshold] [pointRadius] [curveRadius] [surfaceRadius] [skeletonDirectionRadius]
				//                    [minGrayValue] [maxGrayValue] [stepSize]
				if(args == 21) {					
					DoSkeletonizationAndPruningAbeysinghe2007(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]),
						StringUtils::StringToDouble(argv[9]), StringUtils::StringToDouble(argv[10]), StringUtils::StringToDouble(argv[11]), StringUtils::StringToInt(argv[12]), StringUtils::StringToInt(argv[13]),
						StringUtils::StringToInt(argv[14]), StringUtils::StringToInt(argv[15]), StringUtils::StringToInt(argv[16]), StringUtils::StringToInt(argv[17]), StringUtils::StringToInt(argv[18]), StringUtils::StringToInt(argv[19]), StringUtils::StringToInt(argv[20]));
					error = false;
				} 
				break;
			case DO_INTERACTIVE_SKELETONIZATION:
				if(args == 18) {
					DoInteractiveSkeletonization(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), 
						StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]), 
						StringUtils::StringToInt(argv[8]), StringUtils::StringToInt(argv[10]), StringUtils::StringToInt(argv[11]), 
						StringUtils::StringToInt(argv[12]), StringUtils::StringToInt(argv[13]), StringUtils::StringToInt(argv[14]), 
						StringUtils::StringToInt(argv[15]), StringUtils::StringToDouble(argv[16]), StringUtils::StringToDouble(argv[17]));
					error = false;					
				}
				break;
			/*case DO_TREE_SKELETONIZATION:
				if(args == 15) {
					DoTreeSkeletonization(StringUtils::StringToInt(argv[2]), argv[3], argv[4], StringUtils::StringToInt(argv[5]), 
						StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]), 
						StringUtils::StringToInt(argv[8]), StringUtils::StringToInt(argv[10]), StringUtils::StringToInt(argv[11]), 
						StringUtils::StringToInt(argv[12]), StringUtils::StringToDouble(argv[13]), StringUtils::StringToDouble(argv[14]));
					error = false;					
				}
				break;*/
			case DO_DISPLAY_VOXEL_COUNT:
				// GraySkeletonCPP.exe DO_DISPLAY_VOXEL_COUNT [inputfile]
				if(args == 3) {
					DoDisplayVoxelCount(argv[2]);
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
			case DO_RESIZE:
				// GraySkeletonCPP.exe DO_RESIZE [inputfile] [outfile] [newsizeX] [newsizeY] [newsizeZ]"
				if(args == 7) {
					Do_Resize(argv[2], argv[3], StringUtils::StringToInt(argv[4]), StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]));
					error = false;
				}
			case DO_CROPPING:
				// GraySkeletonCPP.exe DO_CROPPING [inputfile] [outfile] [startx] [endx] [starty] [endy] [startz] [endz]
				if(args == 10) {					
					DoCropping(argv[2], argv[3], StringUtils::StringToInt(argv[4]), StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]), StringUtils::StringToInt(argv[9]));
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
			case DO_SMOOTHING:
				// GraySkeletonCPP.exe DO_SMOOTHING [inputfile] [outfile] [minGrayscale] [maxGrayscale] [stRadius] [iterations]
				if(args == 8) {					
					DoSmoothing(argv[2], argv[3], StringUtils::StringToDouble(argv[4]), StringUtils::StringToDouble(argv[5]), StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]));
					error = false;
				} 
				break;
			case DO_THRESHOLD_GRAY_RANGE:
				// GraySkeletonCPP.exe DO_THRESHOLD_GRAY_RANGE [inputfile] [outfile] [minGrayscale] [maxGrayscale]
				if(args == 6) {					
					DoThresholdGrayRange(argv[2], argv[3], StringUtils::StringToDouble(argv[4]), StringUtils::StringToDouble(argv[5]));
					error = false;
				} 
				break;
			case DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED:
				// GraySkeletonCPP.exe DO_SMOOTHING_ANISOTROPIC_AXIS_ALIGNED [inputfile] [outfile] [xRadius] [yRadius] [zRadius] [iterations]
				if(args == 8) {					
					DoSmoothingAnisotropicAxisAligned(argv[2], argv[3], StringUtils::StringToInt(argv[4]), StringUtils::StringToInt(argv[5]), StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]));
					error = false;
				} 
				break;
			case DO_CONVERSION:
				// GraySkeletonCPP.exe DO_CONVERSION [inputfile] [inputformat] [outfile] [outformat] [x-size] [y-size] [z-size]
				if(args == 9) {					
					VolumeFormatConverter::ConvertVolume(argv[2], argv[3], argv[4], argv[5], StringUtils::StringToInt(argv[6]), StringUtils::StringToInt(argv[7]), StringUtils::StringToInt(argv[8]));
					error = false;
				} 
				break;					
		}		
		if(error) {
			DisplayInputParams(function);
		}
	} else {
		DisplayInputParams(DO_NOTHING);
	}



	appTimeManager.PopAndDisplayTime("\nTotal : %f seconds!\n");
}

#endif 
