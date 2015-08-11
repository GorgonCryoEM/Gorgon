// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Originally written in python by Matthew Baker (mlbaker@gmail.com).. Rewritten in C++ by Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   C++ Implementation of SSEHunter


#ifndef PROTEINMORPH_SSE_HUNTER_H
#define PROTEINMORPH_SSE_HUNTER_H

#include <SkeletonMaker/volume.h>
#include "NonManifoldMesh.h"
#include <vector>
#include <algorithm>
#include <map>
#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>
#include <MathTools/CrossCorrelation.h>
#include <GraphMatch/PDBAtom.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <Gorgon/MeshRenderer.h>
#include <cmath>

//#define USE_TIME_MANAGER
#ifdef USE_TIME_MANAGER
	#include <Foundation/TimeManager.h>
	using wustl_mm::Foundation::TimeManager;
#endif

using namespace std;
using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::SkeletonMaker;
using wustl_mm::Visualization::NonManifoldMesh_Annotated;

namespace wustl_mm {
	namespace Protein_Morph {

		class AtomCoordinatesAndSize
		{
		public:
			float x;
			float y;
			float z;
			short size;
			AtomCoordinatesAndSize() { x = y = z = size = 0; }
			AtomCoordinatesAndSize(float x, float y, float z, short size) {
				this->x = x;
				this->y = y;
				this->z = z;
				this->size = size;
			}
		};

		bool compare_z(AtomCoordinatesAndSize left, AtomCoordinatesAndSize right)
		{
			return (left.z < right.z);
		}

		enum RadialProfileType {GAUSSIAN, GAUSSIAN_DIP, POLYNOMIAL};
		
		class SSEHunter{
		public:
			SSEHunter();
			~SSEHunter();
			
//			//TODO: Finish coding the geometry score portion of this function
//			map<unsigned long long, PDBAtom> GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold,
//															float correlationCoeff, float skeletonCoeff, float geometryCoeff,
//															RadialProfileType type = POLYNOMIAL, float deltaAltRadians=5*PI/180);

			
			
			void CreatePseudoAtoms(Volume * vol, float resolution, float threshold);
			int GetNumberOfPseudoAtoms();
			PDBAtom& GetPseudoAtom(int i);
			
			void SetCorrelationScores(Volume * vol, RadialProfileType type, float resolution, float deltaAltRadians);
			void SetSkeletonScores(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution);
//			//TODO: Finish coding SetGeometryScores
//			void SetGeometryScores(Volume * vol, float resolution, float threshold);
			vector< vector<float> > GetAtomDistances();
//			vector< vector<Vector3DInt> > GetNeighborhoodVoxels(Volume * vol, float threshold);
			vector<float> GetLocalDirectionalityScores(Volume * vol);
			
		private:
			void UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ);

			//Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
			float RadialProfile(float r, RadialProfileType type); //r in angstroms
			float RadialProfileGaussian(float r); // r in angstroms
			float RadialProfileGaussianDip(float r);// r in angstroms
			float RadialProfilePolynomial(float r);// r in angstroms
//			Volume* GetTemplateCylinder(int xsize, int ysize, int zsize, float alt, float az,
//						RadialProfileType type = POLYNOMIAL, float len=16.2, float apix_x=1, float apix_y=-1, float apix_z=-1);
		public:
			void ApplyTemplateCylinder(float cylData[], int xsize, int ysize, int zsize, int zFFTPadding, float alt, float az,
						RadialProfileType type = POLYNOMIAL, float len=16.2, float apix_x=1, bool reset=true, float apix_y=-1, float apix_z=-1);
//			Volume * GetTemplateHelix(double length, float apix, float resolution, int mapSize);
			void NormThresh(Volume& map, float thresh);
			Volume * HelixCorrelation(Volume* map_vol, RadialProfileType type = POLYNOMIAL, float length = 16.2, float deltaAltRadians = 5*PI/180,
						bool use_mcf = true, Volume* az_vol = NULL, Volume* alt_vol = NULL);
		private:
//			void NormalizeEdgeMean(Volume* vol);
//			void ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0=-1);

//			//Ross Coleman: modified from EMAN2 PointArray::pdb2mrc_by_summation by Wen Jian
//			Volume * AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms);

			
			
			vector<Vector3DInt> atomVolumePositions; // holds the i, j, k indices that give the voxel position of the pseudoatoms
			vector<PDBAtom> patoms; // pseudoatoms
			static const float max_radius; //for all r > max_radius, RadialProfile(r, {any type}) ~= 0

		};
		
		const float SSEHunter::max_radius = 14.0f;
		
		SSEHunter::SSEHunter() {
		}

		SSEHunter::~SSEHunter() {
		}		

//		map<unsigned long long, PDBAtom> SSEHunter::GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold,
//																   float correlationCoeff, float skeletonCoeff, float geometryCoeff,
//																   RadialProfileType type, float deltaAltRadians) {
//			cout << "GetScoredAtoms()\n";
//			CreatePseudoAtoms(vol, resolution, threshold);
//			SetSkeletonScores(vol, skeleton, resolution);
//			SetGeometryScores(vol, resolution, threshold);
//			SetCorrelationScores(vol, type, resolution, deltaAltRadians);
//			map<unsigned long long, PDBAtom> atomMap;
//			atomMap.clear();
//			for(unsigned int i = 0; i < patoms.size();  i++) {
//				patoms[i].SetTempFactor( patoms[i].GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
//				atomMap[patoms[i].GetHashKey()] = patoms[i];
//			}
//			return atomMap;
//
//		}
		
		// SSEHunter::CreatePseudoAtoms
		// threshold: the minimum density value that will be represented with a pseudoatom
		void SSEHunter::CreatePseudoAtoms(Volume * vol, float resolution, float threshold) {
			cout << "CreatePseudoAtoms()" << endl;
			Volume * tempVol = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol);
			patoms.clear();
			atomVolumePositions.clear();
			float rangeminX = -1.0f*resolution/vol->getSpacingX();
			float rangemaxX = resolution/vol->getSpacingX();
			float rangeminY = -1.0f*resolution/vol->getSpacingY();
			float rangemaxY = resolution/vol->getSpacingY();
			float rangeminZ = -1.0f*resolution/vol->getSpacingZ();
			float rangemaxZ = resolution/vol->getSpacingZ();			
			int mX, mY, mZ;
			double maxVal = tempVol->getMaxValuePosition(mX, mY, mZ);
			int i = 1;
			PDBAtom atom;
			while (maxVal >= threshold) {
				atom.SetSerial(i);
				atom.SetName("CA");
				atom.SetResName("GLY");
				atom.SetChainId('A');
				atom.SetResSeq(i);
				atom.SetPosition(Vector3DFloat(vol->getOriginX() + mX*vol->getSpacingX(), vol->getOriginY() + mY*vol->getSpacingY(), vol->getOriginZ() + mZ*vol->getSpacingZ()));
				atom.SetOccupancy(1.0f);
				atom.SetTempFactor(0.0f);
				atom.SetElement("S_00");
				atom.SetCharge("0");
				patoms.push_back(atom);
				atomVolumePositions.push_back(Vector3DInt(mX, mY, mZ));
				UpdateMap(tempVol, Vector3DInt(mX, mY, mZ), rangeminX, rangeminY, rangeminZ, rangemaxX, rangemaxY, rangemaxZ);
				maxVal = tempVol->getMaxValuePosition(mX, mY, mZ);
				i++;
				
			}
		}
		
		// SSEHunter::UpdateMap
		// called by SSEHunter::CreatePseudoAtoms after each pseudoatom is chosen
		void SSEHunter::UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ) {
			int rMinX = (int)round(rangeminX/2.0);
			int rMaxX = (int)(round(rangemaxX/2.0) + 1);
			int rMinY = (int)round(rangeminY/2.0);
			int rMaxY = (int)(round(rangemaxY/2.0) + 1);
			int rMinZ = (int)round(rangeminZ/2.0);
			int rMaxZ = (int)(round(rangemaxZ/2.0) + 1);
			float maxDistance = sqrt(3.0f * pow((float)max(max(abs(rMinX), abs(rMinY)), abs(rMinZ)), 2));
			
			float tempVal, value;
			float distance;
			
			for(int x = rMinX; x < rMaxX; x++){
				for(int y = rMinY; y < rMaxY; y++) {
					for(int z = rMinZ; z < rMaxZ; z++) {
						tempVal = vol->getDataAt(loc.X() + x, loc.Y() + y, loc.Z() + z);
						distance = sqrt((float)(x*x + y*y + z*z));
						if((x==0) && (y==0) && (z==0)) {
							value = 0;
						} else {
							value = tempVal*(distance / maxDistance);
						}
						vol->setDataAt(loc.X() + x, loc.Y() + y, loc.Z() + z, value);
					}
				}
			}
		}
		
		int SSEHunter::GetNumberOfPseudoAtoms() {
			return patoms.size();
		}
		
		PDBAtom& SSEHunter::GetPseudoAtom(int i) {
			return patoms[i];
		}

		void SSEHunter::SetSkeletonScores(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution) {
			cout << "SetSkeletonScores()\n";
//			float maxDistance = 4*sqrt(skeleton->scale[0]+skeleton->scale[1]+skeleton->scale[2]);//resolution;  // TODO: In EMAN1 the maximum distance is sqrt(3*4*4) voxels, we're using Angstroms here
			const unsigned int SCORE_RANGE = 4;
			const unsigned int MAX_DISTANCE_SQUARED = 3*SCORE_RANGE*SCORE_RANGE;
			Vector3DFloat skeletonOrigin = Vector3DFloat(skeleton->GetOriginX(), skeleton->GetOriginY(), skeleton->GetOriginZ());
			Vector3DFloat skeletonAtom;
			Vector3DFloat pAtomPosition;
			float score = 0;
			unsigned int count = 0;
			float maxscore = -1;
			float minscore = 1;
			for(unsigned int i = 0; i < patoms.size(); i++) {
				pAtomPosition = patoms[i].GetPosition();
				pAtomPosition -= skeletonOrigin;
				for (unsigned int n = 0; n < 3; n++)
					pAtomPosition[n] = pAtomPosition[n] * (1.0/skeleton->scale[n]);
				for (unsigned int j = 0; j < skeleton->vertices.size(); j++) {
					skeletonAtom = skeleton->vertices[j].position;
					Vector3DFloat d = skeletonAtom - pAtomPosition;
					float distance_squared = d.X()*d.X() + d.Y()*d.Y() + d.Z()*d.Z();
					if (abs(d.X()) <= SCORE_RANGE && abs(d.Y()) <= SCORE_RANGE && abs(d.Z()) <= SCORE_RANGE) { // 8x8x8 cubic search area
						double typeCost; //TODO: If a vertex is part of both the sheet skeleton and the helix skeleton, typeCost = 0;
						if (skeleton->IsSurfaceVertex(i)) { //TODO: Is this the same as testing if it's in the sheet skeleton
							typeCost = -1.0;
						} else { //TODO test whether in the helix skeleton
							typeCost = 1.0;
						}
						score += typeCost * (1.0 - distance_squared / MAX_DISTANCE_SQUARED);
						count++;
					}
				}
				if (count > 0)
					score /= count;

				if (score < minscore)
					minscore = score;
				if (score > maxscore)
					maxscore = score;

				patoms[i].SetSkeletonScore(score);
				score = 0;
				count = 0;
			}
			//Normalization (see main method of skeleton.C (with flag == 6) in EMAN1)
			for (unsigned int i = 0; i < patoms.size(); i++) {
				score = patoms[i].GetSkeletonScore();
				if (score > 0)
					score /= maxscore;
				else
					score /= -minscore;
				patoms[i].SetSkeletonScore(score);
			}
		}
		
		// SSEHunter::GetAtomDistances
		// patoms: the vector of pseudoatoms
		// returns a matrix of distances between pairs of pseudoatoms
		vector< vector<float> > SSEHunter::GetAtomDistances() {
			vector< vector<float> > distances;
			vector<float> atomDistances;
			for(unsigned int i = 0; i < patoms.size(); i++) {
				atomDistances.clear();
				for(unsigned int j = 0; j < patoms.size(); j++) {
					atomDistances.push_back( (patoms[i].GetPosition() - patoms[j].GetPosition()).Length() );
				}
				distances.push_back(atomDistances);
			}
			return distances;
		}
		
//		// SSEHunter::GetNeighborhoodVoxels
//		// patoms: pseudoatoms
//		// atomVolumePositions: the i,j,k indices for the location of the pseudoatoms in vol
//		vector< vector<Vector3DInt> > SSEHunter::GetNeighborhoodVoxels(Volume * vol, float threshold) {
//			int kernelWidthX = (int)round(5.0/vol->getSpacingX());
//			int kernelWidthY = (int)round(5.0/vol->getSpacingY());
//			int kernelWidthZ = (int)round(5.0/vol->getSpacingZ());
//
//			vector< vector<Vector3DInt> > voxels;
//			vector<Vector3DInt> voxelCoords;
//			Vector3DInt voxel;
//
//			for(unsigned int i = 0; i < atomVolumePositions.size(); i++) {
//				voxelCoords.clear();
//
//				for(int addX = -kernelWidthX; addX < kernelWidthX; addX++) {
//					for(int addY = -kernelWidthY; addY < kernelWidthY; addY++) {
//						for(int addZ = -kernelWidthZ; addZ < kernelWidthZ; addZ++) {
//							voxel = Vector3DInt(addX + atomVolumePositions[i].X(), addY + atomVolumePositions[i].Y(), addZ + atomVolumePositions[i].Z());
//							if (vol->getDataAt(voxel.X(), voxel.Y(), voxel.Z()) > threshold) {
//								voxelCoords.push_back(voxel);
//							}
//						}
//					}
//				}
//				voxels.push_back(voxelCoords);
//			}
//			return voxels;
//		}

		vector<float> SSEHunter::GetLocalDirectionalityScores(Volume * vol) {
			Volume * tempVol = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol);
			double minVal = tempVol->getMin();
			int offset = MAX_GAUSSIAN_FILTER_RADIUS;
			
			tempVol->pad(offset, minVal);
			Volume * maskVol = new Volume(tempVol->getSizeX(), tempVol->getSizeY(), tempVol->getSizeZ());
			for(unsigned int i = 0; i < atomVolumePositions.size(); i++) {
				maskVol->setDataAt(atomVolumePositions[i].X() + offset, atomVolumePositions[i].Y() + offset, atomVolumePositions[i].Z() + offset, 1.0);
			}
			
			int kernelWidth = min(offset, (int)round(2.0/min(min(vol->getSpacingX(), vol->getSpacingY()), vol->getSpacingZ())));
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(kernelWidth, kernelWidth, kernelWidth, kernelWidth);
			
			Vector3DFloat * volumeGradient = skeletonizer->GetVolumeGradient(tempVol);
			EigenResults3D * eigens = skeletonizer->GetEigenResults(maskVol, volumeGradient, skeletonizer->gaussianFilterPointRadius, kernelWidth, true);
			
			float minScore = 10, maxScore = -10;
			vector<float> aspectRatios;
			int index;
			float dx, dy, dz, x, y, z;
			float score;
			for(unsigned int i = 0; i < atomVolumePositions.size(); i++) {
				index = maskVol->getIndex(atomVolumePositions[i].X() + offset, atomVolumePositions[i].Y() + offset, atomVolumePositions[i].Z() + offset);

				x = eigens[index].values[0];
				y = eigens[index].values[1];
				z = eigens[index].values[2];
				if(isZero(x)){
					dx = 1; dy = 1; dz = 1;					
				} else if(isZero(y)) {
					dx = 0; dy = 1; dz = 0;
				} else {
					dx = y/x; dy = z/y; dz = z/x;
				}
 				
				//score = (1 - dz) * (dx - dy);
				score = dx - dy;
				minScore = min(score, minScore);
				maxScore = max(score, maxScore);
//				printf("%f, %f, %f, %f, %f\n ", patoms[i].GetTempFactor(), eigens[index].values[0], eigens[index].values[1], eigens[index].values[2], score);
				aspectRatios.push_back(score);
			}
			
			for(unsigned int i = 0; i < aspectRatios.size(); i++) {
				aspectRatios[i] = (2.0f * (aspectRatios[i] - minScore) / (maxScore - minScore)) - 1.0f;
//				printf("%f\n", aspectRatios[i]);
			}
			
			delete [] eigens;
			delete [] volumeGradient;
			delete skeletonizer;			
			delete maskVol;			
			delete tempVol;	
			return aspectRatios;
		}	

//		//TODO: finish this function
//		void SSEHunter::SetGeometryScores(Volume * vol, float resolution, float threshold) {
//			cout << "SetGeometryScores()\n";
//			vector< vector<float> > distances = GetAtomDistances();
//			//vector< vector<Vector3DInt> > neighbors = GetNeighborhoodVoxels(vol, threshold);
//			vector<float> localDirectionalityScore = GetLocalDirectionalityScores(vol);
//			vector<float> geometryScore;
//
//			for(unsigned int i = 0; i < patoms.size(); i++) {
//				geometryScore.push_back(localDirectionalityScore[i]);
//			}
//
//
//			for(unsigned int i = 0; i < geometryScore.size(); i++) {
//				patoms[i].SetGeometryScore(geometryScore[i]);
//			}
//
//		}

		float SSEHunter::RadialProfile(float r, RadialProfileType type) // r in angstroms
		{
			float ret;
			switch (type) {
				case GAUSSIAN:
					ret = RadialProfileGaussian(r);
					break;
				case GAUSSIAN_DIP:
					ret = RadialProfileGaussianDip(r);
					break;
				case POLYNOMIAL:
					ret = RadialProfilePolynomial(r);
					break;
				default:
					//TODO: throw an exception
					ret = RadialProfilePolynomial(r);
			}
			return ret;
		}

		float SSEHunter::RadialProfileGaussian(float r) // r in angstroms
		{
			if (r > 7)
				return 0; //for speed
			r /= 2;
			return exp(-r * r);
		}

		float SSEHunter::RadialProfileGaussianDip(float r) //r in angstroms
		{
			if (r > 14)
				return 0; //for speed
			// pure Gaussian falloff + negative dip, so mean is 0
			r /= 4;
			return (1 - r * r) * exp(-r * r);
		}

		// polynomial fitting to the radial profile of real helix density
		float SSEHunter::RadialProfilePolynomial(float r) //r in angstroms
		{
			//iterations ==> rounding is problematic with float types if
			// 15 < r < 20 and really bad if 20 < r < 30, so use double
			double ret;

			// polynomial fitting to the radial profile of real helix density
			// f=a0*x^n+a1+x^(n-1)+ ... +a[n-1]*x+an
			//float an[11]={2.847024584977009e-10,-3.063997224364090e-08,1.418801040660860e-06,-3.678676414383996e-05,5.804871622801710e-04,-5.640340018430164e-03,3.208802421493864e-02,-9.068475313823952e-02,7.097329559749284e-02,-9.993347339658298e-02,1.000000000000000e+00};

			// now the fitting to the original profile
			if (r >= 12.2)
				return 0; //We don't want that part of the polynomial --> goes way below zero
			static float an[15] = { -3.9185246832229140e-16,
					3.3957205298900993e-14, 2.0343351971222658e-12,
					-4.4935965816879751e-10, 3.0668169835080933e-08,
					-1.1904544689091790e-06, 2.9753088549414953e-05,
					-4.9802112876220150e-04, 5.5900917825309360e-03,
					-4.0823714462925299e-02, 1.8021733669148599e-01,
					-4.0992557296268717e-01, 3.3980328566901458e-01,
					-3.6062024812411908e-01, 1.0000000000000000e+00 };

			ret = an[0];
			for (int i = 1; i < 15; i++) {
				ret = ret * r + an[i];
			}
			return ret;
		}

//		//synthesize cylinder that resembles the density seen from an alpha helix
//		//len in Angstroms, default to 3 turns
//		//The cylinder will be centered in the Volume
//		Volume* SSEHunter::GetTemplateCylinder(int xsize, int ysize, int zsize, float alt, float az,
//				RadialProfileType type, float len, float apix_x, float apix_y, float apix_z) {
//			/*
//			 *[ x' ]     [1      0       0    ] [ cos(az)  sin(az)  0][ x ]
//			 *[ y' ]  =  [0   cos(al)  sin(al)] [-sin(az)  cos(az)  0][ y ]
//			 *[ z' ]     [0  -sin(al)  cos(al)] [    0       0      1][ z ]
//			 */
//			if (apix_x <= 0) {
//				apix_x = abs(apix_x);
//			}
//			if (apix_y <= 0) {
//				apix_y = apix_x;
//			}
//			if (apix_z <=0) {
//				apix_z = apix_x;
//			}
//			Volume * cyl = new Volume(xsize, ysize, zsize);
//			cyl->setSpacing(apix_x, apix_y, apix_z);
//
//			float half_xsize = xsize/2.0;
//			float half_ysize = ysize/2.0;
//			float half_zsize = zsize/2.0;
//			float cos_alt = cos(alt);
//			float sin_alt = sin(alt);
//			float cos_az = cos(az);
//			float sin_az = sin(az);
//			float radius;
//			float x, y, z;
//			float xprime, yprime, zprime;
//
//			for (int i = 0; i < xsize; i++) {
//				for (int j = 0; j < ysize; j++) {
//					for (int k = 0; k < zsize; k++) {
//						x = apix_x*(i-half_xsize);
//						y = apix_y*(j-half_ysize);
//						z = apix_z*(k-half_zsize);
//						zprime =  (x*sin_az-y*cos_az) * sin_alt + z*cos_alt;
//						if ((2*zprime > -len ) && (2*zprime < len)) {
//							xprime =   x*cos_az+y*sin_az;
//							yprime = (-x*sin_az+y*cos_az) * cos_alt + z*sin_alt;
//							radius = sqrt(xprime*xprime+yprime*yprime);
//							cyl->setDataAt( i, j, k, RadialProfile(radius, type) );
//						} else {
//							cyl->setDataAt( i, j, k, 0);
//						}
//					}
//				}
//			}
//
//			return cyl;
//
//		}


		//cylinder that resembles the density seen from an alpha helix
		//The cylinder will be centered in the float array
		//cylData: must be allocated with malloc rather than new, because FFTW3 expects that
		//zFFTPadding; FFTW3 requires the fastest changing index to be padded, z (not x) is currently the fast changing index in Gorgon
		//len in Angstroms, default to 3 turns
		//Note: Working with float arrays is MUCH faster than working with Volume objects, so this is used instead of the older GetTemplateCylinder() method
		void SSEHunter::ApplyTemplateCylinder(float* cylData, int xsize, int ysize, int zsize, int zFFTPadding, float alt, float az,
											RadialProfileType type, float len, float apix_x, bool reset, float apix_y, float apix_z) {
			/*
			 *[ x' ]     [1      0       0    ] [ cos(az)  sin(az)  0][ x ]
			 *[ y' ]  =  [0   cos(al)  sin(al)] [-sin(az)  cos(az)  0][ y ]
			 *[ z' ]     [0  -sin(al)  cos(al)] [    0       0      1][ z ]
			 */
			if (apix_x <= 0) {
				apix_x = abs(apix_x);
			}
			if (apix_y <= 0) {
				apix_y = apix_x;
			}
			if (apix_z <=0) {
				apix_z = apix_x;
			}

			if (reset) {
				for (int i=0; i<xsize*ysize*zsize; i++) {
					cylData[i]=0;
				}
			}

			int array_size = xsize*ysize*(zsize+zFFTPadding);
			//Voxel in the 3D array that is the center of the cylinder
			int i0 = xsize/2;
			int j0 = ysize/2;
			int k0 = zsize/2;

			//The radial profile of the cylinder ~=0 for r > this->max_radius
			//Cylinders of all orientations will fit inside a cube determined by cylinder length and max_radius
			float cube_half_length = (max_radius < len ? len : max_radius);
			int i_half_len = ceil( cube_half_length / apix_x);
			int j_half_len = ceil( cube_half_length / apix_y);
			int k_half_len = ceil( cube_half_length / apix_z);

			float cos_alt = cos(alt);
			float sin_alt = sin(alt);
			float cos_az = cos(az);
			float sin_az = sin(az);
			float radius;
			float x, y, z;
			float xprime, yprime, zprime;
			int ix;

			for (int i = -i_half_len; i <= i_half_len; i++) {
				for (int j = -j_half_len; j <= j_half_len; j++) {
					for (int k = -k_half_len; k <= k_half_len; k++) {
						ix = (k+k0) + ( (j+j0)+ysize*(i+i0) )*(zsize+zFFTPadding);
						//The array might be too small to hold the cube (in Angstroms) that contains all cylinder orientations
						if (ix >=0 && ix < array_size) {
							x = apix_x*i;
							y = apix_y*j;
							z = apix_z*k;
							zprime =  (x*sin_az-y*cos_az) * sin_alt + z*cos_alt;
							if ((2*zprime > -len ) && (2*zprime < len)) {
								xprime =   x*cos_az+y*sin_az;
								yprime = (-x*sin_az+y*cos_az) * cos_alt + z*sin_alt;
								radius = sqrt(xprime*xprime+yprime*yprime);
								/* Cylinder.C modelHelix() in EMAN1 only applies the radial profile if radius is less than an integer max radius.
								 * However, that max radius is in pixels whereas the radius is in Angstroms, so they shouldn't be compared.
								 * Also, this results in a cylinder that has more of an octagonal outer radius rather than a circular outer radius.
								 * Instead, we set a max radius inside the RadialProfile functions.
								 */
								cylData[ix] = RadialProfile(radius, type);
							} else {
								cylData[ix] = 0;
							}
						}
					}
				}
			}
		}
		
		
//		//TODO: TEST
//		//While a good idea for accuracy, we would have to rotate the template helix around its axis, adding another degree of freedom and HelixCorrelation() already takes minutes
//		Volume * SSEHunter::GetTemplateHelix(double length, float apix, float resolution, int mapSize)
//		{
//			vector< AtomCoordinatesAndSize > atoms;
//
//			AtomCoordinatesAndSize nitrogen, c_alpha, carbon, oxygen;
//			// Generating PDB Atoms
//			for (int j=0; j <= round(length/1.54); j++)
//			{
//				nitrogen = AtomCoordinatesAndSize( 1.6*cos( (0  + 100*j) * PI/180.0), 1.6*sin( (0  + 100*j) * PI/180.0), 1.54*j + 0.00, 7.0 );
//				c_alpha  = AtomCoordinatesAndSize( 2.3*cos( (28 + 100*j) * PI/180.0), 2.3*sin( (28 + 100*j) * PI/180.0), 1.54*j + 0.83, 6.0 );
//				carbon   = AtomCoordinatesAndSize( 2.0*cos( (61 + 100*j) * PI/180.0), 2.0*sin( (61 + 100*j) * PI/180.0), 1.54*j + 1.70, 6.0 );
//				oxygen   = AtomCoordinatesAndSize( 2.0*cos( (61 + 100*j) * PI/180.0), 2.0*sin( (61 + 100*j) * PI/180.0), 1.54*j + 3.09, 8.0 );
//				atoms.push_back(nitrogen);
//				atoms.push_back(c_alpha);
//				atoms.push_back(carbon);
//				atoms.push_back(oxygen);
//			}
//
//			// TODO: check if the cast for mapSize is the way to go
//			Volume* helixVolume = AtomsToVolumeBySummation(static_cast<int>(mapSize), apix, resolution, atoms);
//			NormalizeEdgeMean(helixVolume);
//			Vector3DFloat cm = helixVolume->getCenterOfMass();
//			helixVolume->setOrigin(cm.X(), cm.Y(), cm.Z());
//			ApplyPolynomialProfileToHelix(helixVolume, length);
//			return helixVolume;
//
//			//Generating volume
//			//mrcHelix=EMData()
////#	mrcHelix=points.pdb2mrc_by_summation(box,apix,res)
////#	mrcHelix.process_inplace("normalize.edgemean")
////#	mrcHelix.process_inplace("xform.centerofmass")
////#	aout=mrcHelix.copy()
////#	aout.to_zero()
////#	mrcHelix.process_inplace("filter.highpass.gauss",{"apix":apix,"cutoff_freq":100})
////
////#	for i in range(box):
////#		r = Region(0,0,i,box,box,1)
////#		slice1 = mrcHelix.get_clip(r)
////#		thresh = slice1.process("threshold.binary", {"value":10})
////#		neg = thresh.process("mask.addshells",{"nshells":2})
////#		real_neg = neg-thresh
////#		real_neg.mult(-10)
////#		pos = neg.process("mask.addshells",{"nshells":1})
////#		real_pos = pos-neg
////#		real_pos.mult(10)
////#		solution = slice1.copy()
////#		solution.mult(thresh)
////#		solution += real_neg
////#		solution += real_pos
////#		aout.insert_clip(solution,[0,0,i])
////#	aout.write_image("helix.mrc")
////#	return aout
////#	mrcHelix.write_image("helix.mrc")
//
//		}
		

//		// TODO: Test this function!!!!
//		Volume * SSEHunter::AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms)
//		{
//		#ifdef DEBUG
//			printf("SSEHunter::AtomsToVolumeBySummation(): %d points\tmapsize = %4d\tapix = %g\tres = %g\n",atoms.size(),map_size, apix, res);
//		#endif
//			double gauss_real_width = res / (PI);	// in Angstrom, res is in Angstrom
//			//if ( gauss_real_width < apix) LOGERR("PointArray::projection_by_summation(): apix(%g) is too large for resolution (%g Angstrom in Fourier space) with %g pixels of 1/e half width", apix, res, gauss_real_width);
//
//			double min_table_val = 1e-7;
//			double max_table_x = sqrt(-log(min_table_val));	// for exp(-x*x)
//
//			double table_step_size = 0.001;	// number of steps for each pixel
//			double inv_table_step_size = 1.0 / table_step_size;
//			int table_size = int (max_table_x * gauss_real_width / (apix * table_step_size) * 1.25);
//			vector<double> table;
//			table.resize(table_size);
//			//double *table = (double *) malloc(sizeof(double) * table_size);
//			for (int i = 0; i < table_size; i++) {
//				double x = -i * table_step_size * apix / gauss_real_width;
//				table[i] = exp(-x * x);
//			}
//
//			int gbox = int (max_table_x * gauss_real_width / apix);	// local box half size in pixels to consider for each point
//			if (gbox <= 0)
//				gbox = 1;
//
//
//
//		//	sort_by_axis(2);			// sort by Z-axis
//			sort(atoms.begin(), atoms.end(), compare_z);
//
//
//
//			Volume *map = new Volume(map_size, map_size, map_size);
//			map->fill(0);
//		//	float *pd = map->get_data();
//			for ( size_t s = 0; s < atoms.size(); ++s) {
//				double xc = atoms[s].x / apix + map_size / 2;
//				double yc = atoms[s].y / apix + map_size / 2;
//				double zc = atoms[s].z / apix + map_size / 2;
//				double fval = atoms[s].size;
//				int imin = int (xc) - gbox, imax = int (xc) + gbox;
//				int jmin = int (yc) - gbox, jmax = int (yc) + gbox;
//				int kmin = int (zc) - gbox, kmax = int (zc) + gbox;
//				if (imin < 0)
//					imin = 0;
//				if (jmin < 0)
//					jmin = 0;
//				if (kmin < 0)
//					kmin = 0;
//				if (imax > map_size)
//					imax = map_size;
//				if (jmax > map_size)
//					jmax = map_size;
//				if (kmax > map_size)
//					kmax = map_size;
//
//				for (int k = kmin; k < kmax; k++) {
//					size_t table_index_z = size_t (fabs(k - zc) * inv_table_step_size);
//					if ( table_index_z >= table.size() ) continue;
//					double zval = table[table_index_z];
//		//			size_t pd_index_z = k * map_size * map_size;
//					for (int j = jmin; j < jmax; j++) {
//						size_t table_index_y = size_t (fabs(j - yc) * inv_table_step_size);
//						if ( table_index_y >= table.size() ) continue;
//						double yval = table[table_index_y];
//		//				size_t pd_index = pd_index_z + j * map_size + imin;
//						for (int i = imin; i < imax; i++) // , pd_index++)
//						{
//							size_t table_index_x = size_t (fabs(i - xc) * inv_table_step_size);
//							if ( table_index_x >= table.size() ) continue;
//							double xval = table[table_index_x];
//		//					pd[pd_index] += (float) (fval * zval * yval * xval);
//							double val = (fval * zval * yval * xval) + map->getDataAt(i,j,k);
//							map->setDataAt(i,j,k, val);
//						}
//					}
//				}
//			}
//			//for(int i=0; i<map_size*map_size; i++) pd[i]/=sqrt(PI);
//		//	map->update();
//			map->setSpacing(apix,apix,apix);
//			map->setOrigin( -map_size/2*apix, -map_size/2*apix, -map_size/2*apix );
//
//			return map;
//		}

//		//TODO: TEST
//		void SSEHunter::NormalizeEdgeMean(Volume* vol)
//		{
//			int nx = vol->getSizeX();
//			int ny = vol->getSizeY();
//			int nz = vol->getSizeZ();
//			int N = nx*ny*nz;
//
//			float edge_mean = vol->getEdgeMean();
//			float std_dev = vol->getStdDev();
//			float val;
//
//			for (int i = 0; i < N; i++)
//			{
//				val = (vol->getDataAt(i)- edge_mean) / std_dev;
//				vol->setDataAt(i, val);
//			}
//		}

//		//TODO: TEST
//		void SSEHunter::ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0)
//		{
//			Volume* vol = in;
//			int nx = vol->getSizeX();
//			int ny = vol->getSizeY();
//			int nz = vol->getSizeZ();
//			float apix_x = vol->getSpacingX(); //TODO: Ask Matt if I correctly handled cases where apix_x != apix_y or apix_x != apix_z
//			float apix_y = vol->getSpacingY();
//			float apix_z = vol->getSpacingZ();
//
//			if (z0 < 0 || z0 >= nz)
//				z0 = nz / 2;
//
//			int z_start = floor( ( z0 - 0.5*lengthAngstroms/apix_z ) + 0.5);
//			int z_stop = floor( ( z0 + 0.5*lengthAngstroms/apix_z ) + 0.5);
//
//			double rho_x_sum, rho_y_sum, rho_sum;
//			double x_cm, y_cm;
//			double radius;
//			float val;
//
//			for (int k = 0; k < nz; k++) //taking slices along z axis
//			{
//				rho_x_sum = rho_y_sum = rho_sum = 0; //Set to zero for a new slice
//
//				if (k >= z_start && k <= z_stop)
//				//Apply the radial profile only between z_start and z_stop on the z axis
//				{
//					//Calculating CM for the slice...
//					for (int j = 0; j < ny; j++)
//					{
//						for (int i = 0; i < nx; i++)
//						{
//							val = vol->getDataAt(i,j,k);
//							rho_x_sum += val*i;
//							rho_y_sum += val*j;
//							rho_sum += val;
//						}
//					}
//					if (rho_sum != 0) //If there was net mass was zero, we can't find the CM, so we'll just leave the slice as is
//					{
//						x_cm = rho_x_sum/rho_sum;
//						y_cm = rho_y_sum/rho_sum;
//
//						//Applying radial profile...
//						for (int j=0; j<ny;j++)
//						{
//							for (int i=0;i<nx;i++)
//							{
//								radius = hypot( (i-x_cm)*apix_x, (j-y_cm)*apix_y );
//								vol->setDataAt(i,j,k, RadialProfilePolynomial((float)radius) );
//							}
//						}
//					}
//				}
//				else //k < z_start || k > z_stop
//				//Clear the map, setting the density to zero everywhere else.
//				{
//					for (int j=0; j<ny; j++)
//						for(int i=0; i<nx; i++)
//						{
//							vol->setDataAt(i,j,k, 0);
//						}
//				}
//			}
//		}

		void SSEHunter::NormThresh(Volume& map, float thresh) {
			int	nx = map.getSizeX();
			int ny = map.getSizeY();
			int nz = map.getSizeZ();
			float norm = map.getMax()-thresh;
			float val;

			for (int i = 0; i < nx; i++) {
				for (int j = 0; j < ny; j++) {
					for (int k = 0; k < nz; k++) {
						val = map.getDataAt(i,j,k);
						val = (val > thresh ? (val - thresh)/norm : 0);
						map.setDataAt(i,j,k,val);
					}
				}
			}
		}

		Volume * SSEHunter::HelixCorrelation(Volume* model, RadialProfileType type, float length,
												 float deltaAltRadians, bool use_mcf, Volume* az_vol, Volume* alt_vol) {
			cout << "HelixCorrelation()\n";

#ifdef USE_TIME_MANAGER
			TimeManager timer;
			int total_timer = timer.StartStopWatch();
			timer.PauseStopWatch(total_timer);
			int setup_timer = timer.StartStopWatch();
			timer.PauseStopWatch(setup_timer);
			int cylinder_creation_timer = timer.StartStopWatch();
			timer.PauseStopWatch(cylinder_creation_timer);
			int ccf_timer = timer.StartStopWatch();
			timer.PauseStopWatch(ccf_timer);
			int bestCCF_timer = timer.StartStopWatch();
			timer.PauseStopWatch(bestCCF_timer);
			
			timer.ResumeStopWatch(total_timer);
			timer.ResumeStopWatch(setup_timer);
#endif
			
			// TODO: Make it work with helices
			int nx = model->getSizeX();
			int ny = model->getSizeY();
			int nz = model->getSizeZ();
			int fftPaddingFastIx = nz % 2 ? 1 : 2;
			int array_size = nx*ny*(nz+fftPaddingFastIx);
			int N = nx*ny*nz;
			
			float* ccf = (float*) malloc( sizeof(float)*nx*ny*(nz+fftPaddingFastIx) );
			float* best_ccf = (float*) malloc( sizeof(float)*nx*ny*(nz+fftPaddingFastIx) );
			for (int i=0; i<array_size; i++) {
				best_ccf[i]=0;
			}

			Volume * bestCCF = new Volume(nx, ny, nz); // This is the returned volume
			float orig_x = model->getOriginX();
			float orig_y = model->getOriginY();
			float orig_z = model->getOriginZ();
			float apix_x = model->getSpacingX();
			float apix_y = model->getSpacingY();
			float apix_z = model->getSpacingZ();

			bestCCF->setOrigin(orig_x, orig_y, orig_z);
			bestCCF->setSpacing(apix_x, apix_y, apix_z);
			float* best_alt = NULL;
			float* best_az = NULL;
			
			if (az_vol) {
				best_alt = (float*) malloc( sizeof(float)*nx*ny*(nz+fftPaddingFastIx) );
				for (int i=0; i<array_size; i++) {
					best_alt[i]=0;
				}
				az_vol->setOrigin(orig_x, orig_y, orig_z);
				az_vol->setSpacing(apix_x, apix_y, apix_z);
			}
			if (alt_vol) {
				best_az = (float*) malloc( sizeof(float)*nx*ny*(nz+fftPaddingFastIx) );
				for (int i=0; i<array_size; i++) {
					best_az[i] = 0;
				}
				alt_vol->setOrigin(orig_x, orig_y, orig_z);
				alt_vol->setSpacing(apix_x, apix_y, apix_z);
			}
			
			float* cyl = (float*) malloc( sizeof(float)*nx*ny*nz );
			for (int i=0; i<nx*ny*nz; i++) {
				cyl[i] = 0;
			}

			Volume model_copy(*model); //get a copy before doing NormThresh()
			NormThresh( model_copy, model_copy.getMean() + 2*model_copy.getStdDev() );
			float* density = model_copy.getArrayCopy(0, 0, fftPaddingFastIx, 0);
			fftInPlace(density, nz, ny, nx);


			float val;
			float c1, c2, d1, d2;
			/* If alt=az=0, make the cylinder's axis be <0,0,1>, the k_hat unit vector
			 * Then, alt is angle from z axis to cylinder's axis and (az + pi) is angle from x axis to projection of cyl axis on xy plane
			 * Because of lengthwise symmetry, all orientations of a cylinder lie on a hemisphere: alt in [0,pi/2] and az in [0, 2*pi]
			 * We want delta(surface area) to be constant... for differentials dA = r*d(alt)*r*sin(alt)*d(az)=r^2*sin(alt)*d(alt)*d(az)
			 * Since r = 1, delta_A ~= sin(alt)*delta_alt*delta_az
			 * At alt=pi/4, make delta_az == delta_alt ==> sin(pi/4)*delta_alt*delta_alt = sin(alt)*delta_alt*delta_az
			 * Thus, delta_az = (delta_alt/sqrt(2))/sin(alt);
			 */
			deltaAltRadians = abs(deltaAltRadians);
			float K = deltaAltRadians/sqrt(2.0);
#ifdef USE_TIME_MANAGER
			timer.PauseStopWatch(setup_timer);
#endif
			cout << "0 <= alt < " << 90+180/PI*deltaAltRadians/2.0 << ": ";
			for (float alt = 1E-12; alt < PI/2.0 + deltaAltRadians/2.0; alt += deltaAltRadians) { // the "+ deltaAltRadians/2.0" as in EMAN1
			
				float deltaAzRadians = K/sin(alt);
				printf("%5.1f\t", alt*180/PI);
				cout << flush;
				for (float az = 0; az < 2*PI + deltaAzRadians/2.0; az += deltaAzRadians) { // the "+ deltaAzRadians/2.0" as in EMAN1

#ifdef USE_TIME_MANAGER
					timer.ResumeStopWatch(cylinder_creation_timer);
#endif
					
					ApplyTemplateCylinder(cyl, nx, ny, nz, 0, alt, az, type, length, apix_x, false, apix_y, apix_z);

#ifdef USE_TIME_MANAGER
					timer.PauseStopWatch(cylinder_creation_timer);
					timer.ResumeStopWatch(ccf_timer);
#endif

					fftOutOfPlace(cyl, ccf, nz, ny, nx);
					if (use_mcf) {
						//Mutual Fourier Transform: product in Fourier space used to calculate MCF (mutual correlation function)
						mftInPlace(ccf, density, array_size);
					}
					else {

						//cross Fourier transform: product in Fourier space used to calculate CCF
						cftInPlace(ccf, density, array_size);
					}
					iftInPlace(ccf, nz, ny, nx); // ccf holds un-normalized CCF values
					
#ifdef USE_TIME_MANAGER
					timer.PauseStopWatch(ccf_timer);
					timer.ResumeStopWatch(bestCCF_timer);
#endif
					
					for (int i=0; i<array_size; i++) {
						val = ccf[i] / N; // normalize by dividing by N = nx*ny*nz
						if( val > best_ccf[i] ) {
							best_ccf[i] = val;
							if (best_alt)
								best_alt[i] = alt;
							if (best_az)
								best_az[i] = az;
						}
					}
					
#ifdef USE_TIME_MANAGER
					timer.PauseStopWatch(bestCCF_timer);
#endif
					
				}

			}
			cout << endl;
			//cout << "\nFinished with all cylinder orientations." << endl;
			free(density);
			free(cyl);
			free(ccf);

#ifdef USE_TIME_MANAGER
			int normalization_timer = timer.StartStopWatch();
#endif
			
			int i2, j2, k2;
			int ix;
			for (int i=0; i<nx; i++) {
				for (int j=0; j<ny; j++) {
					for (int k=0; k<nz; k++) {
						ix = k+(j+i*ny)*(nz+fftPaddingFastIx);
						
						// shifting the data to center it.
						i2 = (i+nx/2) % nx;
						j2 = (j+ny/2) % ny;
						k2 = (k+nz/2) % nz;
						
						val = best_ccf[ix];
						bestCCF->setDataAt(i2,j2,k2, val);
						if (az_vol && best_az)
							az_vol->setDataAt(i2,j2,k2, best_az[ix]);
						if (alt_vol && best_alt)
							alt_vol->setDataAt(i2,j2,k2, best_alt[ix]);
					}
				}
			}
			
			free(best_ccf);
			if (best_az)
				free(best_az);
			if (best_alt)
				free(best_alt);

			//Normalize bestCCF and model_copy
			double max = bestCCF->getMax();
			for (int i=0; i < N; i++) {
				val = bestCCF->getDataAt(i);
				bestCCF->setDataAt(i, val/max);
			}
			max = model_copy.getMax();
			for (int i=0; i < N; i++) {
				val = model_copy.getDataAt(i);
				model_copy.setDataAt(i, val/max);
			}
			//Weight results to favor areas inside high density regions of model_copy.
			for (int i=0; i < N; i++) {
				val = bestCCF->getDataAt(i);
				bestCCF->setDataAt(i, val*model_copy.getDataAt(i));
				// Note: all voxels in model_copy should have non-negative values because of earlier thresholding
			}

			//Normalize the modified bestCCF map
			max = bestCCF->getMax();
			for (int i = 0; i < N; i++) {
				val = bestCCF->getDataAt(i);
				bestCCF->setDataAt(i, val/max);
			}
			
#ifdef USE_TIME_MANAGER
			timer.PauseStopWatch(normalization_timer);
			
			timer.PauseStopWatch(total_timer);
			timer.DisplayStopWatch(setup_timer, "Setup time: %f\n");
			timer.DisplayStopWatch(cylinder_creation_timer, "Cylinder creation time: %f\n");
			timer.DisplayStopWatch(ccf_timer, "CCF or MCF time: %f\n");
			timer.DisplayStopWatch(bestCCF_timer, "bestCCF time: %f\n");
			timer.DisplayStopWatch(normalization_timer, "Normalization time: %f\n");
			timer.DisplayStopWatch(total_timer, "Total time: %f\n");
#endif
			
			cout << "HelixCorrelation() returning...\n";
			return bestCCF;
		}
		
		void SSEHunter::SetCorrelationScores(Volume * vol, RadialProfileType type, float resolution, float deltaAltRadians) {
			cout << "SetCorrelationScores()\n";
			float cylinderLength = 16.2;
			Volume* bestCCF = HelixCorrelation(vol, type, cylinderLength, deltaAltRadians);
			cout << "Min: " << bestCCF->getMin() << ", Max: " << bestCCF->getMax() << ", Mean: " << bestCCF->getMean() << ", Sigma: " << bestCCF->getStdDev() << "\n";
			PDBAtom patom;
			vector<float> helixScores;
			Vector3DFloat position;
			float value;
			float totVal = 0;
			float maxVal = 0;
			float xorg = bestCCF->getOriginX();
			float yorg = bestCCF->getOriginY();
			float zorg = bestCCF->getOriginZ();
			float apix_x = bestCCF->getSpacingX();
			float apix_y = bestCCF->getSpacingY();
			float apix_z = bestCCF->getSpacingZ();

			for (unsigned int ix = 0; ix < patoms.size(); ix++) {
				patom = patoms[ix];
				position = patom.GetPosition();
				value = bestCCF->getDataAt( round((position.X()-xorg)/apix_x),
										   round((position.Y()-yorg)/apix_y),
										   round((position.Z()-zorg)/apix_z) );
				helixScores.push_back(value);
				totVal += value;
				if (value > maxVal) {
					maxVal = value;
				}
			}
			float avgVal = totVal / helixScores.size();

			for (unsigned int i = 0; i < patoms.size(); i++) {
				value = helixScores[i];
				if ( value >= 0.9*avgVal) {
					value /= maxVal;
				} else {
					value = (value - avgVal)/avgVal;
				}
				helixScores[i] = value;
				patoms[i].SetCorrelationScore(helixScores[i]);
			}
			delete bestCCF;
		}
		
	}
}





#endif
