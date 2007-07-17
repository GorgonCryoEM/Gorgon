#ifndef GRAY_IMAGE_SKELETONIZER_H
#define GRAY_IMAGE_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "VolumeSkeletonizer.h"
#include "../MatlabInterface/MatlabWrapper.h"

using namespace wustl_mm::MatlabInterface;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class GrayImageSkeletonizer {
		public:
			GrayImageSkeletonizer();
			~GrayImageSkeletonizer();
			void PerformJuSkeletonization(char * inputFile, char * outputFile);
		private:		
			void AddIterationToImage(GrayImage * compositeImage, Volume * iterationVolume, int threshold);
			void MarkDeletablePixels(GrayImage * deletedPixels, GrayImage * pixelClassification, GrayImage * originalImage);
			GrayImage * ClassifyImagePixels(GrayImage * sourceImage, GrayImage * maskImage);
			int GetN4Count(GrayImage * sourceImage, int xPos, int yPos);
			int GetN8Count(GrayImage * sourceImage, int xPos, int yPos);
			bool IsPixelDeletable( int x, int y, GrayImage * originalImage, GrayImage * deletedPixels, GrayImage * pixelClassification);
			bool IsPoint(GrayImage * sourceImage, int xPos, int yPos);
			bool IsSurfaceBody(GrayImage * sourceImage, int xPos, int yPos);
			bool IsSurface(GrayImage * sourceImage, int xPos, int yPos);
			bool IsCurveEnd(GrayImage * sourceImage, int xPos, int yPos);
			bool FollowsCurveStructureTensor(GrayImage * sourceImage, int xPos, int yPos);
			unsigned char GetSkeletalPixelClassification(GrayImage * sourceImage, int xPos, int yPos);
			double GetPartialDerivativeX(GrayImage * sourceImage, int x, int y);
			double GetPartialDerivativeY(GrayImage * sourceImage, int x, int y);
		private:
			VolumeSkeletonizer * volumeSkeletonizer;
			MatlabWrapper * math;
		};	

		GrayImageSkeletonizer::GrayImageSkeletonizer() {
			volumeSkeletonizer = new VolumeSkeletonizer();
			math = volumeSkeletonizer->math;
		}

		GrayImageSkeletonizer::~GrayImageSkeletonizer() {
			delete volumeSkeletonizer;
		}

		void GrayImageSkeletonizer::PerformJuSkeletonization(char * inputFile, char * outputFile) {
			GrayImage * image = ImageReaderBMP::LoadGrayscaleImage(inputFile, true);
			image->Pad(PARTIAL_DERIVATIVE_MASK_SIZE, 0);

			GrayImage * compositeImage = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * deletedPixels = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * thresholdedImage;
			GrayImage * pixelClassification;
			GrayImage * finalSkeleton;
			GrayImage * topologyImage;
			GrayImage * newVoxelImage;

			Volume * imageVol = image->ToVolume();
			Volume * preservedVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * curveVolume;
			Volume * topologyVolume;
			Volume * newVoxelVolume;
			
			GrayImageList * outputImages = new GrayImageList();
			char compositeFile[1000];		

			for(int threshold = 255; threshold > 0; threshold-=2) {		
				printf("%i\n", threshold);
				// Thresholding the image Only for display purposes.
				thresholdedImage = new GrayImage(image); 
				thresholdedImage->Threshold(threshold, false);

				// Skeletonizing while preserving curve features and topology
				curveVolume = volumeSkeletonizer->GetJuCurveSkeleton(imageVol, preservedVolume, threshold, false);
				volumeSkeletonizer->PruneCurves(curveVolume, PRUNE_AMOUNT);
				volumeSkeletonizer->VoxelOr(curveVolume, preservedVolume);
				topologyVolume = volumeSkeletonizer->GetJuTopologySkeleton(imageVol, curveVolume, threshold);

				// Finding the new voxels
				newVoxelVolume = new Volume(topologyVolume->getSizeX(), topologyVolume->getSizeY(), topologyVolume->getSizeZ(), 0, 0, 0, topologyVolume);
				newVoxelVolume->subtract(preservedVolume);				
				newVoxelImage = GrayImage::GrayImageVolumeToImage(newVoxelVolume);
				delete newVoxelVolume;

				// Making the composite image
				AddIterationToImage(compositeImage, topologyVolume, threshold);
				// Removing deleted pixels from the image
				finalSkeleton = new GrayImage(compositeImage);
				finalSkeleton->ApplyMask(deletedPixels, PIXEL_BINARY_TRUE, false);

				
				// Deleting pixels which satisfy the deletability criteria
				pixelClassification = ClassifyImagePixels(finalSkeleton, newVoxelImage);
				MarkDeletablePixels(deletedPixels, pixelClassification, image);


				// Display List
				outputImages->AddImage(new GrayImage(thresholdedImage));					// Thresholded Images
				outputImages->AddImage(GrayImage::GrayImageVolumeToImage(curveVolume));		// Curve Preserving skeleton after pruning
				outputImages->AddImage(GrayImage::GrayImageVolumeToImage(topologyVolume));	// Topology Preserving skelton
				outputImages->AddImage(new GrayImage(compositeImage));						// Grayscale skeleton
				outputImages->AddImage(new GrayImage(newVoxelImage));						// Added pixels
				outputImages->AddImage(new GrayImage(pixelClassification));					// Pixel Classification
				outputImages->AddImage(new GrayImage(deletedPixels));						// Deleted Pixels
				outputImages->AddImage(new GrayImage(finalSkeleton));						// Final skeleton after pixel deletion

				delete preservedVolume;
				preservedVolume = topologyVolume;
				delete curveVolume;
				delete pixelClassification;
				delete thresholdedImage;
				delete finalSkeleton;
				delete newVoxelImage;
			}	
			
			outputImages->AddImage(new GrayImage(image));	// Source Image
			compositeImage->Pad(PARTIAL_DERIVATIVE_MASK_SIZE, 0);

			ImageReaderBMP::SaveGrayscaleImage(compositeImage, outputFile);
			sprintf(compositeFile, "%s-comp.bmp", outputFile);
			
			GrayImage * outputImage = outputImages->GetCompositeImage(8);

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


		void GrayImageSkeletonizer::MarkDeletablePixels(GrayImage * deletedPixels, GrayImage * pixelClassification, GrayImage * originalImage) {
			for(int x = 0; x < deletedPixels->GetSizeX(); x++) {
				for(int y = 0; y < deletedPixels->GetSizeY(); y++) {
					if((deletedPixels->GetDataAt(x, y) != PIXEL_BINARY_TRUE) && IsPixelDeletable(x, y, originalImage, deletedPixels, pixelClassification)) {
						deletedPixels->SetDataAt(x, y, PIXEL_BINARY_TRUE);
					}
				}
			}

		}

		GrayImage * GrayImageSkeletonizer::ClassifyImagePixels(GrayImage * sourceImage, GrayImage * maskImage) {
			int sizeX = sourceImage->GetSizeX();
			int sizeY = sourceImage->GetSizeY();
			GrayImage * classifiedImage = new GrayImage(sizeX, sizeY);

			for(int x = 0; x < sizeX; x++)  {
				for(int y = 0; y < sizeY; y++)  {
					if(maskImage->GetDataAt(x, y) != PIXEL_BINARY_FALSE) {
						classifiedImage->SetDataAt(x, y, GetSkeletalPixelClassification(sourceImage, x, y));
					}
				}
			}
			return classifiedImage;
		}

		int GrayImageSkeletonizer::GetN4Count(GrayImage * sourceImage, int xPos, int yPos) {
			int n4Count = 0;
			for(int i = 0; i < 4; i++) {
				if(sourceImage->GetDataAt(xPos + IMAGE_NEIGHBORS_4[i][0], yPos + IMAGE_NEIGHBORS_4[i][1])) {
					n4Count++;
				}
			}
			return n4Count;

		}

		int GrayImageSkeletonizer::GetN8Count(GrayImage * sourceImage, int xPos, int yPos) {
			int n4Count = 0;
			for(int i = 0; i < 8; i++) {
				if(sourceImage->GetDataAt(xPos + IMAGE_NEIGHBORS_8[i][0], yPos + IMAGE_NEIGHBORS_8[i][1])) {
					n4Count++;
				}
			}
			return n4Count;
		}
		bool GrayImageSkeletonizer::IsPixelDeletable( int x, int y, GrayImage * originalImage, GrayImage * deletedPixels, GrayImage * pixelClassification) {
			bool deletable = false;
			switch(pixelClassification->GetDataAt(x, y)) {
				case PIXEL_CLASS_POINT: 
				case PIXEL_CLASS_SURFACE_BODY: 
					//deletable = true;
					//break;
				case PIXEL_CLASS_SURFACE_BORDER:
				case PIXEL_CLASS_CURVE_BODY:
				case PIXEL_CLASS_CURVE_END:
					deletable = FollowsCurveStructureTensor(originalImage, x, y);
					break;
					
			}
			return deletable;
		}

		bool GrayImageSkeletonizer::IsPoint(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN4Count(sourceImage, xPos, yPos) == 0);

		}

		bool GrayImageSkeletonizer::IsSurfaceBody(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN8Count(sourceImage, xPos, yPos) == 8);
		}


		bool GrayImageSkeletonizer::IsSurface(GrayImage * sourceImage, int xPos, int yPos) {
			return (
				((sourceImage->GetDataAt(xPos + 1, yPos) > 0) && (sourceImage->GetDataAt(xPos + 1, yPos - 1) > 0) && (sourceImage->GetDataAt(xPos, yPos-1) > 0)) ||
				((sourceImage->GetDataAt(xPos - 1, yPos) > 0) && (sourceImage->GetDataAt(xPos - 1, yPos - 1) > 0) && (sourceImage->GetDataAt(xPos, yPos-1) > 0)) ||
				((sourceImage->GetDataAt(xPos - 1, yPos) > 0) && (sourceImage->GetDataAt(xPos - 1, yPos + 1) > 0) && (sourceImage->GetDataAt(xPos, yPos+1) > 0)) ||
				((sourceImage->GetDataAt(xPos + 1, yPos) > 0) && (sourceImage->GetDataAt(xPos + 1, yPos + 1) > 0) && (sourceImage->GetDataAt(xPos, yPos+1) > 0))
				);
		}

		bool GrayImageSkeletonizer::IsCurveEnd(GrayImage * sourceImage, int xPos, int yPos) {
			return (GetN4Count(sourceImage, xPos, yPos) == 1);
		}

		bool GrayImageSkeletonizer::FollowsCurveStructureTensor(GrayImage * sourceImage, int xPos, int yPos) {
			double iX = GetPartialDerivativeX(sourceImage, xPos, yPos);
			double iY = GetPartialDerivativeY(sourceImage, xPos, yPos);
			
			EigenVectorsAndValues2D eigenResult;
			eigenResult.structureTensor[0][0] = iX * iX;
			eigenResult.structureTensor[0][1] = iX * iY;
			eigenResult.structureTensor[1][0] = iX * iY;
			eigenResult.structureTensor[1][1] = iY * iY;

			math->EigenAnalysis(eigenResult);

			if(eigenResult.eigenValues[1] > 0.00001) {
				printf("\nstructure tensor\n");
				for(int i = 0; i < 2; i++) {
					for(int j = 0; j < 2; j++) {									
						printf("%f \t", eigenResult.structureTensor[i][j]);
					}
					printf("\n");
				}


				printf("Eigen Vectors:\n");
				for(int i = 0; i < 2; i++) {
					for(int j = 0; j < 2; j++) {									
						printf("%f \t", eigenResult.eigenVectors[i][j]);
					}
					printf("\n");
				}
				printf("Eigen Values: %f, %f\n", eigenResult.eigenValues[0], eigenResult.eigenValues[1]);
				getchar();
			}

			return true;
		}

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
		double GrayImageSkeletonizer::GetPartialDerivativeX(GrayImage * sourceImage, int x, int y) {
			double total = 0;
			double numPoints = PARTIAL_DERIVATIVE_MASK_SIZE * (PARTIAL_DERIVATIVE_MASK_SIZE-1);
			int maskStart = 0 - PARTIAL_DERIVATIVE_MASK_SIZE / 2;
			int maskEnd = maskStart + PARTIAL_DERIVATIVE_MASK_SIZE;
			
			for(int xDiff = maskStart; xDiff < maskEnd; xDiff++) {
				for(int yDiff = maskStart; yDiff <= maskEnd; yDiff++) {
					total += abs(sourceImage->GetDataAt(x+xDiff, y+yDiff) - sourceImage->GetDataAt(x+xDiff+1, y+yDiff));						
				}
			}
			return total / numPoints;
		}

		double GrayImageSkeletonizer::GetPartialDerivativeY(GrayImage * sourceImage, int x, int y) {
			double total = 0;
			double numPoints = PARTIAL_DERIVATIVE_MASK_SIZE * (PARTIAL_DERIVATIVE_MASK_SIZE-1);
			int maskStart = 0 - PARTIAL_DERIVATIVE_MASK_SIZE / 2;
			int maskEnd = maskStart + PARTIAL_DERIVATIVE_MASK_SIZE;
			
			for(int xDiff = maskStart; xDiff <= maskEnd; xDiff++) {
				for(int yDiff = maskStart; yDiff < maskEnd; yDiff++) {
					total += abs(sourceImage->GetDataAt(x+xDiff, y+yDiff) - sourceImage->GetDataAt(x+xDiff, y+yDiff+1));						
				}
			}
			return total / numPoints;
		}


	}
}

#endif