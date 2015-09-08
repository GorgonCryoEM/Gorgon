// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the TXT file format.


#ifndef GRAYSKELETONCPP_VOLUME_READER_TXT_H
#define GRAYSKELETONCPP_VOLUME_READER_TXT_H

#include "VolumeReader.h"

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {

		class VolumeReaderTXT : VolumeReader {
		public:
			static Volume * LoadVolume(string fileName);
			static Volume * SaveVolume(Volume * volume, string fileName);
		};

		Volume * VolumeReaderTXT::LoadVolume(string fileName) {
			FILE * inFile = fopen(fileName.c_str(), "rt");
			int xSize, ySize, zSize, value;
			fscanf(inFile, "%d %d %d\n", &xSize, &ySize, &zSize);
			Volume * vol = new Volume(xSize,ySize,zSize);
			for(int z = 0; z < zSize; z++) {
				for(int y = 0; y < ySize; y++) {
					for(int x = 0; x < xSize; x++) {
						fscanf(inFile, "%d ", &value);
						vol->setDataAt(x, y, z, value);
					}
				}
			}

			fclose(inFile);			
			return vol;
		}

	}
}

#endif
