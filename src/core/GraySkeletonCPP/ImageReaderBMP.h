// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Reads grayscale images from BMP files


#ifndef GRAYSKELETONCPP_IMAGE_READER_BMP_H
#define GRAYSKELETONCPP_IMAGE_READER_BMP_H

#include "GrayImage.h"
#include "ImageReader.h"
#include "GlobalDefinitions.h"
#include <string>
#include <SkeletonMaker/volume.h>
#include <Foundation/StringUtils.h>


using namespace std;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
	
		#pragma pack(push)
		#pragma pack(1)

		struct ImageReaderBMPFileHeader {
			short	bfType;
			int		bfSize;
			short	bfReserved1;
			short	bfReserved2;
			int		bfOffBits;
		};

		struct ImageReaderBMPInfoHeader {
			int		biSize;
			int		biWidth;
			int		biHeight;
			short	biPlanes;
			short	biBitCount;
			int		biCompression;
			int		biSizeImage;
			int		biXPelsPerMeter;
			int		biYPelsPerMeter;
			int		biClrUsed;
			int		biClrImportant;
		};

		struct ImageReaderBMPRgbQuad {
			char rgbBlue;
			char rgbGreen;
			char rgbRed;
			char rgbReserved;
		};
		#pragma pack(pop)

		class ImageReaderBMP : ImageReader {
		public:

			static GrayImage * LoadGrayscaleImage(string fileName);
			static Volume * LoadVolumeAsImageSet(string fileName, int sliceCount);
			static void SaveGrayscaleImage(GrayImage * image, string fileName);
			static void SaveVolumeAsImageSet(Volume * volume, string fileName);
		private:
			static ImageReaderBMPFileHeader GetBMPFileHeader(GrayImage * image);
			static ImageReaderBMPInfoHeader GetBMPInfoHeader(GrayImage * image);
		};

		GrayImage * ImageReaderBMP::LoadGrayscaleImage(string fileName) {
			FILE * inFile = fopen(fileName.c_str(), "rb");
			ImageReaderBMPFileHeader fileHeader;
			ImageReaderBMPInfoHeader infoHeader;
			ImageReaderBMPRgbQuad palette[256];

			fread(&fileHeader, sizeof(fileHeader), 1, inFile); 
			fread(&infoHeader, sizeof(infoHeader), 1, inFile); 

			if( infoHeader.biBitCount <= 8) {
				fread(&palette, sizeof(ImageReaderBMPRgbQuad), 256, inFile); 
			}
			

			int width = ((int)(infoHeader.biWidth / 4)) * 4;
			if(width < infoHeader.biWidth) {
				width = width + 4;
			}
			unsigned char value;
			unsigned char value8;
			unsigned char value24[3];
			unsigned char value32[4];
			bool error = false;

			GrayImage * image = new GrayImage(infoHeader.biWidth, infoHeader.biHeight);

			for(int y = 0; y < infoHeader.biHeight; y++) {		
				for(int x = 0; x < width; x++) {

					switch(infoHeader.biBitCount) {
						case 8:
							fread(&value8, sizeof(unsigned char), 1, inFile);													
							if(x < infoHeader.biWidth) {
								value = (unsigned char)round(((double)palette[value8].rgbBlue + (double)palette[value8].rgbGreen +  (double)palette[value8].rgbRed) / 3.0);
								image->SetDataAt(x, y, value);
							}
							break;
						case 24:
							fread(&value24, sizeof(unsigned char), 3, inFile);													
							if(x < infoHeader.biWidth) {
								value = (unsigned char)round(((double)value24[0] + (double)value24[1] +  (double)value24[2]) / 3.0);
								image->SetDataAt(x, y, value);
							}
							break;
						case 32:
							fread(&value32, sizeof(unsigned char), 4, inFile);													
							if(x < infoHeader.biWidth) {
								value = (unsigned char)round(((double)value32[0] + (double)value32[1] +  (double)value32[2]) / 3.0);
								image->SetDataAt(x, y, value);
							}
							break;

						default:
							error = true;
							break;
					}
				}
			}

			if(error) {
				printf("%s is in an unsupported format... Image might be corrupted!\n", (char *)fileName.c_str());
			}

			fclose(inFile);
			return image;
		}

		Volume * ImageReaderBMP::LoadVolumeAsImageSet(string fileName, int sliceCount) {
			Volume * vol;
			string actualFileName;
			GrayImage * image;


			for(int z = 0; z < sliceCount; z++) {
				actualFileName = fileName + StringUtils::IntToString(z, 5) + string(".bmp");
				image = LoadGrayscaleImage(actualFileName);	
				if(z == 0) {
					vol = new Volume(image->GetSizeX(), image->GetSizeY(), sliceCount);
				}
				for(int x = 0; x < image->GetSizeX(); x++) {
					for(int y = 0; y < image->GetSizeY(); y++) {
						vol->setDataAt(x, y, z, 255 - image->GetDataAt(x, y));
					}
				}

				delete image;
			}

			return vol;
		}

		void ImageReaderBMP::SaveGrayscaleImage(GrayImage * image, string fileName) {
			FILE * outFile = fopen(fileName.c_str(), "wb");
			ImageReaderBMPFileHeader fileHeader = GetBMPFileHeader(image);
			ImageReaderBMPInfoHeader infoHeader = GetBMPInfoHeader(image);

			ImageReaderBMPRgbQuad palette[256];
			for(int i = 0; i < 256; i++) {
				palette[i].rgbBlue = i;
				palette[i].rgbGreen = i;
				palette[i].rgbRed = i;
				palette[i].rgbReserved = 0;
			}

			fwrite((char *)(&fileHeader), sizeof(ImageReaderBMPFileHeader), 1, outFile); 
			fwrite((char *)(&infoHeader), sizeof(ImageReaderBMPInfoHeader), 1, outFile); 
			fwrite((char *)(&palette), sizeof(ImageReaderBMPRgbQuad), 256, outFile); 

			int width = ((int)(image->GetSizeX() / 4)) * 4;
			if(width < image->GetSizeX()) {
				width = width + 4;
			}
			unsigned char data;

			for(int y = 0; y < image->GetSizeY(); y++) {
				for(int x = 0; x < width; x++) {
					if(x >= image->GetSizeX()) {
						data = 0;
					} else {
						data = image->GetDataAt(x, y);
					}
					fwrite((char *)(&data), 1, 1, outFile); 
				}
			}
			fclose(outFile);
		}




		void ImageReaderBMP::SaveVolumeAsImageSet(Volume * volume, string fileName) {
			volume->normalize(0.0, 255.0);
			string actualFileName;
			int value;

			for(int z = 0; z < volume->getSizeZ(); z++) {
				GrayImage * image = new GrayImage(volume->getSizeX(), volume->getSizeY());
				for(int x = 0; x < volume->getSizeX(); x++) {
					for(int y = 0; y < volume->getSizeY(); y++) {
						value = 255 - (int)round(volume->getDataAt(x, y, z));
						//printf("%d ", value);

						image->SetDataAt(x, y, value);
					}
				}
				actualFileName = fileName + StringUtils::IntToString(z, 5) + string(".bmp");
				SaveGrayscaleImage(image, actualFileName);	

				delete image;
				//printf("\n"); flushall();
			}
		}

		ImageReaderBMPFileHeader ImageReaderBMP::GetBMPFileHeader(GrayImage * image) {
			ImageReaderBMPFileHeader header;
			header.bfType = 19778;
			header.bfSize = 1078 + image->GetSizeX() * image->GetSizeY();
			header.bfReserved1 = 0;
			header.bfReserved2 = 0;
			header.bfOffBits = 1078;
			return header;
		}

		ImageReaderBMPInfoHeader ImageReaderBMP::GetBMPInfoHeader(GrayImage * image) {
			ImageReaderBMPInfoHeader header;
			header.biSize = 40;
			header.biWidth = image->GetSizeX();
			header.biHeight = image->GetSizeY();
			header.biPlanes = 0;
			header.biBitCount = 8;
			header.biCompression = 0;
			header.biSizeImage = 0;
			header.biXPelsPerMeter = 0;
			header.biYPelsPerMeter = 0;
			header.biClrUsed = 0;
			header.biClrImportant = 0;
			return header;
		}
	}
};

#endif
