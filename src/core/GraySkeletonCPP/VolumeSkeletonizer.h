// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs skeletonization on a grayscale volume


#ifndef GRAYSKELETONCPP_VOLUME_SKELETONIZER_H
#define GRAYSKELETONCPP_VOLUME_SKELETONIZER_H

#include "GrayImage.h"
#include "GrayImageList.h"
#include "ImageReaderBMP.h"
#include "ImageReaderMRC.h"
#include "GlobalDefinitions.h"
#include "VolumeDeltaAnalyzer.h"
#include <MathTools/Combinatorics.h>
#include <SkeletonMaker/PriorityQueue.h>
#include <MathTools/MathLib.h>
#include <MathTools/DataStructures.h>
#include <MathTools/Vector3D.h>
#include "NormalFinder.h"
#include <string>
#include <Foundation/TimeManager.h>
#include <new>
#include <functional>
#include <list>

using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;
using namespace std;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace GraySkeletonCPP {
	
		struct ImmersionBeachElement {
			Vector3DInt p;
			int binIndex;
		};


		class VolumeSkeletonizer{
		public:
			VolumeSkeletonizer(int pointRadius, int curveRadius, int surfaceRadius, int skeletonDirectionRadius);
			~VolumeSkeletonizer();
			Volume * CleanImmersionSkeleton(Volume * skeleton, string outputPath);
			Volume * PerformImmersionSkeletonizationAndPruning(Volume * sourceVol, Volume * preserveVol, double startGray, double endGray, double stepSize, int smoothingIterations, int smoothingRadius, int minCurveSize, int minSurfaceSize, int maxCurveHole, int maxSurfaceHole, string outputPath, bool doPruning, double pointThreshold, double curveThreshold, double surfaceThreshold);
			Volume * PerformSkeletonizationAndPruning(Volume * imageVol, string outputPath);
			Volume * PerformImmersionSkeletonization(Volume * imageVol, string outputPath);
			Volume * PerformJuSkeletonization(Volume * imageVol, string outputPath, int minGray, int maxGray, int stepSize);
			Volume * PerformPureJuSkeletonization(Volume * imageVol, string outputPath, double threshold, int minCurveWidth, int minSurfaceWidth);
			Volume * PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputPath);
			Volume * GetJuSurfaceSkeleton(Volume * sourceVolume, Volume * preserve, double threshold);
			Volume * GetJuCurveSkeleton(Volume * sourceVolume, Volume * preserve, double threshold, bool is3D);
			Volume * GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold);			
			void CleanupVolume(Volume * sourceVolume, double low, double high);
			void NormalizeVolume(Volume * sourceVolume);			
			void PruneCurves(Volume * sourceVolume, int pruneLength);
			void PruneSurfaces(Volume * sourceVolume, int pruneLength);
			void PruneUsingStructureTensor(Volume * skeleton, Volume * sourceVolume, Volume * preserveVol, Vector3DFloat * volumeGradient, EigenResults3D * volumeEigens, ProbabilityDistribution3D & filter, double threshold, char pruningClass, string outputPath);
			void SmoothenVolume(Volume * &sourceVolume, double minGrayscale, double maxGrayscale, int stRadius);
			void ThresholdGrayValueRange(Volume * sourceVolume, double minGrayValue, double maxGrayValue);
			void VoxelBinarySubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
			void VoxelSubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
			void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
			Vector3DFloat GetCurveDirection(Volume * skeleton, int x, int y, int z);
			Vector3DFloat GetCurveDirection(Volume * skeleton, int x, int y, int z, int radius);
			Vector3DFloat GetSurfaceNormal(Volume * skeleton, int x, int y, int z);
			Vector3DFloat GetSurfaceNormal(Volume * skeleton, int x, int y, int z, int radius, Vector3DFloat * localDirections);
			Vector3DFloat * GetVolumeGradient(Volume * sourceVolume);
			Vector3DFloat * GetVolumeGradient2(Volume * sourceVolume);
			Vector3DFloat * GetSkeletonDirection(Volume * skeleton, int type);


			void GetEigenResult(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			void GetEigenResult2(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear);
			EigenResults3D * GetEigenResults(Volume * maskVol, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);
			EigenResults3D * GetEigenResults2(Volume * maskVol, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask);

			static Volume * PerformAnisotropicSmoothingAxisAligned(Volume * sourceVolume, int xRadius, int yRadius, int zRadius);
		protected:
			
			double AngleToParameter(double angle);
			double GetVoxelCost(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, int type);
			void AddIterationToVolume(Volume * compositeVolume, Volume * iterationVolume, unsigned char threshold);
			void ApplyMask(Volume * sourceVolume, Volume * maskVolume, unsigned char maskValue, bool keepMaskValue);
			void FindOrthogonalAxes(Vector3DFloat axis, Vector3DFloat & res1, Vector3DFloat & res2);			
			void GetSTBasedDistribution(ProbabilityDistribution3D & distributionInfo, EigenResults3D eigen);
			void HueR(double value, double &r, double &g, double &b);
			void HueRB(double value, double &r, double &g, double &b);
			void HueRGB(double value, double &r, double &g, double &b);
			void MarkDeletableVoxels(Volume * deletedVol, Volume * currentVolume, Volume * preservedVolume);
			void RemoveCubesFromSurfaceSkeleton(Volume * sourceSkeleton);
			void RemoveIsolatedNonCurves(Volume * skeleton);
			void WriteEigenResultsToFile(Volume * sourceVolume, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToOFFFileWireframe(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath);
			void WriteEigenResultsToVRMLFile(Volume * sourceVolume, Volume * cost, Volume * skeleton, EigenResults3D * eigenResults, string outputPath, bool doInverse);
			void WriteSkeletonDirectionToVRMLFile(Volume * skeleton, Volume * cost, Vector3DFloat * skeletonDirections, string outputPath, bool surfaces, double curveSize);
			void WriteVolumeToVRMLFile(Volume * vol, string outputPath);			
			Vector3DFloat XYZtoUVW(Vector3DFloat vec, Vector3DFloat u, Vector3DFloat v, Vector3DFloat w);
			Volume * FillCurveHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize);
			Volume * FillSurfaceHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize);
			Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass);
			Volume * GetImmersionThinning(Volume * sourceVolume, Volume * preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass);									

			
			static const char THINNING_CLASS_SURFACE_PRESERVATION;
			static const char THINNING_CLASS_CURVE_PRESERVATION_2D;
			static const char THINNING_CLASS_CURVE_PRESERVATION;
			static const char THINNING_CLASS_POINT_PRESERVATION;
			static const char THINNING_CLASS_TOPOLOGY_PRESERVATION;
			static const char PRUNING_CLASS_PRUNE_SURFACES;
			static const char PRUNING_CLASS_PRUNE_CURVES;
			static const char PRUNING_CLASS_PRUNE_POINTS;
			
		public:
			MathLib * math;
			NormalFinder * surfaceNormalFinder;
			ProbabilityDistribution3D gaussianFilterPointRadius;
			ProbabilityDistribution3D gaussianFilterCurveRadius;
			ProbabilityDistribution3D gaussianFilterSurfaceRadius;
			ProbabilityDistribution3D gaussianFilterMaxRadius;
			ProbabilityDistribution3D uniformFilterSkeletonDirectionRadius;
			int pointRadius;
			int curveRadius;
			int surfaceRadius;
			int skeletonDirectionRadius;

		};
		
		const char VolumeSkeletonizer::THINNING_CLASS_SURFACE_PRESERVATION = 4;
		const char VolumeSkeletonizer::THINNING_CLASS_CURVE_PRESERVATION_2D = 3;
		const char VolumeSkeletonizer::THINNING_CLASS_CURVE_PRESERVATION = 2;
		const char VolumeSkeletonizer::THINNING_CLASS_POINT_PRESERVATION = 1;
		const char VolumeSkeletonizer::THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
		const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_SURFACES = 5;
		const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_CURVES = 6;
		const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_POINTS = 7;	
		

		VolumeSkeletonizer::VolumeSkeletonizer(int pointRadius, int curveRadius, int surfaceRadius, int skeletonDirectionRadius) {
			math = new MathLib();
			surfaceNormalFinder = new NormalFinder();
			this->pointRadius = pointRadius;
			this->curveRadius = curveRadius;
			this->surfaceRadius = surfaceRadius;
			this->skeletonDirectionRadius = skeletonDirectionRadius;

			gaussianFilterPointRadius.radius = pointRadius;
			math->GetBinomialDistribution(gaussianFilterPointRadius);

			gaussianFilterCurveRadius.radius = curveRadius;
			math->GetBinomialDistribution(gaussianFilterCurveRadius);

			gaussianFilterSurfaceRadius.radius = surfaceRadius;
			math->GetBinomialDistribution(gaussianFilterSurfaceRadius);

			gaussianFilterMaxRadius.radius = MAX_GAUSSIAN_FILTER_RADIUS;
			math->GetBinomialDistribution(gaussianFilterMaxRadius);

			uniformFilterSkeletonDirectionRadius.radius = skeletonDirectionRadius;
			math->GetUniformDistribution(uniformFilterSkeletonDirectionRadius);
		}

		VolumeSkeletonizer::~VolumeSkeletonizer() {
			delete math;
			delete surfaceNormalFinder;
		}


		double VolumeSkeletonizer::AngleToParameter(double angle) {
			angle = fmod(angle, PI);
			return (1-abs(2*angle/PI -1));
		}

		double VolumeSkeletonizer::GetVoxelCost(EigenResults3D imageEigen, Vector3DFloat skeletonDirection, int type) {
			double cost = 1;

			if(!isZero(imageEigen.values[0])) {
				double theta, a, b;
				Vector3DFloat temp, skelDirectionST, n;
				float u1 = 1.0;
				float u2 = abs(imageEigen.values[1]/imageEigen.values[0]);
				float u3 = abs(imageEigen.values[2]/imageEigen.values[0]);
				Vector3DFloat v1 = imageEigen.vectors[0];
				Vector3DFloat v2 = imageEigen.vectors[1];
				Vector3DFloat v3 = imageEigen.vectors[2];				
				switch(type) {
					case PRUNING_CLASS_PRUNE_POINTS:

						cost = u3*u3 / (u1*u2);
						break;
					case PRUNING_CLASS_PRUNE_CURVES:

						if(skeletonDirection.IsBadNormal()) {
							cost = 1.0;
						} else {
							n = XYZtoUVW(skeletonDirection, v1, v2, v3);
							a = u1 * u2 * u3;
							b = sqrt(u2*u2*u3*u3*n.X()*n.X() + u1*u1*u3*u3*n.Y()*n.Y() + u1*u1*u2*u2*n.Z()*n.Z());
							temp = n*(a/b);
							cost = u3/ temp.Length();
						}


						break;
					case PRUNING_CLASS_PRUNE_SURFACES:
				
						{
							if(skeletonDirection.IsBadNormal()) {
								cost = 1.0;
							} else {
								Vector3DFloat n1, n2, m1, m2;
								skelDirectionST = XYZtoUVW(skeletonDirection, imageEigen.vectors[0],imageEigen.vectors[1], imageEigen.vectors[2]);
								FindOrthogonalAxes(skelDirectionST, n1, n2);

								m1 = Vector3DFloat(n1.values[0]/u1, n1.values[1]/u2, n1.values[2]/u3);
								m2 = Vector3DFloat(n2.values[0]/u1, n2.values[1]/u2, n2.values[2]/u3);
								theta = atan((2.0 * (m1 * m2)) / ((m1 * m1) - (m2 * m2))) / 2.0;							
								a = 1.0 / ((m1 * cos(theta)) + (m2 * sin(theta))).Length();
								b = 1.0 / ((m1 * sin(theta)) - (m2 * cos(theta))).Length();
								cost = (u2 * u3) / (a*b);
							}
						}
						break;
				}
				
			} else {
				cost = 1.0;
				//printf("Zero\n");
			}

			if(cost != cost) {
				cost = 1.0;
			}
			return cost;

		}

		EigenResults3D * VolumeSkeletonizer::GetEigenResults(Volume * maskVol, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
			EigenResults3D * resultTable = new(std::nothrow) EigenResults3D[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];

			if(resultTable == NULL) {
				printf("Not enough memory to store eigens, going to slower mode!\n"); 
			} else {
				for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < maskVol->getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
					for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < maskVol->getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
						for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < maskVol->getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
							GetEigenResult(resultTable[maskVol->getIndex(x, y, z)], imageGradient, gaussianFilter, x, y, z,
										   maskVol->getSizeX(), maskVol->getSizeY(), maskVol->getSizeZ(), gaussianFilterRadius, (useMask && (maskVol->getDataAt(x, y, z) == 0))); 
						}
					}
				}
			}
			return resultTable;
		}

		// Use 8 local gradients instead of 1 gradient
		EigenResults3D * VolumeSkeletonizer::GetEigenResults2(Volume * maskVol, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int gaussianFilterRadius, bool useMask) {
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

		Vector3DFloat VolumeSkeletonizer::XYZtoUVW(Vector3DFloat vec, Vector3DFloat u, Vector3DFloat v, Vector3DFloat w) {
			float uContri = vec * u; 
			float vContri = vec * v;
			float wContri = vec * w;
			Vector3DFloat inUVW = Vector3DFloat(uContri, vContri, wContri);
			inUVW.Normalize();
			return inUVW;
		}

		Vector3DFloat VolumeSkeletonizer::GetCurveDirection(Volume * skeleton, int x, int y, int z) {
			Vector3DFloat v1, currentPos;
			Vector3DFloat * n6 = new Vector3DFloat[6];
			int n6Count;
			Vector3DFloat direction = Vector3DFloat(0,0,0);
			if(skeleton->getDataAt(x,y,z) <= 0) {
				return direction;
			}
			
			currentPos = Vector3DFloat((float)x, (float)y, (float)z);						
			n6Count = 0;
			for(int i = 0; i < 6; i++) {							
				if(skeleton->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0) {
					n6[n6Count] = Vector3DFloat(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]);
					if(n6Count > 0) {
						v1 = n6[n6Count-1] - n6[n6Count];
						direction = direction + v1;
					}
					n6Count++;
				}
			}
			if(n6Count == 1) {
				v1 = Vector3DFloat(x, y, z);
				v1 = v1 - n6[0];
				direction = direction + v1;
			}
			direction.Normalize();
			if(n6Count > 2) {
				direction = Vector3DFloat(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
			}

			delete [] n6;
			return direction;
		}


		Vector3DFloat VolumeSkeletonizer::GetCurveDirection(Volume * skeleton, int x, int y, int z, int radius) {
			Vector3DFloat direction = Vector3DFloat(0,0,0);
			if(DiscreteMesh::GetN6Count(skeleton, x, y, z) > 2) {
				direction = Vector3DFloat(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
			} else {

				int margin = 2;
				int size = (radius+margin)*2 + 1;
				Volume * block = new Volume(size, size, size);
				for(int xx = margin; xx <= size-margin; xx++) {
					for(int yy = margin; yy <= size-margin; yy++) {
						for(int zz = margin; zz <= size-margin; zz++) {
							block->setDataAt(xx, yy, zz, skeleton->getDataAt(x-radius-margin+xx, y-radius-margin+yy, z-radius-margin+zz));
						}
					}
				}

				Volume * visited = new Volume(size, size, size);
//				:TODO: rename llist
				vector<Vector3DInt> llist;
				llist.push_back(Vector3DInt(margin+radius, margin+radius, margin+radius));
				Vector3DInt currentPos;
				Vector3DInt * n6;
				int n6Count;

				while(llist.size() > 0) {
					currentPos = llist[llist.size()-1];
					llist.pop_back();
					visited->setDataAt(currentPos.X(), currentPos.Y(), currentPos.Z(), 1);
					n6Count = DiscreteMesh::GetN6(n6, block, currentPos.X(), currentPos.Y(), currentPos.Z());

					if(DiscreteMesh::GetN6Count(skeleton, x+currentPos.X()-margin-radius, y+currentPos.Y()-margin-radius, z+currentPos.Z()-margin-radius) <= 2) {
						for(int i = 0; i < n6Count; i++) {
							if(visited->getDataAt(n6[i].X(), n6[i].Y(), n6[i].Z()) < 1) {
								llist.push_back(n6[i]);
							}
						}
					}
					delete [] n6;
				}

				delete block;

				Vector3DFloat * gradient = GetVolumeGradient(visited);
				EigenResults3D eigen;
				GetEigenResult(eigen, gradient, uniformFilterSkeletonDirectionRadius,  
					margin+radius, margin+radius, margin+radius,
					size, size, size, radius, false);

				delete [] gradient;
				delete visited;

				direction = eigen.vectors[2];
			}

			return direction;
		}
		Vector3DFloat VolumeSkeletonizer::GetSurfaceNormal(Volume * skeleton, int x, int y, int z) {
			surfaceNormalFinder->InitializeGraph(skeleton, x, y, z);
			return surfaceNormalFinder->GetSurfaceNormal();
		}



		Vector3DFloat VolumeSkeletonizer::GetSurfaceNormal(Volume * skeleton, int x, int y, int z, int radius, Vector3DFloat * localDirections) {
			Vector3DFloat direction = localDirections[skeleton->getIndex(x, y, z)];


			if(!direction.IsBadNormal()) {
				int margin = 2;
				int size = (radius+margin)*2 + 1;
				Volume * block = new Volume(size, size, size);

				for(int xx = margin; xx <= size-margin; xx++) {
					for(int yy = margin; yy <= size-margin; yy++) {
						for(int zz = margin; zz <= size-margin; zz++) {
							block->setDataAt(xx, yy, zz, skeleton->getDataAt(x-radius-margin+xx, y-radius-margin+yy, z-radius-margin+zz));
						}
					}
				}

				Volume * visited = new Volume(size, size, size);
//				:TODO: rename llist
				vector<Vector3DInt> llist;
				llist.push_back(Vector3DInt(margin+radius, margin+radius, margin+radius));
				Vector3DInt currentPos, newPos;
				Vector3DFloat tempDir;

				while(llist.size() > 0) {
					currentPos = llist[llist.size()-1];
					llist.pop_back();
					visited->setDataAt(currentPos.X(), currentPos.Y(), currentPos.Z(), 1);
					tempDir = localDirections[skeleton->getIndex(x+currentPos.X()-margin-radius, y+currentPos.Y()-margin-radius, z+currentPos.Z()-margin-radius)];

					if(!tempDir.IsBadNormal()) {
						for(int i = 0; i < 12; i++) {
							if((block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][0][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][0][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][0][2]) > 0) &&
							   (block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][1][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][1][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][1][2]) > 0) &&
							   (block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][2][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][2][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][2][2]) > 0)) {
								for(int j = 0; j < 3; j++) {
									newPos = currentPos + Vector3DInt(VOLUME_NEIGHBOR_FACES[i][j][0], VOLUME_NEIGHBOR_FACES[i][j][1], VOLUME_NEIGHBOR_FACES[i][j][2]);
									if(visited->getDataAt(newPos.X(), newPos.Y(), newPos.Z()) < 1) {
										llist.push_back(newPos);
									}
								}
							}
						}					
					}
				}


				delete block;

				Vector3DFloat * gradient = GetVolumeGradient(visited);

				EigenResults3D eigen;

				GetEigenResult(eigen, gradient, uniformFilterSkeletonDirectionRadius,  
					margin+radius, margin+radius, margin+radius,
					size, size, size, radius, false);


				delete [] gradient;
				delete visited;

				direction = eigen.vectors[0];
				
			}

			return direction;
		}
		// Gradient = (x+1,y,z) - (x-1,y,z) ....
		Vector3DFloat * VolumeSkeletonizer::GetVolumeGradient(Volume * sourceVolume) {
			Vector3DFloat * gradient = new Vector3DFloat[sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ()];
			int index;

			for(int x = 0; x < sourceVolume->getSizeX(); x = x + sourceVolume->getSizeX()-1) {
				for(int y = 0; y < sourceVolume->getSizeY(); y = y + sourceVolume->getSizeY()-1) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z = z + sourceVolume->getSizeZ()-1) {
						index = sourceVolume->getIndex(x, y, z);
						gradient[index] = Vector3DFloat(0, 0, 0);
					}
				}
			}
			Vector3DFloat grad1, grad2;

			for(int x = 1; x < sourceVolume->getSizeX()-1; x++) {
				for(int y = 1; y < sourceVolume->getSizeY()-1; y++) {
					for(int z = 1; z < sourceVolume->getSizeZ()-1; z++) {
						index = sourceVolume->getIndex(x, y, z);
						grad1 = Vector3DFloat(sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x-1, y, z),
												   sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y-1, z),
												   sourceVolume->getDataAt(x, y, z) - sourceVolume->getDataAt(x, y, z-1));

						grad2 = Vector3DFloat(sourceVolume->getDataAt(x+1, y, z) - sourceVolume->getDataAt(x, y, z),
												   sourceVolume->getDataAt(x, y+1, z) - sourceVolume->getDataAt(x, y, z),
												   sourceVolume->getDataAt(x, y, z+1) - sourceVolume->getDataAt(x, y, z));

						
						for(int i = 0; i < 3; i++) {
							if(abs(grad1.values[i]) > abs(grad2.values[i])) {
								gradient[index].values[i] = grad1.values[i];
							} else {
								gradient[index].values[i] = grad2.values[i];
							}
						}
					}
				}
			}
			return gradient;
		}
		// Gradient = (x,y,z) - (x-1,y,z) ....
		Vector3DFloat * VolumeSkeletonizer::GetVolumeGradient2(Volume * sourceVolume) {
			int size = sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ();
			Vector3DFloat * gradient = new Vector3DFloat[sourceVolume->getSizeX() * sourceVolume->getSizeY() * sourceVolume->getSizeZ() * 8];
			int index, index2;

			for(int x = 0; x < sourceVolume->getSizeX(); x = x + sourceVolume->getSizeX()-1) {
				for(int y = 0; y < sourceVolume->getSizeY(); y = y + sourceVolume->getSizeY()-1) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z = z + sourceVolume->getSizeZ()-1) {
						index = sourceVolume->getIndex(x, y, z);
						for(int a = 0; a < 8; a++) {
							index2 = a * size + index;
							gradient[index2] = Vector3DFloat(0, 0, 0);
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

		Vector3DFloat * VolumeSkeletonizer::GetSkeletonDirection(Volume * skeleton, int type) {
			int index;

			Vector3DFloat * localDirections = new Vector3DFloat[skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ()];

			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ()-1; z++) {						
						index = skeleton->getIndex(x, y, z);
						localDirections[index] = Vector3DFloat(0,0,0);
						if(skeleton->getDataAt(x,y,z) > 0) {
							switch(type){
								case PRUNING_CLASS_PRUNE_SURFACES:
									localDirections[index] = GetSurfaceNormal(skeleton, x, y, z);
									break;
							}
						}
					}
				}
			}

			Vector3DFloat * directions = new Vector3DFloat[skeleton->getSizeX() * skeleton->getSizeY() * skeleton->getSizeZ()];

			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ()-1; z++) {						
						index = skeleton->getIndex(x, y, z);
						directions[index] = Vector3DFloat(0,0,0);
						if(skeleton->getDataAt(x,y,z) > 0) {
							switch(type){
								case PRUNING_CLASS_PRUNE_CURVES:
									directions[index] = GetCurveDirection(skeleton, x, y, z, skeletonDirectionRadius);
									break;
								case PRUNING_CLASS_PRUNE_SURFACES:
									directions[index] = GetSurfaceNormal(skeleton, x, y, z, skeletonDirectionRadius, localDirections);
									break;
							}
						}
					}
				}
			}
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
							sourceVolume->setDataAt(x, y, z, 0);
						}
						if(sourceVolume->getDataAt(x, y, z) > high) {
							sourceVolume->setDataAt(x, y, z, high);
						}
					}
				}
			}
		}
		void VolumeSkeletonizer::FindOrthogonalAxes(Vector3DFloat axis, Vector3DFloat & res1, Vector3DFloat & res2) {
			res1 = Vector3DFloat(1.0, 0.0, 0.0);
			if(abs(axis * res1) > 0.95) {
				res1 = Vector3DFloat(0.0, 1.0, 0.0);
			}
			res1 = axis ^ res1;
			res2 = axis ^ res1;
			res1.Normalize();
			res2.Normalize();
		}

		void VolumeSkeletonizer::GetEigenResult(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear) {
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
		void VolumeSkeletonizer::GetEigenResult2(EigenResults3D & returnVal, Vector3DFloat * imageGradient, ProbabilityDistribution3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear) {
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


		void VolumeSkeletonizer::GetSTBasedDistribution(ProbabilityDistribution3D & distributionInfo, EigenResults3D eigen) {
			Vector3DFloat skeletonDirection;
			double total = 0;
			double cell;
			for(int x = -distributionInfo.radius; x <= distributionInfo.radius; x++) {
				for(int y = -distributionInfo.radius; y <= distributionInfo.radius; y++) {
					for(int z = -distributionInfo.radius; z <= distributionInfo.radius; z++) {
						if((x!=0) && (y!=0) && (z!=0)) {
							skeletonDirection = Vector3DFloat(0,0,0) - Vector3DFloat(x, y, z);
							skeletonDirection.Normalize();
							cell = GetVoxelCost(eigen, skeletonDirection, PRUNING_CLASS_PRUNE_CURVES);
							distributionInfo.values[x+distributionInfo.radius][y+distributionInfo.radius][z+distributionInfo.radius] = cell;
							total += cell;
						}
					}
				}
			}

			for(int x = -distributionInfo.radius; x <= distributionInfo.radius; x++) {
				for(int y = -distributionInfo.radius; y <= distributionInfo.radius; y++) {
					for(int z = -distributionInfo.radius; z <= distributionInfo.radius; z++) {
						distributionInfo.values[x+distributionInfo.radius][y+distributionInfo.radius][z+distributionInfo.radius] = 
							distributionInfo.values[x+distributionInfo.radius][y+distributionInfo.radius][z+distributionInfo.radius] / total;
					}
				}
			}

		}
		void VolumeSkeletonizer::HueR(double value, double &r, double &g, double &b) {
			r = value*value*value*value;
			g = 0;
			b = 0;
		}

		void VolumeSkeletonizer::HueRB(double value, double &r, double &g, double &b) {
				double v2 = pow(value, 1.5);					
				r = (1 - v2);	
				g = 0;
				b = v2;				
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

		void VolumeSkeletonizer::PruneCurves(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeHelix(pruneLength);
		}
		void VolumeSkeletonizer::PruneSurfaces(Volume * sourceVolume, int pruneLength) {
			sourceVolume->erodeSheet(pruneLength);
		}		
		void VolumeSkeletonizer::PruneUsingStructureTensor(Volume * skeleton, Volume * sourceVolume, Volume * preserveVol, Vector3DFloat * volumeGradient, EigenResults3D * volumeEigens, ProbabilityDistribution3D & filter, double threshold, char pruningClass, string outputPath) {									
			Volume * tempSkel = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), 0, 0, 0, skeleton);
			Volume * costVol = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ()); 
			Vector3DFloat * skeletonDirections = GetSkeletonDirection(skeleton, pruningClass);
			int index;
			double cost;
			EigenResults3D eigen;

			for(int x = 0; x < skeleton->getSizeX(); x++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int z = 0; z < skeleton->getSizeZ(); z++) {						
						index = skeleton->getIndex(x, y, z);						
						if(((preserveVol == NULL) || ((preserveVol != NULL) && preserveVol->getDataAt(index) < 0.5)) && (tempSkel->getDataAt(index) > 0)) {
							if(volumeEigens == NULL) {
								GetEigenResult(eigen, volumeGradient, filter, x, y, z, skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), filter.radius, false);
							} else {
								eigen = volumeEigens[index];
							}
							cost = GetVoxelCost(eigen, skeletonDirections[index], pruningClass);
							if(cost < threshold) {
								skeleton->setDataAt(index, 0.0);
							}
							costVol->setDataAt(index, cost);							
						}
					}
				}
			}

			
			#ifdef SAVE_INTERMEDIATE_RESULTS
				costVol->toMRCFile((char *)((outputPath + "-Score.mrc").c_str()));
				WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", (pruningClass != PRUNING_CLASS_PRUNE_SURFACES));
				WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens-inverted.wrl", true);
				WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", false);
				WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skeletonDirections, outputPath + "-SkeletonDirections.wrl", pruningClass == PRUNING_CLASS_PRUNE_SURFACES, 0.1);
				if(pruningClass == PRUNING_CLASS_PRUNE_CURVES) {
					WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skeletonDirections, outputPath + "-SkeletonDirections-small.wrl", false, 0.06);
				}
			#endif
			delete costVol;
			delete tempSkel;
			delete [] skeletonDirections;
		}

		void VolumeSkeletonizer::RemoveCubesFromSurfaceSkeleton(Volume * sourceSkeleton) {
			Volume * dummySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			for(int x = 1; x < sourceSkeleton->getSizeX()-1; x++) {
				for(int y = 1; y < sourceSkeleton->getSizeY()-1; y++) {
					for(int z = 1; z < sourceSkeleton->getSizeZ()-1; z++) {
						if(!DiscreteMesh::IsSurfaceBody(dummySkeleton, x, y, z, false)) {
							sourceSkeleton->setDataAt(x, y, z, 0);
						}
					}
				}
			}			
		}

		void VolumeSkeletonizer::RemoveIsolatedNonCurves(Volume * skeleton) {
			Volume * visited = new Volume(skeleton->getSizeX(), skeleton->getSizeY(), skeleton->getSizeZ(), 0,0,0, skeleton);

			vector<Vector3DInt> queue;
			vector<bool> colors;
			int currentColor;
			Vector3DInt currPos, futurePos;

			for(int x = 1; x < skeleton->getSizeX()-1; x++) {
				for(int y = 1; y < skeleton->getSizeY()-1; y++) {
					for(int z = 1; z < skeleton->getSizeZ(); z++) {
						if(visited->getDataAt(x, y, z) > 0) {							
							colors.push_back(false);
							currentColor = colors.size();							
							queue.push_back(Vector3DInt(x, y, z));

							while(queue.size() > 0) {
								currPos = queue[queue.size()-1];
								queue.pop_back();
								if(visited->getDataAt(currPos.X(), currPos.Y(), currPos.Z()) > 0) {
									visited->setDataAt(currPos.X(), currPos.Y(), currPos.Z(), -currentColor);								
									colors[currentColor-1] = colors[currentColor-1] || DiscreteMesh::IsCurveBody(skeleton, currPos.X(), currPos.Y(), currPos.Z()) || DiscreteMesh::IsCurveEnd(skeleton, currPos.X(), currPos.Y(), currPos.Z());

									for(int i = 0; i < 6; i++) {
										futurePos = Vector3DInt(currPos.X() + VOLUME_NEIGHBORS_6[i][0], currPos.Y() + VOLUME_NEIGHBORS_6[i][1], currPos.Z() + VOLUME_NEIGHBORS_6[i][2]);
										if(visited->getDataAt(futurePos.X(), futurePos.Y(), futurePos.Z()) > 0) {
											queue.push_back(futurePos);
										}
									}
								}
							}
						}
					}
				}
			}

			for(int i = 0; i < (int)colors.size(); i++) {
				if(!colors[i]) {
					currentColor = -(i+1);
					for(int x = 1; x < skeleton->getSizeX()-1; x++) {
						for(int y = 1; y < skeleton->getSizeY()-1; y++) {
							for(int z = 1; z < skeleton->getSizeZ(); z++) {
								if(isZero(visited->getDataAt(x, y, z) - (double)currentColor)) {
									skeleton->setDataAt(x, y, z, 0);
								}
							}
						}
					}
				}
			}
		}

		void VolumeSkeletonizer::SmoothenVolume(Volume * & sourceVolume, double minGrayscale, double maxGrayscale, int stRadius) {
			ProbabilityDistribution3D mask;
			mask.radius = 1;

			ProbabilityDistribution3D smoothenMask;
			smoothenMask.radius = stRadius;
			math->GetBinomialDistribution(smoothenMask);

			sourceVolume->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			Volume * maskVolume = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			double data;
			for(int x = 0; x < sourceVolume->getSizeX(); x++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
						data = sourceVolume->getDataAt(x, y, z);
						if((data >= minGrayscale) && (data <= maxGrayscale)) {
							maskVolume->setDataAt(x, y, z, 1.0);
						} else {
							maskVolume->setDataAt(x, y, z, 0.0);
						}						
					}
				}
			}

			Vector3DFloat * volumeGradient = GetVolumeGradient(sourceVolume);
			EigenResults3D * eigens = GetEigenResults(maskVolume, volumeGradient, smoothenMask, stRadius, true);	
			Volume * destVolume = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			double sourceData;

			for(int x = mask.radius; x < sourceVolume->getSizeX()-mask.radius; x++) {
				for(int y = mask.radius; y < sourceVolume->getSizeY()-mask.radius; y++) {
					for(int z = mask.radius; z < sourceVolume->getSizeZ()-mask.radius; z++) {
						sourceData = sourceVolume->getDataAt(x, y, z);
						if((sourceData >= minGrayscale) && (sourceData <= maxGrayscale)) {
							GetSTBasedDistribution(mask, eigens[sourceVolume->getIndex(x, y, z)]);

							for(int xx = -mask.radius; xx <= mask.radius; xx++) {
								for(int yy = -mask.radius; yy <= mask.radius; yy++) {
									for(int zz = -mask.radius; zz <= mask.radius; zz++) {									
										destVolume->setDataAt(x, y, z, 
											destVolume->getDataAt(x, y, z) + sourceVolume->getDataAt(x+xx, y+yy, z+zz) *  mask.values[xx+mask.radius][yy+mask.radius][zz+mask.radius]);
									}						
								}
							}
							destVolume->setDataAt(x, y, z, sourceVolume->getDataAt(x, y, z) * 0.5 + destVolume->getDataAt(x, y, z) * 0.5);
						} else {
							destVolume->setDataAt(x, y, z, sourceVolume->getDataAt(x, y, z));
						}
					}
				}
			}	

			destVolume->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			delete sourceVolume;			
			sourceVolume = destVolume;

			delete maskVolume;
			delete [] volumeGradient;
			delete [] eigens;
		}
		void VolumeSkeletonizer::ThresholdGrayValueRange(Volume * sourceVolume, double minGrayValue, double maxGrayValue) {
			double data;
			for(int x = 0; x < sourceVolume->getSizeX(); x++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
						data = sourceVolume->getDataAt(x, y, z);
						if((data < minGrayValue) || (data > maxGrayValue)) {
							sourceVolume->setDataAt(x, y, z, 0.0);
						}
					}
				}
			}
		}
		void VolumeSkeletonizer::VoxelBinarySubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
			for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
				for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
					for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
						if(sourceVolume2->getDataAt(x, y, z) > 0) {
							sourceAndDestVolume1->setDataAt(x, y, z, 0);
						}
					}
				}
			}
		}

		void VolumeSkeletonizer::VoxelSubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
			for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
				for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
					for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
						sourceAndDestVolume1->setDataAt(x, y, z, sourceAndDestVolume1->getDataAt(x, y, z) - sourceVolume2->getDataAt(x, y, z));
					}
				}
			}
		}

		void VolumeSkeletonizer::VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
			if(sourceVolume2 != NULL) {
				for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
					for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
						for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
							sourceAndDestVolume1->setDataAt(x, y, z, max(sourceAndDestVolume1->getDataAt(x, y, z), sourceVolume2->getDataAt(x, y, z)));
						}
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
			double theta1, theta2;
			Vector3DFloat axis, axis1, axis2, axis3;
			Vector3DFloat newY, newX;
			Vector3DFloat xAxis = Vector3DFloat(1.0, 0.0, 0.0);
			Vector3DFloat yAxis = Vector3DFloat(0.0, 1.0, 0.0);

			double sizeX, sizeY, sizeZ;

			for(int z = 0; z < sourceVolume->getSizeZ(); z++) {
				for(int y = 0; y < sourceVolume->getSizeY(); y++) {
					for(int x = 0; x < sourceVolume->getSizeX(); x++) {
						if(skeleton->getDataAt(x, y, z) > 0) {
							index = sourceVolume->getIndex(x, y, z);
							
							colorCost = cost->getDataAt(x, y, z);
							//HueR(colorCost, r, g, b);
							r = 0.3;
							g = 0.3;
							b = 0.3;

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
							if(!((axis1.values[0] != axis1.values[0]) || (axis1.values[1] != axis1.values[1]) || (axis1.values[2] != axis1.values[2]) ||
								(axis2.values[0] != axis2.values[0]) || (axis2.values[1] != axis2.values[1]) || (axis2.values[2] != axis2.values[2]) ||
								(sizeX != sizeX) || (sizeY != sizeY) || (sizeZ != sizeZ) || (r != r) || (g != g) || (b != b) || (theta1 != theta1) || (theta2 != theta2))) {  // Checking to see if there are no NAN values
								fprintf(outFile, "Group{\n children [\n Transform{\n translation %i %i %i \n  rotation %lf %lf %lf %lf \n  children [Transform{\n rotation %lf %lf %lf %lf \n scale %lf %lf %lf \n children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %lf %lf %lf \n transparency 0.1 }} \n geometry Sphere {radius 0.5}}]}]}]}\n",
									//x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
									x, y, z,
									axis2.values[0], axis2.values[1], axis2.values[2], -theta2,
									axis1.values[0], axis1.values[1], axis1.values[2], -theta1,
									sizeX, sizeY, sizeZ,
									r, g, b);						
							}

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

		void VolumeSkeletonizer::WriteSkeletonDirectionToVRMLFile(Volume * skeleton, Volume * cost, Vector3DFloat * skeletonDirections, string outputPath, bool surfaces, double curveSize) {
			int index;
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");
			srand( (unsigned)time( NULL ) );
			double r,g,b,colorCost;

			for(int z = 0; z < skeleton->getSizeZ(); z++) {
				for(int y = 0; y < skeleton->getSizeY(); y++) {
					for(int x = 0; x < skeleton->getSizeX(); x++) {
						index = skeleton->getIndex(x, y, z);
						if((skeleton->getDataAt(index) > 0) &&  !skeletonDirections[index].IsBadNormal()) {
							colorCost = cost->getDataAt(x, y, z);
							HueRB(colorCost, r, g, b);
							Vector3DFloat axis = skeletonDirections[index] ^ Vector3DFloat(1.0, 0.0, 0.0);
							axis.Normalize();
							double angle = -(skeletonDirections[index] * Vector3DFloat(1.0, 0.0, 0.0));
							angle = acos(angle);

							if(surfaces) {
								if((axis.values[0] == 0) && (axis.values[1] == 0) && (axis.values[2] == 0)) {
									fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f}} \n geometry Box {size 0.01 0.5 0.5}}]}]}\n",
										//x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
										x, y, z,
										r, g, b);
								} else {
									fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %f %f %f %f \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f}} \n geometry Box {size 0.01 0.5 0.5}}]}]}\n",
										//x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
										x, y, z,
										axis.values[0], axis.values[1], axis.values[2], angle,
										r, g, b);
								}
							} else {
								if((axis.values[0] == 0) && (axis.values[1] == 0) && (axis.values[2] == 0)) {
									fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f }} \n geometry Box {size 0.9 %f %f}}]}]}\n",
										//x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
										x, y, z,
										r, g, b,
										curveSize, curveSize);
								} else {
									fprintf(outFile, "Group{\n children[\n Transform{\n translation %i %i %i \n  rotation %f %f %f %f \n  children [\n Shape {\n appearance Appearance {\n material Material {emissiveColor %f %f %f}} \n geometry Box {size 0.9 %f %f}}]}]}\n",
										//x - MAX_GAUSSIAN_FILTER_RADIUS, y - MAX_GAUSSIAN_FILTER_RADIUS, z - MAX_GAUSSIAN_FILTER_RADIUS,
										x, y, z,
										axis.values[0], axis.values[1], axis.values[2], angle,
										r, g, b,
										curveSize, curveSize);
								}
							}
						
						}
					}
				}
			}

			fclose(outFile);
			delete outFile;
		}
		void VolumeSkeletonizer::WriteVolumeToVRMLFile(Volume * vol, string outputPath) {
			FILE * outFile = fopen(outputPath.c_str(), "wt");
			fprintf(outFile, "#VRML V2.0 utf8\n");
			fprintf(outFile, "DEF object0 Transform {\n children [\n Shape {\n geometry IndexedFaceSet {\n coord Coordinate { point [");

			for(int x = 0; x < vol->getSizeX(); x++) {
				for(int y = 0; y < vol->getSizeY(); y++) {
					for(int z = 0; z < vol->getSizeZ(); z++) {
						fprintf(outFile, "%i %i %i", x, y, z);
						if((x < vol->getSizeX()-1) || (y < vol->getSizeY()-1) || (z < vol->getSizeZ()-1)) {
							fprintf(outFile, ", ");
						}
					}
				}
			}

			fprintf(outFile, "]\n } \n creaseAngle 0.610865 \n coordIndex [");
			bool printComma = false;

			for(int x = 1; x < vol->getSizeX()-1; x++) {
				for(int y = 1; y < vol->getSizeY()-1; y++) {
					for(int z = 1; z < vol->getSizeZ()-1; z++) {
						for(int i = 0; i < 12; i++) {
							if( (vol->getDataAt(x, y, z) > 0) && 
								(vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][0][0], y + VOLUME_NEIGHBOR_FACES[i][0][1], z + VOLUME_NEIGHBOR_FACES[i][0][2]) > 0) &&
								(vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][1][0], y + VOLUME_NEIGHBOR_FACES[i][1][1], z + VOLUME_NEIGHBOR_FACES[i][1][2]) > 0) &&
								(vol->getDataAt(x + VOLUME_NEIGHBOR_FACES[i][2][0], y + VOLUME_NEIGHBOR_FACES[i][2][1], z + VOLUME_NEIGHBOR_FACES[i][2][2]) > 0)) {

								if(printComma) {
									fprintf(outFile, ",");
								}
								printComma = true;
								fprintf(outFile, "%i, %i, %i, %i, -1, %i, %i, %i, %i, -1",
									vol->getIndex(x, y, z),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][0][0], y + VOLUME_NEIGHBOR_FACES[i][0][1], z + VOLUME_NEIGHBOR_FACES[i][0][2]),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][1][0], y + VOLUME_NEIGHBOR_FACES[i][1][1], z + VOLUME_NEIGHBOR_FACES[i][1][2]),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][2][0], y + VOLUME_NEIGHBOR_FACES[i][2][1], z + VOLUME_NEIGHBOR_FACES[i][2][2]),
									vol->getIndex(x, y, z),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][2][0], y + VOLUME_NEIGHBOR_FACES[i][2][1], z + VOLUME_NEIGHBOR_FACES[i][2][2]),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][1][0], y + VOLUME_NEIGHBOR_FACES[i][1][1], z + VOLUME_NEIGHBOR_FACES[i][1][2]),
									vol->getIndex(x + VOLUME_NEIGHBOR_FACES[i][0][0], y + VOLUME_NEIGHBOR_FACES[i][0][1], z + VOLUME_NEIGHBOR_FACES[i][0][2]));
								
							}
						}						
					}
				}
			}
			fprintf(outFile, "]\n}\n}\n]\n}");
			fclose(outFile);
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
				for(unsigned int i = 0; i < bins[g].size(); i++) {
					temp->setDataAt(bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2], 1);
				}
				for(unsigned int i = 0; i < bins[g].size(); i++) {
					if(DiscreteMesh::IsVolumeBody(temp, bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2])) {
						cleanedSkel->setDataAt(bins[g][i].values[0], bins[g][i].values[1], bins[g][i].values[2], 0);
					}
				}
				delete temp;
			}

			return cleanedSkel;
		}

		Volume * VolumeSkeletonizer::FillCurveHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize) {
			Volume * holes = new Volume(originalSkeleton->getSizeX(),  originalSkeleton->getSizeY(), originalSkeleton->getSizeZ(), 0, 0, 0, originalSkeleton);
			VoxelSubtract(holes, thresholdedSkeleton);
			PruneCurves(holes, maxHoleSize);

			Volume * filledSkeleton = new Volume(originalSkeleton->getSizeX(),  originalSkeleton->getSizeY(), originalSkeleton->getSizeZ(), 0, 0, 0, originalSkeleton);
			VoxelSubtract(filledSkeleton, holes);
			delete holes;			
			return filledSkeleton;
		}

		Volume * VolumeSkeletonizer::FillSurfaceHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize) {
			Volume * holes = new Volume(originalSkeleton->getSizeX(),  originalSkeleton->getSizeY(), originalSkeleton->getSizeZ(), 0, 0, 0, originalSkeleton);
			VoxelSubtract(holes, thresholdedSkeleton);
			PruneSurfaces(holes, maxHoleSize);

			Volume * filledSkeleton = new Volume(originalSkeleton->getSizeX(),  originalSkeleton->getSizeY(), originalSkeleton->getSizeZ(), 0, 0, 0, originalSkeleton);
			VoxelSubtract(filledSkeleton, holes);
			delete holes;			
			return filledSkeleton;
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



		Volume * VolumeSkeletonizer::PerformImmersionSkeletonizationAndPruning(Volume * sourceVol, Volume * preserveVol, double startGray, double endGray, double stepSize, int smoothingIterations, int smoothingRadius, int minCurveSize, int minSurfaceSize, int maxCurveHole, int maxSurfaceHole, string outputPath, bool doPruning, double pointThreshold, double curveThreshold, double surfaceThreshold) {
			appTimeManager.PushCurrentTime();
			for(int i = 0; i < smoothingIterations; i++) {
				SmoothenVolume(sourceVol, startGray, endGray, smoothingRadius);
			}
			appTimeManager.PopAndDisplayTime("Smoothing : %f seconds!\n");
			Vector3DFloat * volumeGradient = NULL;
			EigenResults3D * volumeEigens;
			sourceVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			if(preserveVol != NULL) {
				preserveVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			}

			if(doPruning) {
				volumeGradient = GetVolumeGradient(sourceVol);			
			}

			Volume * nullVol = new Volume(sourceVol->getSizeX(), sourceVol->getSizeY(), sourceVol->getSizeZ());
			appTimeManager.PushCurrentTime();
			Volume * surfaceVol = GetImmersionThinning(sourceVol, preserveVol, startGray, endGray, stepSize, THINNING_CLASS_SURFACE_PRESERVATION);			
			appTimeManager.PopAndDisplayTime("Surface Thinning : %f seconds!\n");
			
			#ifdef SAVE_INTERMEDIATE_RESULTS
				surfaceVol->toMRCFile((char *)(outputPath + "-S-Pre-Prune-Pre-Erode.mrc").c_str());				
			#endif

			PruneSurfaces(surfaceVol, minSurfaceSize);

			appTimeManager.PushCurrentTime();
			if(doPruning) {
				#ifdef SAVE_INTERMEDIATE_RESULTS
					surfaceVol->toMRCFile((char *)(outputPath + "-S-Pre-Prune.mrc").c_str());				
					WriteVolumeToVRMLFile(surfaceVol, outputPath + "-S-Pre-Prune.wrl");
				#endif
				appTimeManager.PushCurrentTime();
				volumeEigens = GetEigenResults(surfaceVol, volumeGradient, gaussianFilterSurfaceRadius, surfaceRadius, true);
				appTimeManager.PopAndDisplayTime("  Getting Eigens : %f seconds!\n");

				appTimeManager.PushCurrentTime();
				Volume * prunedSurfaceVol = new Volume(surfaceVol->getSizeX(), surfaceVol->getSizeY(), surfaceVol->getSizeZ(), 0, 0, 0, surfaceVol);				
				appTimeManager.PopAndDisplayTime("  Getting Copy of surface : %f seconds!\n");


				appTimeManager.PushCurrentTime();
				PruneUsingStructureTensor(prunedSurfaceVol, sourceVol, preserveVol, volumeGradient, volumeEigens, gaussianFilterSurfaceRadius, surfaceThreshold, PRUNING_CLASS_PRUNE_SURFACES, outputPath + "-S");
				appTimeManager.PopAndDisplayTime("  Pruning: %f seconds!\n");

				appTimeManager.PushCurrentTime();
				delete [] volumeEigens;
				#ifdef SAVE_INTERMEDIATE_RESULTS
					prunedSurfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Prune.mrc").c_str());
				#endif

				delete surfaceVol;
				surfaceVol = prunedSurfaceVol;
				appTimeManager.PopAndDisplayTime("  Memory Cleanup: %f seconds!\n");

			}		

			PruneSurfaces(surfaceVol, minSurfaceSize);		
			appTimeManager.PopAndDisplayTime("Surface Pruning  : %f seconds!\n");

			#ifdef SAVE_INTERMEDIATE_RESULTS
				surfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Erosion.mrc").c_str());
			#endif

			Volume * cleanedSurfaceVol = GetJuSurfaceSkeleton(surfaceVol, nullVol, 0.5);		
			PruneSurfaces(cleanedSurfaceVol, minSurfaceSize);
			#ifdef SAVE_INTERMEDIATE_RESULTS
				cleanedSurfaceVol->toMRCFile((char *)(outputPath + "-S-Cleaned.mrc").c_str());
			#endif

			delete surfaceVol;
			surfaceVol = cleanedSurfaceVol;
			VoxelOr(surfaceVol, preserveVol);

			appTimeManager.PushCurrentTime();

			Volume * curveVol = GetImmersionThinning(sourceVol, surfaceVol, startGray, endGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
			appTimeManager.PopAndDisplayTime("Curve Thinning   : %f seconds!\n");

			#ifdef SAVE_INTERMEDIATE_RESULTS
				curveVol->toMRCFile((char *)(outputPath + "-C-Pre-Prune_Pre-Erode.mrc").c_str());
			#endif

			PruneCurves(curveVol, minCurveSize);
			VoxelBinarySubtract(curveVol, surfaceVol);

			appTimeManager.PushCurrentTime();
			if(doPruning) {				
				#ifdef SAVE_INTERMEDIATE_RESULTS
					curveVol->toMRCFile((char *)(outputPath + "-C-Pre-Prune.mrc").c_str());
				#endif

				volumeEigens = GetEigenResults(curveVol, volumeGradient, gaussianFilterCurveRadius, curveRadius, true);
				Volume * prunedCurveVol = new Volume(curveVol->getSizeX(), curveVol->getSizeY(), curveVol->getSizeZ(), 0, 0, 0, curveVol);
				PruneUsingStructureTensor(prunedCurveVol, sourceVol, preserveVol, volumeGradient, volumeEigens, gaussianFilterCurveRadius, curveThreshold, PRUNING_CLASS_PRUNE_CURVES, outputPath + "-C");
				delete [] volumeEigens;
				#ifdef SAVE_INTERMEDIATE_RESULTS
					prunedCurveVol->toMRCFile((char *)(outputPath + "-C-Post-Prune.mrc").c_str());
				#endif

				Volume * filledCurveVol = FillCurveHoles(prunedCurveVol, curveVol, maxCurveHole);
				#ifdef SAVE_INTERMEDIATE_RESULTS
					filledCurveVol->toMRCFile((char *)(outputPath + "-C-Post-Fill.mrc").c_str());
				#endif
				delete curveVol;
				delete prunedCurveVol;
				curveVol = filledCurveVol;

				
			}

			VoxelOr(curveVol, surfaceVol);
			PruneCurves(curveVol, minCurveSize);		
			appTimeManager.PopAndDisplayTime("Curve Pruning    : %f seconds!\n");
			#ifdef SAVE_INTERMEDIATE_RESULTS
				curveVol->toMRCFile((char *)(outputPath + "-C-Post-Erosion.mrc").c_str());
			#endif

			Volume * cleanedCurveVol = GetJuCurveSkeleton(curveVol, surfaceVol, 0.5, true);		
			PruneCurves(cleanedCurveVol, minCurveSize);		
			#ifdef SAVE_INTERMEDIATE_RESULTS
				cleanedCurveVol->toMRCFile((char *)(outputPath + "-C-Cleaned.mrc").c_str());
			#endif

			delete curveVol;
			curveVol = cleanedCurveVol;

			VoxelOr(curveVol, surfaceVol);
			#ifdef SAVE_INTERMEDIATE_RESULTS
				curveVol->toMRCFile((char *)(outputPath + "-SC.mrc").c_str());
			#endif
			
			// Removed as points will never be preserved.
			// Volume * pointVol = GetImmersionThinning(sourceVol, curveVol, startGray, endGray, stepSize, THINNING_CLASS_POINT_PRESERVATION);
			//if(doPruning) {
			//	pointVol->toMRCFile((char *)(outputPath + "-P-Pre-Prune.mrc").c_str());
			//	volumeEigens = GetEigenResults2(pointVol, volumeGradient, gaussianFilterPointRadius, pointRadius, true);
			//	PruneUsingStructureTensor(pointVol, sourceVol, volumeEigens, pointThreshold, PRUNING_CLASS_PRUNE_POINTS, outputPath + "-P");
			//	delete [] volumeEigens;
			//	pointVol->toMRCFile((char *)(outputPath + "-P-Post-Prune.mrc").c_str());
			//}

			//VoxelOr(pointVol, curveVol);

			delete surfaceVol;
			//delete curveVol;
			delete nullVol;
			delete [] volumeGradient;

			#ifdef SAVE_INTERMEDIATE_RESULTS
				curveVol->toOFFCells2((char *)(outputPath + "-SC.off").c_str());
			#endif

			sourceVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
			curveVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);			
			return curveVol;					
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
//			:TODO: why do we have to use namespace std, it is declared already
			std::list<int> cleanupIndices;
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
				printf("Threshold = %i - %i points\n", g, (int)bins[g].size());
				int iteration = 1;
				do { 
					printf("\tIteration : %i\n", iteration++);

					for(unsigned int i = 0; i < bins[g].size(); i++) {
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
					for(unsigned int i = 0; i < cleanupIndices.size(); i++) {
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

		Volume * VolumeSkeletonizer::PerformJuSkeletonization(Volume * imageVol, string outputPath, int minGray, int maxGray, int stepSize) {
			imageVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * compositeVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * deletedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			for(int threshold = maxGray; threshold >= minGray; threshold-= stepSize) {		
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



		Volume * VolumeSkeletonizer::PerformPureJuSkeletonization(Volume * imageVol, string outputPath, double threshold, int minCurveWidth, int minSurfaceWidth) {
			imageVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			
			Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
			Volume * surfaceVol;
			Volume * curveVol;
			Volume * topologyVol;		

			//printf("\t\t\tUSING THRESHOLD : %f\n", threshold);
			// Skeletonizing while preserving surface features curve features and topology
			surfaceVol = GetJuSurfaceSkeleton(imageVol, preservedVol, threshold);
			PruneSurfaces(surfaceVol, minSurfaceWidth);
			VoxelOr(preservedVol, surfaceVol);

			curveVol = VolumeSkeletonizer::GetJuCurveSkeleton(imageVol, preservedVol, threshold, true);
			VolumeSkeletonizer::PruneCurves(curveVol, minCurveWidth);
			VoxelOr(preservedVol, curveVol);

			topologyVol = VolumeSkeletonizer::GetJuTopologySkeleton(imageVol, preservedVol, threshold);

			imageVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);			
			topologyVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

			delete preservedVol;
			delete surfaceVol;
			delete curveVol;

			return topologyVol;
		}


		Volume * VolumeSkeletonizer::PerformSkeletonPruning(Volume * sourceVolume, Volume * sourceSkeleton, double curveThreshold, double surfaceThreshold, int minGray, int maxGray, string outputPath) {
			//double pointThreshold = curveThreshold;
			//printf("Performing Skeleton Pruning\n");
			//sourceVolume->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
			//sourceSkeleton->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

			//printf("Getting volume Eigens\n");
			//Vector3DFloat * volumeGradient = GetVolumeGradient(sourceVolume);
			//EigenResults3D * volumeEigens = GetEigenResults(sourceSkeleton, volumeGradient, gaussianFilterMaxRadius, MAX_GAUSSIAN_FILTER_RADIUS, true);

			//WriteEigenResultsToOFFFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.off");
			//WriteEigenResultsToVRMLFile(sourceVolume, new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ()), sourceSkeleton, volumeEigens, outputPath + "NoColorEigens.wrl", false);

			//Volume * curveScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			//Volume * pointScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			//Volume * surfaceScore = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			//

			//Volume * binarySkeleton;

			//Volume * preservedSkeleton = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			//VolumeDeltaAnalyzer * analyzer;
			//int pointCount, curveCount, surfaceCount, index, featurePointCount;
			//double cost, cost2;
			//Vector3DInt * pointList;
			//SkeletalCurve * featureList;
			//Vector3DFloat * skeletonDirections;
			//SkeletalCurve feature;

			//{ //Remove after debugging.. not needed at all only for visualization
			//	binarySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			//	binarySkeleton->threshold(139);			
			//	skeletonDirections = GetSkeletonDirection(binarySkeleton);
			//	WriteSkeletonDirectionToVRMLFile(binarySkeleton, skeletonDirections, outputPath + "-G-SkelDirections.wrl");

			//	delete binarySkeleton;
			//	delete [] skeletonDirections;
			//}


			//for(int grayValue = maxGray; grayValue >= minGray; grayValue-=5) {
			//	printf("Threshold :%i\n", grayValue);
			//	binarySkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			//	binarySkeleton->threshold(grayValue);			
			//	skeletonDirections = GetSkeletonDirection(binarySkeleton);

			//	analyzer = new VolumeDeltaAnalyzer(preservedSkeleton, binarySkeleton);
			//	analyzer->GetNewPoints(pointCount, pointList);

			//	for(int i = 0; i < pointCount; i++) {
			//		index = binarySkeleton->getIndex(pointList[i].values[0], pointList[i].values[1], pointList[i].values[2]);				
			//		cost = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_POINT);
			//		pointScore->setDataAt(index, max(cost, pointScore->getDataAt(index)));
			//		if(cost >= pointThreshold) {
			//			preservedSkeleton->setDataAt(pointList[i].values[0], pointList[i].values[1], pointList[i].values[2], 1);
			//		}
			//	}

			//	analyzer->GetNewCurves(curveCount, featureList);
			//	for(int i = 0; i < curveCount; i++) {
			//		feature = featureList[i];
			//		cost = 0;
			//		featurePointCount = 0;
			//		for(int j = 0; j < feature.points.size(); j++) {
			//			index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
			//			if(!(preservedSkeleton->getDataAt(index) > 0)) {
			//				cost2 = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_CURVE_BODY);
			//				curveScore->setDataAt(index, max(cost2, curveScore->getDataAt(index)));
			//				cost += cost2;
			//				featurePointCount++;	
			//			}
			//		}
			//		if(featurePointCount != 0) {
			//			cost = cost / (double)featurePointCount;
			//		} else {
			//			cost = 0;
			//			printf("                 zero cost!!!\n");
			//		}
			//	
			//		if(cost > curveThreshold) {
			//			for(int j = 0; j < feature.points.size(); j++) {
			//				index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
			//				preservedSkeleton->setDataAt(index, 1);							
			//			}
			//		}
			//	}
			//	delete [] featureList;


			//	analyzer->GetNewSurfaces(surfaceCount, featureList);
			//	for(int i = 0; i < surfaceCount; i++) {
			//		feature = featureList[i];
			//		cost = 0;
			//		featurePointCount = 0;
			//		for(int j = 0; j < feature.points.size(); j++) {
			//			index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
			//			if(!(preservedSkeleton->getDataAt(index) > 0)) {
			//				cost2 = GetVoxelCost(volumeEigens[index], skeletonDirections[index], VOXEL_CLASS_SURFACE_BODY);
			//				surfaceScore->setDataAt(index, max(cost2, surfaceScore->getDataAt(index)));
			//				cost += cost2;
			//				featurePointCount++;	
			//			}
			//		}
			//		if(featurePointCount != 0) {
			//			cost = cost / (double)featurePointCount;
			//		} else {
			//			cost = 0;
			//			//printf("                 zero cost!!!\n");
			//		}
			//	
			//		if(cost > surfaceThreshold) {
			//			for(int j = 0; j < feature.points.size(); j++) {
			//				index = binarySkeleton->getIndex(feature.points[j].values[0], feature.points[j].values[1], feature.points[j].values[2]);
			//				preservedSkeleton->setDataAt(index, 1);							
			//			}
			//		}
			//	}
			//	delete [] featureList;


			//	printf("%i surfaces, %i curves, %i points\n", surfaceCount, curveCount, pointCount);

			//	delete binarySkeleton;				
			//	delete [] skeletonDirections;
			//	delete analyzer;
			//	delete [] pointList;
			//}
			//
			//Volume * finalSkeleton = new Volume(sourceSkeleton->getSizeX(), sourceSkeleton->getSizeY(), sourceSkeleton->getSizeZ(), 0, 0, 0, sourceSkeleton);
			//finalSkeleton->applyMask(preservedSkeleton, 1.0, true);
			//			
			//WriteEigenResultsToVRMLFile(sourceVolume, curveScore, sourceSkeleton, volumeEigens, outputPath + "-C-Eigens.wrl", false);
			//WriteEigenResultsToVRMLFile(sourceVolume, surfaceScore, sourceSkeleton, volumeEigens, outputPath + "-S-Eigens.wrl", false);

			////pointScore->toMRCFile((char *)(outputPath + "-P-Score.mrc").c_str());
			//curveScore->toMRCFile((char *)(outputPath + "-C-Score.mrc").c_str());
			//surfaceScore->toMRCFile((char *)(outputPath + "-S-Score.mrc").c_str());
			////preservedSkeleton->toMRCFile((char *)(outputPath + "-G-BinarySkeleton.mrc").c_str());
			////preservedSkeleton->toOFFCells2((char *)(outputPath + "-G-BinarySkeleton.off").c_str());
			////finalSkeleton->toOFFCells2((char *)(outputPath + "-G-GraySkeleton.off").c_str());

			//delete pointScore;
			//delete curveScore;
			//delete surfaceScore;
			//delete preservedSkeleton;
			//delete [] volumeEigens;
			//delete [] volumeGradient;

			//return finalSkeleton;
			return NULL;

		}



		Volume * VolumeSkeletonizer::PerformAnisotropicSmoothingAxisAligned(Volume * sourceVolume, int xRadius, int yRadius, int zRadius) {
			Volume * smoothedVol = new Volume(sourceVolume->getSizeX(), sourceVolume->getSizeY(), sourceVolume->getSizeZ());
			ProbabilityDistribution3D dist;
			Combinatorics::GetAnisotropicDistributionAxisAligned(dist, xRadius, yRadius, zRadius);
			float val;
			
			for(int x = dist.radius; x < sourceVolume->getSizeX() - dist.radius; x++) {
				for(int y = dist.radius; y < sourceVolume->getSizeY() - dist.radius; y++) {
					for(int z = dist.radius; z < sourceVolume->getSizeZ() - dist.radius; z++) {
						val = 0;						
						for(int xx = -xRadius; xx <= xRadius; xx++) {
							for(int yy = -yRadius; yy <= yRadius; yy++) {
								for(int zz = -zRadius; zz <= zRadius; zz++) {
									val += (sourceVolume->getDataAt(x+xx, y+yy, z+zz) * dist.values[xx+dist.radius][yy+dist.radius][zz+dist.radius]);									
								}
							}
						}
						smoothedVol->setDataAt(x, y, z, val);
					}
				}
			}

			return smoothedVol;
		}
	}
}

#endif
