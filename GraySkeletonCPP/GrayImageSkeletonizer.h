#ifndef GRAY_IMAGE_SKELETONIZER_H
#define GRAY_IMAGE_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"

namespace WUSTL_MM {
	class GrayImageSkeletonizer {
	public:
		static void PerformSkeletonization(char * inputFile, char * outputFile);
	private:
		static const int PRUNE_AMOUNT = 3;
		static void AddIterationToImage(GrayImage * compositeImage, Volume * iterationVolume, int threshold);	
	};


	void GrayImageSkeletonizer::AddIterationToImage(GrayImage * compositeImage, Volume * iterationVolume, int threshold) {
		for(int x = 0; x < iterationVolume->getSizeX(); x++) {
			for(int y = 0; y < iterationVolume ->getSizeY(); y++) {
				if((compositeImage->GetDataAt(x,y) == 0) && (iterationVolume->getDataAt(x, y, GRAYIMAGE_IN_VOLUME_Z) > 0)) {
					compositeImage->SetDataAt(x,y,threshold);
				}
			}
		}
	}

	void GrayImageSkeletonizer::PerformSkeletonization(char * inputFile, char * outputFile) {
		GrayImage * image = ImageReaderBMP::LoadGrayscaleImage(inputFile);
		GrayImage * compositeImage = new GrayImage(image->GetSizeX(), image->GetSizeY());
		Volume * imageVol = image->ToVolume();
		Volume * preservedVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
		
		GrayImageList * outputImages = new GrayImageList();
		char compositeFile[1000];
		

		for(int threshold = 255; threshold > 0; threshold--) {		
			GrayImage * thresholdedImage = new GrayImage(image);
			thresholdedImage->Threshold(threshold, false);
			outputImages->AddImage(thresholdedImage);

			Volume * curveVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
			curveVolume->curveSkeleton(threshold, preservedVolume);			
			outputImages->AddImage(GrayImage::GrayImageVolumeToImage(curveVolume));
			curveVolume->erodeHelix(PRUNE_AMOUNT);
			outputImages->AddImage(GrayImage::GrayImageVolumeToImage(curveVolume));

			Volume * topologyVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
			topologyVolume->skeleton(threshold, preservedVolume, curveVolume);
			outputImages->AddImage(GrayImage::GrayImageVolumeToImage(topologyVolume));

			AddIterationToImage(compositeImage, topologyVolume, threshold);
			
			outputImages->AddImage(new GrayImage(compositeImage));

			delete preservedVolume;
			preservedVolume = topologyVolume;

			delete curveVolume;
		}	
		outputImages->AddImage(new GrayImage(image));

		ImageReaderBMP::SaveGrayscaleImage(compositeImage, outputFile);
		sprintf(compositeFile, "%s-comp.bmp", outputFile);
		
		GrayImage * outputImage = outputImages->GetCompositeImage(16*5, 16);

		ImageReaderBMP::SaveGrayscaleImage(outputImage, compositeFile);

		outputImages->Clear(true);

		delete outputImages;
		delete outputImage;
		delete preservedVolume;
		delete imageVol;
		delete compositeImage;
		delete image;
	}

	
}

#endif