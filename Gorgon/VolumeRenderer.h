#ifndef GORGON_MARCHING_CUBES_H
#define GORGON_MARCHING_CUBES_H

#include <iostream>
#include <tchar.h>
#include <GL/glut.h>
#include <string>
#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
#include "VolumeMesh.h"
#include "GlobalConstants.h"
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>

using namespace std;
using namespace wustl_mm::GraySkeletonCPP;

namespace wustl_mm {
	namespace Visualization {

		class VolumeRenderer : public Renderer {
		public:
			VolumeRenderer();
			~VolumeRenderer();

			float GetMaxDensity();
			float GetMinDensity();
			float GetSurfaceValue() const ;
			int GetSampleInterval() const ;
			string GetSupportedFileFormats();
			void LoadFile(string fileName);
			void Unload();
			void SetSurfaceValue(const float value);
			void SetSampleInterval(const int size);
			void Draw() const;
			void UpdateBoundingBox() ;
		
		private:
			float GetVoxelData(int x, int y, int z);
			float GetOffset(float fValue1, float fValue2, float fValueDesired);
			void CalculateSurface();
			void MarchingCube(int iX, int iY, int iZ, int iScale);
			void GetNormal(Vector3 &normal, int iX, int iY, int iZ);

		private:
			VolumeMesh _mesh;
			float _surf_value;
			int _sample;
			Volume * _voxel;
		};

		VolumeRenderer::VolumeRenderer() {
			_mesh = *(new VolumeMesh());
			_voxel = NULL;
			_surf_value = 1.5;
			_sample = 1;
		}
		VolumeRenderer::~VolumeRenderer() {
			delete &_mesh;
			if(_voxel != NULL) {
				delete _voxel;
			}
		}

		float VolumeRenderer::GetSurfaceValue() const { 
			return _surf_value; 
		}

		float VolumeRenderer::GetOffset(float fValue1, float fValue2, float fValueDesired)
		{
				double fDelta = fValue2 - fValue1;

				if(fDelta == 0.0)
				{
						return 0.5;
				}
				return (fValueDesired - fValue1)/fDelta;
		}

		float VolumeRenderer::GetVoxelData(int x, int y, int z) {
			if((x < 0) || (x > _voxel->getSizeX()-1) || (y < 0) || (y > _voxel->getSizeY()-1) || (z < 0) || (z > _voxel->getSizeZ()-1)) {
				return 0.0f;
			} else {
				return _voxel->getDataAt(x, y, z);
			}
		}

		float VolumeRenderer::GetMinDensity() {
			return _voxel->getMin();
		}

		float VolumeRenderer::GetMaxDensity(){
			return _voxel->getMax();
		}
		int VolumeRenderer::GetSampleInterval() const  { 
			return _sample; 
		}

		string VolumeRenderer::GetSupportedFileFormats() {
			return "Volumes (*.mrc)";
		}

		void VolumeRenderer::Draw() const {
			if(_voxel != NULL) {
				_mesh.drawGL(); 
			}
		}

		void VolumeRenderer::SetSurfaceValue(const float value) {
			_surf_value = value;
			CalculateSurface();
		}

		void VolumeRenderer::SetSampleInterval(const int size) {
			_sample = size;
			CalculateSurface();
		}

		void VolumeRenderer::CalculateSurface() {
			_mesh.clear();
			if(_voxel != NULL) {
				int iX, iY, iZ;
				for(iX = 0; iX < _voxel->getSizeX(); iX++) {
					for(iY = 0; iY < _voxel->getSizeY(); iY++) {
						for(iZ = 0; iZ < _voxel->getSizeZ(); iZ++) {
								MarchingCube(iX*_sample, iY*_sample, iZ*_sample, _sample);
						}
					}
				}
			}
		}



		void VolumeRenderer::MarchingCube(int iX, int iY, int iZ, int iScale){
				extern int aiCubeEdgeFlags[256];
				extern int a2iTriangleConnectionTable[256][16];

				int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
				float fOffset;
				Vector3 sColor;
				float afCubeValue[8];
				Vector3 asEdgeVertex[12];
				Vector3 asEdgeNorm[12];

				//Make a local copy of the values at the cube's corners
				for(iVertex = 0; iVertex < 8; iVertex++)
				{
					afCubeValue[iVertex] = GetVoxelData(iX + a2iVertexOffset[iVertex][0]*iScale,
														iY + a2iVertexOffset[iVertex][1]*iScale,
														iZ + a2iVertexOffset[iVertex][2]*iScale);
				}

				//Find which vertices are inside of the surface and which are outside
				iFlagIndex = 0;
				for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
				{
						if(afCubeValue[iVertexTest] <= _surf_value) 
								iFlagIndex |= 1<<iVertexTest;
				}

				//Find which edges are intersected by the surface
				iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

				//If the cube is entirely inside or outside of the surface, then there will be no intersections
				if(iEdgeFlags == 0) 
				{
						return;
				}

				//Find the point of intersection of the surface with each edge
				//Then find the normal to the surface at those points
				for(iEdge = 0; iEdge < 12; iEdge++)
				{
						//if there is an intersection on this edge
						if(iEdgeFlags & (1<<iEdge))
						{
								fOffset = GetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
															 afCubeValue[ a2iEdgeConnection[iEdge][1] ], _surf_value);

								asEdgeVertex[iEdge][0] = (float)(iX + (a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]) +  fOffset * (float)(a2iEdgeDirection[iEdge][0]) * iScale);
								asEdgeVertex[iEdge][1] = (float)(iY + (a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]) +  fOffset * (float)(a2iEdgeDirection[iEdge][1]) * iScale);
								asEdgeVertex[iEdge][2] = (float)(iZ + (a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]) +  fOffset * (float)(a2iEdgeDirection[iEdge][2]) * iScale);

								GetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2]);
						}
				}


				//Draw the triangles that were found.  There can be up to five per cube
				for(iTriangle = 0; iTriangle < 5; iTriangle++)
				{
						if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
								break;
						Point3 pts[3];
						Vector3 norms[3];
						for(iCorner = 0; iCorner < 3; iCorner++)
						{
								iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

								//vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
								//glColor3f(sColor.fX, sColor.fY, sColor.fZ);
								pts[iCorner][0] = asEdgeVertex[iVertex][0];
								pts[iCorner][1] = asEdgeVertex[iVertex][1];
								pts[iCorner][2] = asEdgeVertex[iVertex][2];
								norms[iCorner][0] = asEdgeNorm[iVertex][0];
								norms[iCorner][1] = asEdgeNorm[iVertex][1];
								norms[iCorner][2] = asEdgeNorm[iVertex][2];
						}
						for(iCorner = 0; iCorner < 3; iCorner++)
							_mesh.addTriangle(pts[0],pts[1],pts[2],norms[0],norms[1],norms[2]);
				}
		}

		void VolumeRenderer::LoadFile(string fileName) {
			if(_voxel != NULL) {
				delete _voxel;
			}
			_voxel = VolumeFormatConverter::LoadVolume(fileName);
			CalculateSurface();
			UpdateBoundingBox();
		}

		void VolumeRenderer::Unload() {
			if(_voxel != NULL) {
				delete _voxel;
			}
			_voxel = NULL;
			CalculateSurface();
			UpdateBoundingBox();

		}
		void VolumeRenderer::UpdateBoundingBox() {
			if(_voxel == NULL) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = 0;
					maxPts[i] = 1;
				}
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = 0;
				}
				maxPts[0] = _voxel->getSizeX()-1;
				maxPts[1] = _voxel->getSizeY()-1;
				maxPts[2] = _voxel->getSizeZ()-1;
			}
		}

		void VolumeRenderer::GetNormal(Vector3 &normal, int iX, int iY, int iZ) {
				normal[0] = GetVoxelData(iX+1, iY, iZ) + GetVoxelData(iX-1, iY, iZ);
				normal[1] = GetVoxelData(iX, iY+1, iZ) + GetVoxelData(iX, iY-1, iZ);
				normal[2] = GetVoxelData(iX, iY, iZ+1) + GetVoxelData(iX, iY, iZ-1);
				normal.normalize();
		}


	}
}

#endif