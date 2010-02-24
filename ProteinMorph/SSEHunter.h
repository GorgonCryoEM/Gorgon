// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Originally written in python by Matthew Baker (mlbaker@gmail.com).. Rewritten in C++ by Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   C++ Implementation of SSEHunter

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.22  2010/02/20 14:20:08  colemanr
//   HelixCorrelation() modifications
//
//   Revision 1.21  2010/01/16 22:29:17  colemanr
//   trivial
//
//   Revision 1.20  2010/01/16 20:05:15  colemanr
//   moving the total score calculation of SSEHunter to Python
//
//   Revision 1.19  2010/01/15 02:12:57  colemanr
//   changing public/private for functions that will be wrapped in Python
//
//   Revision 1.18  2010/01/12 17:06:45  colemanr
//   fixed HelixCorrelation() array bounds errors on bestCCF object
//
//   Revision 1.17  2010/01/10 05:31:43  colemanr
//   PDBAtoms now store their correlation, skeleton, and geometry scores. Changing the weighting for these three scores in the GUI now changes the total score for each pseudoatom.
//
//   Revision 1.16  2010/01/06 03:48:13  colemanr
//   helix correlation score fixes
//
//   Revision 1.15  2009/12/25 17:32:21  colemanr
//   fixed runtime error if alt and az volumes are not used
//   fixed various compiler warning messages
//
//   Revision 1.14  2009/12/21 22:03:16  ssa1
//   Checking in FFTW windows binaries
//
//   Revision 1.13  2009/12/21 20:38:42  colemanr
//   HelixCorrelation() normalizes the output map to values between zero and one, now.
//   Added AddHelixCorrelationWeights()
//   Added FFTW3 conditional compilation
//
//   Revision 1.12  2009/12/19 05:49:55  colemanr
//   SSEHunter::HelixCorrelation() now uses float arrays not volumes to hold the template cylinder.
//   Added SSEHunter::ApplyTemplateHelix(float map[], ...).
//   HelixCorrelation() now does proper complex number multiplication, and it no longer has 2 memory leaks.
//
//   Revision 1.11  2009/12/17 23:15:45  colemanr
//   fixed a memory leak in SSEHunter::HelixCorrelation()
//
//   Revision 1.10  2009/12/09 21:17:06  colemanr
//   Cylinder's can be generated with any orientation, now. Added helix correlation using cylinders to represent alpha-helix density.
//
//   Revision 1.9  2009/11/03 18:05:10  colemanr
//   bug-fix for AtomsToVolumeBySummation; changed names for radial profile functions
//
//   Revision 1.8  2009/11/02 19:48:07  colemanr
//   using round() rather than Round() and added an include to get NonManifoldMesh_Annotated
//
//   Revision 1.7  2009/10/30 06:12:37  colemanr
//   added functions used to calculate a template helix
//
//   Revision 1.6  2009/10/28 21:23:49  colemanr
//   added AtomsToVolumeBySummation based on EMAN2's PointArray::pdb2mrc_by_summation
//
//   Revision 1.5  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.4  2009/10/08 23:29:10  colemanr
//   I added functions that will create a cylinder Volume with density resembling
//   that of an alpha helix.
//
//   Revision 1.3  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//
//   Revision 1.2  2009/08/10 20:03:40  ssa1
//   SSEHunter interfaced into Gorgon
//
//   Revision 1.1  2009/08/10 13:54:38  ssa1
//   Adding initial ssehunter program
//

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
			
			//TODO: Finish coding the geometry score portion of this function
			map<unsigned long long, PDBAtom> GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, 
															float correlationCoeff, float skeletonCoeff, float geometryCoeff,
															RadialProfileType type = GAUSSIAN_DIP, float deltaAltRadians=5*PI/180);

			
			
			void CreatePseudoAtoms(Volume * vol, float resolution, float threshold);
			int GetNumberOfPseudoAtoms();
			PDBAtom& GetPseudoAtom(int i);
			
			void SetCorrelationScores(Volume * vol, RadialProfileType type, float resolution, float deltaAltRadians);
			void SetSkeletonScores(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution);
			//TODO: Finish coding SetGeometryScores
			void SetGeometryScores(Volume * vol, float resolution, float threshold);
			vector< vector<float> > GetAtomDistances();
			vector< vector<Vector3DInt> > GetNeighborhoodVoxels(Volume * vol, float threshold);
			vector<float> GetLocalDirectionalityScores(Volume * vol);
			
		private:
			void UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ);

			//Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
			float RadialProfile(float r, RadialProfileType type); //r in angstroms
			float RadialProfileGaussian(float r); // r in angstroms
			float RadialProfileGaussianDip(float r);// r in angstroms
			float RadialProfilePolynomial(float r);// r in angstroms
			Volume* GetTemplateCylinder(int xsize, int ysize, int zsize, float alt, float az,
						RadialProfileType type = POLYNOMIAL, float len=16.2, float apix_x=1, float apix_y=-1, float apix_z=-1);
		public:
			void ApplyTemplateCylinder(float cylData[], int xsize, int ysize, int zsize, int fastIxFFTPadding, float alt, float az,
						RadialProfileType type = POLYNOMIAL, float len=16.2, float apix_x=1, float apix_y=-1, float apix_z=-1);
			Volume * GetTemplateHelix(double length, float apix, float resolution, int mapSize);
			Volume * HelixCorrelation(Volume* map_vol, RadialProfileType type = POLYNOMIAL, float length = 16.2,
									  float deltaAltRadians = 5*PI/180, Volume* az_vol = NULL, Volume* alt_vol = NULL);
		private:
			void NormalizeEdgeMean(Volume* vol);
			void ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0=-1);

			//Ross Coleman: modified from EMAN2 PointArray::pdb2mrc_by_summation by Wen Jian
			Volume * AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms);

			
			
			vector<Vector3DInt> atomVolumePositions; // holds the i, j, k indices that give the voxel position of the pseudoatoms
			vector<PDBAtom> patoms; // pseudoatoms

		};
		
		
		SSEHunter::SSEHunter() {
		}

		SSEHunter::~SSEHunter() {
		}		

		map<unsigned long long, PDBAtom> SSEHunter::GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, 
																   float correlationCoeff, float skeletonCoeff, float geometryCoeff, 
																   RadialProfileType type, float deltaAltRadians) {
			cout << "GetScoredAtoms()\n";
			CreatePseudoAtoms(vol, resolution, threshold);
			SetSkeletonScores(vol, skeleton, resolution);
			SetGeometryScores(vol, resolution, threshold);
			SetCorrelationScores(vol, type, resolution, deltaAltRadians);
			map<unsigned long long, PDBAtom> atomMap;
			atomMap.clear();
			for(unsigned int i = 0; i < patoms.size();  i++) {
				patoms[i].SetTempFactor( patoms[i].GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
				atomMap[patoms[i].GetHashKey()] = patoms[i];
			}
			return atomMap;
			
		}
		
		// SSEHunter::GetPseudoAtoms 
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
				atom.SetName("C");
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
			cout << "CreatePseudoAtoms() is finished" << endl;	
		}
		
		// SSEHunter::UpdateMap
		// called by SSEHunter::GetPseudoAtoms after each pseudoatom is chosen
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
			
			for(int x = rMinX; x <= rMaxX; x++){
				for(int y = rMinY; y <= rMaxY; y++) {
					for(int z = rMinZ; z <= rMaxZ; z++) {
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
			cout << "SetSkeletonScores\n";
			int vertexIx;
			float distance, maxDistance = resolution;  // TODO: Max distance is hardcoded as 5 Angstroms
			Vector3DFloat skeletonOrigin = Vector3DFloat(skeleton->GetOriginX(), skeleton->GetOriginY(), skeleton->GetOriginZ());
			Vector3DFloat atomInSkeleton;
			Vector3DFloat skeletonInAtom;
			for(unsigned int i = 0; i < patoms.size(); i++) {
				atomInSkeleton = patoms[i].GetPosition() - skeletonOrigin;
				atomInSkeleton = Vector3DFloat(atomInSkeleton.X()/skeleton->scale[0], atomInSkeleton.Y()/skeleton->scale[1], atomInSkeleton.Z()/skeleton->scale[2]);
				
				vertexIx = skeleton->GetClosestVertexIndex(atomInSkeleton);
				skeletonInAtom = skeleton->vertices[vertexIx].position;
				skeletonInAtom = Vector3DFloat(skeletonInAtom.X()*skeleton->scale[0], skeletonInAtom.Y()*skeleton->scale[1], skeletonInAtom.Z()*skeleton->scale[2]);
				skeletonInAtom = skeletonInAtom + skeletonOrigin;
				
				distance = (skeletonInAtom - patoms[i].GetPosition()).Length();
				double typeCost;
				if(skeleton->IsSurfaceVertex(vertexIx)) {
					typeCost = -1.0;
				} else {
					typeCost = 1.0;
				}
				patoms[i].SetSkeletonScore(typeCost * (1.0 - min(maxDistance, distance) / maxDistance));
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
		
		// SSEHunter::GetNeighborhoodVoxels
		// patoms: pseudoatoms
		// atomVolumePositions: the i,j,k indices for the location of the pseudoatoms in vol
		vector< vector<Vector3DInt> > SSEHunter::GetNeighborhoodVoxels(Volume * vol, float threshold) {
			int kernelWidthX = (int)round(5.0/vol->getSpacingX());
			int kernelWidthY = (int)round(5.0/vol->getSpacingY());
			int kernelWidthZ = (int)round(5.0/vol->getSpacingZ());
			
			vector< vector<Vector3DInt> > voxels;
			vector<Vector3DInt> voxelCoords;
			Vector3DInt voxel;
			
			for(unsigned int i = 0; i < atomVolumePositions.size(); i++) {
				voxelCoords.clear();
				
				for(int addX = -kernelWidthX; addX < kernelWidthX; addX++) {
					for(int addY = -kernelWidthY; addY < kernelWidthY; addY++) {
						for(int addZ = -kernelWidthZ; addZ < kernelWidthZ; addZ++) {
							voxel = Vector3DInt(addX + atomVolumePositions[i].X(), addY + atomVolumePositions[i].Y(), addZ + atomVolumePositions[i].Z());
							if (vol->getDataAt(voxel.X(), voxel.Y(), voxel.Z()) > threshold) {
								voxelCoords.push_back(voxel);
							}
						}
					}
				}
				voxels.push_back(voxelCoords);
			}
			return voxels;		
		}		

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
				eigens[index];
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
				printf("%f, %f, %f, %f, %f\n ", patoms[i].GetTempFactor(), eigens[index].values[0], eigens[index].values[1], eigens[index].values[2], score);
				aspectRatios.push_back(score);
			}
			
			for(unsigned int i = 0; i < aspectRatios.size(); i++) {
				aspectRatios[i] = (2.0f * (aspectRatios[i] - minScore) / (maxScore - minScore)) - 1.0f;
			}
			
			delete [] eigens;
			delete [] volumeGradient;
			delete skeletonizer;			
			delete maskVol;			
			delete tempVol;	
			return aspectRatios;
		}	

		//TODO: finish this function
		void SSEHunter::SetGeometryScores(Volume * vol, float resolution, float threshold) {
			cout << "SetGeometryScores()\n";
			vector< vector<float> > distances = GetAtomDistances();
			//vector< vector<Vector3DInt> > neighbors = GetNeighborhoodVoxels(vol, threshold);
			vector<float> localDirectionalityScore = GetLocalDirectionalityScores(vol);
			vector<float> geometryScore;
			
			for(unsigned int i = 0; i < patoms.size(); i++) {
				geometryScore.push_back(localDirectionalityScore[i]);
			}
			
			
			for(unsigned int i = 0; i < geometryScore.size(); i++) {
				patoms[i].SetGeometryScore(geometryScore[i]);
			}			
			
		}

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
			r /= 2;
			return exp(-r * r);
		}

		float SSEHunter::RadialProfileGaussianDip(float r) //r in angstroms
		{
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
			if (r >= 20)
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

		//synthesize cylinder that resembles the density seen from an alpha helix
		//len in Angstroms, default to 3 turns
		//The cylinder will be centered in the Volume
		Volume* SSEHunter::GetTemplateCylinder(int xsize, int ysize, int zsize, float alt, float az,
				RadialProfileType type, float len, float apix_x, float apix_y, float apix_z) {
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
			Volume * cyl = new Volume(xsize, ysize, zsize);
			cyl->setSpacing(apix_x, apix_y, apix_z);
			
			float half_xsize = xsize/2.0;
			float half_ysize = ysize/2.0;
			float half_zsize = zsize/2.0;
			float cos_alt = cos(alt);
			float sin_alt = sin(alt);
			float cos_az = cos(az);
			float sin_az = sin(az);
			float radius;
			float x, y, z;
			float xprime, yprime, zprime;

			for (int i = 0; i < xsize; i++) {
				for (int j = 0; j < ysize; j++) {
					for (int k = 0; k < zsize; k++) {
						x = apix_x*(i-half_xsize);
						y = apix_y*(j-half_ysize);
						z = apix_z*(k-half_zsize);
						zprime =  (x*sin_az-y*cos_az) * sin_alt + z*cos_alt;
						if ((2*zprime > -len ) && (2*zprime < len)) {
							xprime =   x*cos_az+y*sin_az;
							yprime = (-x*sin_az+y*cos_az) * cos_alt + z*sin_alt;
							radius = sqrt(xprime*xprime+yprime*yprime);
							cyl->setDataAt( i, j, k, RadialProfile(radius, type) );
						} else {
							cyl->setDataAt( i, j, k, 0);
						}
					}
				}
			}

			return cyl;
			
		}

		//cylinder that resembles the density seen from an alpha helix
		//len in Angstroms, default to 3 turns
		//The cylinder will be centered in the float array
		void SSEHunter::ApplyTemplateCylinder(float* cylData, int xsize, int ysize, int zsize, int fastIxFFTPadding,
					float alt, float az, RadialProfileType type, float len, float apix_x, float apix_y, float apix_z) {
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
			float half_xsize = xsize/2.0;
			float half_ysize = ysize/2.0;
			float half_zsize = zsize/2.0;
			float cos_alt = cos(alt);
			float sin_alt = sin(alt);
			float cos_az = cos(az);
			float sin_az = sin(az);
			float radius;
			float x, y, z;
			float xprime, yprime, zprime;

			for (int i = 0; i < xsize; i++) {
				for (int j = 0; j < ysize; j++) {
					for (int k = 0; k < zsize; k++) {
						x = apix_x*(i-half_xsize);
						y = apix_y*(j-half_ysize);
						z = apix_z*(k-half_zsize);
						zprime =  (x*sin_az-y*cos_az) * sin_alt + z*cos_alt;
						if ((2*zprime > -len ) && (2*zprime < len)) {
							xprime =   x*cos_az+y*sin_az;
							yprime = (-x*sin_az+y*cos_az) * cos_alt + z*sin_alt;
							radius = sqrt(xprime*xprime+yprime*yprime);
							cylData[k + (j + ysize*i)*(zsize+fastIxFFTPadding)] = RadialProfile(radius, type);
						} else {
							cylData[k + (j + ysize*i)*(zsize+fastIxFFTPadding)] = 0;
						}
					}
				}
			}
		}

		//TODO: TEST
		Volume * SSEHunter::GetTemplateHelix(double length, float apix, float resolution, int mapSize)
		{
			vector< AtomCoordinatesAndSize > atoms;

			AtomCoordinatesAndSize nitrogen, c_alpha, carbon, oxygen;
			// Generating PDB Atoms
			for (int j=0; j <= round(length/1.54); j++)
			{
				nitrogen = AtomCoordinatesAndSize( 1.6*cos( (0  + 100*j) * PI/180.0), 1.6*sin( (0  + 100*j) * PI/180.0), 1.54*j + 0.00, 7.0 );
				c_alpha  = AtomCoordinatesAndSize( 2.3*cos( (28 + 100*j) * PI/180.0), 2.3*sin( (28 + 100*j) * PI/180.0), 1.54*j + 0.83, 6.0 );
				carbon   = AtomCoordinatesAndSize( 2.0*cos( (61 + 100*j) * PI/180.0), 2.0*sin( (61 + 100*j) * PI/180.0), 1.54*j + 1.70, 6.0 );
				oxygen   = AtomCoordinatesAndSize( 2.0*cos( (61 + 100*j) * PI/180.0), 2.0*sin( (61 + 100*j) * PI/180.0), 1.54*j + 3.09, 8.0 );
				atoms.push_back(nitrogen);
				atoms.push_back(c_alpha);
				atoms.push_back(carbon);
				atoms.push_back(oxygen);
			}

			// TODO: check if the cast for mapSize is the way to go
			Volume* helixVolume = AtomsToVolumeBySummation(static_cast<int>(mapSize), apix, resolution, atoms);
			NormalizeEdgeMean(helixVolume);
			Vector3DFloat cm = helixVolume->getCenterOfMass();
			helixVolume->setOrigin(cm.X(), cm.Y(), cm.Z());
			ApplyPolynomialProfileToHelix(helixVolume, length);
			return helixVolume;

			//Generating volume
			//mrcHelix=EMData()
//#	mrcHelix=points.pdb2mrc_by_summation(box,apix,res)
//#	mrcHelix.process_inplace("normalize.edgemean")
//#	mrcHelix.process_inplace("xform.centerofmass")
//#	aout=mrcHelix.copy()
//#	aout.to_zero()
//#	mrcHelix.process_inplace("filter.highpass.gauss",{"apix":apix,"cutoff_freq":100})
//
//#	for i in range(box):
//#		r = Region(0,0,i,box,box,1)
//#		slice1 = mrcHelix.get_clip(r)
//#		thresh = slice1.process("threshold.binary", {"value":10})
//#		neg = thresh.process("mask.addshells",{"nshells":2})
//#		real_neg = neg-thresh
//#		real_neg.mult(-10)
//#		pos = neg.process("mask.addshells",{"nshells":1})
//#		real_pos = pos-neg
//#		real_pos.mult(10)
//#		solution = slice1.copy()
//#		solution.mult(thresh)
//#		solution += real_neg
//#		solution += real_pos
//#		aout.insert_clip(solution,[0,0,i])
//#	aout.write_image("helix.mrc")
//#	return aout
//#	mrcHelix.write_image("helix.mrc")

		}
		

		// TODO: Test this function!!!!
		Volume * SSEHunter::AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms)
		{
		#ifdef DEBUG
			printf("SSEHunter::AtomsToVolumeBySummation(): %d points\tmapsize = %4d\tapix = %g\tres = %g\n",atoms.size(),map_size, apix, res);
		#endif
			double gauss_real_width = res / (PI);	// in Angstrom, res is in Angstrom
			//if ( gauss_real_width < apix) LOGERR("PointArray::projection_by_summation(): apix(%g) is too large for resolution (%g Angstrom in Fourier space) with %g pixels of 1/e half width", apix, res, gauss_real_width);

			double min_table_val = 1e-7;
			double max_table_x = sqrt(-log(min_table_val));	// for exp(-x*x)

			double table_step_size = 0.001;	// number of steps for each pixel
			double inv_table_step_size = 1.0 / table_step_size;
			int table_size = int (max_table_x * gauss_real_width / (apix * table_step_size) * 1.25);
			vector<double> table;
			table.resize(table_size);
			//double *table = (double *) malloc(sizeof(double) * table_size);
			for (int i = 0; i < table_size; i++) {
				double x = -i * table_step_size * apix / gauss_real_width;
				table[i] = exp(-x * x);
			}

			int gbox = int (max_table_x * gauss_real_width / apix);	// local box half size in pixels to consider for each point
			if (gbox <= 0)
				gbox = 1;



		//	sort_by_axis(2);			// sort by Z-axis
			sort(atoms.begin(), atoms.end(), compare_z);



			Volume *map = new Volume(map_size, map_size, map_size);
			map->fill(0);
		//	float *pd = map->get_data();
			for ( size_t s = 0; s < atoms.size(); ++s) {
				double xc = atoms[s].x / apix + map_size / 2;
				double yc = atoms[s].y / apix + map_size / 2;
				double zc = atoms[s].z / apix + map_size / 2;
				double fval = atoms[s].size;
				int imin = int (xc) - gbox, imax = int (xc) + gbox;
				int jmin = int (yc) - gbox, jmax = int (yc) + gbox;
				int kmin = int (zc) - gbox, kmax = int (zc) + gbox;
				if (imin < 0)
					imin = 0;
				if (jmin < 0)
					jmin = 0;
				if (kmin < 0)
					kmin = 0;
				if (imax > map_size)
					imax = map_size;
				if (jmax > map_size)
					jmax = map_size;
				if (kmax > map_size)
					kmax = map_size;

				for (int k = kmin; k < kmax; k++) {
					size_t table_index_z = size_t (fabs(k - zc) * inv_table_step_size);
					if ( table_index_z >= table.size() ) continue;
					double zval = table[table_index_z];
		//			size_t pd_index_z = k * map_size * map_size;
					for (int j = jmin; j < jmax; j++) {
						size_t table_index_y = size_t (fabs(j - yc) * inv_table_step_size);
						if ( table_index_y >= table.size() ) continue;
						double yval = table[table_index_y];
		//				size_t pd_index = pd_index_z + j * map_size + imin;
						for (int i = imin; i < imax; i++) // , pd_index++)
						{
							size_t table_index_x = size_t (fabs(i - xc) * inv_table_step_size);
							if ( table_index_x >= table.size() ) continue;
							double xval = table[table_index_x];
		//					pd[pd_index] += (float) (fval * zval * yval * xval);
							double val = (fval * zval * yval * xval) + map->getDataAt(i,j,k);
							map->setDataAt(i,j,k, val);
						}
					}
				}
			}
			//for(int i=0; i<map_size*map_size; i++) pd[i]/=sqrt(PI);
		//	map->update();
			map->setSpacing(apix,apix,apix);
			map->setOrigin( -map_size/2*apix, -map_size/2*apix, -map_size/2*apix );

			return map;
		}

		//TODO: TEST
		void SSEHunter::NormalizeEdgeMean(Volume* vol)
		{
			int nx = vol->getSizeX();
			int ny = vol->getSizeY();
			int nz = vol->getSizeZ();
			int N = nx*ny*nz;

			float edge_mean = vol->getEdgeMean();
			float std_dev = vol->getStdDev();
			float val;

			for (int i = 0; i < N; i++)
			{
				val = (vol->getDataAt(i)- edge_mean) / std_dev;
				vol->setDataAt(i, val);
			}
		}

		//TODO: TEST
		void SSEHunter::ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0)
		{
			Volume* vol = in;
			int nx = vol->getSizeX();
			int ny = vol->getSizeY();
			int nz = vol->getSizeZ();
			float apix_x = vol->getSpacingX(); //TODO: Ask Matt if I correctly handled cases where apix_x != apix_y or apix_x != apix_z
			float apix_y = vol->getSpacingY();
			float apix_z = vol->getSpacingZ();

			if (z0 < 0 || z0 >= nz)
				z0 = nz / 2;

			int z_start = floor( ( z0 - 0.5*lengthAngstroms/apix_z ) + 0.5);
			int z_stop = floor( ( z0 + 0.5*lengthAngstroms/apix_z ) + 0.5);

			double rho_x_sum, rho_y_sum, rho_sum;
			double x_cm, y_cm;
			double radius;
			float val;

			for (int k = 0; k < nz; k++) //taking slices along z axis
			{
				rho_x_sum = rho_y_sum = rho_sum = 0; //Set to zero for a new slice

				if (k >= z_start && k <= z_stop)
				//Apply the radial profile only between z_start and z_stop on the z axis
				{
					//Calculating CM for the slice...
					for (int j = 0; j < ny; j++)
					{
						for (int i = 0; i < nx; i++)
						{
							val = vol->getDataAt(i,j,k);
							rho_x_sum += val*i;
							rho_y_sum += val*j;
							rho_sum += val;
						}
					}
					if (rho_sum != 0) //If there was net mass was zero, we can't find the CM, so we'll just leave the slice as is
					{
						x_cm = rho_x_sum/rho_sum;
						y_cm = rho_y_sum/rho_sum;

						//Applying radial profile...
						for (int j=0; j<ny;j++)
						{
							for (int i=0;i<nx;i++)
							{
								radius = hypot( (i-x_cm)*apix_x, (j-y_cm)*apix_y );
								vol->setDataAt(i,j,k, RadialProfilePolynomial((float)radius) );
							}
						}
					}
				}
				else //k < z_start || k > z_stop
				//Clear the map, setting the density to zero everywhere else.
				{
					for (int j=0; j<ny; j++)
						for(int i=0; i<nx; i++)
						{
							vol->setDataAt(i,j,k, 0);
						}
				}
			}
		}

		Volume * SSEHunter::HelixCorrelation(Volume* density_vol, RadialProfileType type, float length,
												 float deltaAltRadians, Volume* az_vol, Volume* alt_vol) {
			cout << "HelixCorrelation()\n";
			// TODO: Make it work with helices
			int nx = density_vol->getSizeX();
			int ny = density_vol->getSizeY();
			int nz = density_vol->getSizeZ();
			
			Volume * bestCCF = new Volume(nx, ny, nz); // This is the returned volume
			float orig_x = density_vol->getOriginX();
			float orig_y = density_vol->getOriginY();
			float orig_z = density_vol->getOriginZ();
			float apix_x = density_vol->getSpacingX();
			float apix_y = density_vol->getSpacingY();
			float apix_z = density_vol->getSpacingZ();
			
			bestCCF->setOrigin(orig_x, orig_y, orig_z);
			bestCCF->setSpacing(apix_x, apix_y, apix_z);
			if (az_vol) {
				az_vol->setOrigin(orig_x, orig_y, orig_z);
				az_vol->setSpacing(apix_x, apix_y, apix_z);
			}
			if (alt_vol) {
				alt_vol->setOrigin(orig_x, orig_y, orig_z);
				alt_vol->setSpacing(apix_x, apix_y, apix_z);
			}
			int fftPaddingFastIx = nz % 2 ? 1 : 2;

			int array_size = nx*ny*(nz+fftPaddingFastIx);
			int N = nx*ny*nz;

			float* density = density_vol->getArrayCopy(0, 0, fftPaddingFastIx, 0);
			fftInPlace(density, nz, ny, nx);

			// Rotating the cylinder to all possible orientations
			double axis_vect[3] = {0,0,0};
			float* cyl = (float*) malloc( sizeof(float)*nx*ny*(nz+fftPaddingFastIx) );
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
			float K = deltaAltRadians/sqrt(2);
			float x,y,z;
			float cos_alt, sin_alt, cos_az, sin_az;
			for (float alt = 0; alt < PI/2.0 + deltaAltRadians/2.0; alt += deltaAltRadians) { // the "+ deltaAltRadians/2.0" as in EMAN1
				cos_alt = cos(alt);
				sin_alt = sin(alt);
				float deltaAzRadians = K/sin_alt;
				for (float az = 0; az < 2*PI + deltaAzRadians/2.0; az += deltaAzRadians) { // the "+ deltaAzRadians/2.0" as in EMAN1
					ApplyTemplateCylinder(cyl, nx, ny, nz, fftPaddingFastIx, alt, az, type, length, apix_x, apix_y, apix_z);
					fftInPlace(cyl, nz, ny, nx);

					///Doing CCF: product in Fourier space
					for (int i=0; i<array_size; i+=2) {
						// conj(c1+c2*i) * (d1+d2*i) = (c1-c2*i) * (d1+d2*i) = (c1*d1+c2*d2) + (c1*d2-c2*d1)*i
						c1 = cyl[i];
						c2 = cyl[i+1];
						d1 = density[i];
						d2 = density[i+1];

						cyl[i] = c1*d1+c2*d2;
						cyl[i+1] = c1*d2-c2*d1;
					}
					iftInPlace(cyl, nz, ny, nx); // cyl holds un-normalized CCF values

					int i2, j2, k2;

					for (int i=0; i<nx; i++) {
						for (int j=0; j<ny; j++) {
							for (int k=0; k<nz; k++) {

								// shifting the data to center it.
								i2 = (i+nx/2) % nx;
								j2 = (j+ny/2) % ny;
								k2 = (k+nz/2) % nz;

								val = cyl[k+(j+i*ny)*(nz+fftPaddingFastIx)] / N; // normalize by dividing by N = nx*ny*nz
								if ( val > bestCCF->getDataAt(i2,j2,k2) ) {
									bestCCF->setDataAt(i2,j2,k2, val);
									if (az_vol)
										az_vol->setDataAt(i2,j2,k2, az);
									if (alt_vol)
										alt_vol->setDataAt(i2,j2,k2, alt);
								}
							}
						}
					}
				}
				cout << 200*alt/PI << "%\t" << flush;
			}
			cout << "\nFinished with all cylinder orientations.\n";
			free(density);
			free(cyl);

			//Normalize the bestCCF map to give values from zero to one
			double min = bestCCF->getMin();
			double max = bestCCF->getMax();

			if (max > min) { //Don't divide by zero.
				for (int i=0; i<nx; i++) {
					for (int j=0; j<ny; j++) {
						for (int k=0; k<nz; k++) {
							val = bestCCF->getDataAt(i,j,k);
							bestCCF->setDataAt(i,j,k, (val-min)/(max-min)); //Make the values fall between zero and one
						}
					}
				}
			}
			else { // max == min, so make the map zero everywhere
				for (int i=0; i<nx; i++) {
					for (int j=0; j<ny; j++) {
						for (int k=0; k<nz; k++) {
							val = bestCCF->getDataAt(i,j,k);
							bestCCF->setDataAt(i,j,k, val-min); //Make the values fall between zero and one
						}
					}
				}
			}
			cout << "HelixCorrelation() returning...\n";
			return bestCCF;
		}
		
		void SSEHunter::SetCorrelationScores(Volume * vol, RadialProfileType type, float resolution, float deltaAltRadians) {
			cout << "SetCorrelationScores()\n";
			float cylinderLength = 16.2;
			Volume* bestCCF = HelixCorrelation(vol, type, cylinderLength, deltaAltRadians);
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
