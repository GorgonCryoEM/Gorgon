// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads Grayscale images from MRC files


#ifndef GRAYSKELETONCPP_IMAGE_READER_MRC_H
#define GRAYSKELETONCPP_IMAGE_READER_MRC_H

#include "GrayImage.h"
#include "ImageReader.h"
#include <SkeletonMaker/reader.h>
#include <SkeletonMaker/volume.h>
#include <string>

using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class ImageReaderMRC : ImageReader {
		public:
			static GrayImage * LoadGrayscaleImage(string fileName, int slice, char dimension);
			static void SaveGrayscaleImage(GrayImage * image, string fileName);
			static void SaveGrayscaleImage(Volume * volume, string fileName);
		};

		GrayImage * ImageReaderMRC::LoadGrayscaleImage(string fileName, int slice, char dimension) {
			double minVal = 0.0, maxVal = 0.0, currVal = 0.0;
			int sizeX = 0, sizeY = 0;
			GrayImage * outImage;

			Volume * vol = (MRCReaderPicker::pick((char *)fileName.c_str()))->getVolume();

			switch(dimension) {
				case 'x' :
					sizeX = vol->getSizeZ();
					sizeY = vol->getSizeY();
					minVal = vol->getDataAt(slice, 0, 0);
					maxVal = vol->getDataAt(slice, 0, 0);
					break;
				case 'y' :
					sizeX = vol->getSizeX();
					sizeY = vol->getSizeZ();
					minVal = vol->getDataAt(0, slice, 0);
					maxVal = vol->getDataAt(0, slice, 0);
					break;
				case 'z' :
					sizeX = vol->getSizeX();
					sizeY = vol->getSizeY();
					minVal = vol->getDataAt(0, 0, slice);
					maxVal = vol->getDataAt(0, 0, slice);
					break;
			}
			outImage = new GrayImage(sizeX, sizeY);

			for(int i = 0; i < sizeX; i++) {
				for(int j = 0; j < sizeY; j++) {
					switch(dimension) {
						case 'x' :
							currVal = vol->getDataAt(slice, j, i);
							break;
						case 'y' :
							currVal = vol->getDataAt(i, slice, j);
							break;
						case 'z' :
							currVal = vol->getDataAt(i, j, slice);
							break;
						default:
							currVal = 0;
							break;
					}
					if(currVal < minVal) {
						minVal = currVal;
					}
					if(currVal > maxVal) {
						maxVal = currVal;
					}					
				}
			}
		
			if(minVal == maxVal) {
				for(int i = 0; i < sizeX; i++) {
					for(int j = 0; j < sizeY; j++) {
						outImage->SetDataAt(i, j, 0);
					}
				}

			} else {			
				for(int i = 0; i < sizeX; i++) {
					for(int j = 0; j < sizeY; j++) {
						switch(dimension) {
							case 'x' :
								currVal = vol->getDataAt(slice, j, i);
								break;
							case 'y' :
								currVal = vol->getDataAt(i, slice, j);
								break;
							case 'z' :
								currVal = vol->getDataAt(i, j, slice);
								break;
						}
						
						outImage->SetDataAt(i, j, (((currVal - minVal) / (maxVal - minVal)) * 255.0));								
					}
				}
			}

			delete vol;
			return outImage;
		}

		void ImageReaderMRC::SaveGrayscaleImage(GrayImage * image, string fileName) {
			Volume * volume = image->ToVolume();
			volume->toMRCFile((char *)fileName.c_str());
			delete volume;
		}
		void ImageReaderMRC::SaveGrayscaleImage(Volume * volume, string fileName) {
			
			int sizeX = volume->getSizeX();
			int sizeY = volume->getSizeY();
			int sizeZ = volume->getSizeZ();
			printf("This will not work if the dimensions exceed 260\n");
			double avg[260][260];
			Volume * tempVolume = new Volume(sizeX, sizeY, sizeZ, 0, 0, 0, volume);
			tempVolume->normalize(0.0, 255.0);
			double maxVal;
			double avgVal;
			double val;
			double minAvg, maxAvg;

			
			GrayImage * maxImage = new GrayImage(sizeX, sizeY);
			GrayImage * avgImage = new GrayImage(sizeX, sizeY);
			minAvg = 3000;
			maxAvg = -1;

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					maxVal = 0;
					avgVal = 0;
					for(int z = 0; z < sizeZ; z++) {
						val = tempVolume->getDataAt(x, y, z);
						maxVal = max(maxVal, val);
						avgVal += val/(double)sizeZ;	
						
					}
					maxImage->SetDataAt(x, y, (unsigned char)round(maxVal));
					avg[x][y] = avgVal;
					minAvg = min(minAvg, avgVal);
					maxAvg = max(maxAvg, avgVal);
				}
			}

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					avg[x][y] = ((avg[x][y] - minAvg) / (maxAvg - minAvg)) * 255.0;
					avgImage->SetDataAt(x, y, (unsigned char)round(avg[x][y]));
				}
			}

			ImageReaderBMP::SaveGrayscaleImage(maxImage, "max_xy_" + fileName);
			ImageReaderBMP::SaveGrayscaleImage(avgImage, "avg_xy_" + fileName);
			delete maxImage;
			delete avgImage;



			maxImage = new GrayImage(sizeX, sizeZ);
			avgImage = new GrayImage(sizeX, sizeZ);
			minAvg = 3000;
			maxAvg = -1;

			for(int x = 0; x < sizeX; x++) {
				for(int z = 0; z < sizeZ; z++) {
					maxVal = 0;
					avgVal = 0;
					for(int y = 0; y < sizeY; y++) {
						val = tempVolume->getDataAt(x, y, z);
						maxVal = max(maxVal, val);
						avgVal += val/(double)sizeY;	
						
					}
					maxImage->SetDataAt(x, z, (unsigned char)round(maxVal));
					avg[x][z] = avgVal;
					minAvg = min(minAvg, avgVal);
					maxAvg = max(maxAvg, avgVal);
				}
			}

			for(int x = 0; x < sizeX; x++) {
				for(int z = 0; z < sizeZ; z++) {
					avg[x][z] = ((avg[x][z] - minAvg) / (maxAvg - minAvg)) * 255.0;
					avgImage->SetDataAt(x, z, (unsigned char)round(avg[x][z]));
				}
			}

			ImageReaderBMP::SaveGrayscaleImage(maxImage, "max_xz_" + fileName);
			ImageReaderBMP::SaveGrayscaleImage(avgImage, "avg_xz_" + fileName);
			delete maxImage;
			delete avgImage;


			maxImage = new GrayImage(sizeY, sizeZ);
			avgImage = new GrayImage(sizeY, sizeZ);
			minAvg = 3000;
			maxAvg = -1;

			for(int y = 0; y < sizeY; y++) {
				for(int z = 0; z < sizeZ; z++) {
					maxVal = 0;
					avgVal = 0;
					for(int x = 0; x < sizeX; x++) {
						val = tempVolume->getDataAt(x, y, z);
						maxVal = max(maxVal, val);
						avgVal += val/(double)sizeX;	
						
					}
					maxImage->SetDataAt(y, z, (unsigned char)round(maxVal));
					avg[y][z] = avgVal;
					minAvg = min(minAvg, avgVal);
					maxAvg = max(maxAvg, avgVal);
				}
			}

			for(int y = 0; y < sizeY; y++) {
				for(int z = 0; z < sizeZ; z++) {
					avg[y][z] = ((avg[y][z] - minAvg) / (maxAvg - minAvg)) * 255.0;
					avgImage->SetDataAt(y, z, (unsigned char)round(avg[y][z]));
				}
			}

			ImageReaderBMP::SaveGrayscaleImage(maxImage, "max_yz_" + fileName);
			ImageReaderBMP::SaveGrayscaleImage(avgImage, "avg_yz_" + fileName);
			delete maxImage;
			delete avgImage;

			delete tempVolume;

		}
	}
};

#endif
