#ifndef VOLUME_READER_H
#define VOLUME_READER_H

#include "..\SkeletonMaker\volume.h"

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeReader {
		public:
			static Volume * LoadVolume(char * fileName);
			static void SaveVolume(Volume * volume, char * fileName);
		};

		Volume * VolumeReader::LoadVolume(char * fileName) {
			printf("VolumeReader::LoadVolume not implemented");
			return NULL;
		}


		void VolumeReader::SaveVolume(Volume * volume, char * fileName) {
			printf("VolumeReader::SaveVolume not implemented");
		}
	}
}

#endif