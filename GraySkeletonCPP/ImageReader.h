#ifndef IMAGE_READER_H
#define IMAGE_READER_H

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class ImageReader {
		public:
			static GrayImage * LoadRGBImage(char * fileName);
			static GrayImage * LoadGrayscaleImage(char * fileName);
			static void SaveRGBImage(GrayImage * image, char * fileName);
			static void SaveGrayscaleImage(GrayImage * image, char * fileName);
		};

		GrayImage * ImageReader::LoadRGBImage(char * fileName) {
			printf("ImageReader::LoadRGBImage not implemented");
			return NULL;
		}

		GrayImage *  ImageReader::LoadGrayscaleImage(char * fileName) {
			printf("ImageReader::LoadGrayscaleImage not implemented");
			return NULL;
		}

		void ImageReader::SaveRGBImage(GrayImage * image, char * fileName) {
			printf("ImageReader::SaveRGBImage not implemented");
		}

		void ImageReader::SaveGrayscaleImage(GrayImage * image, char * fileName) {
			printf("ImageReader::SaveGrayscaleImage not implemented");
		}
	}
}
	
#endif