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
			void PerformPureJuSkeletonization(Volume * imageVol, string outputFile, double threshold);
			Volume * VolumeSkeletonizer::PerformJuSkeletonization(Volume * imageVol, string outputFile);
			Volume * PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputFile);
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
			EigenResults3D * GetEigenResults(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			void WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputFile);
			void WriteEigenResultsToVRMLFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputFile);
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


		void VolumeSkeletonizer::PerformPureJuSkeletonization(Volume * imageVol, string outputFile, double threshold) {
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
			imageVol->toMRCFile((char *)(outputFile + "volume.mrc").c_str());
			imageVol->toOFFCells2((char *)(outputFile + "volume.off").c_str());
			
			
			topologyVol->pad(-GAUSSIAN_FILTER_RADIUS, 0);

			topologyVol->toMRCFile((char *)(outputFile+ ".mrc").c_str());
			topologyVol->toOFFCells2((char *)(outputFile + ".off").c_str());
			delete topologyVol;
		}


		Volume * VolumeSkeletonizer::PerformJuSkeletonization(Volume * imageVol, string outputFile) {

			NormalizeVolume(imageVol);

			imageVol->pad(GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * compositeVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * deletedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			for(int threshold = 255; threshold > 0; threshold-= 50) {		
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
				//sprintf(fileName, "%s-shapePreserved-%f.off", outputFile.c_str(), threshold/255.0);
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

			string tempFile;
			tempFile = outputFile + "-WithoutRemoving.mrc";
			compositeVol->toMRCFile((char *)tempFile.c_str());


			if(WRITE_DEBUG_FILES) {
				tempFile = outputFile + "-deletedVoxels.mrc";
				deletedVol->toMRCFile((char *)tempFile.c_str());
			}

			ApplyMask(compositeVol, deletedVol, VOXEL_BINARY_TRUE, false);			

			delete deletedVol;
			delete preservedVol;

			compositeVol->toOFFCells2((char *)(outputFile + ".off").c_str());

			return compositeVol;

		}



		Volume * VolumeSkeletonizer::PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputFile) {
			double pointThreshold = curveThreshold;
			printf("Performing Skeleton Pruning\n");
			sourceVolume->pad(GAUSSIAN_FILTER_RADIUS, 0);
			sourceSkeleton->pad(GAUSSIAN_FILTER_RADIUS, 0);

			printf("Getting volume Eigens\n");
			Vector3D * volumeGradient = GetVolumeGradient(sourceVolume);
			EigenResults3D * volumeEigens = GetEigenResults(sourceVolume, volumeGradient, gaussianFilterMaxRadius, GAUSSIAN_FILTER_RADIUS, false);
			//WriteEigenResultsToFile(sourceVolume, volumeEigens, outputFile + "Eigens.dat");
			WriteEigenResultsToVRMLFile(sourceVolume, volumeEigens, outputFile + "Eigens.wrl");

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
			Vector3D * skeletonGradient;
			Vector3D * skeletonDirections;
			SkeletalCurve feature;			


			for(int grayValue = maxGray; grayValue >= minGray; grayValue-=5) {
				printf("Threshold :%i\n", grayValue);
				binarySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
				binarySkeleton->threshold(grayValue);			
				skeletonGradient = GetVolumeGradient(binarySkeleton);
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

				//// SIGGRAPH
				//Volume * tempVolume = new Volume(preservedSkeleton->getSizeX(), preservedSkeleton->getSizeY(), preservedSkeleton->getSizeZ(), 0, 0, 0, preservedSkeleton);
				//tempVolume->pad(-GAUSSIAN_FILTER_RADIUS, 0);

				//char * fileName = new char[100];
				//sprintf(fileName, "%s-pruned-%f.off", outputFile.c_str(), grayValue/255.0);
				//tempVolume->toOFFCells2(fileName);				
				//delete [] fileName; 
				//delete tempVolume;

				delete binarySkeleton;				
				delete [] skeletonGradient;
				delete [] skeletonDirections;
				delete analyzer;
				delete [] pointList;
			}
			
			Volume * finalSkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			finalSkeleton->applyMask(preservedSkeleton, 1.0, true);
			
			//pointScore->toMRCFile((char *)(outputFile + "-pointScore.mrc").c_str());
			curveScore->toMRCFile((char *)(outputFile + "-curveScore.mrc").c_str());
			//surfaceScore->toMRCFile((char *)(outputFile + "-surfaceScore.mrc").c_str());
			preservedSkeleton->toMRCFile((char *)(outputFile + "-binarySkeleton.mrc").c_str());
			finalSkeleton->toMRCFile((char *)(outputFile + "-graySkeleton.mrc").c_str());
			preservedSkeleton->toOFFCells2((char *)(outputFile + "-binarySkeleton.off").c_str());
			//finalSkeleton->toOFFCells2((char *)(outputFile + "-graySkeleton.off").c_str());

			delete pointScore;
			delete curveScore;
			delete surfaceScore;
			delete preservedSkeleton;
			delete binarySkeleton;
			delete [] skeletonGradient;
			delete [] skeletonDirections;
			delete [] volumeGradient;
			delete [] volumeEigens;


			return finalSkeleton;

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
									VectorLib::Difference(v0, faces[1], faces[0]);
									VectorLib::Difference(v1, faces[3], faces[0]);
									VectorLib::CrossProduct(v2, v0, v1);
									VectorLib::Normalize(v2);
									VectorLib::Initialize(v0, 0, 0, 0);
									VectorLib::Addition(v1, v0, v2);
									DiscreteMesh::FindCurveBase(v0, v1);
									VectorLib::Difference(v2, v1, v0);
									VectorLib::Addition(directions[index], directions[index], v2);									
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
						break;
					case VOXEL_CLASS_SURFACE_BODY:
					case VOXEL_CLASS_SURFACE_BORDER:
						dotValue = VectorLib::DotProduct(skeletonDirection, imageEigen.vectors[0]);
						ratio = (imageEigen.values[1] + imageEigen.values[2])/ (2.0*imageEigen.values[0]);
						cost = abs(dotValue) * (1 - ratio);
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
				
				if((returnVal.values[0] < returnVal.values[1]) || (returnVal.values[1] < returnVal.values[2])) {
					printf("EIGEN VALUES OUT OF ORDER!!!\n");
				}


			}
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




		void VolumeSkeletonizer::WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputFile) {
			int index;
			FILE * outFile = fopen(outputFile.c_str(), "wt");
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

		void VolumeSkeletonizer::WriteEigenResultsToVRMLFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputFile) {
			int index;
			FILE * outFile = fopen(outputFile.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						index = sourceVolume->getIndex(x, y, z);
						if(eigenResults[index].values[0] != 0) {
						fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation 0 0 0 0 \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor 0 0.25 0 }} \n geometry Box {size %f %f %f}}]}]}\n",
								x, y, z,
								eigenResults[index].values[0]/eigenResults[index].values[0], eigenResults[index].values[1]/eigenResults[index].values[0], eigenResults[index].values[2]/eigenResults[index].values[0]);
						}
							 
							//eigenResults[index].values[0], eigenResults[index].values[1], eigenResults[index].values[2],
							//eigenResults[index].vectors[0].values[0], eigenResults[index].vectors[0].values[1], eigenResults[index].vectors[0].values[2], 
							//eigenResults[index].vectors[1].values[0], eigenResults[index].vectors[1].values[1], eigenResults[index].vectors[1].values[2], 
							//eigenResults[index].vectors[2].values[0], eigenResults[index].vectors[2].values[1], eigenResults[index].vectors[2].values[2]);
					}
				}
			}

			fclose(outFile);
			delete outFile;
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