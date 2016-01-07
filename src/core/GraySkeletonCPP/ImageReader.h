#ifndef CORE_GRAYSKELETONCPP_IMAGE_READER_H
#define CORE_GRAYSKELETONCPP_IMAGE_READER_H

#include "GrayImage.h"

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

#endif
