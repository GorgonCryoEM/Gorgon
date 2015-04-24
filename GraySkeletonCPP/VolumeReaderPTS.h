// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the PTS file format.

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraySkeletonCPP/VolumeReaderPTS.h,v $
//   $Revision: 1.4 $
//   $Date: 2009/10/13 18:09:34 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: VolumeReaderPTS.h,v $
//   Revision 1.4  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.3  2009/03/03 22:33:11  ssa1
//   Point clouds are little gaussians.. (no longer bins)
//
//   Revision 1.2  2009/03/02 17:21:40  ssa1
//   Point clouds are little gaussians.. (no longer bins)
//
//   Revision 1.1  2009/03/02 16:31:47  ssa1
//   Adding in Point Clouds and Structure Tensor Fields


#ifndef GRAYSKELETONCPP_VOLUME_READER_PTS_H
#define GRAYSKELETONCPP_VOLUME_READER_PTS_H

#include "VolumeReader.h"
#include <vector>
#include <MathTools/Vector3D.h>

using namespace std;

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {

		class VolumeReaderPTS : VolumeReader {
		public:
			static Volume * LoadVolume(string fileName);
			static void SaveVolume(Volume * volume, string fileName);
		};

		Volume * VolumeReaderPTS::LoadVolume(string fileName) {		
			int count;
			double x, y, z;
			double maxX, maxY, maxZ, minX, minY, minZ;			

			// Reading
			FILE * inFile = fopen(fileName.c_str(), "rt");
			fscanf(inFile, "%d\n", &count);

			for(int i = 0; i < count; i++){
				fscanf(inFile, "%lf %lf %lf\n", &x, &y, &z);
				if(i == 0) {
					maxX = x; maxY = y; maxZ = z;
					minX = x; minY = y; minZ = z;
				}
				minX = min(x, minX);
				minY = min(y, minY);
				minZ = min(z, minZ);
				maxX = max(x, maxX);
				maxY = max(y, maxY);
				maxZ = max(z, maxZ);
			}
			fclose(inFile);	

			// Convert to volume
			double maxVolumeSize = 256.0;
			double xRatio = maxVolumeSize / (maxX - minX);
			double yRatio = maxVolumeSize / (maxY - minY);
			double zRatio = maxVolumeSize / (maxZ - minZ);
			double ratio = min(xRatio, min(yRatio, zRatio));

			int sizeX = 1 + (int)(round(ratio * (maxX - minX)));
			int sizeY = 1 + (int)(round(ratio * (maxY - minY)));
			int sizeZ = 1 + (int)(round(ratio * (maxZ - minZ)));

			Volume * vol = new Volume(sizeX, sizeY, sizeZ);

			int xx, yy, zz;
			int radius = 3;

			inFile = fopen(fileName.c_str(), "rt");
			fscanf(inFile, "%d\n", &count);

			double currVal, r;
			double cost;
			for(int i = 0; i < count; i++){
				fscanf(inFile, "%lf %lf %lf\n", &x, &y, &z);
				xx = (int)round((x - minX) * ratio);
				yy = (int)round((y - minY) * ratio);
				zz = (int)round((z - minZ) * ratio);		
	
				for(int xxx = max(xx - radius, 0); xxx < min(xx + radius, sizeX); xxx++) {
					for(int yyy = max(yy - radius, 0); yyy < min(yy + radius, sizeY); yyy++) {
						for(int zzz = max(zz - radius, 0); zzz < min(zz + radius, sizeZ); zzz++) {
							currVal = vol->getDataAt(xxx, yyy, zzz);
							r = (Vector3DDouble((x - minX) * ratio, (y - minY) * ratio, (z - minZ) * ratio) - Vector3DDouble(xxx, yyy, zzz)).Length();
							cost = pow(2.718281828, -(r * r) / 2.0);
							vol->setDataAt(xxx, yyy, zzz, currVal + cost);
						}
					}
				}
				
			}
	
			fclose(inFile);		
			
			return vol;
		}

	}
}

#endif