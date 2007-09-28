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
#include "..\MatlabInterface\Vector3D.h"
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
			VolumeSkeletonizer(int pointRadius, int curveRadius, int surfaceRadius);
			~VolumeSkeletonizer();
			Volume * CleanImmersionSkeleton(Volume * skeleton, string outputPath);
			Volume * PerformImmersionSkeletonizationAndPruning(Volume * sourceVol, double startGray, double endGray, double stepSize, int minCurveSize, int minSurfaceSize, string outputPath, bool doPruning, double pointThreshold, double curveThreshold, double surfaceThreshold);
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
			void PruneUsingStructureTensor(Volume * skeleton, Volume * sourceVolume, EigenResults3D * volumeEigens, double threshold, char pruningClass, string outputPath);
			void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
			Vector3D * GetVolumeGradient(Volume * sourceVolume);
			Vector3D * GetVolumeGradient2(Volume * sourceVolume);
			Vector3D * GetSkeletonDirection(Volume * skeleton);
		private:
			
			double GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type);
			EigenResults3D * GetEigenResults(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			EigenResults3D * GetEigenResults2(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			void AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold);
			void ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue);
			void FindOrthogonalAxes(Vector3D axis, Vector3D & res1, Vector3D & res2);
			void GetEigenResult(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			void GetEigenResult2(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			void HueR(double value, double &r, double &g, double &b);
			void HueRGB(double value, double &r, double &g, double &b);
			void MarkDeletableVoxels(Volume * deletedVol, Volume * currentVolume, Volume * preservedVolume);
			void WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFileWireframe(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToVRMLFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath, bool doInverse);
			void WriteSkeletonDirectionToVRMLFile(Volume * skeleton, Vector3D * skeletonDirections, string outputPath);
			Vector3D XYZtoUVW(Vector3D vec, Vector3D u, Vector3D v, Vector3D w);
			Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass);
			Volume * GetImmersionThinning(Volume * sourceVolume, Volume * preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass);

			static const char THINNING_CLASS_SURFACE_PRESERVATION = 4;
			static const char THINNING_CLASS_CURVE_PRESERVATION_2D = 3;
			static const char THINNING_CLASS_CURVE_PRESERVATION = 2;
			static const char THINNING_CLASS_POINT_PRESERVATION = 1;
			static const char THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
			static const char PRUNING_CLASS_PRUNE_SURFACES = 5;
			static const char PRUNING_CLASS_PRUNE_CURVES = 6;
			static const char PRUNING_CLASS_PRUNE_POINTS = 7;
		public:
			MathLib * math;
			ProbabilityDistribution3D gaussianFilterPointRadius;
			ProbabilityDistribution3D gaussianFilterCurveRadius;
			ProbabilityDistribution3D gaussianFilterSurfaceRadius;
			ProbabilityDistribution3D gaussianFilterMaxRadius;
			int pointRadius;
			int curveRadius;
			int surfaceRadius;
		};

		VolumeSkeletonizer::VolumeSkeletonizer(int pointRadius, int curveRadius, int surfaceRadius) {
			math = new MathLib();
			this->pointRadius = pointRadius;
			this->curveRadius = curveRadius;
			this->surfaceRadius = surfaceRadius;

			gaussianFilterPointRadius.radius = pointRadius;
			math->GetBinomialDistribution(gaussianFilterPointRadius);

			gaussianFilterCurveRadius.radius = curveRadius;
			math->GetBinomialDistribution(gaussianFilterCurveRadius);

			gaussianFilterSurfaceRadius.radius = surfaceRadius;
			math->GetBinomialDistribution(gaussianFilterSurfaceRadius);

			gaussianFilterMaxRadius.radius = MAX_GAUSSIAN_FILTER_RADIUS;
			math->GetBinomialDistribution(gaussianFilterMaxRadius);
		}

		VolumeSkeletonizer::~VolumeSkeletonizer() {
			delete math;
		}


		double VolumeSkeletonizer::GetVoxelCost(EigenResults3D imageEigen, Vector3D skeletonDirection, int type) {
			double cost = 1;

			if(!isZero(imageEigen.values[0])) {
				double theta, a, b;
				Vector3D temp, skelDirectionST, n;
				double u1 = 1.0;
				double u2 = imageEigen.values[1]/imageEigen.values[0];
				double u3 = imageEigen.values[2]/imageEigen.values[0];
				Vector3D v1 = imageEigen.vectors[0];
				Vector3D v2 = imageEigen.vectors[1];
				Vector3D v3 = imageEigen.vectors[2];				
				switch(type) {
					case PRUNING_CLASS_PRUNE_POINTS:
						//dotValue = 1;
						//ratio = imageEigen.values[2] / imageEigen.values[0];
						//cost = abs(ratio);
						cost = imageEigen.values[2] * imageEigen.values[2] / (imageEigen.values[0] * imageEigen.values[1]);
						break;
					case PRUNING_CLASS_PRUNE_CURVES:
						{
							/*			Vector3D n1 = skeletonDirection
							Vector3D m1 = Vector3D();
							Vector3D m2 = Vector3D();*/
							/*dotValue = skeletonDirection * imageEigen.vectors[2];
							ratio = (2.0 * imageEigen.values[2])/ (imageEigen.values[0] + imageEigen.values[1]);
							cost = abs(dotValue) * (1-ratio);*/

							Vector3D n1, n2, m1, m2;
							skelDirectionST = XYZtoUVW(skeletonDirection, imageEigen.vectors[0],imageEigen.vectors[1], imageEigen.vectors[2]);
							FindOrthogonalAxes(skelDirectionST, n1, n2);

							m1 = Vector3D(n1.values[0]/imageEigen.values[0], n1.values[1]/imageEigen.values[1], n1.values[2]/imageEigen.values[2]);
							m2 = Vector3D(n2.values[0]/imageEigen.values[0], n2.values[1]/imageEigen.values[1], n2.values[2]/imageEigen.values[2]);
							theta = atan((2.0 * (m1 * m2)) / ((m1 * m1) - (m2 * m2))) / 2.0;							
							a = 1.0 / ((m1 * cos(theta)) + (m2 * sin(theta))).Length();
							b = 1.0 / ((m1 * sin(theta)) + (m2 * cos(theta))).Length();

							cost = sqrt(a*b / (imageEigen.values[0] * imageEigen.values[1]));							
						}
						break;
					case PRUNING_CLASS_PRUNE_SURFACES:
						/*dotValue = skeletonDirection * imageEigen.vectors[0];
						ratio = (imageEigen.values[1] + imageEigen.values[2])/ (2.0*imageEigen.values[0]);
						cost = abs(dotValue)*(1 - ratio);	*/

						//// Line inside ellipsoid vs. max line length
						//n = XYZtoUVW(skeletonDirection, v1, v2, v3);
						////temp = Vector3D(imageEigen.values[0] * n.values[0], imageEigen.values[1] * n.values[1], imageEigen.values[2] * n.values[2]);
						//a = u1 * u2 * u3;
						//b = sqrt(u2*u2*u3*u3*n.X()*n.X() + u1*u1*u3*u3*n.Y()*n.Y() + u1*u1*u2*u2*n.Z()*n.Z());
						//if(isZero(b)) {
						//	cost = 1.0;
						//} else {
						//	temp = Vector3D(a*n.X()/b, a*n.Y()/b, a*n.Z()/b);
						//	cost = temp.Length() / u1;
						//}

						//Projection onto v1 vs. max line length						
						n = XYZtoUVW(skeletonDirection, v1, v2, v3);						

						cost = abs(n.X())/(u1+u2+u3);			//(n.X/u1)  * (u1 / (u1+u2+u3)) ; simplified
						//cost = abs(n.X());					//(n.X/u1)						; but u1 = 1
						

						////Linear Interpolation					
						//{
						//	double t12, t13, t123, n12, n13;
						//	n = XYZtoUVW(skeletonDirection, v1, v2, v3);

						//	t12 = 2*acos(Vector3D[n.X(), n.Y(), 0)
						//	

						//
						//	cost = abs(n.X())/(u1+u2+u3);			//(n.X/u1)  * (u1 / (u1+u2+u3)) ; simplified
						//}
						////cost = abs(n.X());					//(n.X/u1)						; but u1 = 1
	



						
						break;
				}
				
			} else {
				cost = 0.0;
				printf("Zero\n");
			}

			if((cost > 1.0)  || (cost < 0)){
				printf("Cost : %f\n", cost);
			}
			return cost;

		}

		EigenResults3D * VolumeSkeletonizer::GetEigenResults(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
			EigenResults3D * resultTable = new EigenResults3D[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];

			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < maskVol->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < maskVol->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < maskVol->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
						GetEigenResult(resultTable[maskVol->getIndex(x, y, z)], imageGradient, gaussianFilter, x, y, z,
									   maskVol->getSizeX(), maskVol->getSizeY(), maskVol->getSizeZ(), gaussianFilterRadius, (useMask && (maskVol->getDataAt(x, y, z) == 0))); 
					}
				}
			}
			return resultTable;
		}

		// Use 8 local gradients instead of 1 gradient
		EigenResults3D * VolumeSkeletonizer::GetEigenResults2(Volume * maskVol, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
			EigenResults3D * resultTable = new EigenResults3D[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];

			for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < maskVol->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
				for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < maskVol->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
					for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < maskVol->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
						GetEigenResult2(resultTable[maskVol->getIndex(x, y, z)], imageGradient, gaussianFilter, x, y, z,
									   maskVol->getSizeX(), maskVol->getSizeY(), maskVol->getSizeZ(), gaussianFilterRadius, (useMask && (maskVol->getDataAt(x, y, z) == 0))); 
					}
				}
			}
			return resultTable;
		}

		Vector3D VolumeSkeletonizer::XYZtoUVW(Vector3D vec, Vector3D u, Vector3D v, Vector3D w) {
			double uContri = vec * u; 
			double vContri = vec * v;
			double wContri = vec * w;
			Vector3D inUVW = Vector3D(uContri, vContri, wContri);
			inUVW.Normalize();
			return inUVW;
		}

		// Gradient = (x+1,y,z) - (x-1,y,z) ....
		Vector3D * VolumeSkeletonizer::GetVolumeGradient(Volume * sourceVolume) {
			Vector3D * gradient = new Vector3D[sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ()];
			int index;

			for(int x = 0; x < sourceVolume->getSizeX(); x = x + sourceVolume->getSizeX()-1) {
				for(int y = 0; y < sourceVolume->getSizeY(); y = y + sourceVolume->getSizeY()-1) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z = z + sourceVolume->getSizeZ()-1) {
						index = sourceVolume->getIndex(x, y, z);
						gradient[index] = Vector3D(0, 0, 0);
					}
				}
			}

			for(int x = 1; x < sourceVolume->getSizeX()-1; x++) {
				for(int y = 1; y < sourceVolume->getSizeY()-1; y++) {
					for(int z = 1; z < sourceVolume->getSizeZ()-1; z++) {
						index = sourceVolume->getIndex(x, y, z);
						gradient[index] = Vector3D(sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x-1, y, z),
												   sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y-1, z),
												   sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z-1));
					}
				}
			}
			return gradient;
		}
		// Gradient = (x,y,z) - (x-1,y,z) ....
		Vector3D * VolumeSkeletonizer::GetVolumeGradient2(Volume * sourceVolume) {
			int size = sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ();
			Vector3D * gradient = new Vector3D[sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ() * 8];
			int index, index2;

			for(int x = 0; x < sourceVolume->getSizeX(); x = x + sourceVolume->getSizeX()-1) {
				for(int y = 0; y < sourceVolume->getSizeY(); y = y + sourceVolume->getSizeY()-1) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z = z + sourceVolume->getSizeZ()-1) {
						index = sourceVolume->getIndex(x, y, z);
						for(int a = 0; a < 8; a++) {
							index2 = a * size + index;
							gradient[index2] = Vector3D(0, 0, 0);
						}
					}
				}
			}

			for(int x = 1; x < sourceVolume->getSizeX()-1; x++) {
				for(int y = 1; y < sourceVolume->getSizeY()-1; y++) {
					for(int z = 1; z < sourceVolume->getSizeZ()-1; z++) {
						index = sourceVolume->getIndex(x, y, z);
						index2 = 0 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z);

						index2 = 1 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x-1, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z);

						index2 = 2 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y-1, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z);

						index2 = 3 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y, z-1);

						index2 = 4 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x-1, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y-1, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z);

						index2 = 5 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x-1, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y, z-1);

						index2 = 6 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y-1, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y, z-1);

						index2 = 7 * size + index;
						gradient[index2].values[0] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x-1, y, z);
						gradient[index2].values[1] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y-1, z);
						gradient[index2].values[2] = sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y, z-1);
					}
				}
			}
			return gradient;
		}
		Vector3D * VolumeSkeletonizer::GetSkeletonDirection(Volume * skeleton) {
			Vector3D * directions = new Vector3D[skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ()];
			int index;
			Vector3D v0, v1, v2, currentPos;
			Vector3D * n6 = new Vector3D[6];
			int n6Count;
			bool facesFound[12];

			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ()-1; z++) {
						
						index = skeleton->getIndex(x, y, z);

						directions[index] = Vector3D(0,0,0);
						currentPos = Vector3D(x, y, z);

						if(DiscreteMesh::IsPoint(skeleton, x, y, z) || (skeleton->getDataAt(x, y, z) <= 0)) {
							// Set direction to {0,0,0} already done by default.
						} else if (DiscreteMesh::IsCurveBody(skeleton, x, y, z) || DiscreteMesh::IsCurveEnd(skeleton, x, y, z)) {
							n6Count = 0;
							for(int i = 0; i < 6; i++) {							
								if(skeleton->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0) {
									n6[n6Count] = Vector3D(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]);
									if(n6Count > 0) {
										v1 = n6[n6Count-1] - n6[n6Count];
										directions[index] = directions[index] + v1;
									}
									n6Count++;
								}
							}
							if(n6Count == 1) {
								v1 = Vector3D(x, y, z);
								v1 = v1 - n6[0];
								directions[index] = directions[index] + v1;
							}

						} else if (DiscreteMesh::IsSurfaceBody(skeleton, x, y, z, true) || DiscreteMesh::IsSurfaceBorder(skeleton, x, y, z)) {
							for(int i = 0; i < 12; i++) {
								facesFound[i] = true;
								for(int j = 0; j < 3; j++) {
									if(skeleton->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][j][0], y + VOLUME_NEIGHBOR_FACES[i][j][1], z + VOLUME_NEIGHBOR_FACES[i][j][2]) <= 0) {
										facesFound[i] = false;
									}
								}
							}
							Vector3D faces[4];
							faces[0] = Vector3D(0, 0, 0);
							for(int i = 0; i < 12; i++) {
								if(facesFound[i]) {
									for(int j = 0; j < 3; j++) {									
										faces[j+1] = Vector3D(VOLUME_NEIGHBOR_FACES[i][j][0], VOLUME_NEIGHBOR_FACES[i][j][1], VOLUME_NEIGHBOR_FACES[i][j][2]);
									}

									if (facesFound[VOLUME_NEIGHBOR_NUMBERS[i][0]] || facesFound[VOLUME_NEIGHBOR_NUMBERS[i][1]]) {										
										v0 = faces[3] - faces[0];
										v1 = faces[1] - faces[0];
									} else {
										v0 = faces[1] - faces[0];
										v1 = faces[3] - faces[0];
									}
									v2 = v0 ^ v1;
									v2.Normalize();
									directions[index] = directions[index] + v2;		
								}						
							}

						}

						directions[index].Normalize();
					}
				}
			}

			delete [] n6;


			return directions;
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
		void VolumeSkeletonizer::FindOrthogonalAxes(Vector3D axis, Vector3D & res1, Vector3D & res2) {
			res1 = Vector3D(1.0, 0.0, 0.0);
			if(abs(axis * res1) > 0.95) {
				res1 = Vector3D(0.0, 1.0, 0.0);
			}
			res1 = axis ^ res1;
			res2 = axis ^ res1;
			res1.Normalize();
			res2.Normalize();
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
				
				assert((returnVal.values[0] >= returnVal.values[1]) && (returnVal.values[1] >= returnVal.values[2]));
					

			}
		}


		// Works on 8 local gradients instead of 1
		void VolumeSkeletonizer::GetEigenResult2(EigenResults3D & returnVal, Vector3D * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear) {
			int size = sizeX*sizeY*sizeZ;
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
				int index2, index3;

				for(int r = 0; r < 3; r++) {
					for(int c = 0; c < 3; c++) {
						eigenData.structureTensor[r][c] = 0;
					}
				}
			
				for(int xx = -gaussianFilterRadius; xx <= gaussianFilterRadius; xx++) {
					for(int yy = -gaussianFilterRadius; yy <= gaussianFilterRadius; yy++) {
						for(int zz = -gaussianFilterRadius; zz <= gaussianFilterRadius; zz++) {
							index2 = (x+xx) * sizeY * sizeZ + (y+yy) * sizeZ + z + zz;
							probability = gaussianFilter.values[xx+gaussianFilterRadius][yy+gaussianFilterRadius][zz+gaussianFilterRadius] / 8;
							for (int a = 0; a < 8; a++) {
								index3 = a*size + index2;
								for(int r = 0; r < 3; r++) {
									for(int c = 0; c < 3; c++) {
										eigenData.structureTensor[r][c] += imageGradient[index3].values[r] * imageGradient[index3].values[c] * probability;
									}
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
					//returnVal.vectors[r].Normalize();
				}
				
				assert((returnVal.values[0] >= returnVal.values[1]) && (returnVal.values[1] >= returnVal.values[2]));
					

			}
		}


		void VolumeSkeletonizer::HueR(double value, double &r, double &g, double &b) {
			r = value*value*value*value;
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
			imageVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			printf("\t\t\tUSING THRESHOLD : %f\n", threshold);
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
			imageVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			imageVol->toMRCFile((char *)(outputPath + "volume.mrc").c_str());
			imageVol->toOFFCells2((char *)(outputPath + "volume.off").c_str());
			
			
			topologyVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

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
		void VolumeSkeletonizer::PruneUsingStructureTensor(Volume * skeleton, Volume * sourceVolume, EigenResults3D * volumeEigens, double threshold, char pruningClass, string outputPath) {									
			Volume * tempSkel = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), 0, 0, 0, skeleton);
			Volume * costVol = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ()); 			
			Vector3D * skeletonDirections = GetSkeletonDirection(skeleton);
			int index;
			double cost;

			for(int x = 0; x < skeleton->getSizeX(); x++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int z = 0; z < skeleton->getSizeZ(); z++) {						
						index = skeleton->getIndex(x, y, z);						
						if((tempSkel->getDataAt(index) > 0) && (
								((pruningClass == PRUNING_CLASS_PRUNE_SURFACES) && (DiscreteMesh::IsSurfaceBorder(tempSkel, x, y, z) || DiscreteMesh::IsSurfaceBody(tempSkel, x, y, z, true))) ||
								((pruningClass == PRUNING_CLASS_PRUNE_CURVES) && (DiscreteMesh::IsCurveEnd(tempSkel, x, y, z) || DiscreteMesh::IsCurveBody(tempSkel, x, y, z))) || 
								((pruningClass == PRUNING_CLASS_PRUNE_POINTS) && DiscreteMesh::IsPoint(tempSkel, x, y, z)))) {
							cost = GetVoxelCost(volumeEigens[index], skeletonDirections[index], pruningClass);
							if(cost < threshold) {
								skeleton->setDataAt(index, 0.0);
							}
							costVol->setDataAt(index, cost);
						}
					}
				}
			}

			costVol->toMRCFile((char *)((outputPath + "-Score.mrc").c_str()));
			//WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", (pruningClass != PRUNING_CLASS_PRUNE_SURFACES));
			WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens-inverted.wrl", true);
			WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", false);
			WriteSkeletonDirectionToVRMLFile(tempSkel, skeletonDirections, outputPath + "-SkeletonDirections.wrl");
			delete costVol;
			delete tempSkel;
			delete [] skeletonDirections;
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

							xd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[0] + ratio[1] * eigenResults[index].vectors[1].values[0] + ratio[2] * eigenResults[index].vectors[2].values[0]) + MAX_GAUSSIAN_FILTER_RADIUS; 
							yd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[1] + ratio[1] * eigenResults[index].vectors[1].values[1] + ratio[2] * eigenResults[index].vectors[2].values[1]) + MAX_GAUSSIAN_FILTER_RADIUS;
							zd = 0.5 * (ratio[0] * eigenResults[index].vectors[0].values[2] + ratio[1] * eigenResults[index].vectors[1].values[2] + ratio[2] * eigenResults[index].vectors[2].values[2]) + MAX_GAUSSIAN_FILTER_RADIUS;

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

							fprintf(outFile, "%i %i %i \n", x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS);
							for(int v = 0; v < 3; v++) {							
								ratio = 0.7 * eigenResults[index].values[2]/eigenResults[index].values[v];
								fprintf(outFile, "%f %f %f \n", 
									x - MAX_GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[0], 
									y - MAX_GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[1], 
									z - MAX_GAUSSIAN_FILTER_RADIUS + ratio * eigenResults[index].vectors[v].values[2]);
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


		void VolumeSkeletonizer::WriteEigenResultsToVRMLFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath, bool doInverse) {
			int index;
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");
			srand( (unsigned)time( NULL ) );
			double r, g, b, colorCost;
			double theta, theta1, theta2, theta3, s;
			Vector3D axis, axis1, axis2, axis3;
			Vector3D newY, newX;
			Vector3D xAxis = Vector3D(1.0, 0.0, 0.0);
			Vector3D yAxis = Vector3D(0.0, 1.0, 0.0);

			double sizeX, sizeY, sizeZ;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							
							colorCost = cost->getDataAt(x, y, z);
							HueR(colorCost, r, g, b);

							axis1 = eigenResults[index].vectors[0] ^ xAxis;
							axis1.Normalize();
							theta1 = acos(eigenResults[index].vectors[0] * xAxis);
							
							newX = xAxis.Rotate(axis1, theta1);
							newY = yAxis.Rotate(axis1, theta1);
							axis2 = (eigenResults[index].vectors[1] ^ newY);
							axis2.Normalize();
							theta2 = acos(eigenResults[index].vectors[1] * newY);

							if(doInverse) {
								sizeX = isZero(eigenResults[index].values[0])? 0.00001 :eigenResults[index].values[2]/eigenResults[index].values[0];
								sizeY = isZero(eigenResults[index].values[1])? 0.00001 :eigenResults[index].values[2]/eigenResults[index].values[1];
								sizeZ = 1.0;
							} else {
								sizeX = 1.0;
								sizeY = isZero(eigenResults[index].values[0])? 0.00001 :eigenResults[index].values[1]/eigenResults[index].values[0];
								sizeZ = isZero(eigenResults[index].values[0])? 0.00001 :eigenResults[index].values[2]/eigenResults[index].values[0];
							}

							//fprintf(outFile, "Group{\n children [\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [Transform{\n rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}]}\n",
							//	x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
							//	axis2.values[0], axis2.values[1], axis2.values[2], -theta2,
							//	axis1.values[0], axis1.values[1], axis1.values[2], -theta1,
							//	r, g, b,									
							//	0.7 * sizeX, 0.7 * sizeY, 0.7 * sizeZ);
							fprintf(outFile, "Group{\n children [\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [Transform{\n rotation %lf %lf %lf %lf \n scale %lf %lf %lf \n children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf \n transparency 0.5 }} \n geometry Sphere {radius 0.5}}]}]}]}\n",
								x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
								axis2.values[0], axis2.values[1], axis2.values[2], -theta2,
								axis1.values[0], axis1.values[1], axis1.values[2], -theta1,
								sizeX, sizeY, sizeZ,
								r, g, b);								
							

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
							//	x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
							//	axis1.values[0], axis1.values[1], axis1.values[2], theta1,
							//	0.0, 0.5, 0.0,									
							//	0.7 * eigenResults[index].values[2]/eigenResults[index].values[0], 0.01, 0.01);

							//fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}\n",
							//	x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
							//	axis2.values[0], axis2.values[1], axis2.values[2], theta2,
							//	0.0, 0.0, 0.5,									
							//	0.01, 0.7 * eigenResults[index].values[2]/eigenResults[index].values[1], 0.01);

							//fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf }} \n geometry Box {size %lf %lf %lf}}]}]}\n",
							//	x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
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
							Vector3D axis = skeletonDirections[index] ^ Vector3D(1.0, 0.0, 0.0);
							axis.Normalize();
							double angle = -(skeletonDirections[index] * Vector3D(1.0, 0.0, 0.0));
							angle = acos(angle);

							if((axis.values[0] == 0) && (axis.values[1] == 0) && (axis.values[2] == 0)) {
								fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f }} \n geometry Box {size 0.7 0.05 0.05}}]}]}\n",
									x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
									r, g, b);
							} else {
								fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %f %f %f %f \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f }} \n geometry Box {size 0.7 0.05 0.05}}]}]}\n",
									x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
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
						bins[(int)round(skeleton->getDataAt(x, y, z))].push_back(Vector3DInt(x, y, z));
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



		Volume * VolumeSkeletonizer::PerformImmersionSkeletonizationAndPruning(Volume * sourceVol, double startGray, double endGray, double stepSize, int minCurveSize, int minSurfaceSize, string outputPath, bool doPruning, double pointThreshold, double curveThreshold, double surfaceThreshold) {
			Vector3D * volumeGradient;
			EigenResults3D * volumeEigens;
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			if(doPruning) {
				volumeGradient = GetVolumeGradient2(sourceVol);			
			}

			Volume * surfaceVol = GetImmersionThinning(sourceVol, NULL, startGray, endGray, stepSize, THINNING_CLASS_SURFACE_PRESERVATION);
			if(doPruning) {
				surfaceVol->toMRCFile((char *)(outputPath + "-S-Pre-Prune.mrc").c_str());
				volumeEigens = GetEigenResults2(surfaceVol, volumeGradient, gaussianFilterSurfaceRadius, surfaceRadius, true);
				PruneUsingStructureTensor(surfaceVol, sourceVol, volumeEigens, surfaceThreshold, PRUNING_CLASS_PRUNE_SURFACES, outputPath + "-S");
				delete [] volumeEigens;
				surfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Prune.mrc").c_str());
			}			
			PruneSurfaces(surfaceVol, minSurfaceSize);			
			surfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Erosion.mrc").c_str());


			Volume * curveVol = GetImmersionThinning(sourceVol, surfaceVol, startGray, endGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			if(doPruning) {
				curveVol->toMRCFile((char *)(outputPath + "-C-Pre-Prune.mrc").c_str());
				volumeEigens = GetEigenResults2(curveVol, volumeGradient, gaussianFilterCurveRadius, curveRadius, true);
				PruneUsingStructureTensor(curveVol, sourceVol, volumeEigens, curveThreshold, PRUNING_CLASS_PRUNE_CURVES, outputPath + "-C");
				delete [] volumeEigens;
				curveVol->toMRCFile((char *)(outputPath + "-C-Post-Prune.mrc").c_str());
			}
			PruneCurves(curveVol, minCurveSize);			
			curveVol->toMRCFile((char *)(outputPath + "-C-Post-Erosion.mrc").c_str());
			

			Volume * pointVol = GetImmersionThinning(sourceVol, curveVol, startGray, endGray, stepSize, THINNING_CLASS_POINT_PRESERVATION);
			if(doPruning) {
				pointVol->toMRCFile((char *)(outputPath + "-P-Pre-Prune.mrc").c_str());
				volumeEigens = GetEigenResults2(pointVol, volumeGradient, gaussianFilterPointRadius, pointRadius, true);
				PruneUsingStructureTensor(pointVol, sourceVol, volumeEigens, pointThreshold, PRUNING_CLASS_PRUNE_POINTS, outputPath + "-P");
				delete [] volumeEigens;
				pointVol->toMRCFile((char *)(outputPath + "-P-Post-Prune.mrc").c_str());
			}

			delete surfaceVol;
			delete curveVol;
			delete [] volumeGradient;

			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			pointVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
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
						bins[(int)round(skeleton->getDataAt(x, y, z))].push_back(Vector3DInt(x, y, z));
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
			imageVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			
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
				//tempVolume->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

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

			compositeVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			deletedVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

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
			sourceVolume->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			sourceSkeleton->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			printf("Getting volume Eigens\n");
			Vector3D * volumeGradient = GetVolumeGradient(sourceVolume);
			EigenResults3D * volumeEigens = GetEigenResults(sourceSkeleton, volumeGradient, gaussianFilterMaxRadius, MAX_GAUSSIAN_FILTER_RADIUS, true);

			WriteEigenResultsToOFFFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.off");
			WriteEigenResultsToVRMLFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.wrl", false);

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
						
			WriteEigenResultsToVRMLFile(sourceVolume, curveScore, sourceSkeleton, volumeEigens, outputPath + "-C-Eigens.wrl", false);
			WriteEigenResultsToVRMLFile(sourceVolume, surfaceScore, sourceSkeleton, volumeEigens, outputPath + "-S-Eigens.wrl", false);

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



	}
}

#endif