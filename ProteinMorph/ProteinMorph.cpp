// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the Protein Morph Module.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.12  2009/11/19 18:19:24  ssa1
//   Improved flexible fitting.. (Split nodes to guarantee direction)
//
//   Revision 1.11  2009/11/04 20:29:38  ssa1
//   Implementing Triangle based clique search and chain based flexible fitting.
//
//   Revision 1.10  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.9  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.8  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//
//   Revision 1.7  2008/11/13 15:49:00  ssa1
//   Performance improvements for creating a mesh from marching cubes
//
//   Revision 1.6  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//

#ifndef PROTEINMORPH_SWITCHBOARD_CPP
#define PROTEINMORPH_SWITCHBOARD_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <Foundation/TimeManager.h>
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/VolumeReaderATOM.h>
#include <string>
#include <Foundation/StringUtils.h>
#include "SSECorrespondenceFinder.h"
#include "SSECorrespondenceFeature.h"
#include "NonManifoldMesh.h"
#include "NonManifoldMesh2.h"
#include <MathTools/MathLib.h>
#include <SkeletonMaker/PriorityQueue.h>

using namespace std;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace Protein_Morph {

		const int DO_NOTHING = -1;
		const int DO_SIMPLIFY = 1;


		void DisplayInputFormat(int function) {
			switch(function) {
				case DO_NOTHING:
					printf("ProteinMorph.exe [function] {[args]}*\n\n");
					printf("\t[function = %i]    - Simplify the skeleton\n", DO_SIMPLIFY);
					break;
				case DO_SIMPLIFY:
					printf("ProteinMorph.exe %i [skeleton] [helices] [sheets] [fixedPoints] [outFile]\n\n", DO_SIMPLIFY);
					printf("\t[skeleton]   -The ATOM file containing the skeletal voxels\n");
					printf("\t[helices]    -The ATOM file containing the helix voxels\n");
					printf("\t[sheets]     -The ATOM file containing the sheet voxels\n");
					printf("\t[fixedPoints]-The FIX file containing the fixed voxel indices\n");
					printf("\t[outFile]    -The name of the output file\n");
					break;
				default:
					DisplayInputFormat(DO_NOTHING);
			}
		}

		void DoSimplify(string skeleton, string helix, string sheet, string fix, string outFile) {
			string outPath = outFile.substr(0, outFile.rfind("."));	
			Volume * skeletonVol = VolumeReaderATOM::LoadVolume(skeleton, fix, helix);
			//Volume * helixVol = VolumeReaderATOM::LoadVolume(helix, "", skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());
			Volume * sheetVol = VolumeReaderATOM::LoadVolume(sheet, "", "", skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());
			//NonManifoldMesh * mesh = new ProteinMesh(skeletonVol, helixVol, sheetVol);
			NonManifoldMesh_NoTags * mesh = new NonManifoldMesh_NoTags(skeletonVol);
			NonManifoldMesh_NoTags * smoothedMesh = mesh->SmoothLaplacian(0.5);
			NonManifoldMesh_NoTags * smoothedMesh2 = smoothedMesh->SmoothLaplacian(0.5);
			NonManifoldMesh_NoTags * smoothedMesh3 = smoothedMesh2->SmoothLaplacian(0.5);
			mesh->ToOffCells(outPath + "-Skeleton.off");
			smoothedMesh->ToOffCells(outPath + "-Skeleton-Smoothed.off");
			smoothedMesh2->ToOffCells(outPath + "-Skeleton-Smoothed2.off");
			smoothedMesh3->ToOffCells(outPath + "-Skeleton-Smoothed3.off");
			smoothedMesh3->ToMathematicaFile(outPath + "-Skeleton-Smoothed.nb");
			delete mesh;
			delete smoothedMesh;
			delete smoothedMesh2;
			delete smoothedMesh3;
			delete skeletonVol;
			//delete helixVol;
			delete sheetVol;
		}
	}
}

int main( int args, char * argv[] ) {
	//NonManifoldMesh_NoTags * m = NonManifoldMesh_NoTags::LoadOffFile("C:\\_WUSTL\\sasakthi\\research\\source\\Gorgon\\src_py\\temp\\Apple.off");
	//int function = DO_NOTHING;
	//bool error = true;

	//if(args >= 2) {
	//	function = StringUtils::StringToInt(argv[1]);
	//	switch(function) {
	//		case DO_SIMPLIFY:
	//			if(args == 7) {
	//				DoSimplify(argv[2], argv[3], argv[4], argv[5], argv[6]);
	//				error = false;
	//			}
	//			break;
	//	}
	//} 
	//
	//if(error) {
	//	DisplayInputFormat(function);
	//}

	vector<SSECorrespondenceFeature> fl1, fl2;


	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(1,1,0), Vector3DFloat(4,5,0)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(5,6,0), Vector3DFloat(10,7,0)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(12,7,0), Vector3DFloat(17,7,0)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(19,7,0), Vector3DFloat(24,6,0)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(25,5,0), Vector3DFloat(28,1,0)));
		
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(0,5,0), Vector3DFloat(5,6,0)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(6,7,0), Vector3DFloat(11,8,0)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(13,8,0), Vector3DFloat(18,8,0)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(20,8,0), Vector3DFloat(25,7,0)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(26,6,0), Vector3DFloat(31,5,0))); 

/*	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(-2.36, -4.82, -14.82), Vector3DFloat(8.02, 5.66, -13.65)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(8.14, -2.94, -7.86), Vector3DFloat(-4.29, -6.07, 0.87)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(-0.47, 9.74, -4.92), Vector3DFloat(11.38, 4.39, 5.46)));
	fl1.push_back(SSECorrespondenceFeature(Vector3DFloat(0.56, -8.16, 6.86), Vector3DFloat(2.45, -13.67, 4.68)));

	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(-2.15, -4.40, -14.25), Vector3DFloat(8.23, 6.08, -13.08)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(3.26, -5.05, -12.20), Vector3DFloat(-4.07, -5.65, 1.44)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(-0.26, 10.16, -4.35), Vector3DFloat(11.59, 4.81, 6.03)));
	fl2.push_back(SSECorrespondenceFeature(Vector3DFloat(0.77, -7.74, 7.43), Vector3DFloat(2.67, -13.25, 5.25)));
*/
	SSECorrespondenceFinder finder;
	//finder.InitializeFeatures(fl1, fl2);
	if(args != 17) {
		printf("ProteinMorph [method] [splitNodes] [multipleSearch] [pdb1(highres)] [pdb2(volume)] [rigidityThreshold] [featureChangeThreshold] [rigidityAngleCoeff] [rigidityCentroidDistanceCoeff] [rigidityFeatureChangeCoeff] [rigidComponentCoeff] [intraComponentCoeff] [jointAngleThreshold] [dihedralAngleThreshold] [centroidDistanceThreshold] [maxSolutionCount]\n");
		printf("\t[method] : The algorithm to use, 1 : Clique, 2: Greedy Valence, 3: Greedy Valence Triangle based Clique \n");
		printf("\t[splitNodes] : 0: dont split, 1: split based on direction \n");
		printf("\t[multipleSearch] : 0: Single Search, 1: Search for multiple instances of pdb1 in pdb2\n");
	} else {	
		finder.InitializeFeaturesFromPDBFiles(argv[4], argv[5]);
		finder.InitializeConstants(
			StringUtils::StringToDouble(argv[6]), 
			StringUtils::StringToDouble(argv[7]), 
			StringUtils::StringToDouble(argv[8]), 
			StringUtils::StringToDouble(argv[9]), 
			StringUtils::StringToDouble(argv[10]),
			StringUtils::StringToDouble(argv[11]), 
			StringUtils::StringToDouble(argv[12]), 
			StringUtils::StringToDouble(argv[13]), 
			StringUtils::StringToDouble(argv[14]), 
			StringUtils::StringToDouble(argv[15]), 
			StringUtils::StringToInt(argv[16]));

		finder.PrintFeatureListsMathematica();
		vector< vector < vector<SSECorrespondenceNode> > > corr;
		vector < vector<SSECorrespondenceNode> > corr2;
		bool useDirection = (StringUtils::StringToInt(argv[2]) == 1);
		bool multipleSearch = (StringUtils::StringToInt(argv[3]) == 1);

		switch(StringUtils::StringToInt(argv[1])) {
			case 1:
				if(multipleSearch) {
					printf("Multiple search not implemented for this method \n");
				} else {
					corr = finder.GetAStarCliqueBasedFeatureCorrespondence(true, useDirection);
				}
				break;
			case 2:
				if(multipleSearch) {
					corr2 = finder.GetValenceBasedFeatureCorrespondenceSet(false, useDirection);
				} else {
					corr2 = finder.GetValenceBasedFeatureCorrespondence(true, useDirection);
				}
				break;
			case 3:
				if(multipleSearch) {
					corr2 = finder.GetValenceBasedFeatureCorrespondenceSet(true, useDirection);
				} else {
					corr2 = finder.GetValenceTriangleBasedFeatureCorrespondence(true, useDirection);
				}
				break;
			case 4:
				if(multipleSearch) {
					printf("Multiple search not implemented for this method \n");
				} else {
					corr = finder.GetAStarTriangleBasedFeatureCorrespondence(true, useDirection);
				}
				break;
			default:
				printf("Unknown method\n");
				break;
		}		
	}	

	//float j1, j2, d;
	//f.GetFeatureAngles(j1, j2, d, f1, f2);
	//printf("%f %f %f\n", j1*180.0/PI, j2*180.0/PI, d*180.0/PI);

	return 0;
}



#endif