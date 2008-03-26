#ifndef PROTEINMORPH_SWITCHBOARD_CPP
#define PROTEINMORPH_SWITCHBOARD_CPP

#include "ProteinMesh.h"
#include <Foundation/TimeManager.h>
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/VolumeReaderATOM.h>
#include <string>
#include <Foundation/StringUtils.h>
#include "NonManifoldMesh.h"


using namespace std;
using namespace wustl_mm::GraySkeletonCPP;

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
			NonManifoldMesh * mesh = new ProteinMesh(skeletonVol, NULL, sheetVol);
			NonManifoldMesh * smoothedMesh = mesh->SmoothLaplacian(0.5);
			NonManifoldMesh * smoothedMesh2 = smoothedMesh->SmoothLaplacian(0.5);
			NonManifoldMesh * smoothedMesh3 = smoothedMesh2->SmoothLaplacian(0.5);
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

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::Foundation;

int main( int args, char * argv[] ) {
	int function = DO_NOTHING;
	bool error = true;

	if(args >= 2) {
		function = StringUtils::StringToInt(argv[1]);
		switch(function) {
			case DO_SIMPLIFY:
				if(args == 7) {
					DoSimplify(argv[2], argv[3], argv[4], argv[5], argv[6]);
					error = false;
				}
				break;
		}
	} 
	
	if(error) {
		DisplayInputFormat(function);
	}

	return 0;
}



#endif