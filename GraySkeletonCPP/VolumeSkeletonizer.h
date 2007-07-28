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

using namespace wustl_mm::MatlabInterface;
using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeSkeletonizer{
		public:
			VolumeSkeletonizer();
			~VolumeSkeletonizer();
			Volume * VolumeSkeletonizer::PerformJuSkeletonization(string inputFile, string outputFile);
			Volume * GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			Volume * GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D);
			Volume * GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);

			void PruneCurves(Volume * sourceVolume, int pruneLength);
			void PruneSurfaces(Volume * sourceVolume, int pruneLength);
			Vector3D * GetVolumeGradient(Volume * sourceVolume);
			Vector3D * GetSkeletonDirection(Volume * skeleton);
		private:
			
			double GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type);
			void GetEigenResult(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass);
			void AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold);
			void MarkDeletableVoxels(Volume * deletedVol, Volume * currentVolume, Volume * preservedVolume);
			void NormalizeVolume(Volume * sourceVolume);
			void ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue);

			static const char THINNING_CLASS_SURFACE_PRESERVATION = 3;
			static const char THINNING_CLASS_CURVE_PRESERVATION_2D = 2;
			static const char THINNING_CLASS_CURVE_PRESERVATION = 1;
			static const char THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
		public:
			MathLib * math;
			ProbabilityDistribution3D gaussianFilterMaxRadius;
			ProbabilityDistribution3D gaussianFilterOneRadius;
		};

		// Public Functions
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

		Volume * VolumeSkeletonizer::PerformJuSkeletonization(string inputFile, string outputFile) {

			Volume * imageVol = (MRCReaderPicker::pick((char *)inputFile.c_str()))->getVolume();
			NormalizeVolume(imageVol);
			imageVol->pad(GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * compositeVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * deletedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

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

			compositeVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);
			deletedVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);

			string tempFile;

			if(WRITE_DEBUG_FILES) {
				tempFile = outputFile + "-WithoutRemoving.mrc";
				compositeVol->toMRCFile((char *)tempFile.c_str());
				tempFile = outputFile + "-deletedVoxels.mrc";
				deletedVol->toMRCFile((char *)tempFile.c_str());
			}

			ApplyMask(compositeVol, deletedVol, VOXEL_BINARY_TRUE, false);			

			delete imageVol;
			delete deletedVol;
			delete preservedVol;
			return compositeVol;

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
		// Private Functions
		Vector3D * VolumeSkeletonizer::GetSkeletonDirection(Volume * skeleton) {
			Vector3D * directions = new Vector3D[skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ()];
			EigenResults3D eigenDirection;
			int index;
			Vector3D v0, v1, v2, currentPos;
			bool allFound;


			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ()-1; z++) {
						index = skeleton->getIndex(x, y, z);

						VectorLib::Initialize(directions[index], 0,0,0);
						VectorLib::Initialize(currentPos, x, y, z);

						if(DiscreteMesh::IsPoint(skeleton, x, y, z) || (skeleton->getDataAt(x, y, z) <= 0)) {
							// Set direction to {0,0,0} already done by default.
						} else if (DiscreteMesh::IsCurveBody(skeleton, x, y, z) || DiscreteMesh::IsCurveEnd(skeleton, x, y, z)) {
							for(int i = 0; i < 6; i++) {
								if(skeleton->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0) {
									VectorLib::Initialize(v0, x, y, z);
									VectorLib::Initialize(v1, VOLUME_NEIGHBORS_6[i][0], VOLUME_NEIGHBORS_6[i][1], VOLUME_NEIGHBORS_6[i][2]);
									VectorLib::Addition(v1, v1, currentPos);
									DiscreteMesh::FindCurveBase(v0, v1);
									VectorLib::Difference(v1, v1, v0);
									VectorLib::Addition(directions[index], v1);
								}
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
									VectorLib::Difference(v0, faces[1], faces[0]);
									VectorLib::Difference(v1, faces[2], faces[0]);
									VectorLib::CrossProduct(v2, v0, v1);
									VectorLib::Normalize(v2);
									VectorLib::Initialize(v0, 0, 0, 0);
									VectorLib::Addition(v1, v0, v2);
									DiscreteMesh::FindCurveBase(v0, v1);
									VectorLib::Difference(v2, v1, v0);
									VectorLib::Addition(directions[index], v2);									
								}								
							}
						}

						VectorLib::Normalize(directions[index]);
					}
				}
			}


			return directions;
		}


		double VolumeSkeletonizer::GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type) {
			double cost = 1;
			if(imageEigen.values[0] != 0) {
				double dotValue = skeletonDirection.values[0] * imageEigen.vectors[0].values[0] + skeletonDirection.values[1] * imageEigen.vectors[0].values[1];
				//double dotValue = skeletonDirection.values[0] * imageEigen.vectors[1].values[0] + skeletonDirection.values[1] * imageEigen.vectors[1].values[1];
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
						ratio = (imageEigen.values[1] - imageEigen.values[2])/ imageEigen.values[0];
						cost = 1.0 - abs(dotValue) * (1-abs(ratio));
						//cost = abs(dotValue) * (1-abs(ratio));
						break;
					case VOXEL_CLASS_SURFACE_BODY:
					case VOXEL_CLASS_SURFACE_BORDER:
						dotValue = VectorLib::DotProduct(skeletonDirection, imageEigen.vectors[0]);
						ratio = (imageEigen.values[0] - imageEigen.values[1])/ imageEigen.values[0];
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
			}
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



	}
}

#endif