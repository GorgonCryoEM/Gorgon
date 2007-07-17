#ifndef VOLUME_SKELETONIZER_H
#define VOLUME_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "..\SkeletonMaker\PriorityQueue.h"
#include "..\MatlabInterface\MatlabWrapper.h"

using namespace wustl_mm::MatlabInterface;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeSkeletonizer{
		public:
			VolumeSkeletonizer();
			~VolumeSkeletonizer();
			void VolumeSkeletonizer::PerformJuSkeletonization(char * inputFile, char * outputFile);
			Volume * GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			Volume * GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D);
			Volume * GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);

			void PruneCurves(Volume * sourceVolume, int pruneLength);
			void PruneSurfaces(Volume * sourceVolume, int pruneLength);
			double GetPartialDerivativeX(Volume * sourceVolume, int x, int y, int z);
			double GetPartialDerivativeY(Volume * sourceVolume, int x, int y, int z);
			double GetPartialDerivativeZ(Volume * sourceVolume, int x, int y, int z);
		private:
			Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass);
			void AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold);
			void MarkDeletableVoxels(Volume * deletedVol, Volume * currentVolume, Volume * preservedVolume);
			void NormalizeVolume(Volume * sourceVolume);
			void ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue);

			int GetN6Count(Volume * sourceVolume, int x, int y, int z);
			int GetN26Count(Volume * sourceVolume, int x, int y, int z);
			bool IsPoint(Volume * sourceVolume, int x, int y, int z);
			bool IsCurveEnd(Volume * sourceVolume, int x, int y, int z);
			bool IsVolumeBorder(Volume * sourceVolume, int x, int y, int z);
			bool IsVolumeBody(Volume * sourceVolume, int x, int y, int z);

			static const char THINNING_CLASS_SURFACE_PRESERVATION = 3;
			static const char THINNING_CLASS_CURVE_PRESERVATION_2D = 2;
			static const char THINNING_CLASS_CURVE_PRESERVATION = 1;
			static const char THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
		public:
			MatlabWrapper * math;
		};

		// Public Functions
		VolumeSkeletonizer::VolumeSkeletonizer() {
			math = new MatlabWrapper();
		}

		VolumeSkeletonizer::~VolumeSkeletonizer() {
			delete math;
		}

		void VolumeSkeletonizer::PerformJuSkeletonization(char * inputFile, char * outputFile) {

			Volume * imageVol = (MRCReaderPicker::pick(inputFile))->getVolume();
			NormalizeVolume(imageVol);
			imageVol->pad(PARTIAL_DERIVATIVE_MASK_SIZE / 2, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * compositeVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * deletedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		
			char * fileName =  new char[1000];

			for(unsigned char threshold = 255; threshold > 0; threshold--) {		
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
				
				// Making the composite image
				AddIterationToVolume(compositeVol, topologyVol, threshold);

				delete preservedVol;
				preservedVol = topologyVol;

				delete surfaceVol;
				delete curveVol;
				
			}

			compositeVol->pad(-(PARTIAL_DERIVATIVE_MASK_SIZE / 2), 0);
			deletedVol->pad(-(PARTIAL_DERIVATIVE_MASK_SIZE / 2), 0);

			sprintf(fileName, "%s-WithoutRemoving.mrc", outputFile);
			compositeVol->toMRCFile(fileName);

			sprintf(fileName, "%s-deletedVoxels.mrc", outputFile);
			deletedVol->toMRCFile(fileName);

			ApplyMask(compositeVol, deletedVol, VOXEL_BINARY_TRUE, false);
			
			compositeVol->toMRCFile(outputFile);		

			delete [] fileName;
			delete imageVol;
			delete deletedVol;
			delete preservedVol;
			delete compositeVol;		
		}


		Volume * VolumeSkeletonizer::GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold){
			return GetJuThinning(sourceVolume, preserve, threshold, THINNING_CLASS_SURFACE_PRESERVATION);
		}

		Volume * VolumeSkeletonizer::GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D){
			char thinningClass = is3D ? THINNING_CLASS_CURVE_PRESERVATION : THINNING_CLASS_CURVE_PRESERVATION_2D;		
			return GetJuThinning(sourceVolume, preserve, threshold, thinningClass);
		}

		Volume * VolumeSkeletonizer::GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold){
			return GetJuThinning(sourceVolume, preserve, threshold, THINNING_CLASS_TOPOLOGY_PRESERVATION);
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

		void VolumeSkeletonizer::PruneCurves(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeHelix(pruneLength);
		}
		void VolumeSkeletonizer::PruneSurfaces(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeSheet(pruneLength);
		}		
		double VolumeSkeletonizer::GetPartialDerivativeX(Volume * sourceVolume, int x, int y, int z) {
			double total = 0;
			double numPoints = PARTIAL_DERIVATIVE_MASK_SIZE * PARTIAL_DERIVATIVE_MASK_SIZE * (PARTIAL_DERIVATIVE_MASK_SIZE-1);
			int maskStart = 0 - PARTIAL_DERIVATIVE_MASK_SIZE / 2;
			int maskEnd = maskStart + PARTIAL_DERIVATIVE_MASK_SIZE;
			
			for(int xDiff = maskStart; xDiff < maskEnd; xDiff++) {
				for(int yDiff = maskStart; yDiff <= maskEnd; yDiff++) {
					for(int zDiff = maskStart; zDiff <= maskEnd; zDiff++) {
						total += abs(sourceVolume->getDataAt(x+xDiff, y+yDiff, z+zDiff) - sourceVolume->getDataAt(x+xDiff+1, y+yDiff, z+zDiff));						
					}
				}
			}
			return total / numPoints;
		}



		double VolumeSkeletonizer::GetPartialDerivativeY(Volume * sourceVolume, int x, int y, int z) {
			double total = 0;
			double numPoints = PARTIAL_DERIVATIVE_MASK_SIZE * PARTIAL_DERIVATIVE_MASK_SIZE * (PARTIAL_DERIVATIVE_MASK_SIZE-1);
			int maskStart = 0 - PARTIAL_DERIVATIVE_MASK_SIZE / 2;
			int maskEnd = maskStart + PARTIAL_DERIVATIVE_MASK_SIZE;
			
			for(int xDiff = maskStart; xDiff <= maskEnd; xDiff++) {
				for(int yDiff = maskStart; yDiff < maskEnd; yDiff++) {
					for(int zDiff = maskStart; zDiff <= maskEnd; zDiff++) {
						total += abs(sourceVolume->getDataAt(x+xDiff, y+yDiff, z+zDiff) - sourceVolume->getDataAt(x+xDiff, y+yDiff+1, z+zDiff));
					}
				}
			}
			return total / numPoints;
		}



		double VolumeSkeletonizer::GetPartialDerivativeZ(Volume * sourceVolume, int x, int y, int z) {
			double total = 0;
			double numPoints = PARTIAL_DERIVATIVE_MASK_SIZE * PARTIAL_DERIVATIVE_MASK_SIZE * (PARTIAL_DERIVATIVE_MASK_SIZE-1);
			int maskStart = 0 - PARTIAL_DERIVATIVE_MASK_SIZE / 2;
			int maskEnd = maskStart + PARTIAL_DERIVATIVE_MASK_SIZE;
			
			for(int xDiff = maskStart; xDiff <= maskEnd; xDiff++) {
				for(int yDiff = maskStart; yDiff <= maskEnd; yDiff++) {
					for(int zDiff = maskStart; zDiff < maskEnd; zDiff++) {
						total += abs(sourceVolume->getDataAt(x+xDiff, y+yDiff, z+zDiff) - sourceVolume->getDataAt(x+xDiff, y+yDiff, z+zDiff+1));	
					}
				}
			}
			return total / numPoints;
		}



		// Private Functions
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

		void VolumeSkeletonizer::MarkDeletableVoxels(Volume * deletedVolume, Volume * currentVolume, Volume * preservedVolume) {
			Volume * newVoxels = new Volume(currentVolume->getSizeX(), currentVolume->getSizeY(), currentVolume->getSizeZ(), 0, 0, 0, currentVolume);
			newVoxels->subtract(preservedVolume);

			for(int x = 0; x < currentVolume->getSizeX(); x++) {
				for(int y = 0; y < currentVolume->getSizeY(); y++) {
					for(int z = 0; z < currentVolume->getSizeZ(); z++) {
			
						if(newVoxels->getDataAt(x, y, z) > 0) {
							if(IsVolumeBody(currentVolume, x, y, z) || IsVolumeBorder(currentVolume, x, y, z)) {
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

		int VolumeSkeletonizer::GetN6Count(Volume * sourceVolume, int x, int y, int z) {
			int n26Count = 0;
			for(int i = 0; i < 6; i++) {
				if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0)  {
					n26Count++;
				}
			}
			return n26Count;
		}

		int VolumeSkeletonizer::GetN26Count(Volume * sourceVolume, int x, int y, int z) {
			int n26Count = 0;
			for(int i = 0; i < 26; i++) {
				if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_26[i][0], y + VOLUME_NEIGHBORS_26[i][1], z + VOLUME_NEIGHBORS_26[i][2]) > 0)  {
					n26Count++;
				}
			}
			return n26Count;
		}

		bool VolumeSkeletonizer::IsPoint(Volume * sourceVolume, int x, int y, int z) {
			return (GetN6Count(sourceVolume, x, y, z) == 0);
		}

		bool VolumeSkeletonizer::IsCurveEnd(Volume * sourceVolume, int x, int y, int z) {
			return (GetN6Count(sourceVolume, x, y, z) == 1);
		}

		bool VolumeSkeletonizer::IsVolumeBorder(Volume * sourceVolume, int x, int y, int z) {
			bool isBorder = false;
			bool allInCube;
			for(int cube = 0; cube < 8; cube++) {
				allInCube = true;
				for(int p = 0; p < 7; p++) {
					allInCube = allInCube && (sourceVolume->getDataAt(x + VOLUME_NEIGHBOR_CUBES[cube][p][0], y + VOLUME_NEIGHBOR_CUBES[cube][p][1], z + VOLUME_NEIGHBOR_CUBES[cube][p][2]) > 0);
				}
				isBorder = isBorder || allInCube;
			}
			return isBorder && !IsVolumeBody(sourceVolume, x, y, z);
		}
		bool VolumeSkeletonizer::IsVolumeBody(Volume * sourceVolume, int x, int y, int z) {
			return (GetN26Count(sourceVolume, x, y, z) == 26);
		}
	}
}

#endif