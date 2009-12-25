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
			
			map<unsigned long long, PDBAtom> GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, 
															float skeletonCoeff, float correlationCoeff, float geometryCoeff,
															RadialProfileType type = GAUSSIAN_DIP, float deltaAngleRadians=5*PI/180);
		private:
			vector<PDBAtom> GetPseudoAtoms(vector<Vector3DInt> & atomVolumePositions, Volume * vol, float resolution, float threshold);
			void UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ);
			void AddSkeletonWeights(vector<PDBAtom> & patoms, Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float influence);
			void AddHelixCorrelationWeights(vector<PDBAtom>& patoms, Volume * vol, RadialProfileType type, float resolution, float influence, float deltaAngleRadians);
			void AddGeometryWeights(vector<PDBAtom> & patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float resolution, float threshold, float influence);
			vector< vector<float> > GetAtomDistances(vector<PDBAtom> patoms);
			vector< vector<Vector3DInt> > GetNeighborhoodVoxels(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float threshold);
			vector<float> GetLocalDirectionalityScores(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol);
		public:
			//Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
			float RadialProfile(float r, RadialProfileType type); //r in angstroms
		private:
			float RadialProfileGaussian(float r); // r in angstroms
			float RadialProfileGaussianDip(float r);// r in angstroms
			float RadialProfilePolynomial(float r);// r in angstroms
		public:
			Volume* GetTemplateCylinder(int xsize, int ysize, int zsize, RadialProfileType type = POLYNOMIAL, float len = 16.2,
												float apix_x = 1, float apix_y = 1, float apix_z = 1,
												double axis_vector_x=0, double axis_vector_y=1, double axis_vector_z=0);
			void ApplyTemplateCylinder(float cylData[], int xsize, int ysize, int zsize, int fastIxFFTPadding,
										RadialProfileType type = POLYNOMIAL, float len=16.2, float apix_x=1, float apix_y=1, float apix_z=1,
										double axis_vector_x=0, double axis_vector_y=1, double axis_vector_z=0);
			Volume * GetTemplateHelix(double length, float apix, float resolution, int mapSize);
			Volume * HelixCorrelation(Volume* map_vol, RadialProfileType type = POLYNOMIAL, float length = 16.2,
									  float deltaAngleRadians = 5*PI/180, Volume* az_vol = NULL, Volume* alt_vol = NULL);
		private:
			void NormalizeEdgeMean(Volume* vol);
			void ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0=-1);

			//Ross Coleman: modified from EMAN2 PointArray::pdb2mrc_by_summation by Wen Jian
			Volume * AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms);


		};
		
		SSEHunter::SSEHunter() {
		
		}

		SSEHunter::~SSEHunter() {
		}		

		map<unsigned long long, PDBAtom> SSEHunter::GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, 
																   float skeletonCoeff, float correlationCoeff, float geometryCoeff, 
																   RadialProfileType type, float deltaAngleRadians) {
			cout << "GetScoredAtoms()\n";
			vector<Vector3DInt>  atomVolumePositions;
			vector<PDBAtom> patoms = GetPseudoAtoms(atomVolumePositions, vol, resolution, threshold);
			AddSkeletonWeights(patoms, vol, skeleton, resolution, skeletonCoeff);
			AddGeometryWeights(patoms, atomVolumePositions, vol, resolution, threshold, geometryCoeff);
			AddHelixCorrelationWeights(patoms, vol, type, resolution, correlationCoeff, deltaAngleRadians);
			map<unsigned long long, PDBAtom> atomMap;
			atomMap.clear();
			for(unsigned int i = 0; i < patoms.size();  i++) {
				atomMap[patoms[i].GetHashKey()] = patoms[i];
			}
			return atomMap;
			
		}

		vector<PDBAtom> SSEHunter::GetPseudoAtoms(vector<Vector3DInt> & atomVolumePositions, Volume * vol, float resolution, float threshold) {
			Volume * tempVol = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol);
			vector<PDBAtom> patoms;
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
			return patoms;		
		}		

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

		void SSEHunter::AddSkeletonWeights(vector<PDBAtom> & patoms, Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float influence) {
			cout << "AddSkeletonWeights\n";
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
				patoms[i].SetTempFactor(patoms[i].GetTempFactor() + influence * typeCost * (1.0 - min(maxDistance, distance) / maxDistance));
			}
		}

		vector< vector<float> > SSEHunter::GetAtomDistances(vector<PDBAtom> patoms) {
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

		vector< vector<Vector3DInt> > SSEHunter::GetNeighborhoodVoxels(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float threshold) {
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

		vector<float> SSEHunter::GetLocalDirectionalityScores(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol) {
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

		void SSEHunter::AddGeometryWeights(vector<PDBAtom> & patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float resolution, float threshold, float influence) {
			cout << "AddGeometryWeights()\n";
			vector< vector<float> > distances = GetAtomDistances(patoms);
			//vector< vector<Vector3DInt> > neighbors = GetNeighborhoodVoxels(patoms, atomVolumePositions, vol, threshold);
			vector<float> localDirectionalityScore = GetLocalDirectionalityScores(patoms, atomVolumePositions, vol);
			vector<float> geometryScore;
			
			for(unsigned int i = 0; i < patoms.size(); i++) {
				geometryScore.push_back(localDirectionalityScore[i]);
			}
			
			
			for(unsigned int i = 0; i < geometryScore.size(); i++) {
				patoms[i].SetTempFactor(patoms[i].GetTempFactor() + influence * geometryScore[i]);
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
		Volume* SSEHunter::GetTemplateCylinder(int xsize, int ysize, int zsize, RadialProfileType type, float len, 
											float apix_x, float apix_y, float apix_z,
											double axis_vector_x, double axis_vector_y, double axis_vector_z) {
			// The centroid of the Cylinder is at the center of the map.
			float x0 = apix_x * xsize / 2.0;
			float y0 = apix_y * ysize / 2.0;
			float z0 = apix_z * zsize / 2.0;
			
			Volume * cyl = new Volume(xsize, ysize, zsize);
			cyl->setSpacing(apix_x, apix_y, apix_z);
			
			//Find the unit vector that points along the desired axis for the cylinder
			double axis_vect_length = sqrt( axis_vector_x*axis_vector_x + axis_vector_y*axis_vector_y + axis_vector_z*axis_vector_z );
			double axis_x = axis_vector_x/axis_vect_length;
			double axis_y = axis_vector_y/axis_vect_length;
			double axis_z = axis_vector_z/axis_vect_length;
			
			float radius;
			double x, y, z;
			double projection;
			float value;
			double cross_prod[3] = {0,0,0};
			for (int i = 0; i < xsize; i++) {
				for (int j = 0; j < ysize; j++) {
					for (int k = 0; k < zsize; k++) {
						x = i*apix_x - x0;
						y = j*apix_y - y0;
						z = k*apix_z - z0;
						
						// The cross product gives the perpendicular distance to the cylinder's axis vector
						cross_prod[0] = y*axis_z-z*axis_y;
						cross_prod[1] = z*axis_x-x*axis_z;
						cross_prod[2] = x*axis_y-y*axis_x;
						radius = (float) sqrt( cross_prod[0]*cross_prod[0] + cross_prod[1]*cross_prod[1] + cross_prod[2]*cross_prod[2] );
						projection = x*axis_x + y*axis_y + z*axis_z;
						
						if ((projection > -len/2.0 ) && (projection < len/2.0 )) {
							value = RadialProfile(radius, type);
							cyl->setDataAt(i,j,k, value);
						} else {
							cyl->setDataAt(i,j,k, 0);
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
											RadialProfileType type, float len, float apix_x, float apix_y, float apix_z,
											double axis_vector_x, double axis_vector_y, double axis_vector_z) {
			// The centroid of the Cylinder is at the center of the map.
			float x0 = apix_x * xsize / 2.0;
			float y0 = apix_y * ysize / 2.0;
			float z0 = apix_z * zsize / 2.0;

			//Find the unit vector that points along the desired axis for the cylinder
			double axis_vect_length = sqrt( axis_vector_x*axis_vector_x + axis_vector_y*axis_vector_y + axis_vector_z*axis_vector_z );
			double axis_x = axis_vector_x/axis_vect_length;
			double axis_y = axis_vector_y/axis_vect_length;
			double axis_z = axis_vector_z/axis_vect_length;

			float radius;
			double x, y, z;
			double projection;
			float value;
			double cross_prod[3] = {0,0,0};
			for (int i = 0; i < xsize; i++) {
				for (int j = 0; j < ysize; j++) {
					for (int k = 0; k < zsize; k++) {
						x = i*apix_x - x0;
						y = j*apix_y - y0;
						z = k*apix_z - z0;

						// The cross product gives the perpendicular distance to the cylinder's axis vector
						cross_prod[0] = y*axis_z-z*axis_y;
						cross_prod[1] = z*axis_x-x*axis_z;
						cross_prod[2] = x*axis_y-y*axis_x;
						radius = (float) sqrt( cross_prod[0]*cross_prod[0] + cross_prod[1]*cross_prod[1] + cross_prod[2]*cross_prod[2] );
						projection = x*axis_x + y*axis_y + z*axis_z;

						if ((projection > -len/2.0 ) && (projection < len/2.0 )) {
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
												 float deltaAngleRadians, Volume* az_vol, Volume* alt_vol) {
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

			for (float az = 0; az < 2*PI; az += deltaAngleRadians) { // Angle from x axis to projection on xy plane
				cout << 50*az/PI << "%\t" << flush;
				for (float alt = 0; alt < PI; alt += deltaAngleRadians) { // Angle from the z axis

					// Unit vector along cylinder's axis
					axis_vect[0] = -sin(az)*cos(alt);
					axis_vect[1] = -cos(alt);
					axis_vect[2] = sin(az)*sin(alt);

					ApplyTemplateCylinder(cyl, nx, ny, nz, fftPaddingFastIx, type, length,
										apix_x, apix_y, apix_z, axis_vect[0], axis_vect[1], axis_vect[2]);
					fftInPlace(cyl, nz, ny, nx);

					//Finding the complex conjugate
					for (int i=0; i<array_size; i+=2) {
						// conj(c1+c2*i) * (d1+d2*i) = (c1-c2*i) * (d1+d2*i) = (c1*d1+c2*d2) + (c1*d2-c2*d1)*i
						c1 = cyl[i];
						c2 = cyl[i+1];
						d1 = density[i];
						d2 = density[i+1];

						cyl[i] = c1*d1+c2*d2;
						cyl[i+1] = c1*d2-c2*d1;
					}
					iftInPlace(cyl, nz, ny, nx); // cyl_data will now hold the CCF values

					int i2, j2, k2;

					for (int i=0; i<nx; i++) {
						for (int j=0; j<ny; j++) {
							for (int k=0; k<nz+fftPaddingFastIx; k++) {

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
						for (int k=0; k<nz+fftPaddingFastIx; k++) {
							val = bestCCF->getDataAt(i,j,k);
							bestCCF->setDataAt(i,j,k, (val-min)/(max-min)); //Make the values fall between zero and one
						}
					}
				}
			}
			else { // max == min, so make the map zero everywhere
				for (int i=0; i<nx; i++) {
					for (int j=0; j<ny; j++) {
						for (int k=0; k<nz+fftPaddingFastIx; k++) {
							val = bestCCF->getDataAt(i,j,k);
							bestCCF->setDataAt(i,j,k, val-min); //Make the values fall between zero and one
						}
					}
				}
			}
			return bestCCF;
		}
		
		void SSEHunter::AddHelixCorrelationWeights(vector<PDBAtom>& patoms, Volume * vol, RadialProfileType type, float resolution, float influence, float deltaAngleRadians) {
			cout << "AddHelixCorrelationWeights()\n";
			float cylinderLength = 16.2;
			Volume* bestCCF = HelixCorrelation(vol, type, cylinderLength, deltaAngleRadians);
			PDBAtom patom;
			vector<float> helixScores;
			Vector3DFloat position;
			float value;
			float totVal = 0;
			float maxVal = 0;

			for (unsigned int i = 0; i < patoms.size(); i++) {
				patom = patoms[i];
				position = patom.GetPosition();
				//TODO: get value at just the single closest voxel
				value = bestCCF->getInterpDataAt(position.X(), position.Y(), position.Z()); 
				helixScores.push_back(value);
				totVal += value;
				if (value > maxVal) {
					maxVal = value;
				}
			}
			float avgVal = maxVal / helixScores.size();

			for (unsigned int i = 0; i < patoms.size(); i++) {
				value = helixScores[i];
				if ( value >= 0.9*avgVal) {
					value /= maxVal;
				} else {
					value = (value - avgVal)/avgVal;
				}
				helixScores[i] = value;
				patoms[i].SetTempFactor(patoms[i].GetTempFactor() + influence * helixScores[i]);
			}
			delete bestCCF;
		}
		
	}
}





#endif
