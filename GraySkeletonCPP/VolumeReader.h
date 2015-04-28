// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Base class for all Volume Readers


#ifndef GRAYSKELETONCPP_VOLUME_READER_H
#define GRAYSKELETONCPP_VOLUME_READER_H

#include <SkeletonMaker/volume.h>

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeReader {
		public:
			static Volume * LoadVolume(string fileName);
			static void SaveVolume(Volume * volume, string fileName);
		};

		Volume * VolumeReader::LoadVolume(string fileName) {
			printf("VolumeReader::LoadVolume not implemented");
			return NULL;
		}


		void VolumeReader::SaveVolume(Volume * volume, string fileName) {
			printf("VolumeReader::SaveVolume not implemented");
		}
	}
}

#endif
