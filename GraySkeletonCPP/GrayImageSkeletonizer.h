// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs skeletonization of grayscale images


#ifndef GRAYSKELETONCPP_GRAY_IMAGE_SKELETONIZER_H
#define GRAYSKELETONCPP_GRAY_IMAGE_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "VolumeSkeletonizer.h"
#include "VolumeDeltaAnalyzer.h"
#include "DiscreteMesh.h"
#include <MathTools/MathLib.h>
#include <MathTools/DataStructures.h>
#include <string>

using namespace wustl_mm::MathTools;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class GrayImageSkeletonizer {
		public:
			GrayImageSkeletonizer(int pointRadius, int curveRadius);
			~GrayImageSkeletonizer();
			GrayImage * PerformJuSkeletonization(GrayImage * image, string outputFile, int minGray, int maxGray, int stepSize);
			GrayImage * PerformSkeletonPruning(GrayImage * sourceImage, GrayImage * sourceSkeleton, double pointThreshold, double curveThreshold, int minGray, int maxGray, string outputFile);
			GrayImage * PerformImmersionSkeletonization(GrayImage * image, string outputFile);
			GrayImage * CleanImmersionSkeleton(GrayImage * skeleton, string outputFile);

		private:		

			void GetEigenResult(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution2D & gaussianFilter, int x, int y, int sizeX, int sizeY, int gaussianFilterRadius, bool clear);
			EigenResults3D * GetEigenResults(GrayImage * sourceImage, Vector3DFloat * imageGradient, ProbabilityDistribution2D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			Vector3DFloat * GetSkeletonDirection(GrayImage * skeleton);
			void NormalizeVector2D(Vector3DFloat & vector);
			double GetPixelCost(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, int type);
			GrayImage * GetPixelCosts(GrayImage * skeleton, EigenResults3D * imageEigens, Vector3DFloat * skeletonDirections);
			void WriteEigenResultsToFile(GrayImage * sourceImage, EigenResults3D * eigenResults, string outputFile);
			void WriteDirectionsToFile(GrayImage * sourceImage, Vector3DFloat * skeletonDirections, string outputFile);

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
			unsigned char GetSkeletalPixelClassification(GrayImage * sourceImage, int xPos, int yPos);
			int GetIndex(int x, int y, int xSize, int ySize);
		private:
			VolumeSkeletonizer * volumeSkeletonizer;
			MathLib * math;
			ProbabilityDistribution2D gaussianFilterMaxRadius;
			ProbabilityDistribution2D gaussianFilterOneRadius;
			int pointRadius;
			int curveRadius;
		};	

		GrayImageSkeletonizer::GrayImageSkeletonizer(int pointRadius, int curveRadius) {
			volumeSkeletonizer = new VolumeSkeletonizer(pointRadius, curveRadius, 0, DEFAULT_SKELETON_DIRECTION_RADIUS);
			math = volumeSkeletonizer->math;
			this->pointRadius = pointRadius;
			this->curveRadius = curveRadius;

			gaussianFilterMaxRadius.radius = MAX_GAUSSIAN_FILTER_RADIUS;
			math->GetBinomialDistribution(gaussianFilterMaxRadius);

			gaussianFilterOneRadius.radius = 1;
			math->GetBinomialDistribution(gaussianFilterOneRadius);
		}


		GrayImageSkeletonizer::~GrayImageSkeletonizer() {
			delete volumeSkeletonizer;
		}

		GrayImage * GrayImageSkeletonizer::PerformJuSkeletonization(GrayImage * image, string outputFile, int minGray, int maxGray, int stepSize) {
			image->Pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			GrayImage * compositeImage = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * deletedPixels = new GrayImage(image->GetSizeX(), image->GetSizeY());
			GrayImage * thresholdedImage;
			GrayImage * pixelClassification;
			GrayImage * finalSkeleton;
			GrayImage * newVoxelImage;

			Volume * imageVol = image->ToVolume();
			Volume * preservedVolume = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * curveVolume;
			Volume * topologyVolume;
			Volume * newVoxelVolume;


			GrayImageList * outputImages = new GrayImageList();
			
			for(int threshold = maxGray; threshold >= minGray; threshold-= stepSize) {		
				printf("\t\tTHRESHOLD : %i\n", threshold);
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
			
			image->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			outputImages->AddImage(new GrayImage(image));
			compositeImage->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			
			deletedPixels->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			compositeImage->ApplyMask(deletedPixels, PIXEL_BINARY_TRUE, false);
			//GrayImage * outputImage = outputImages->GetCompositeImage(8);

			if(WRITE_DEBUG_FILES) {
				ImageReaderBMP::SaveGrayscaleImage(compositeImage, outputFile + "-surfacesRemoved.bmp");
				//ImageReaderBMP::SaveGrayscaleImage(outputImage, outputFile + "-progressLog.bmp");
			}

			outputImages->Clear(true);

			delete deletedPixels;
			delete outputImages;
			//delete outputImage;
			delete preservedVolume;
			delete imageVol;

			return compositeImage;
		}


		GrayImage * GrayImageSkeletonizer::PerformSkeletonPruning(GrayImage * sourceImage, GrayImage * sourceSkeleton, double pointThreshold, double curveThreshold, int minGray, int maxGray, string outputFile) {
			printf("Cost Threshold point %f  curve %f\n", pointThreshold, curveThreshold);
			printf("Performing Skeleton Pruning\n");
			sourceImage->Pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			sourceSkeleton->Pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			printf("Getting image Eigens\n");
			Volume * imageVol = sourceImage->ToVolume();
			Vector3DFloat * imageGradient = volumeSkeletonizer->GetVolumeGradient(imageVol);
			EigenResults3D * imageEigens = GetEigenResults(sourceImage, imageGradient, gaussianFilterMaxRadius, MAX_GAUSSIAN_FILTER_RADIUS, false);

			GrayImage * binarySkeleton;

			Volume * binarySkeletonVol;
			Volume * preservedSkeletonVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			VolumeDeltaAnalyzer * analyzer;
			int count, index, curvePointCount;
			double cost, cost2;
			Vector3DInt * pointList;
			SkeletalCurve * curveList;
			Vector3DFloat * skeletonDirections;
			SkeletalCurve curve;

			GrayImageList * images = new GrayImageList();


			for(int grayValue = maxGray; grayValue >= minGray; grayValue--) {
				printf("Threshold :%i\n", grayValue);
				binarySkeleton = new GrayImage(sourceSkeleton);
				binarySkeleton->Threshold(grayValue, false);			
				images->AddImage(new GrayImage(binarySkeleton));
				binarySkeletonVol = binarySkeleton->ToVolume();
				skeletonDirections = GetSkeletonDirection(binarySkeleton);

				analyzer = new VolumeDeltaAnalyzer(preservedSkeletonVol, binarySkeletonVol);
				analyzer->GetNewPoints(count, pointList);

				GrayImage * newPointImage = new GrayImage(binarySkeleton->GetSizeX(), binarySkeleton->GetSizeY()); 
				GrayImage * newCostImage = new GrayImage(binarySkeleton->GetSizeX(), binarySkeleton->GetSizeY()); 
				GrayImage * newCurveCostImage = new GrayImage(binarySkeleton->GetSizeX(), binarySkeleton->GetSizeY()); 
				for(int i = 0; i < count; i++) {
					index = binarySkeleton->GetIndex(pointList[i].values[0], pointList[i].values[1]);				
					cost = GetPixelCost(imageEigens[index], skeletonDirections[index], PIXEL_CLASS_POINT);
					newPointImage->SetDataAt(pointList[i].values[0], pointList[i].values[1], 255);
					newCostImage->SetDataAt(pointList[i].values[0], pointList[i].values[1], (unsigned int)(cost * 255.0));
					if(cost >= pointThreshold) {
						preservedSkeletonVol->setDataAt(pointList[i].values[0], pointList[i].values[1], pointList[i].values[2], 1);
					}
				}
				images->AddImage(newPointImage);

				GrayImage * newCurveImage = new GrayImage(binarySkeleton->GetSizeX(), binarySkeleton->GetSizeY()); 
				analyzer->GetNewCurves(count, curveList);
				int ccc = 0;
				for(int i = 0; i < count; i++) {
					curve = curveList[i];
					cost = 0;
					curvePointCount = 0;
					for(unsigned int j = 0; j < curve.points.size(); j++) {
						ccc++;
						index = binarySkeleton->GetIndex(curve.points[j].values[0], curve.points[j].values[1]);
						newCurveImage->SetDataAt(curve.points[j].values[0], curve.points[j].values[1], 255);
						if(!(preservedSkeletonVol->getDataAt(curve.points[j].values[0], curve.points[j].values[1], curve.points[j].values[2]) > 0)) {
							cost2 = GetPixelCost(imageEigens[index], skeletonDirections[index], PIXEL_CLASS_CURVE_BODY);
							cost += cost2;

							newCostImage->SetDataAt(curve.points[j].values[0], curve.points[j].values[1], (unsigned int)(cost2 * 255.0));

							curvePointCount++;	
						}
					}
					if(curvePointCount != 0) {
						cost = cost / (double)curvePointCount;
					} else {
						cost = 0;
						printf("                 zero cost!!!\n");
					}

					for(unsigned int j = 0; j < curve.points.size(); j++) {
						newCurveCostImage->SetDataAt(curve.points[j].values[0], curve.points[j].values[1], (unsigned int)(cost * 255.0));
					}
					
					if(cost > curveThreshold) {
						for(unsigned int j = 0; j < curve.points.size(); j++) {
							index = binarySkeletonVol->getIndex(curve.points[j].values[0], curve.points[j].values[1], curve.points[j].values[2]);
							preservedSkeletonVol->setDataAt(index, 1);							
						}
					}

				}


				printf("%i curves, %i points\n", count,ccc);

				images->AddImage(newCurveImage);
				images->AddImage(newCostImage);
				images->AddImage(newCurveCostImage);
				images->AddImage(GrayImage::GrayImageVolumeToImage(preservedSkeletonVol));
				delete binarySkeleton;				
				delete binarySkeletonVol;
				delete [] skeletonDirections;
				delete analyzer;
				delete [] pointList;
				delete [] curveList;

				//SIGGRAPH OUTPUTS
				/*GrayImage * presImage = GrayImage::GrayImageVolumeToImage(preservedSkeletonVol);
				presImage->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

				char * fileName = new char[100];
				sprintf(fileName, "-pruned-%f.bmp", (double)grayValue/255.0);
				printf("%s \n", fileName);
				ImageReaderBMP::SaveGrayscaleImage(presImage, outputFile + fileName);
				delete [] fileName;  */

			}			
			
			GrayImage * finalSkeleton = new GrayImage(sourceSkeleton);
			GrayImage * preservedSkeleton = GrayImage::GrayImageVolumeToImage(preservedSkeletonVol);
			finalSkeleton->ApplyMask(preservedSkeleton, 255, true);
			GrayImage * comp = images->GetCompositeImage(6);

			sourceImage->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			sourceSkeleton->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			finalSkeleton->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			preservedSkeleton->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);


			binarySkeleton = new GrayImage(sourceSkeleton);
			binarySkeleton->Threshold(1, false);			
			binarySkeletonVol = binarySkeleton->ToVolume();
			skeletonDirections = GetSkeletonDirection(binarySkeleton);
			GrayImage * costs = GetPixelCosts(sourceSkeleton, imageEigens, skeletonDirections);

			ImageReaderBMP::SaveGrayscaleImage(preservedSkeleton, outputFile + "-binarySkeleton.bmp");

			if(WRITE_DEBUG_FILES) {
				ImageReaderBMP::SaveGrayscaleImage(finalSkeleton, outputFile + "-finalSkeleton.bmp");
				ImageReaderBMP::SaveGrayscaleImage(comp, outputFile + "-debugCOMP.bmp");
				ImageReaderBMP::SaveGrayscaleImage(costs, outputFile + "-finalCosts.bmp");
				WriteEigenResultsToFile(sourceImage, imageEigens, outputFile + "-eigenResults.txt");
				WriteDirectionsToFile(sourceImage, skeletonDirections, outputFile + "-skeletonDirections.txt");
			}
			delete comp;
			images->Clear(true);
			delete images;
			delete preservedSkeleton;
			delete binarySkeleton;
			delete binarySkeletonVol;
			delete [] skeletonDirections;
			delete costs;


			delete imageVol;
			delete [] imageGradient;
			delete [] imageEigens;
			delete preservedSkeletonVol;

			return finalSkeleton;

		}

		GrayImage * GrayImageSkeletonizer::PerformImmersionSkeletonization(GrayImage * image, string outputFile) {
			image->Pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			Volume * imageVol = image->ToVolume();
			Volume * skeletonVol = volumeSkeletonizer->PerformImmersionSkeletonization(imageVol, outputFile);
			GrayImage * skeleton = GrayImage::GrayImageVolumeToImage(skeletonVol);
			GrayImage * cleanedSkel = CleanImmersionSkeleton(skeleton, outputFile);
			image->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			cleanedSkel->Pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			cleanedSkel->Threshold(1, false);
			delete imageVol;
			delete skeletonVol;
			delete skeleton;
			return cleanedSkel;
		}


		GrayImage * GrayImageSkeletonizer::CleanImmersionSkeleton(GrayImage * skeleton, string outputFile) {
			GrayImage * cleanedSkel = new GrayImage(skeleton);
			typedef vector<Vector3DInt> BinType;
			vector<BinType> bins;
			for(int g = 0; g < 256; g++) {
				bins.push_back(BinType());
			}

			for(int x = 0; x < skeleton->GetSizeX(); x++) {
				for(int y = 0; y < skeleton->GetSizeY(); y++) {
					bins[skeleton->GetDataAt(x, y)].push_back(Vector3DInt(x, y, 0));
				}
			}

			GrayImage * temp = new GrayImage(skeleton->GetSizeX(), skeleton->GetSizeY());
			for(int g = 255; g >= 1; g--) {
				//temp = new GrayImage(skeleton->GetSizeX(), skeleton->GetSizeY());
				for(unsigned int i = 0; i < bins[g].size(); i++) {
					temp->SetDataAt(bins[g][i].values[0], bins[g][i].values[1], 1);
				}
				for(unsigned int i = 0; i < bins[g].size(); i++) {
					if(IsSurfaceBody(temp, bins[g][i].values[0], bins[g][i].values[1])) {
						cleanedSkel->SetDataAt(bins[g][i].values[0], bins[g][i].values[1], 0);
						//temp->SetDataAt(bins[g][i].values[0], bins[g][i].values[1], 0);
					}
				}
			}
			delete temp;

			return cleanedSkel;
		}


		
		void GrayImageSkeletonizer::GetEigenResult(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution2D & gaussianFilter, int x, int y, int sizeX, int sizeY, int gaussianFilterRadius, bool clear) {
			if(clear) {
				for(int r = 0; r < 2; r++) {
					returnVal.values[r] = 0;
					for(int c = 0; c < 2; c++) {
						returnVal.vectors[r].values[c] = 0;
					}
				}
			} else {
				EigenVectorsAndValues2D eigenData;
				double probability;
				int index2;

				for(int r = 0; r < 2; r++) {
					for(int c = 0; c < 2; c++) {
						eigenData.structureTensor[r][c] = 0;
					}
				}
			
				for(int xx = -gaussianFilterRadius; xx <= gaussianFilterRadius; xx++) {
					for(int yy = -gaussianFilterRadius; yy <= gaussianFilterRadius; yy++) {
						index2 = (x+xx) * sizeY * GRAYIMAGE_VOLUME_Z_SIZE + (y+yy) * GRAYIMAGE_VOLUME_Z_SIZE + GRAYIMAGE_IN_VOLUME_Z;
						probability = gaussianFilter.values[xx+gaussianFilterRadius][yy+gaussianFilterRadius];
						for(int r = 0; r < 2; r++) {
							for(int c = 0; c < 2; c++) {
								eigenData.structureTensor[r][c] += imageGradient[index2].values[r] * imageGradient[index2].values[c] * probability;
							}
						}
					}
				}
										
				math->EigenAnalysis(eigenData);		
				for(int r = 0; r < 2; r++) {
					returnVal.values[r] = eigenData.eigenValues[r];
					for(int c = 0; c < 2; c++) {
						returnVal.vectors[r].values[c] = eigenData.eigenVectors[r][c];
					}
				}
			}
		}

		EigenResults3D * GrayImageSkeletonizer::GetEigenResults(GrayImage * maskImage, Vector3DFloat * imageGradient, ProbabilityDistribution2D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
			EigenResults3D * resultTable = new EigenResults3D[maskImage->GetSizeX() * maskImage->GetSizeY()];

			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < maskImage->GetSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < maskImage->GetSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					GetEigenResult(resultTable[maskImage->GetIndex(x, y)], imageGradient, gaussianFilter, x, y, 
								   maskImage->GetSizeX(), maskImage->GetSizeY(), gaussianFilterRadius, (useMask && (maskImage->GetDataAt(x, y) == 0))); 
				}
			}
			return resultTable;
		}


		Vector3DFloat * GrayImageSkeletonizer::GetSkeletonDirection(GrayImage * skeleton) {
			Vector3DFloat * directions = new Vector3DFloat[skeleton->GetSizeX() * skeleton->GetSizeY()];
			int index;
			Vector3DFloat v0, v1, v2, currentPos;
			Vector3DFloat * n4 = new Vector3DFloat[4];
			int n4Count;


			for(int x = 1; x < skeleton->GetSizeX()-1; x++) {
				for(int y = 1; y < skeleton->GetSizeY()-1; y++) {
					index = skeleton->GetIndex(x, y);

					directions[index] = Vector3DFloat(0,0,0);
					currentPos = Vector3DFloat(x, y, 0);
			
					if(IsPoint(skeleton, x, y) || (skeleton->GetDataAt(x, y) == 0)) {
						// Set direction to {0,0,0} already done by default.
					} else {
						n4Count = 0;
						for(int i = 0; i < 4; i++) {							
							if(skeleton->GetDataAt(x + IMAGE_NEIGHBORS_4[i][0], y + IMAGE_NEIGHBORS_4[i][1]) > 0) {
								n4[n4Count] = Vector3DFloat(x + IMAGE_NEIGHBORS_4[i][0], y + IMAGE_NEIGHBORS_4[i][1], 0);
								if(n4Count > 0) {
									v1  = n4[n4Count-1] - n4[n4Count];
									directions[index] = directions[index] + v1;
								}
								n4Count++;
							}
						}
						if(n4Count == 1) {
							v1 = Vector3DFloat(x, y, 0);
							v1 = v1 - n4[0];
							directions[index] = directions[index] + v1;
						}
					}
					directions[index] = Vector3DFloat::Normalize(directions[index]);
				}
			}

			delete [] n4;
			return directions;
		}

		void GrayImageSkeletonizer::NormalizeVector2D(Vector3DFloat & vector) {
			double base = sqrt(vector.values[0] * vector.values[0] + vector.values[1] * vector.values[1]);
			if(base == 0) {
				vector.values[0] = 0;
				vector.values[1] = 0;
			} else {
				vector.values[0] = vector.values[0] / base;
				vector.values[1] = vector.values[1] / base;
			}
		}


		double GrayImageSkeletonizer::GetPixelCost(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, int type) {
			double cost = 1;
			if(imageEigen.values[0] != 0) {
				//double dotValue = skeletonDirection.values[0] * imageEigen.vectors[0].values[0] + skeletonDirection.values[1] * imageEigen.vectors[0].values[1];
				double dotValue = skeletonDirection.values[0] * imageEigen.vectors[1].values[0] + skeletonDirection.values[1] * imageEigen.vectors[1].values[1];
				double ratio = imageEigen.values[1] / imageEigen.values[0];
				switch(type) {
					case PIXEL_CLASS_POINT:
						cost = abs(ratio);
						break;
					case PIXEL_CLASS_CURVE_BODY:
					case PIXEL_CLASS_CURVE_END:
						//cost = 1.0 - abs(dotValue) * (1-abs(ratio));
						cost = abs(dotValue) * (1-abs(ratio));
						break;
				}
				if((cost > 1) || (cost < 0)) {
					printf("Cost %f %f %f \n", cost, dotValue, ratio);
				}
			} else {
				cost = 0.0;
				printf("Zero\n");
			}
			return cost;

		}

		GrayImage * GrayImageSkeletonizer::GetPixelCosts(GrayImage * skeleton, EigenResults3D * imageEigens, Vector3DFloat * skeletonDirections) {
			GrayImage * costs = new GrayImage(skeleton->GetSizeX(),skeleton->GetSizeY());			
			int index;
			for(int x = 0; x < skeleton->GetSizeX(); x++) {
				for(int y = 0; y < skeleton->GetSizeY(); y++) {
					index = skeleton->GetIndex(x, y);
					if(skeleton->GetDataAt(x, y) > 0) {
						costs->SetDataAt(x, y, (unsigned char)round(255.0 * GetPixelCost(imageEigens[index], skeletonDirections[index], PIXEL_CLASS_CURVE_BODY)));
					} else {
						costs->SetDataAt(x, y, 0);
					}
				}
			}

			return costs;

		}


		void GrayImageSkeletonizer::WriteEigenResultsToFile(GrayImage * sourceImage, EigenResults3D * eigenResults, string outputFile) {

			int index;
			FILE * outFile = fopen(outputFile.c_str(), "wt");
			fprintf(outFile, "%i\n", sourceImage->GetSizeX());
			fprintf(outFile, "%i\n", sourceImage->GetSizeY());

			for(int x = 0; x < sourceImage->GetSizeX(); x++) {
				for(int y = 0; y < sourceImage->GetSizeY(); y++) {
					index = sourceImage->GetIndex(x, y);
					fprintf(outFile, "%f\n%f\n%f\n%f\n%f\n%f\n", 
						eigenResults[index].values[0], eigenResults[index].values[1],
						eigenResults[index].vectors[0].values[0], eigenResults[index].vectors[0].values[1], 
						eigenResults[index].vectors[1].values[0], eigenResults[index].vectors[1].values[1]);
				}
			}

			//int index;
			//FILE * outFile = fopen(outputFile.c_str(), "wt");
			////fprintf(outFile, "{");
			//for(int y = 0; y < sourceImage->GetSizeY(); y++) {
			//	//fprintf(outFile, "{");
			//	for(int x = 0; x < sourceImage->GetSizeX(); x++) {
			//		index = sourceImage->GetIndex(x, y);

			//		fprintf(outFile, "{{%f, %f}, {{%f, %f}, {%f, %f}}}", eigenResults[index].values[0], eigenResults[index].values[1],
			//			eigenResults[index].vectors[0].values[0], eigenResults[index].vectors[0].values[1], eigenResults[index].vectors[1].values[0], eigenResults[index].vectors[1].values[1]);
			//		if (x < sourceImage->GetSizeX()-1) {
			//			fprintf(outFile, ",\n");
			//		} else {
			//			fprintf(outFile, "\n");
			//		}
			//		
			//	}
			//	fprintf(outFile, "}");
			//	if (y < sourceImage->GetSizeY()-1) {
			//		fprintf(outFile, ",\n");
			//	} else {
			//		fprintf(outFile, "\n");
			//	}
			//}
			//fprintf(outFile, "}");

			fclose(outFile);
			delete outFile;
		}

		void GrayImageSkeletonizer::WriteDirectionsToFile(GrayImage * sourceImage, Vector3DFloat * skeletonDirections, string outputFile) {
			int index;
			FILE * outFile = fopen(outputFile.c_str(), "wt");
			fprintf(outFile, "%i\n", sourceImage->GetSizeX());
			fprintf(outFile, "%i\n", sourceImage->GetSizeY());

			for(int x = 0; x < sourceImage->GetSizeX(); x++) {
				for(int y = 0; y < sourceImage->GetSizeY(); y++) {
					index = sourceImage->GetIndex(x, y);
					fprintf(outFile, "%f\n%f\n", skeletonDirections[index].values[0], skeletonDirections[index].values[1]);
				}
			}

			////fprintf(outFile, "{");
			//for(int y = 0; y < sourceImage->GetSizeY(); y++) {
			//	//fprintf(outFile, "{");
			//	for(int x = 0; x < sourceImage->GetSizeX(); x++) {
			//		index = sourceImage->GetIndex(x, y);

			//		fprintf(outFile, "{%f, %f}", skeletonDirections[index].values[0], skeletonDirections[index].values[1]);
			//		if (x < sourceImage->GetSizeX()-1) {
			//			fprintf(outFile, ",\n");
			//		} else {
			//			fprintf(outFile, "\n");
			//		}
			//		
			//	}
			//	fprintf(outFile, "}");
			//	if (y < sourceImage->GetSizeY()-1) {
			//		fprintf(outFile, ",\n");
			//	} else {
			//		fprintf(outFile, "\n");
			//	}
			//}
			//fprintf(outFile, "}");

			fclose(outFile);
			delete outFile;
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
				if(sourceImage->GetDataAt(xPos + IMAGE_NEIGHBORS_4[i][0], yPos + IMAGE_NEIGHBORS_4[i][1]) > 0) {
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
					deletable = false;
					break;
				case PIXEL_CLASS_SURFACE_BODY: 
					deletable = true;
					break;
				case PIXEL_CLASS_SURFACE_BORDER:
				case PIXEL_CLASS_CURVE_BODY:
				case PIXEL_CLASS_CURVE_END:
					deletable = false;
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
