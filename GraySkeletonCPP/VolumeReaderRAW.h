// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the RAW file format


#ifndef GRAYSKELETONCPP_VOLUME_READER_RAW_H
#define GRAYSKELETONCPP_VOLUME_READER_RAW_H

#include "VolumeReader.h"
#include <sys/stat.h>

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeReaderRAW : VolumeReader {
		public:
			static Volume * LoadVolume(string fileName);
			static Volume * LoadVolume8bit(string fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing);
			static Volume * LoadVolume16bit(string fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing);
			static void SaveVolume16bit(Volume * vol, string fileName);
		};

		Volume * VolumeReaderRAW::LoadVolume(string fileName) {
			struct stat results;
			int sideSize = 128;
		    
			if (stat(fileName.c_str(), &results) == 0) {
				int dimension = (int)round(pow(results.st_size, 0.333333333333));
				if (dimension * dimension * dimension == results.st_size) {
					sideSize = dimension;
				}
			}
			return LoadVolume8bit(fileName, sideSize, sideSize, sideSize, 1, 1, 1);
		}

		Volume * VolumeReaderRAW::LoadVolume8bit(string fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing) {
			FILE* fin = fopen( fileName.c_str(), "rb" );		

			Volume* vol = new Volume( xSize, ySize, zSize) ;	
			
			int size = xSize * ySize * zSize;
			unsigned char * tempdata = new unsigned char[size] ;
			fread(tempdata, sizeof(unsigned char), size, fin) ;
			int ct = 0 ;
			for(int z = 0; z < zSize ; z++) {
				for (int y = 0; y < ySize ; y++) {
					for (int x = 0 ; x < xSize ; x++) {
						vol->setDataAt(x,y,z, tempdata[ ct ]);
						ct ++ ;
					}
				}
			}		
			fclose( fin ) ;
			vol->setSpacing(xSpacing, ySpacing, zSpacing);

			delete [] tempdata ;	
			return vol;
		}

		Volume * VolumeReaderRAW::LoadVolume16bit(string fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing) {
			FILE* fin = fopen( fileName.c_str(), "rb" );		

			Volume* vol = new Volume( xSize, ySize, zSize) ;	
			
			int size = xSize * ySize * zSize;
			unsigned short * tempdata = new unsigned short[size] ;
			fread(tempdata, sizeof(unsigned short), size, fin) ;
			int ct = 0 ;
			for(int z = 0; z < zSize ; z++) {
				for (int y = 0; y < ySize ; y++) {
					for (int x = 0 ; x < xSize ; x++) {
						vol->setDataAt(x,y,z, tempdata[ ct ]);
						ct ++ ;
					}
				}
			}		
			fclose( fin ) ;
			vol->setSpacing(xSpacing, ySpacing, zSpacing);

			delete [] tempdata ;	
			return vol;
		}

		void VolumeReaderRAW::SaveVolume16bit(Volume * vol, string fileName) {
			FILE* fin = fopen( fileName.c_str(), "wb" );		
			
			unsigned short value;


			for(int x = 0; x < vol->getSizeX(); x++) {
				for (int y = 0; y < vol->getSizeY(); y++) {
					for (int z = 0 ; z < vol->getSizeZ(); z++) {
						value = vol->getDataAt(x, y, z);
						fwrite(&value, sizeof(unsigned short), 1, fin);						
					}
				}
			}		
		}
	}
}

#endif
