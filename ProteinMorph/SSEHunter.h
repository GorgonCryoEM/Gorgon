// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
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

#ifndef PROTEINMORPH_SSE_HUNTER_H
#define PROTEINMORPH_SSE_HUNTER_H

#include <SkeletonMaker/Volume.h>
#include "NonManifoldMesh.h"
#include <vector>
#include <map>
#include <MathTools/Vector3D.h>
#include <GraphMatch/PDBAtom.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>

using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraySkeletonCPP;

namespace wustl_mm {
	namespace Protein_Morph {
	
		class SSEHunter{
		public:
			SSEHunter();
			~SSEHunter();
			
			map<unsigned long long, PDBAtom> GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold);
		private:
			vector<PDBAtom> GetPseudoAtoms(vector<Vector3DInt> & atomVolumePositions, Volume * vol, float resolution, float threshold);
			void UpdateMap(Volume * vol, Vector3DInt loc, float rangeminX, float rangeminY, float rangeminZ, float rangemaxX, float rangemaxY, float rangemaxZ);
			void AddSkeletonWeights(vector<PDBAtom> & patoms, Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float influence);
			void AddGeometryWeights(vector<PDBAtom> & patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float resolution, float threshold, float influence);
			vector< vector<float> > GetAtomDistances(vector<PDBAtom> patoms);
			vector< vector<Vector3DInt> > GetNeighborhoodVoxels(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol, float threshold);
			vector<float> GetLocalDirectionalityScores(vector<PDBAtom> patoms, vector<Vector3DInt> atomVolumePositions, Volume * vol);
			
		};
		
		
		SSEHunter::SSEHunter() {
		
		}
		
		SSEHunter::~SSEHunter() {
		}
		
		map<unsigned long long, PDBAtom> SSEHunter::GetScoredAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold) {
			vector<Vector3DInt>  atomVolumePositions;
			vector<PDBAtom> patoms = GetPseudoAtoms(atomVolumePositions, vol, resolution, threshold);
			AddSkeletonWeights(patoms, vol, skeleton, resolution, 1.0f);
			AddGeometryWeights(patoms, atomVolumePositions, vol, resolution, threshold, 1.0f);
			
			map<unsigned long long, PDBAtom> atomMap;
			atomMap.clear();
			for(int i = 0; i < patoms.size();  i++) {
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
						distance = sqrt(x*x + y*y + z*z);
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
			for(int i = 0; i < patoms.size(); i++) {
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
			for(int i = 0; i < patoms.size(); i++) {
				atomDistances.clear();
				for(int j = 0; j < patoms.size(); j++) {
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
			
			for(int i = 0; i < atomVolumePositions.size(); i++) {
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
	}
}


#endif