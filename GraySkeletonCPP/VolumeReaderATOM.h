// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads volumes in the ATOM file format


#ifndef GRAYSKELETONCPP_VOLUME_READER_ATOM_H
#define GRAYSKELETONCPP_VOLUME_READER_ATOM_H

#include "VolumeReader.h"

using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		const int MAX_FIXED_POINT_COUNT = 10000;

		class VolumeReaderATOM : VolumeReader {
		public:
			static void GetVolumeBounds(string fileName, int &minX, int &minY, int &minZ, int &maxX, int &maxY, int &maxZ);
			static Volume * LoadVolume(string fileName, string fixFile = "", string helixFile = "", int sizeX = -1, int sizeY = -1, int sizeZ = -1);
		};

		void VolumeReaderATOM::GetVolumeBounds(string fileName, int &minX, int &minY, int &minZ, int &maxX, int &maxY, int &maxZ) {
			const int MIN_COORD = -1;
			const int MAX_COORD = 2147483647;

			minX = MAX_COORD;
			minY = MAX_COORD;
			minZ = MAX_COORD;
			maxX = MIN_COORD;
			maxY = MIN_COORD;
			maxZ = MIN_COORD;

			FILE* fin = fopen((char *)fileName.c_str(), "rt");
			if (fin == NULL)
			{
				printf("Error reading input file %s.\n", fileName.c_str()) ;
				exit(0) ;
			}
			int x, y, z;
			double gray;
			while(!feof(fin))
			{
				fscanf(fin, "%i %i %i %lf\n", &x, &y, &z, &gray);
				minX = min(x, minX);
				minY = min(y, minY);
				minZ = min(z, minZ);
				maxX = max(x, maxX);
				maxY = max(y, maxY);
				maxZ = max(z, maxZ);
			}	
			fclose(fin);
		}

		Volume * VolumeReaderATOM::LoadVolume(string fileName, string fixFile, string helixFile, int sizeX, int sizeY, int sizeZ) {
			int * fixPoints = new int[MAX_FIXED_POINT_COUNT];
			int fixPointCount = 0;
			int coord;
			FILE * fin;
			if(fixFile.length() > 0) {
				fin = fopen((char *)fixFile.c_str(), "rt");
				while (!feof(fin)) {
					fscanf(fin, "%i\n", &coord);
					fixPoints[fixPointCount] = coord;
					fixPointCount++;
				}
				fclose(fin);
			}

			int helixStart, helixEnd;
			int helixCount = 0;

			int * helixStarts = new int[MAX_FIXED_POINT_COUNT];
			int * helixEnds = new int[MAX_FIXED_POINT_COUNT];
			if(helixFile.length() > 0) {
				fin = fopen((char *)helixFile.c_str(), "rt");
				while (!feof(fin)) {
					fscanf(fin, "%i %i\n", &helixStart, &helixEnd);
					helixStarts[helixCount] = helixStart;
					helixEnds[helixCount] = helixEnd;
					helixCount++;
				}
				fclose(fin);
			}

			int minX, minY, minZ, maxX, maxY, maxZ;
			GetVolumeBounds(fileName, minX, minY, minZ, maxX, maxY, maxZ);

			maxX+=2;
			maxY+=2;
			maxZ+=2;

			Volume * vol;

			if((sizeX > 0) && (sizeY > 0) && (sizeZ > 0)) {
				vol = new Volume(sizeX, sizeY, sizeZ);
			} else {
				vol = new Volume(maxX, maxY, maxZ);
			}

			fin = fopen((char *)fileName.c_str(), "rt");
			if (fin == NULL)
			{
				printf("Error reading input file %s.\n", fileName.c_str()) ;
				exit(0) ;
			}
			int x, y, z;
			double gray;
			int index = 1;
			bool isFixed;
			bool isHelix;
			double helixNum = 0;
			while(!feof(fin))
			{
				fscanf(fin, "%i %i %i %lf\n", &x, &y, &z, &gray);				
				isFixed = false;
				for(int i = 0; (i < fixPointCount) && !isFixed; i++) {
					isFixed = isFixed || index == fixPoints[i];
				}

				isHelix = false;
				for(int i = 0; (i < helixCount) && !isHelix; i++) {
					isHelix = isHelix || (index == helixStarts[i]) || (index == helixEnds[i]);
					if(isHelix) {
						helixNum = (double)i+3.0;
					}
				}

				if(isHelix) {
					vol->setDataAt(x, y, z, helixNum);
				} else if(isFixed) {
					vol->setDataAt(x, y, z, 2.0);
				} else {
					vol->setDataAt(x, y, z, 1.0);
				}

				index++;
			}	
			fclose(fin);
			delete [] fixPoints;
			delete [] helixStarts;
			delete [] helixEnds;
			return vol;
		}
	}
}

#endif
