#ifndef GRAYSKELETONCPP_IMAGE_READER_BMP_H
#define GRAYSKELETONCPP_IMAGE_READER_BMP_H

#include "GrayImage.h"
#include "ImageReader.h"
#include "GlobalDefinitions.h"
#include <string>


using namespace std;

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
			static void SaveGrayscaleImage(GrayImage * image, string fileName);
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
			fread(&palette, sizeof(ImageReaderBMPRgbQuad), 256, inFile); 

			int width = ((int)(infoHeader.biWidth / 4)) * 4;
			if(width < infoHeader.biWidth) {
				width = width + 4;
			}
			unsigned char value;

			GrayImage * image = new GrayImage(infoHeader.biWidth, infoHeader.biHeight);

			for(int y = 0; y < infoHeader.biHeight; y++) {		
				for(int x = 0; x < width; x++) {
					fread(&value, sizeof(unsigned char), 1, inFile);
					if(x < infoHeader.biWidth) {
						value = (unsigned char)round(((double)palette[value].rgbBlue + (double)palette[value].rgbGreen +  (double)palette[value].rgbRed) / 3.0);
						image->SetDataAt(x, y, value);
					}
				}
			}

			fclose(inFile);
			return image;
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