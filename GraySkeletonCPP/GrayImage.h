#ifndef GRAY_IMAGE_H
#define GRAY_IMAGE_H

#include "..\SkeletonMaker\reader.h"
#include "..\SkeletonMaker\volume.h"

namespace WUSTL_MM {
	const int GRAYIMAGE_IN_VOLUME_Z = 2;
	const int GRAYIMAGE_VOLUME_Z_SIZE = 5;

	class GrayImage {
	public:
		GrayImage();
		GrayImage(int sizeX, int sizeY);
		GrayImage(GrayImage * sourceImage);
		~GrayImage();

		unsigned char GetDataAt(int x, int y);
		int GetSizeX();
		int GetSizeY();
		void SetDataAt(int x, int y, unsigned char value);
		Volume * ToVolume();
		static GrayImage * GrayImageVolumeToImage(Volume * volume);
		void Threshold(unsigned char threshold, bool preserveHighValues);
	private:
		int sizeX;
		int sizeY;
		unsigned char * data;
	};

	GrayImage::GrayImage() {
		data = NULL;
		sizeX = 0;
		sizeY = 0;
	}

	GrayImage::GrayImage(int sizeX, int sizeY) {
		this->sizeX = sizeX;
		this->sizeY = sizeY;
		data = new unsigned char[sizeX * sizeY];
		for(int x = 0; x < sizeX; x++){
			for(int y = 0; y < sizeY; y++) {
				SetDataAt(x, y, 0);
			}
		}
	}

	GrayImage::GrayImage(GrayImage * sourceImage) {
		this->sizeX = sourceImage->GetSizeX();
		this->sizeY = sourceImage->GetSizeY();
		data = new unsigned char[sizeX * sizeY];
		for(int x = 0; x < sizeX; x++){
			for(int y = 0; y < sizeY; y++) {
				SetDataAt(x, y, sourceImage->GetDataAt(x, y));
			}
		}

	}

	GrayImage::~GrayImage() {
		if(data != NULL) {
			delete data;
		}
	}

	unsigned char GrayImage::GetDataAt(int x, int y) {
		return data[x + sizeX * y]; 
	}

	int GrayImage::GetSizeX() {
		return sizeX;
	}

	int GrayImage::GetSizeY() {
		return sizeY;
	}

	void GrayImage::SetDataAt(int x, int y, unsigned char value) {
		data[x + sizeX * y] = value;
	}

	Volume * GrayImage::ToVolume() {
		Volume * vol = new Volume(sizeX, sizeY, 5);
		for(int z = 0; z < GRAYIMAGE_VOLUME_Z_SIZE; z++) {
			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {		
					if(z == GRAYIMAGE_IN_VOLUME_Z) {
						vol->setDataAt(x, y, z, (double)GetDataAt(x, y));
					}
					else {
						vol->setDataAt(x, y, z, 0);
					}
				}
			}
		}
		return vol;
	}
	GrayImage * GrayImage::GrayImageVolumeToImage(Volume * volume) {
		GrayImage * image = new GrayImage(volume->getSizeX(), volume->getSizeY());
		double minValue = volume->getDataAt(0,0,5);
		double maxValue = volume->getDataAt(0,0,0);
		double currValue;

		for(int x = 0; x < volume->getSizeX(); x++) {
			for(int y = 0; y < volume->getSizeY(); y++) {
				currValue = volume->getDataAt(x, y, GRAYIMAGE_IN_VOLUME_Z);
				if(currValue < minValue) {
					minValue = currValue;
				}
				if(currValue > maxValue) {
					maxValue = currValue;
				}
			}
		}
		if(minValue == maxValue) {
			for(int x = 0; x < volume->getSizeX(); x++) {
				for(int y = 0; y < volume->getSizeY(); y++) {
					image->SetDataAt(x, y, 0);
				}
			}
		}

		for(int x = 0; x < volume->getSizeX(); x++) {
			for(int y = 0; y < volume->getSizeY(); y++) {
				image->SetDataAt(x, y, (unsigned char)((volume->getDataAt(x, y, GRAYIMAGE_IN_VOLUME_Z) - minValue) * 255.0 / (maxValue - minValue)));
			}
		}
		return image;
	}
	void GrayImage::Threshold(unsigned char threshold, bool preserveHighValues) {
		for(int x = 0; x < sizeX; x++) {
			for(int y = 0; y < sizeY; y++) {
				if(GetDataAt(x, y) < threshold) {
					SetDataAt(x, y, 0);
				} else if (!preserveHighValues) {
					SetDataAt(x, y, 255);
				}
				
			}
		}
	}
};


#endif 