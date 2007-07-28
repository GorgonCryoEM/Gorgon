#ifndef IMAGE_READER_MRC_H
#define IMAGE_READER_MRC_H

#include "GrayImage.h"
#include "ImageReader.h"
#include "..\SkeletonMaker\reader.h"
#include "..\SkeletonMaker\volume.h"
#include <string.h>

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
			double minVal, maxVal, currVal;
			int sizeX, sizeY;
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
	}
};

#endif