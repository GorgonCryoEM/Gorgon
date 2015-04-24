// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the TXT file format.

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraySkeletonCPP/VolumeReaderTXT.h,v $
//   $Revision: 1.4 $
//   $Date: 2009/10/13 18:09:34 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: VolumeReaderTXT.h,v $
//   Revision 1.4  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.3  2008/09/29 16:30:15  ssa1
//   Adding in CVS meta information
//

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