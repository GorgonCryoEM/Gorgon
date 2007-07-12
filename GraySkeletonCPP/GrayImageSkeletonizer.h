#ifndef GRAY_IMAGE_SKELETONIZER_H
#define GRAY_IMAGE_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "VolumeSkeletonizer.h"

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class GrayImageSkeletonizer {
		public:
			static void PerformJuSkeletonization(char * inputFile, char * outputFile);
		private:		
			static void AddIterationToImage(GrayImage * compositeImage, Volume * iterationVolume, int threshold);
			static void MarkDeletablePixels(GrayImage * deletedPixels, GrayImage * pixelClassification);
			static GrayImage * ClassifyImagePixels(GrayImage * sourceImage);
			static int GetN4Count(GrayImage * sourceImage, int xPos, int yPos);
			static int GetN8Count(GrayImage * sourceImage, int xPos, int yPos);
			static bool IsPixelDeletable( int x, int y, GrayImage * deletedPixels, GrayImage * pixelClassification);
			static bool IsPoint(GrayImage * sourceImage, int xPos, int yPos);
			static bool IsSurfaceBody(GrayImage * sourceImage, int xPos, int yPos);
			static bool IsSurface(GrayImage * sourceImage, int xPos, int yPos);
			static bool IsCurveEnd(GrayImage * sourceImage, int xPos, int yPos);
			static unsigned char GetSkeletalPixelClassification(GrayImage * sourceImage, int xPos, int yPos);
		};

		void GrayImageSkeletonizer::PerformJuSkeletonization(char * inputFile, char * outputFile) {
			GrayImage * image = ImageReaderBMP::LoadGrayscaleImage(inputFile, true);
			//image->Blur();
			GrayImage * compositeImage = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * deletedPixels = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * thresholdedImage;
			GrayImage * pixelClassification;
			GrayImage * finalSkeleton;

			Volume * imageVol = image->ToVolume();
			Volume * preservedVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * curveVolume;
			Volume * topologyVolume;
			
			GrayImageList * outputImages = new GrayImageList();
			char compositeFile[1000];
			

			for(unsigned char threshold = 255; threshold > 0; threshold--) {		
				// Thresholding the image Only for display purposes.
				thresholdedImage = new GrayImage(image); 
				thresholdedImage->Threshold(threshold, false);

				// Skeletonizing while preserving curve features and topology
				curveVolume = VolumeSkeletonizer::GetJuCurveSkeleton(imageVol, preservedVolume, threshold);
				VolumeSkeletonizer::PruneCurves(curveVolume, PRUNE_AMOUNT);
				VolumeSkeletonizer::VoxelOr(preservedVolume, curveVolume);
				topologyVolume = VolumeSkeletonizer::GetJuTopologySkeleton(imageVol, preservedVolume, threshold);

				// Making the composite image
				AddIterationToImage(compositeImage, topologyVolume, threshold);

				// Removing deleted pixels from the image
				finalSkeleton = new GrayImage(compositeImage);
				finalSkeleton->ApplyMask(deletedPixels, PIXEL_BINARY_TRUE, false);

				// Deleting pixels which satisfy the deletability criteria
				pixelClassification = ClassifyImagePixels(finalSkeleton);
				MarkDeletablePixels(deletedPixels, pixelClassification);


				// Display List
				outputImages->AddImage(new GrayImage(thresholdedImage));					// Thresholded Images
				outputImages->AddImage(GrayImage::GrayImageVolumeToImage(curveVolume));		// Curve Preserving skeleton after pruning
				outputImages->AddImage(GrayImage::GrayImageVolumeToImage(topologyVolume));	// Topology Preserving skelton
				outputImages->AddImage(new GrayImage(compositeImage));						// Grayscale skeleton
				outputImages->AddImage(new GrayImage(pixelClassification));					// Pixel Classification
				outputImages->AddImage(new GrayImage(deletedPixels));						// Deleted Pixels
				outputImages->AddImage(new GrayImage(finalSkeleton));						// Final skeleton after pixel deletion

				delete preservedVolume;
				preservedVolume = topologyVolume;
				delete curveVolume;
				delete pixelClassification;
				delete thresholdedImage;
			}	
			outputImages->AddImage(new GrayImage(image));	// Source Image

			ImageReaderBMP::SaveGrayscaleImage(compositeImage, outputFile);
			sprintf(compositeFile, "%s-comp.bmp", outputFile);
			
			GrayImage * outputImage = outputImages->GetCompositeImage(16*3);

			ImageReaderBMP::SaveGrayscaleImage(outputImage, compositeFile);

			outputImages->Clear(true);

			delete outputImages;
			delete outputImage;
			delete preservedVolume;
			delete imageVol;
			delete compositeImage;
			delete image;
		}


		void GrayImageSkeletonizer::AddIterationToImage(GrayImage * compositeImage, Volume * iterationVolume, int threshold) {
			for(int x = 0; x < iterationVolume->getSizeX(); x++) {
				for(int y = 0; y < iterationVolume ->getSizeY(); y++) {
					if((compositeImage->GetDataAt(x,y) == 0) && (iterationVolume->getDataAt(x, y, GRAYIMAGE_IN_VOLUME_Z) > 0)) {
						compositeImage->SetDataAt(x,y,threshold);
					}
				}
			}
		}


		void GrayImageSkeletonizer::MarkDeletablePixels(GrayImage * deletedPixels, GrayImage * pixelClassification) {
			for(int x = 0; x < deletedPixels->GetSizeX(); x++) {
				for(int y = 0; y < deletedPixels->GetSizeY(); y++) {
					if((deletedPixels->GetDataAt(x, y) != PIXEL_BINARY_TRUE) && IsPixelDeletable(x, y, deletedPixels, pixelClassification)) {
						deletedPixels->SetDataAt(x, y, PIXEL_BINARY_TRUE);
					}
				}
			}

		}

		GrayImage * GrayImageSkeletonizer::ClassifyImagePixels(GrayImage * sourceImage) {
			int sizeX = sourceImage->GetSizeX();
			int sizeY = sourceImage->GetSizeY();
			GrayImage * classifiedImage = new GrayImage(sizeX, sizeY);

			for(int x = 0; x < sizeX; x++)  {
				for(int y = 0; y < sizeY; y++)  {
					if((x == 0) || (y == 0) || (x == sizeX-1) || (y == sizeY-1)) {
						classifiedImage->SetDataAt(x, y, PIXEL_CLASS_BACKGROUND);
					} else {
						classifiedImage->SetDataAt(x, y, GetSkeletalPixelClassification(sourceImage, x, y));
					}
				}
			}
			return classifiedImage;
		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		int GrayImageSkeletonizer::GetN4Count(GrayImage * sourceImage, int xPos, int yPos) {
			int n4Count = 0;
			for(int i = 0; i < 4; i++) {
				if(sourceImage->GetDataAt(xPos + IMAGE_NEIGHBORS_4[i][0], yPos + IMAGE_NEIGHBORS_4[i][1])) {
					n4Count++;
				}
			}
			return n4Count;

		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		int GrayImageSkeletonizer::GetN8Count(GrayImage * sourceImage, int xPos, int yPos) {
			int n4Count = 0;
			for(int i = 0; i < 8; i++) {
				if(sourceImage->GetDataAt(xPos + IMAGE_NEIGHBORS_8[i][0], yPos + IMAGE_NEIGHBORS_8[i][1])) {
					n4Count++;
				}
			}
			return n4Count;
		}
		bool GrayImageSkeletonizer::IsPixelDeletable( int x, int y, GrayImage * deletedPixels, GrayImage * pixelClassification) {
			return (
				(pixelClassification->GetDataAt(x, y) == PIXEL_CLASS_SURFACE_BODY)	// Surface body pixels
				);
		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		bool GrayImageSkeletonizer::IsPoint(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN4Count(sourceImage, xPos, yPos) == 0);

		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		bool GrayImageSkeletonizer::IsSurfaceBody(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN8Count(sourceImage, xPos, yPos) == 8);
		}


		// PreCondition: xPos and yPos are not pixels in the border of the image
		bool GrayImageSkeletonizer::IsSurface(GrayImage * sourceImage, int xPos, int yPos) {
			return (
				((sourceImage->GetDataAt(xPos + 1, yPos) > 0) && (sourceImage->GetDataAt(xPos + 1, yPos - 1) > 0) && (sourceImage->GetDataAt(xPos, yPos-1) > 0)) ||
				((sourceImage->GetDataAt(xPos - 1, yPos) > 0) && (sourceImage->GetDataAt(xPos - 1, yPos - 1) > 0) && (sourceImage->GetDataAt(xPos, yPos-1) > 0)) ||
				((sourceImage->GetDataAt(xPos - 1, yPos) > 0) && (sourceImage->GetDataAt(xPos - 1, yPos + 1) > 0) && (sourceImage->GetDataAt(xPos, yPos+1) > 0)) ||
				((sourceImage->GetDataAt(xPos + 1, yPos) > 0) && (sourceImage->GetDataAt(xPos + 1, yPos + 1) > 0) && (sourceImage->GetDataAt(xPos, yPos+1) > 0))
				);
		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		bool GrayImageSkeletonizer::IsCurveEnd(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN4Count(sourceImage, xPos, yPos) == 1);
		}

		// PreCondition: xPos and yPos are not pixels in the border of the image
		unsigned char GrayImageSkeletonizer::GetSkeletalPixelClassification(GrayImage * sourceImage, int xPos, int yPos) {
			unsigned char pixelClass = PIXEL_CLASS_BACKGROUND;
			if(sourceImage->GetDataAt(xPos, yPos) != 0) {
				if(IsPoint(sourceImage, xPos, yPos)) {
					pixelClass = PIXEL_CLASS_POINT;
				} else if (IsSurfaceBody(sourceImage, xPos, yPos)) {
					pixelClass = PIXEL_CLASS_SURFACE_BODY;
				} else if (IsSurface(sourceImage, xPos, yPos)) {
					pixelClass = PIXEL_CLASS_SURFACE_BORDER;
				} else if (IsCurveEnd(sourceImage, xPos, yPos)) {
					pixelClass = PIXEL_CLASS_CURVE_END;
				} else {
					pixelClass = PIXEL_CLASS_CURVE_BODY;
				}
			}
			return pixelClass;		
		}
	}
}

#endif