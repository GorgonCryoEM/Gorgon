#ifndef VOLUME_SKELETONIZER_H
#define VOLUME_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "VolumeDeltaAnalyzer.h"
#include "..\SkeletonMaker\PriorityQueue.h"
#include "..\MatlabInterface\MathLib.h"
#include "..\MatlabInterface\DataStructures.h"
#include "..\MatlabInterface\VectorLib.h"
#include <string>
#include <time.h>

using namespace wustl_mm::MatlabInterface;
using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		struct ImmersionBeachElement {
			Vector3DInt p;
			int binIndex;
		};


		class VolumeSkeletonizer{
		public:
			VolumeSkeletonizer();
			~VolumeSkeletonizer();
			Volume * CleanImmersionSkeleton(Volume * skeleton, string outputPath);
			Volume * PerformImmersionSkeletonizationAndPruning(Volume * imageVol, double startGray, double endGray, double stepSize, int minCurveSize, int minSurfaceSize, string outputPath, bool doPruning);
			Volume * PerformSkeletonizationAndPruning(Volume * imageVol, string outputPath);
			Volume * PerformImmersionSkeletonization(Volume * imageVol, string outputPath);
			Volume * PerformJuSkeletonization(Volume * imageVol, string outputPath, int minGray, int maxGray);
			Volume * PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputPath);
			Volume * GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			Volume * GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D);
			Volume * GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			void CleanupVolume(Volume * sourceVolume, double low, double high);
			void NormalizeVolume(Volume * sourceVolume);
			void PerformPureJuSkeletonization(Volume * imageVol, string outputPath, double threshold);
			void PruneCurves(Volume * sourceVolume, int pruneLength);
			void PruneSurfaces(Volume * sourceVolume, int pruneLength);
			void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
			Vector3D * GetVolumeGradient(Volume * sourceVolume);
			Vector3D * GetSkeletonDirection(Volume * skeleton);
		private:
			
			double GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type);
			EigenResults3D * GetEigenResults(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			void AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold);
			void ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue);
			void GetEigenResult(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			void HueR(double value, double &r, double &g, double &b);
			void HueRGB(double value, double &r, double &g, double &b);
			void MarkDeletableVoxels(Volume * deletedVol, Volume * currentVolume, Volume * preservedVolume);
			void WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFileWireframe(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToVRMLFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteSkeletonDirectionToVRMLFile(Volume * skeleton, Vector3D * skeletonDirections, string outputPath);
			Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass);
			Volume * GetImmersionThinning(Volume * sourceVolume, Volume * preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass);

			static const char THINNING_CLASS_SURFACE_PRESERVATION = 4;
			static const char THINNING_CLASS_CURVE_PRESERVATION_2D = 3;
			static const char THINNING_CLASS_CURVE_PRESERVATION = 2;
			static const char THINNING_CLASS_POINT_PRESERVATION = 1;
			static const char THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
		public:
			MathLib * math;
			ProbabilityDistribution3D gaussianFilterMaxRadius;
			ProbabilityDistribution3D gaussianFilterOneRadius;
		};

		VolumeSkeletonizer::VolumeSkeletonizer() {
			math = new MathLib();

			gaussianFilterMaxRadius.radius = GAUSSIAN_FILTER_RADIUS;
			math->GetBinomialDistribution(gaussianFilterMaxRadius);

			gaussianFilterOneRadius.radius = 1;
			math->GetBinomialDistribution(gaussianFilterOneRadius);
		}

		VolumeSkeletonizer::~VolumeSkeletonizer() {
			delete math;
		}


		double VolumeSkeletonizer::GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type) {
			double cost = 1;
			if(imageEigen.values[0] != 0) {
				double dotValue;
				double ratio;
				switch(type) {
					case VOXEL_CLASS_POINT:
						dotValue = 1;
						ratio = imageEigen.values[2] / imageEigen.values[0];
						cost = abs(ratio);
						break;
					case VOXEL_CLASS_CURVE_BODY:
					case VOXEL_CLASS_CURVE_END:
						dotValue = VectorLib::DotProduct(skeletonDirection, imageEigen.vectors[2]);
						ratio = (2.0 * imageEigen.values[2])/ (imageEigen.values[0] + imageEigen.values[1]);
						cost = abs(dotValue) * (1-ratio);
						//cost = abs(dotValue);
						break;
					case VOXEL_CLASS_SURFACE_BODY:
					case VOXEL_CLASS_SURFACE_BORDER:
						dotValue = VectorLib::DotProduct(skeletonDirection, imageEigen.vectors[0]);
						ratio = (imageEigen.values[1] + imageEigen.values[2])/ (2.0*imageEigen.values[0]);
						//ratio = (imageEigen.values[1] - imageEigen.values[2]) / ((imageEigen.values[0] - imageEigen.values[1]) + (imageEigen.values[0] - imageEigen.values[2]));
						cost = abs(dotValue)*(1 - ratio);						
						//cost = abs(dotValue);
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

		EigenResults3D * VolumeSkeletonizer::GetEigenResults(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
			EigenResults3D * resultTable = new EigenResults3D[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];

			for(int x = GAUSSIAN_FILTER_RADIUS; x < maskVol->getSizeX() - GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = GAUSSIAN_FILTER_RADIUS; y < maskVol->getSizeY() - GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = GAUSSIAN_FILTER_RADIUS; z < maskVol->getSizeZ() - GAUSSIAN_FILTER_RADIUS; z++) {
						GetEigenResult(resultTable[maskVol->getIndex(x, y, z)], imageGradient, gaussianFilter, x, y, z,
									   maskVol->getSizeX(), maskVol->getSizeY(), maskVol->getSizeZ(), gaussianFilterRadius, (useMask && (maskVol->getDataAt(x, y, z) == 0))); 
					}
				}
			}
			return resultTable;
		}




		void VolumeSkeletonizer::AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold) {
			for(int x = 0; x < iterationVolume->getSizeX(); x++) {
				for(int y = 0; y < iterationVolume ->getSizeY(); y++) {
					for(int z = 0; z < iterationVolume ->getSizeZ(); z++) {
						if((compositeVolume->getDataAt(x,y,z) == 0) && (iterationVolume->getDataAt(x, y, z) > 0)) {
							compositeVolume->setDataAt(x,y,z,threshold);
						}
					}
				}
			}		
		}

		void VolumeSkeletonizer::ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue) {
			for(int x = 0; x < sourceVolume->getSizeX(); x++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
						if(((maskVolume->getDataAt(x, y, z) == maskValue) && !keepMaskValue) ||
							((maskVolume->getDataAt(x, y, z) != maskValue) && keepMaskValue)) {
							sourceVolume->setDataAt(x, y, z, 0);
						}
					}
				}
			}
		}


		void VolumeSkeletonizer::CleanupVolume(Volume * sourceVolume, double low, double high) {
			for(int x = 0; x < sourceVolume->getSizeX(); x++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
						if(sourceVolume->getDataAt(x, y, z) < low) {
							sourceVolume->setDataAt(x, y, z, low);
						}
						if(sourceVolume->getDataAt(x, y, z) > high) {
							sourceVolume->setDataAt(x, y, z, high);
						}
					}
				}
			}
		}
		void VolumeSkeletonizer::GetEigenResult(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear) {
			if(clear) {
				for(int r = 0; r < 3; r++) {
					returnVal.values[r] = 0;
					for(int c = 0; c < 3; c++) {
						returnVal.vectors[r].values[c] = 0;
					}
				}			
			} else {
				EigenVectorsAndValues3D eigenData;
				double probability;
				int index2;

				for(int r = 0; r < 3; r++) {
					for(int c = 0; c < 3; c++) {
						eigenData.structureTensor[r][c] = 0;
					}
				}
			
				for(int xx = -gaussianFilterRadius; xx <= gaussianFilterRadius; xx++) {
					for(int yy = -gaussianFilterRadius; yy <= gaussianFilterRadius; yy++) {
						for(int zz = -gaussianFilterRadius; zz <= gaussianFilterRadius; zz++) {
							index2 = (x+xx) * sizeY * sizeZ + (y+yy) * sizeZ + z + zz;
							probability = gaussianFilter.values[xx+gaussianFilterRadius][yy+gaussianFilterRadius][zz+gaussianFilterRadius];
							for(int r = 0; r < 3; r++) {
								for(int c = 0; c < 3; c++) {
									eigenData.structureTensor[r][c] += imageGradient[index2].values[r] * imageGradient[index2].values[c] * probability;
								}
							}
						}
					}
				}
						
				math->EigenAnalysis(eigenData);				
				for(int r = 0; r < 3; r++) {
					returnVal.values[r] = eigenData.eigenValues[r];
					for(int c = 0; c < 3; c++) {
						returnVal.vectors[r].values[c] = eigenData.eigenVectors[r][c];
					}
				}

				for(int r = 0; r < 3; r++) {
					//VectorLib::Normalize(returnVal.vectors[r]);
				}
				
				assert((returnVal.values[0] >= returnVal.values[1]) && (returnVal.values[1] >= returnVal.values[2]));
					

			}
		}


		void VolumeSkeletonizer::HueR(double value, double &r, double &g, double &b) {
			r = pow(value, 4.0);
			g = 0;
			b = 0;
		}

		void VolumeSkeletonizer::HueRGB(double value, double &r, double &g, double &b) {
			double v2;
			if(value <= 0.5) {
				v2 = value / 0.5;
				r = 0;
				g = 1 * v2;
				b = 1 * (1 - v2);				
			} else {
				v2 = (value - 0.5)/0.5;
				r = 1 * v2;
				g = 1 * (1- v2);
				b = 0;
			}
		}

		void VolumeSkeletonizer::MarkDeletableVoxels(Volume * deletedVolume, Volume * currentVolume, Volume * preservedVolume) {
			Volume * newVoxels = new Volume(currentVolume->getSizeX(), currentVolume->getSizeY(), currentVolume->getSizeZ(), 0, 0, 0, currentVolume);
			newVoxels->subtract(preservedVolume);

			for(int x = 0; x < currentVolume->getSizeX(); x++) {
				for(int y = 0; y < currentVolume->getSizeY(); y++) {
					for(int z = 0; z < currentVolume->getSizeZ(); z++) {
			
						if(newVoxels->getDataAt(x, y, z) > 0) {
							if(DiscreteMesh::IsVolumeBody(currentVolume, x, y, z) || DiscreteMesh::IsVolumeBorder(currentVolume, x, y, z, false)) {
								deletedVolume->setDataAt(x, y, z, VOXEL_BINARY_TRUE);
							}
						}
					}
				}
			}

			delete newVoxels;

		}

		void VolumeSkeletonizer::NormalizeVolume(Volume * sourceVolume) {
			double minValue = sourceVolume->getDataAt(0,0,0);
			double maxValue = sourceVolume->getDataAt(0,0,0);
			double currValue;
			
			for(int x = 0; x < sourceVolume->getSizeX(); x++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
						currValue = sourceVolume->getDataAt(x, y, z);
						if(currValue < minValue) {
							minValue = currValue;
						}
						if(currValue > maxValue) {
							maxValue = currValue;
						}
					}
				}
			}
			if (minValue == maxValue) {
				for(int x = 0; x < sourceVolume->getSizeX(); x++) {
					for(int y = 0; y < sourceVolume->getSizeY(); y++) {
						for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
							sourceVolume->setDataAt(x, y, z, 0);
						}
					}
				}
			} else {			
				for(int x = 0; x < sourceVolume->getSizeX(); x++) {
					for(int y = 0; y < sourceVolume->getSizeY(); y++) {
						for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
							sourceVolume->setDataAt(x, y, z, (sourceVolume->getDataAt(x, y, z) - minValue) * 255.0 / (maxValue - minValue));
						}
					}
				}
			}
		}

		void VolumeSkeletonizer::PerformPureJuSkeletonization(Volume * imageVol, string outputPath, double threshold) {
			imageVol->pad(GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			printf("\t\t\tUSING THRESHOLD : %i\n", threshold);
			// Skeletonizing while preserving surface features curve features and topology
			surfaceVol = GetJuSurfaceSkeleton(imageVol, preservedVol, threshold);
			PruneSurfaces(surfaceVol, PRUNE_AMOUNT);
			VoxelOr(preservedVol, surfaceVol);

			curveVol = VolumeSkeletonizer::GetJuCurveSkeleton(imageVol, preservedVol, threshold, true);
			VolumeSkeletonizer::PruneCurves(curveVol, PRUNE_AMOUNT);
			VoxelOr(preservedVol, curveVol);

			topologyVol = VolumeSkeletonizer::GetJuTopologySkeleton(imageVol, preservedVol, threshold);


			delete preservedVol;
			delete surfaceVol;
			delete curveVol;

			imageVol->threshold(threshold);
			imageVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);
			imageVol->toMRCFile((char *)(outputPath + "volume.mrc").c_str());
			imageVol->toOFFCells2((char *)(outputPath + "volume.off").c_str());
			
			
			topologyVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);

			topologyVol->toMRCFile((char *)(outputPath+ ".mrc").c_str());
			topologyVol->toOFFCells2((char *)(outputPath + ".off").c_str());
			delete topologyVol;
		}


		void VolumeSkeletonizer::PruneCurves(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeHelix(pruneLength);
		}
		void VolumeSkeletonizer::PruneSurfaces(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeSheet(pruneLength);
		}		
		void VolumeSkeletonizer::VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
			for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
				for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
					for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
						sourceAndDestVolume1->setDataAt(x, y, z, max(sourceAndDestVolume1->getDataAt(x, y, z), sourceVolume2->getDataAt(x, y, z)));
					}
				}
			}
		}

		void VolumeSkeletonizer::WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputPath) {
			int index;
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "%i\n%i\n%i\n", sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						index = sourceVolume->getIndex(x, y, z);
						fprintf(outFile, "{{%f, %f, %f}, {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}}} ", 
							eigenResults[index].values[0], eigenResults[index].values[1], eigenResults[index].values[2],
							eigenResults[index].vectors[0].values[0], eigenResults[index].vectors[0].values[1], eigenResults[index].vectors[0].values[2], 
							eigenResults[index].vectors[1].values[0], eigenResults[index].vectors[1].values[1], eigenResults[index].vectors[1].values[2], 
							eigenResults[index].vectors[2].values[0], eigenResults[index].vectors[2].values[1], eigenResults[index].vectors[2].values[2]);
						fprintf(outFile, "\n");
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}

		void VolumeSkeletonizer::WriteEigenResultsToOFFFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath) {
			int index;
			int pointCount = 0;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							pointCount++;
						}
					}
				}
			}


			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "OFF\n%i %i %i\n", pointCount * 8, pointCount * 6, 0);
			double ratio[3], xd, yd, zd;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							for(int v = 0; v < 3; v++) {							
								ratio[v] = 0.7 * eigenResults[index].values[2]/eigenResults[index].values[v];
							}

							xd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[0] + ratio[1] * eigenResults[index].vectors[1].values[0] + ratio[2] * eigenResults[index].vectors[2].values[0]) + GAUSSIAN_FILTER_RADIUS; 
							yd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[1] + ratio[1] * eigenResults[index].vectors[1].values[1] + ratio[2] * eigenResults[index].vectors[2].values[1]) + GAUSSIAN_FILTER_RADIUS;
							zd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[2] + ratio[1] * eigenResults[index].vectors[1].values[2] + ratio[2] * eigenResults[index].vectors[2].values[2]) + GAUSSIAN_FILTER_RADIUS;

							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd, 
								y - yd, 
								z - zd);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[0] * eigenResults[index].vectors[0].values[0], 
								y - yd + ratio[0] * eigenResults[index].vectors[0].values[1], 
								z - zd + ratio[0] * eigenResults[index].vectors[0].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[0] * eigenResults[index].vectors[0].values[0] + ratio[1] * eigenResults[index].vectors[1].values[0], 
								y - yd + ratio[0] * eigenResults[index].vectors[0].values[1] + ratio[1] * eigenResults[index].vectors[1].values[1], 
								z - zd + ratio[0] * eigenResults[index].vectors[0].values[2] + ratio[1] * eigenResults[index].vectors[1].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[1] * eigenResults[index].vectors[1].values[0], 
								y - yd + ratio[1] * eigenResults[index].vectors[1].values[1], 
								z - zd + ratio[1] * eigenResults[index].vectors[1].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[2] * eigenResults[index].vectors[2].values[0], 
								y - yd + ratio[2] * eigenResults[index].vectors[2].values[1], 
								z - zd + ratio[2] * eigenResults[index].vectors[2].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[0] * eigenResults[index].vectors[0].values[0] + ratio[2] * eigenResults[index].vectors[2].values[0], 
								y - yd + ratio[0] * eigenResults[index].vectors[0].values[1] + ratio[2] * eigenResults[index].vectors[2].values[1], 
								z - zd + ratio[0] * eigenResults[index].vectors[0].values[2] + ratio[2] * eigenResults[index].vectors[2].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[0] * eigenResults[index].vectors[0].values[0] + ratio[1] * eigenResults[index].vectors[1].values[0] + ratio[2] * eigenResults[index].vectors[2].values[0], 
								y - yd + ratio[0] * eigenResults[index].vectors[0].values[1] + ratio[1] * eigenResults[index].vectors[1].values[1] + ratio[2] * eigenResults[index].vectors[2].values[1], 
								z - zd + ratio[0] * eigenResults[index].vectors[0].values[2] + ratio[1] * eigenResults[index].vectors[1].values[2] + ratio[2] * eigenResults[index].vectors[2].values[2]);
							fprintf(outFile, "%lf %lf %lf \n", 
								x - xd + ratio[1] * eigenResults[index].vectors[1].values[0] + ratio[2] * eigenResults[index].vectors[2].values[0], 
								y - yd + ratio[1] * eigenResults[index].vectors[1].values[1] + ratio[2] * eigenResults[index].vectors[2].values[1], 
								z - zd + ratio[1] * eigenResults[index].vectors[1].values[2] + ratio[2] * eigenResults[index].vectors[2].values[2]);
						}
					}
				}
			}

			int cnt = 0;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							fprintf(outFile, "4 %i %i %i %i\n", cnt, cnt+3, cnt+2, cnt+1);
							fprintf(outFile, "4 %i %i %i %i\n", cnt+1, cnt+2, cnt+6, cnt+5);
							fprintf(outFile, "4 %i %i %i %i\n", cnt+3, cnt+7, cnt+6, cnt+2);
							fprintf(outFile, "4 %i %i %i %i\n", cnt, cnt+4, cnt+7, cnt+3);
							fprintf(outFile, "4 %i %i %i %i\n", cnt, cnt+1, cnt+5, cnt+4);
							fprintf(outFile, "4 %i %i %i %i\n", cnt+4, cnt+5, cnt+6, cnt+7);

							cnt+=8;
						}
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}


		void VolumeSkeletonizer::WriteEigenResultsToOFFFileWireframe(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath) {
			int index;
			int pointCount = 0;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							pointCount++;
						}
					}
				}
			}


			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "OFF\n%i %i %i\n", pointCount * 4, pointCount * 3, 0);
			double ratio;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);

							fprintf(outFile, "%i %i %i \n", x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS);
							for(int v = 0; v < 3; v++) {							
								ratio = 0.7 * eigenResults[index].values[2]/eigenResults[index].values[v];
								fprintf(outFile, "%f %f %f \n", 
									x - GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[0], 
									y - GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[1], 
									z - GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[2]);
							}
						}
					}
				}
			}

			int cnt = 0;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							fprintf(outFile, "4 %i %i %i %i 0.0 0.0 1.0 0.75\n", cnt*4, cnt*4, cnt*4 +1, cnt*4+1);
							fprintf(outFile, "4 %i %i %i %i 0.0 1.0 0.0 0.75\n", cnt*4, cnt*4, cnt*4 +2, cnt*4+2);
							fprintf(outFile, "4 %i %i %i %i 1.0 0.0 0.0 0.75\n", cnt*4, cnt*4, cnt*4 +3, cnt*4+3);						
							cnt++;
						}
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}


		void VolumeSkeletonizer::WriteEigenResultsToVRMLFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath) {
			int index;
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");
			srand( (unsigned)time( NULL ) );
			double r, g, b, colorCost;
			double theta, theta1, theta2, theta3, s;
			Vector3D axis, axis1, axis2, axis3;
			Vector3D newY, newX;
			Vector3D xAxis = VectorLib::Initialize(1.0, 0.0, 0.0);
			Vector3D yAxis = VectorLib::Initialize(0.0, 1.0, 0.0);

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							colorCost = cost->getDataAt(x, y, z);
							HueR(colorCost, r, g, b);

							axis1 = VectorLib::CrossProduct(eigenResults[index].vectors[0], xAxis);
							VectorLib::Normalize(axis1);
							theta1 = acos(VectorLib::DotProduct(eigenResults[index].vectors[0], xAxis));

							newX = VectorLib::Rotate(xAxis, axis1, theta1);
							newY = VectorLib::Rotate(yAxis, axis1, theta1);
							axis2 = VectorLib::CrossProduct(eigenResults[index].vectors[1], newY);
							VectorLib::Normalize(axis2);
							theta2 = acos(VectorLib::DotProduct(eigenResults[index].vectors[1], newY));


							fprintf(outFile, "Group{\n children [\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [Transform{\n rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}]}\n",
								x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
								axis2.values[0], axis2.values[1], axis2.values[2], -theta2,
								axis1.values[0], axis1.values[1], axis1.values[2], -theta1,
								r, g, b,									
								0.7 * eigenResults[index].values[2]/eigenResults[index].values[0], 0.7 * eigenResults[index].values[2]/eigenResults[index].values[1], 0.7 * eigenResults[index].values[2]/eigenResults[index].values[2]);


							//axis1 = VectorLib::CrossProduct(eigenResults[index].vectors[0], VectorLib::Initialize(1.0, 0.0, 0.0));
							//VectorLib::Normalize(axis1);
							//theta1 = -acos(VectorLib::DotProduct(eigenResults[index].vectors[0], VectorLib::Initialize(1.0, 0.0, 0.0)));

							//axis2 = VectorLib::CrossProduct(eigenResults[index].vectors[1], VectorLib::Initialize(0.0, 1.0, 0.0));
							//VectorLib::Normalize(axis2);
							//theta2 = -acos(VectorLib::DotProduct(eigenResults[index].vectors[1], VectorLib::Initialize(0.0, 1.0, 0.0)));

							//axis3 = VectorLib::CrossProduct(eigenResults[index].vectors[2], VectorLib::Initialize(0.0, 0.0, 1.0));
							//VectorLib::Normalize(axis3);
							//theta3 = -acos(VectorLib::DotProduct(eigenResults[index].vectors[2], VectorLib::Initialize(0.0, 0.0, 1.0)));


							//fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}\n",
							//	x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
							//	axis1.values[0], axis1.values[1], axis1.values[2], theta1,
							//	0.0, 0.5, 0.0,									
							//	0.7 * eigenResults[index].values[2]/eigenResults[index].values[0], 0.01, 0.01);

							//fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}\n",
							//	x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
							//	axis2.values[0], axis2.values[1], axis2.values[2], theta2,
							//	0.0, 0.0, 0.5,									
							//	0.01, 0.7 * eigenResults[index].values[2]/eigenResults[index].values[1], 0.01);

							//fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}\n",
							//	x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
							//	axis3.values[0], axis3.values[1], axis3.values[2], theta3,
							//	0.5, 0.0, 0.0,									
							//	0.01, 0.01, 0.7 * eigenResults[index].values[2]/eigenResults[index].values[2]);
						}
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}

		void VolumeSkeletonizer::WriteSkeletonDirectionToVRMLFile(Volume * skeleton, Vector3D * skeletonDirections, string outputPath) {
			int index;
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");
			srand( (unsigned)time( NULL ) );
			double r = 0;// ((double)rand() / (RAND_MAX + 1));
			double g = 0;//((double)rand() / (RAND_MAX + 1));
			double b = 0.5;//((double)rand() / (RAND_MAX + 1));

			for(int z = 0; z < skeleton->getSizeZ(); z++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int x = 0; x < skeleton->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = skeleton->getIndex(x, y, z);
							Vector3D axis = VectorLib::CrossProduct(skeletonDirections[index], VectorLib::Initialize(1.0, 0.0, 0.0));
							VectorLib::Normalize(axis);
							double angle = -VectorLib::DotProduct(skeletonDirections[index], VectorLib::Initialize(1.0, 0.0, 0.0));
							angle = acos(angle);

							if((axis.values[0] == 0) && (axis.values[1] == 0) && (axis.values[2] == 0)) {
								fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f }} \n geometry Box {size 0.9 0.1 0.1}}]}]}\n",
									x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
									r, g, b);
							} else {
								fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %f %f %f %f \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f }} \n geometry Box {size 0.9 0.1 0.1}}]}]}\n",
									x - GAUSSIAN_FILTER_RADIUS, y - GAUSSIAN_FILTER_RADIUS, z - GAUSSIAN_FILTER_RADIUS,
									axis.values[0], axis.values[1], axis.values[2], angle,
									r, g, b);
							}
						
						}
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}
		Volume * VolumeSkeletonizer::CleanImmersionSkeleton(Volume * skeleton, string outputPath) {
			Volume * cleanedSkel = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), 0, 0, 0, skeleton);
			typedef vector<Vector3DInt> BinType;
			vector<BinType> bins;
			for(int g = 0; g < 256; g++) {
				bins.push_back(BinType());
			}

			for(int x = 0; x < skeleton->getSizeX(); x++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int z = 0; z < skeleton->getSizeZ(); z++) {				
						bins[(int)round(skeleton->getDataAt(x, y, z))].push_back(VectorLib::Initialize(x, y, z));
					}
				}
			}

			Volume * temp;
			for(int g = 255; g >= 1; g--) {
				temp = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ());
				for(int i = 0; i < bins[g].size(); i++) {
					temp->setDataAt(bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2], 1);
				}
				for(int i = 0; i < bins[g].size(); i++) {
					if(DiscreteMesh::IsVolumeBody(temp, bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2])) {
						cleanedSkel->setDataAt(bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2], 0);
					}
				}
				delete temp;
			}

			return cleanedSkel;
		}

		Volume * VolumeSkeletonizer::GetImmersionThinning(Volume * sourceVolume, Volume * preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass) {
			Volume * thinnedVolume = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ(), 0, 0, 0, sourceVolume);
			thinnedVolume->threshold2(lowGrayscale, 0, 1) ;
			double t;
			switch(thinningClass) {
				case THINNING_CLASS_SURFACE_PRESERVATION :
					for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
						thinnedVolume->surfaceSkeleton(sourceVolume, t, t + stepSize) ;
					}
					break;
				case THINNING_CLASS_CURVE_PRESERVATION :
					for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
						thinnedVolume->curveSkeleton(sourceVolume, t, t + stepSize, preserve);
					}
					break;
				case THINNING_CLASS_CURVE_PRESERVATION_2D :
					for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
						thinnedVolume->curveSkeleton(sourceVolume, t, t + stepSize, preserve);
					}
					break;
				case THINNING_CLASS_POINT_PRESERVATION :
					for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
						thinnedVolume->pointSkeleton(sourceVolume, t, t + stepSize, preserve, preserve);
					}
			}
			return thinnedVolume;
		}





		Volume * VolumeSkeletonizer::GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D){
			char thinningClass = is3D ? THINNING_CLASS_CURVE_PRESERVATION : THINNING_CLASS_CURVE_PRESERVATION_2D;		
			return GetJuThinning(sourceVolume, preserve, threshold, thinningClass);
		}

		Volume * VolumeSkeletonizer::GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold){
			return GetJuThinning(sourceVolume, preserve, threshold, THINNING_CLASS_SURFACE_PRESERVATION);
		}

		Volume * VolumeSkeletonizer::GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold){
			return GetJuThinning(sourceVolume, preserve, threshold, THINNING_CLASS_TOPOLOGY_PRESERVATION);
		}


		Volume * VolumeSkeletonizer::GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass) {
			Volume * thinnedVolume = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ(), 0, 0, 0, sourceVolume);
			switch(thinningClass) {
				case THINNING_CLASS_SURFACE_PRESERVATION :
					thinnedVolume->surfaceSkeletonPres(threshold, preserve);
					break;
				case THINNING_CLASS_CURVE_PRESERVATION :
					thinnedVolume->curveSkeleton(threshold, preserve);
					break;
				case THINNING_CLASS_CURVE_PRESERVATION_2D :
					thinnedVolume->curveSkeleton2D(threshold, preserve);
					break;
				case THINNING_CLASS_TOPOLOGY_PRESERVATION :
					thinnedVolume->skeleton(threshold, preserve, preserve);
			}

			return thinnedVolume;
		}



		Volume * VolumeSkeletonizer::PerformImmersionSkeletonizationAndPruning(Volume * imageVol, double startGray, double endGray, double stepSize, int minCurveSize, int minSurfaceSize, string outputPath, bool doPruning) {
			Volume * sourceVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
			NormalizeVolume(sourceVol);
			Volume * surfaceVol = GetImmersionThinning(sourceVol, NULL, startGray, endGray, stepSize, THINNING_CLASS_SURFACE_PRESERVATION);
			PruneSurfaces(surfaceVol, minSurfaceSize);
			if(doPruning) {
			}
			Volume * curveVol = GetImmersionThinning(sourceVol, surfaceVol, startGray, endGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			PruneCurves(curveVol, minCurveSize);
			if(doPruning) {
			}
			Volume * pointVol = GetImmersionThinning(sourceVol, curveVol, startGray, endGray, stepSize, THINNING_CLASS_POINT_PRESERVATION);
			if(doPruning) {
			}

			delete sourceVol;
			delete surfaceVol;
			delete curveVol;

			return pointVol;			
		}

		Volume * VolumeSkeletonizer::PerformImmersionSkeletonization(Volume * imageVol, string outputPath) {
			Volume * skeleton = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
			typedef vector<Vector3DInt> BinType;
			vector<BinType> bins;
			for(int g = 0; g < 256; g++) {
				bins.push_back(BinType());
			}


			for(int x = 0; x < skeleton->getSizeX(); x++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int z = 0; z < skeleton->getSizeZ(); z++) {				
						bins[(int)round(skeleton->getDataAt(x, y, z))].push_back(VectorLib::Initialize(x, y, z));
					}
				}
			}

			int n6Count;
			int key;
			bool modified;
			double value;
			list<int> cleanupIndices;
			PriorityQueue<ImmersionBeachElement, int> beach(MAX_QUEUELEN);
			ImmersionBeachElement * element;
			int index;

			GrayImageList imageList;
			Volume * thresholdedImage = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
			for(int g = 1; g < 256; g++) {
				thresholdedImage = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ(), 0, 0, 0, imageVol);
				thresholdedImage->threshold(g);

				imageList.AddImage(GrayImage::GrayImageVolumeToImage(thresholdedImage));
				imageList.AddImage(GrayImage::GrayImageVolumeToImage(skeleton));
				delete thresholdedImage;
				printf("Threshold = %i - %i points\n", g, bins[g].size());
				int iteration = 1;
				do { 
					printf("\tIteration : %i\n", iteration++);

					for(int i = 0; i < bins[g].size(); i++) {
						n6Count = DiscreteMesh::GetImmersionN6Count(skeleton, bins[g][i]);
						if(n6Count < 6) {
							element = new ImmersionBeachElement();
							element->p = bins[g][i];
							element->binIndex = i;
							beach.add(element, n6Count);
						}
					}

					modified = false;
					cleanupIndices.clear();
					while(!beach.isEmpty()) {
						beach.remove(element, key);
						value = DiscreteMesh::GetImmersionSkeletalValue(skeleton, element->p);
						skeleton->setDataAt(element->p.values[0], element->p.values[1], element->p.values[2], value);
						if(value != g) {
							cleanupIndices.push_back(element->binIndex);
							modified = true;
						}
						delete element;
					}

					cleanupIndices.sort(greater<int>());
					for(int i = 0; i < cleanupIndices.size(); i++) {
						index = cleanupIndices.front();
						cleanupIndices.pop_front();
						bins[g].erase(bins[g].begin() + index);
					}

				} while (modified);
				imageList.AddImage(GrayImage::GrayImageVolumeToImage(skeleton));
			}

			GrayImage * progressImage = imageList.GetCompositeImage(3);
			ImageReaderBMP::SaveGrayscaleImage(progressImage, outputPath + "-progress.bmp");
			delete progressImage;

			return skeleton;
		}

		Volume * VolumeSkeletonizer::PerformJuSkeletonization(Volume * imageVol, string outputPath, int minGray, int maxGray) {
			imageVol->pad(GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * compositeVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * deletedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			for(int threshold = maxGray; threshold >= minGray; threshold-= 1) {		
				printf("\t\t\tUSING THRESHOLD : %i\n", threshold);
				// Skeletonizing while preserving surface features curve features and topology
				surfaceVol = GetJuSurfaceSkeleton(imageVol, preservedVol, threshold);
				PruneSurfaces(surfaceVol, PRUNE_AMOUNT);
				VoxelOr(preservedVol, surfaceVol);

				curveVol = VolumeSkeletonizer::GetJuCurveSkeleton(imageVol, preservedVol, threshold, true);
				VolumeSkeletonizer::PruneCurves(curveVol, PRUNE_AMOUNT);
				VoxelOr(preservedVol, curveVol);
			
				topologyVol = VolumeSkeletonizer::GetJuTopologySkeleton(imageVol, preservedVol, threshold);
			
				MarkDeletableVoxels(deletedVol, topologyVol, preservedVol);

				//// SIGGRAPH
				//Volume * tempVolume = new Volume(topologyVol->getSizeX(), topologyVol->getSizeY(), topologyVol->getSizeZ(), 0, 0, 0, topologyVol);
				//ApplyMask(tempVolume, deletedVol, VOXEL_BINARY_TRUE, false);
				//tempVolume->pad(-GAUSSIAN_FILTER_RADIUS, 0);

				//char * fileName = new char[100];
				//sprintf(fileName, "%s-shapePreserved-%f.off", outputPath.c_str(), threshold/255.0);
				//tempVolume->toOFFCells2(fileName);				
				//delete [] fileName; 


				
				// Making the composite image
				AddIterationToVolume(compositeVol, topologyVol, threshold);

				delete preservedVol;
				preservedVol = topologyVol;

				delete surfaceVol;
				delete curveVol;
				
			}

			compositeVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);
			deletedVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);

			if(WRITE_DEBUG_FILES) {
				string tempFile;
				tempFile = outputPath + "-WithoutRemoving.mrc";
				compositeVol->toMRCFile((char *)tempFile.c_str());

				tempFile = outputPath + "-deletedVoxels.mrc";
				deletedVol->toMRCFile((char *)tempFile.c_str());
			}

			ApplyMask(compositeVol, deletedVol, VOXEL_BINARY_TRUE, false);			

			delete deletedVol;
			delete preservedVol;

			if(WRITE_DEBUG_FILES) {
				// This method seems to have trouble with the vessel2 dataset.
				// compositeVol->toOFFCells2((char *)(outputPath + ".off").c_str());
			}

			return compositeVol;

		}



		Volume * VolumeSkeletonizer::PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputPath) {
			double pointThreshold = curveThreshold;
			printf("Performing Skeleton Pruning\n");
			sourceVolume->pad(GAUSSIAN_FILTER_RADIUS, 0);
			sourceSkeleton->pad(GAUSSIAN_FILTER_RADIUS, 0);

			printf("Getting volume Eigens\n");
			Vector3D * volumeGradient = GetVolumeGradient(sourceVolume);
			EigenResults3D * volumeEigens = GetEigenResults(sourceSkeleton, volumeGradient, gaussianFilterMaxRadius, GAUSSIAN_FILTER_RADIUS, true);

			WriteEigenResultsToOFFFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.off");
			WriteEigenResultsToVRMLFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.wrl");

			Volume * curveScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			Volume * pointScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			Volume * surfaceScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			

			Volume * binarySkeleton;

			Volume * preservedSkeleton = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			VolumeDeltaAnalyzer * analyzer;
			int pointCount, curveCount, surfaceCount, index, featurePointCount;
			double cost, cost2;
			Vector3DInt * pointList;
			SkeletalCurve * featureList;
			Vector3D * skeletonDirections;
			SkeletalCurve feature;

			{ //Remove after debugging.. not needed at all only for visualization
				binarySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
				binarySkeleton->threshold(139);			
				skeletonDirections = GetSkeletonDirection(binarySkeleton);
				WriteSkeletonDirectionToVRMLFile(binarySkeleton, skeletonDirections, outputPath + "-G-SkelDirections.wrl");

				delete binarySkeleton;
				delete [] skeletonDirections;
			}


			for(int grayValue = maxGray; grayValue >= minGray; grayValue-=5) {
				printf("Threshold :%i\n", grayValue);
				binarySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
				binarySkeleton->threshold(grayValue);			
				skeletonDirections = GetSkeletonDirection(binarySkeleton);

				analyzer = new VolumeDeltaAnalyzer(preservedSkeleton, binarySkeleton);
				analyzer->GetNewPoints(pointCount, pointList);

				for(int i = 0; i < pointCount; i++) {
					index = binarySkeleton->getIndex(pointList[i].values[0], pointList[i].values[1], pointList[i].values[2]);				
					cost = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_POINT);
					pointScore->setDataAt(index, max(cost, pointScore->getDataAt(index)));
					if(cost >= pointThreshold) {
						preservedSkeleton->setDataAt(pointList[i].values[0], pointList[i].values[1], pointList[i].values[2], 1);
					}
				}

				analyzer->GetNewCurves(curveCount, featureList);
				for(int i = 0; i < curveCount; i++) {
					feature = featureList[i];
					cost = 0;
					featurePointCount = 0;
					for(int j = 0; j < feature.points.size(); j++) {
						index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
						if(!(preservedSkeleton->getDataAt(index) > 0)) {
							cost2 = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_CURVE_BODY);
							curveScore->setDataAt(index, max(cost2, curveScore->getDataAt(index)));
							cost += cost2;
							featurePointCount++;	
						}
					}
					if(featurePointCount != 0) {
						cost = cost / (double)featurePointCount;
					} else {
						cost = 0;
						printf("                 zero cost!!!\n");
					}
				
					if(cost > curveThreshold) {
						for(int j = 0; j < feature.points.size(); j++) {
							index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
							preservedSkeleton->setDataAt(index, 1);							
						}
					}
				}
				delete [] featureList;


				analyzer->GetNewSurfaces(surfaceCount, featureList);
				for(int i = 0; i < surfaceCount; i++) {
					feature = featureList[i];
					cost = 0;
					featurePointCount = 0;
					for(int j = 0; j < feature.points.size(); j++) {
						index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
						if(!(preservedSkeleton->getDataAt(index) > 0)) {
							cost2 = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_SURFACE_BODY);
							surfaceScore->setDataAt(index, max(cost2, surfaceScore->getDataAt(index)));
							cost += cost2;
							featurePointCount++;	
						}
					}
					if(featurePointCount != 0) {
						cost = cost / (double)featurePointCount;
					} else {
						cost = 0;
						//printf("                 zero cost!!!\n");
					}
				
					if(cost > surfaceThreshold) {
						for(int j = 0; j < feature.points.size(); j++) {
							index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
							preservedSkeleton->setDataAt(index, 1);							
						}
					}
				}
				delete [] featureList;


				printf("%i surfaces, %i curves, %i points\n", surfaceCount, curveCount, pointCount);

				delete binarySkeleton;				
				delete [] skeletonDirections;
				delete analyzer;
				delete [] pointList;
			}
			
			Volume * finalSkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			finalSkeleton->applyMask(preservedSkeleton, 1.0, true);
						
			WriteEigenResultsToVRMLFile(sourceVolume, curveScore, sourceSkeleton, volumeEigens, outputPath + "-C-Eigens.wrl");
			WriteEigenResultsToVRMLFile(sourceVolume, surfaceScore, sourceSkeleton, volumeEigens, outputPath + "-S-Eigens.wrl");

			//pointScore->toMRCFile((char *)(outputPath + "-P-Score.mrc").c_str());
			curveScore->toMRCFile((char *)(outputPath + "-C-Score.mrc").c_str());
			surfaceScore->toMRCFile((char *)(outputPath + "-S-Score.mrc").c_str());
			//preservedSkeleton->toMRCFile((char *)(outputPath + "-G-BinarySkeleton.mrc").c_str());
			//preservedSkeleton->toOFFCells2((char *)(outputPath + "-G-BinarySkeleton.off").c_str());
			//finalSkeleton->toOFFCells2((char *)(outputPath + "-G-GraySkeleton.off").c_str());

			delete pointScore;
			delete curveScore;
			delete surfaceScore;
			delete preservedSkeleton;
			delete [] volumeEigens;
			delete [] volumeGradient;

			return finalSkeleton;

		}



		Vector3D * VolumeSkeletonizer::GetVolumeGradient(Volume * sourceVolume) {
			Vector3D * gradient = new Vector3D[sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ()];
			int index;

			for(int x = 0; x < sourceVolume->getSizeX(); x = x + sourceVolume->getSizeX()-1) {
				for(int y = 0; y < sourceVolume->getSizeY(); y = y + sourceVolume->getSizeY()-1) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z = z + sourceVolume->getSizeZ()-1) {
						index = sourceVolume->getIndex(x, y, z);
						VectorLib::Initialize(gradient[index], 0, 0, 0);
					}
				}
			}

			for(int x = 1; x < sourceVolume->getSizeX()-1; x++) {
				for(int y = 1; y < sourceVolume->getSizeY()-1; y++) {
					for(int z = 1; z < sourceVolume->getSizeZ()-1; z++) {
						index = sourceVolume->getIndex(x, y, z);
						gradient[index].values[0] = sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x-1, y, z);
						gradient[index].values[1] = sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y-1, z);
						gradient[index].values[2] = sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z-1);
					}
				}
			}
			return gradient;
		}
		Vector3D * VolumeSkeletonizer::GetSkeletonDirection(Volume * skeleton) {
			Vector3D * directions = new Vector3D[skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ()];
			int index;
			Vector3D v0, v1, v2, currentPos;
			bool allFound;
			Vector3D * n6 = new Vector3D[6];
			int n6Count;


			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ()-1; z++) {
						index = skeleton->getIndex(x, y, z);

						VectorLib::Initialize(directions[index], 0,0,0);
						VectorLib::Initialize(currentPos, x, y, z);

						if(DiscreteMesh::IsPoint(skeleton, x, y, z) || (skeleton->getDataAt(x, y, z) <= 0)) {
							// Set direction to {0,0,0} already done by default.
						} else if (DiscreteMesh::IsCurveBody(skeleton, x, y, z) || DiscreteMesh::IsCurveEnd(skeleton, x, y, z)) {
							n6Count = 0;
							for(int i = 0; i < 6; i++) {							
								if(skeleton->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0) {
									VectorLib::Initialize(n6[n6Count], x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]);
									if(n6Count > 0) {
										VectorLib::Difference(v1, n6[n6Count-1], n6[n6Count]);
										VectorLib::Addition(directions[index], directions[index], v1);
									}
									n6Count++;
								}
							}
							if(n6Count == 1) {
								VectorLib::Initialize(v1, x, y, z);
								VectorLib::Difference(v1, v1, n6[0]);
								VectorLib::Addition(directions[index], directions[index], v1);
							}

						} else if (DiscreteMesh::IsSurfaceBody(skeleton, x, y, z, true) || DiscreteMesh::IsSurfaceBorder(skeleton, x, y, z)) {
							Vector3D faces[4];
							for(int i = 0; i < 12; i++) {
								VectorLib::Initialize(faces[0], x, y, z);
								allFound = true;
								for(int j = 0; j < 3; j++) {
									if(skeleton->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][j][0], y + VOLUME_NEIGHBOR_FACES[i][j][1], z + VOLUME_NEIGHBOR_FACES[i][j][2]) > 0) {
										VectorLib::Initialize(faces[j+1], VOLUME_NEIGHBOR_FACES[i][j][0], VOLUME_NEIGHBOR_FACES[i][j][1], VOLUME_NEIGHBOR_FACES[i][j][2]);
										VectorLib::Addition(faces[j+1], faces[j+1], currentPos);
									} else {
										allFound = false;
									}
								}
								if(allFound) {
									DiscreteMesh::FindSurfaceBase(faces[0], faces[1], faces[2], faces[3]);
									//if(DiscreteMesh::IsValidSurface(skeleton, faces[0], faces[1], faces[2], faces[3])) {
										VectorLib::Difference(v0, faces[1], faces[0]);
										VectorLib::Difference(v1, faces[3], faces[0]);
										VectorLib::CrossProduct(v2, v0, v1);
										VectorLib::Normalize(v2);
										VectorLib::Initialize(v0, 0, 0, 0);
										VectorLib::Addition(v1, v0, v2);
										DiscreteMesh::FindCurveBase(v0, v1);
										VectorLib::Difference(v2, v1, v0);
										VectorLib::Addition(directions[index], directions[index], v2);		
									//}
								}								
							}
						}

						VectorLib::Normalize(directions[index]);
					}
				}
			}

			delete [] n6;


			return directions;
		}


	}
}

#endif