#ifndef VOLUME_READER_RAW_H
#define VOLUME_READER_RAW_H

#include "VolumeReader.h"
#include <sys/stat.h>

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeReaderRAW : VolumeReader {
		public:
			static Volume * LoadVolume(char * fileName);
			static Volume * LoadVolume(char * fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing);
			static Volume * SaveVolume(Volume * volume, char * fileName);
		};

		Volume * VolumeReaderRAW::LoadVolume(char * fileName) {
			struct stat results;
			int sideSize = 128;
		    
			if (stat(fileName, &results) == 0) {
				int dimension = (int)round(pow(results.st_size, 0.333333333333));
				if (dimension * dimension * dimension == results.st_size) {
					sideSize = dimension;
				}
			}
			return LoadVolume(fileName, sideSize, sideSize, sideSize, 1, 1, 1);
		}

		Volume * VolumeReaderRAW::LoadVolume(char * fileName, int xSize, int ySize, int zSize, int xSpacing, int ySpacing, int zSpacing) {
			FILE* fin = fopen( fileName, "rb" );		

			Volume* vol = new Volume( xSize, ySize, zSize) ;	
			
			int size = xSize * ySize * zSize;
			unsigned char * tempdata = new unsigned char[size] ;
			fread(tempdata, sizeof(unsigned char), size, fin) ;
			int ct = 0 ;
			for(int x = 0; x < xSize ; x++) {
				for (int y = 0; y < ySize ; y++) {
					for (int z = 0 ; z < zSize ; z++) {
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
	}
}

#endif