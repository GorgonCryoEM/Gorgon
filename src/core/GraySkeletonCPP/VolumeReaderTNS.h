// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the TNS File Format



#ifndef GRAYSKELETONCPP_VOLUME_READER_TNS_H
#define GRAYSKELETONCPP_VOLUME_READER_TNS_H

#include "VolumeReader.h"
#include "VolumeSkeletonizer.h"
#include <MathTools/Vector3D.h>
#include <MathTools/Combinatorics.h>
#include <MathTools/DataStructures.h>



using namespace std;

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {

		class VolumeReaderTNS : VolumeReader {
		public:
			static Volume * LoadVolume(string fileName);
			static void SaveVolume(Volume * volume, string fileName);
		};

		void VolumeReaderTNS::SaveVolume(Volume * volume, string fileName) {

			FILE * outFile = fopen(fileName.c_str(), "wt");
			fprintf(outFile, "%d %d %d \n", volume->getSizeX(), volume->getSizeY(), volume->getSizeZ());

			volume->pad(10, 0);

			VolumeSkeletonizer * skel = new VolumeSkeletonizer(3, 3, 3, 3);

			printf("Calculating gradients.\n");
			Vector3DFloat * volumeGradient = skel->GetVolumeGradient(volume);

			ProbabilityDistribution3D gaussianFilter;
			gaussianFilter.radius = 3;
			skel->math->GetBinomialDistribution(gaussianFilter);

			printf("Calculating Eigens.\n");
			EigenResults3D * volumeEigens = skel->GetEigenResults(volume, volumeGradient, gaussianFilter, 3, false);
			EigenResults3D eigen;

			delete skel;
			delete [] volumeGradient;			


			printf("Saving Eigens.\n");
			for(int x = 10; x < volume->getSizeX() - 10; x++) {
				for(int y = 10; y < volume->getSizeY() - 10; y++) {
					for(int z = 10; z < volume->getSizeZ() - 10; z++) {
						if(volumeEigens == NULL) {
							skel->GetEigenResult(eigen, volumeGradient, gaussianFilter, x, y, z, volume->getSizeX(), volume->getSizeY(), volume->getSizeZ(), gaussianFilter.radius, false);
						} else {
							eigen = volumeEigens[volume->getIndex(x, y, z)];
						}
						fprintf(outFile, "{");
						for(int i = 0; i < 3; i++) {
							fprintf(outFile, "{%lf, {%lf %lf %lf}}", eigen.values[i], eigen.vectors[i].X(), eigen.vectors[i].Y(), eigen.vectors[i].Z());
							if(i < 2) {
								fprintf(outFile, ",");
							}
						}
						fprintf(outFile, "} \n");
					}
				}
			}				
			fclose(outFile);			
			printf("Done.\n");

			if(volumeEigens != NULL) {
				delete [] volumeEigens;
			}

		}

	}
}

#endif
