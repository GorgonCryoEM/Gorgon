// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   The base class for all image readers


#ifndef GRAYSKELETONCPP_IMAGE_READER_H
#define GRAYSKELETONCPP_IMAGE_READER_H

#include "GrayImage.h"

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class ImageReader {
		public:
			static GrayImage * LoadRGBImage(string fileName);
			static GrayImage * LoadGrayscaleImage(string fileName);
			static void SaveRGBImage(GrayImage * image, string fileName);
			static void SaveGrayscaleImage(GrayImage * image, string fileName);
		};

		GrayImage * ImageReader::LoadRGBImage(string fileName) {
			printf("ImageReader::LoadRGBImage not implemented");
			return NULL;
		}

		GrayImage *  ImageReader::LoadGrayscaleImage(string fileName) {
			printf("ImageReader::LoadGrayscaleImage not implemented");
			return NULL;
		}

		void ImageReader::SaveRGBImage(GrayImage * image, string fileName) {
			printf("ImageReader::SaveRGBImage not implemented");
		}

		void ImageReader::SaveGrayscaleImage(GrayImage * image, string fileName) {
			printf("ImageReader::SaveGrayscaleImage not implemented");
		}
	}
}
	
#endif
