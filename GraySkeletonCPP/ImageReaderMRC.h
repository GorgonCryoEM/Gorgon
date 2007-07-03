#ifndef IMAGE_READER_MRC_H
#define IMAGE_READER_MRC_H

#include "GrayImage.h"
#include "ImageReader.h"
#include "..\SkeletonMaker\reader.h"
#include "..\SkeletonMaker\volume.h"

namespace WUSTL_MM {

	class ImageReaderMRC : ImageReader {
	public:
		static GrayImage * LoadGrayscaleImage(char * fileName, int slice, char dimension);
		static void SaveGrayscaleImage(GrayImage * image, char * fileName);
		static void SaveGrayscaleImage(Volume * volume, char * fileName);
	};

	GrayImage * ImageReaderMRC::LoadGrayscaleImage(char * fileName, int slice, char dimension) {
		double minVal, maxVal, currVal;
		int sizeX, sizeY;
		GrayImage * outImage;

		Volume * vol = (MRCReaderPicker::pick(fileName))->getVolume();

		switch(dimension) {
			case 'x' :
				sizeX = vol->sizez;
				sizeY = vol->sizey;
				minVal = vol->getDataAt(slice, 0, 0);
				maxVal = vol->getDataAt(slice, 0, 0);
				break;
			case 'y' :
				sizeX = vol->sizex;
				sizeY = vol->sizez;
				minVal = vol->getDataAt(0, slice, 0);
				maxVal = vol->getDataAt(0, slice, 0);
				break;
			case 'z' :
				sizeX = vol->sizex;
				sizeY = vol->sizey;
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

	void ImageReaderMRC::SaveGrayscaleImage(GrayImage * image, char * fileName) {
		Volume * volume = image->ToVolume();
		volume->toMRCFile(fileName);
		delete volume;
	}

};

#endif