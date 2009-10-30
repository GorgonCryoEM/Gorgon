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
#include <GraphMatch/PDBAtom.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <math.h>

using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::SkeletonMaker;

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
			
			Volume* GetCylinder(int xsize, int ysize, int zsize, int type = 2, float len = 10.8,
						int x0 = -1, int y0 = -1, int z0 = -1, float apix_x = 1, float apix_y = 1, float apix_z = 1)
			{	return GetTemplateCylinder(xsize, ysize, zsize, static_cast<RadialProfileType>(type), len, x0, y0, z0, apix_x, apix_y, apix_z);
			}
			
			map<unsigned long long, PDBAtom> GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float skeletonCoeff, float correlationCoeff, float geometryCoeff);
		private:
			vector<PDBAtom> GetPseudoAtoms(vector<Vector3DInt> & atomVolumePositions, Volume * vol, float resolution, float threshold);
			void UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ);
			void AddSkeletonWeights(vector<PDBAtom> & patoms, Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float influence);
			void AddGeometryWeights(vector<PDBAtom> & patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float resolution, float threshold, float influence);
			vector< vector<float> > GetAtomDistances(vector<PDBAtom> patoms);
			vector< vector<Vector3DInt> > GetNeighborhoodVoxels(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float threshold);
			vector<float> GetLocalDirectionalityScores(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol);

			//Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
			float GetRadialProfile(float r, RadialProfileType type); //r in angstroms
			float GetRadialProfileGaussian(float r); // r in angstroms
			float GetRadialProfileGaussianDip(float r);// r in angstroms
			float GetRadialProfilePolynomial(float r);// r in angstroms
			Volume* GetTemplateCylinder(int xsize, int ysize, int zsize,
					RadialProfileType type = POLYNOMIAL, float len = 10.8,
					int x0 = -1, int y0 = -1, int z0 = -1,
					float apix_x = 1, float apix_y = 1, float apix_z = 1);
			Volume * GetTemplateHelix(double length, float apix, float resolution, float mapSize);
			
			//Ross Coleman: modified from EMAN2 PointArray::pdb2mrc_by_summation by Wen Jian
			Volume * AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms);

			void NormalizeEdgeMean(Volume* vol);
			void ApplyPolynomialProfileToHelix(Volume * in, float lengthAngstroms, int z0=-1);

		};
		
		SSEHunter::SSEHunter() {
		
		}

		SSEHunter::~SSEHunter() {
		}		

		map<unsigned long long, PDBAtom> SSEHunter::GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float skeletonCoeff, float correlationCoeff, float geometryCoeff) {
			vector<Vector3DInt>  atomVolumePositions;
			vector<PDBAtom> patoms = GetPseudoAtoms(atomVolumePositions, vol, resolution, threshold);
			AddSkeletonWeights(patoms, vol, skeleton, resolution, skeletonCoeff);
			AddGeometryWeights(patoms, atomVolumePositions, vol, resolution, threshold, geometryCoeff);
			
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
			int rMinX = (int)Round(rangeminX/2.0);
			int rMaxX = (int)(Round(rangemaxX/2.0) + 1);
			int rMinY = (int)Round(rangeminY/2.0);
			int rMaxY = (int)(Round(rangemaxY/2.0) + 1);
			int rMinZ = (int)Round(rangeminZ/2.0);
			int rMaxZ = (int)(Round(rangemaxZ/2.0) + 1);		
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

		float SSEHunter::GetRadialProfile(float r, RadialProfileType type) // r in angstroms
		{
			if (type == GAUSSIAN)
				return GetRadialProfileGaussian(r);
			else if (type == GAUSSIAN_DIP)
				return GetRadialProfileGaussianDip(r);
			else if (type == POLYNOMIAL)
				return GetRadialProfilePolynomial(r);
			else //TODO: raise proper exception
				return GetRadialProfilePolynomial(r);
		}

		float SSEHunter::GetRadialProfileGaussian(float r) // r in angstroms
		{
			r /= 2;
			return exp(-r * r);
		}

		float SSEHunter::GetRadialProfileGaussianDip(float r) //r in angstroms
		{
			// pure Gaussian falloff + negative dip, so mean is 0
			r /= 2;
			return (1 - r * r / 4) * exp(-r * r / 4);
		}

		// polynomial fitting to the radial profile of real helix density
		float SSEHunter::GetRadialProfilePolynomial(float r) //r in angstroms
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

		//Ross Coleman: modified from EMAN1 Cylinder.C by Wen Jiang
		//synthesize cylinder that resembles the density seen from an alpha helix
		//The cylindrical axis is parallel to the z axis.
		//len is Angstrom, default to 2 turns
		//x0, y0, z0 will be the centroid of the cylinder in voxels
		Volume* SSEHunter::GetTemplateCylinder(int xsize, int ysize, int zsize,
							RadialProfileType type, float len, int x0, int y0, int z0,
							float apix_x, float apix_y, float apix_z)
		{
			//If x0, y0, or z0 is out of the volume set it to the midpoint
			//Default value of -1 --> midpoint
			if (x0 < 0 || x0 >= xsize)
				x0 = xsize / 2;
			if (y0 < 0 || y0 >= ysize)
				y0 = ysize / 2;
			if (z0 < 0 || z0 >= zsize)
				z0 = zsize / 2;

			Volume * cyl = new Volume(xsize, ysize, zsize);
			cyl->setSpacing(apix_x, apix_y, apix_z);
			cyl->setOrigin(x0,y0,z0);
			
			int cyl_voxel_len = (int) (len / apix_z);
			int cyl_k_min = z0 - cyl_voxel_len / 2;
			int cyl_k_max = z0 + cyl_voxel_len / 2;

			int x, y;
			float radius;
			float value;
			for (int i = 0; i < xsize; i++) {
				for (int j = 0; j < ysize; j++) {
					for (int k = 0; k < zsize; k++)
					{
						x = i - x0;//coordinate sys centered on cylinder
						y = j - y0;//coordinate sys centered on cylinder
						radius = sqrt( x*x*apix_x*apix_x + y*y*apix_y*apix_y );
						if ((k > cyl_k_min) && (k < cyl_k_max))
						{
							value = cyl->getDataAt(i,j,k);
							value += GetRadialProfile(radius, type);
							cyl->setDataAt(i,j,k,value);
						}
					}
				}
			}
			return cyl;
		}

		//TODO: TEST
		Volume * SSEHunter::GetTemplateHelix(double length, float apix, float resolution, float mapSize)
		{
			vector< AtomCoordinatesAndSize > atoms;

			double j=0;
			AtomCoordinatesAndSize nitrogen, c_alpha, carbon, oxygen;
			// Generating PDB Atoms
			while (j <= round(length/1.54)) {
				nitrogen = AtomCoordinatesAndSize( cos((100.0 * j * PI)/180.0)*1.6,              sin((100.0 * j * PI)/180.0)*1.6,               j * 1.54,         7.0 );
				c_alpha  = AtomCoordinatesAndSize( cos(((28.0 + (100.0 * j)) * PI)/180.0) * 2.3, sin(((28.0 + (100.0 * j)) * PI)/180.0) * 2.3, (j * 1.54) + 0.83, 6.0 );
				carbon   = AtomCoordinatesAndSize( cos(((61.0 + (100.0 * j)) * PI)/180.0) * 2.0, sin(((61.0 + (100.0 * j)) * PI)/180.0) * 2.0, (j * 1.54) + 1.7,  6.0 );
				oxygen   = AtomCoordinatesAndSize( cos(((61.0 + (100.0 * j)) * PI)/180.0) * 2.0, sin(((61.0 + (100.0 * j)) * PI)/180.0) * 2.0, (j * 1.54) + 3.09, 8.0 );
				j = j + 1.0;
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
		

		//************************************************************************
		// pdb2vol_by_summation
		// TODO: Test this function!!!!
		//************************************************************************
		Volume * SSEHunter::AtomsToVolumeBySummation(int map_size, float apix, float res, vector< AtomCoordinatesAndSize > atoms)
		{
		#ifdef DEBUG
			printf("SSEHunter::pdb2vol_by_summation(): %d points\tmapsize = %4d\tapix = %g\tres = %g\n",positions.size(),map_size, apix, res);
		#endif
			double gauss_real_width = res / (M_PI);	// in Angstrom, res is in Angstrom
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
							double val = fval * zval * yval * xval;
							map->setDataAt(i,j,k, val);
						}
					}
				}
			}
			//for(int i=0; i<map_size*map_size; i++) pd[i]/=sqrt(M_PI);
		//	map->update();
			map->setSpacing(apix,apix,apix);
			map->setOrigin( -map_size/2*apix, -map_size/2*apix, -map_size/2*apix );

			return map;
		}

		//TODO: test
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
								vol->setDataAt(i,j,k, GetRadialProfilePolynomial((float)radius) );
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


	}
}





#endif
