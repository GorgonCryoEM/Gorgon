#ifndef GRAYSKELETONCPP_VOLUME_READER_TXT_H
#define GRAYSKELETONCPP_VOLUME_READER_TXT_H

#include "VolumeReader.h"

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
			fscanf(inFile, "%ld %ld %ld\n", &xSize, &ySize, &zSize);
			Volume * vol = new Volume(xSize,ySize,zSize);
			for(int z = 0; z < zSize; z++) {
				for(int y = 0; y < ySize; y++) {
					for(int x = 0; x < xSize; x++) {
						fscanf(inFile, "%ld ", &value);
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