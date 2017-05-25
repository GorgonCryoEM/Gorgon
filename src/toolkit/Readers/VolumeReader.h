// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Base class for all Volume Readers

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraySkeletonCPP/VolumeReader.h,v $
//   $Revision: 1.8 $
//   $Date: 2009/10/13 18:09:34 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: VolumeReader.h,v $
//   Revision 1.8  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.7  2008/09/29 16:30:15  ssa1
//   Adding in CVS meta information
//

#ifndef GRAYSKELETONCPP_VOLUME_READER_H
#define GRAYSKELETONCPP_VOLUME_READER_H

#include <Core/volume.h>
//wustl_mm::
using namespace SkeletonMaker;
using namespace Core;
//namespace wustl_mm {
//	namespace GraySkeletonCPP {
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
//	}
//}

#endif