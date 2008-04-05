#ifndef GORGON_MARCHING_CUBES_H
#define GORGON_MARCHING_CUBES_H

#include <iostream>
#include <tchar.h>
#include <GL/glut.h>
#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
#include "VoxelData.h"
#include "Mesh.h"
#include <string>
#include "GlobalConstants.h"

using namespace std;

namespace wustl_mm {
	namespace Visualization {

		class VolumeRenderer : public Renderer {
		public:
			VolumeRenderer();
			~VolumeRenderer();

			void SetVoxelData(const VoxelData& data);
			void LoadFile(string fileName);
			void SetSurfaceValue(const float value);
			float GetSurfaceValue() const ;
			void SetSampleDensity(const float size);
			float GetSampleDensity() const ;
			const Mesh& getMesh() const ;
			void Draw() const;
			const Mesh& GetMesh() const;
			string GetSupportedFileFormats();
			void UpdateBoundingBox() ;
			float GetMinDensity();
			float GetMaxDensity();
		
		private:
			void CalculateSurface();
			void MarchingCube(float fX, float fY, float fZ, float fScale);
			void GetNormal(Vector3 &normal, float fX, float fY, float fZ);
			float GetOffset(float fValue1, float fValue2, float fValueDesired);

		private:
			Mesh _mesh;
			float _surf_value, _sample;
			VoxelData _voxel;
		};

		VolumeRenderer::VolumeRenderer() {
			_mesh = *(new Mesh());
			_voxel = *(new VoxelData());
			_surf_value = 1.5;
			_sample = 10;
			CalculateSurface();
		}
		VolumeRenderer::~VolumeRenderer() {
			delete &_mesh;
			delete &_voxel;
		}


		void VolumeRenderer::Draw() const {
			if(_voxel.IsLoaded()) {
				_mesh.drawGL(_voxel.data->getSizeX(), _voxel.data->getSizeY(), _voxel.data->getSizeZ()); 
			}
		}

		void VolumeRenderer::SetVoxelData(const VoxelData& data) {
			delete &_voxel;
			_voxel = data;
			CalculateSurface();
		}

		void VolumeRenderer::SetSurfaceValue(const float value) {
			_surf_value = value;
			CalculateSurface();
		}

		float VolumeRenderer::GetSurfaceValue() const { return _surf_value; }

		void VolumeRenderer::SetSampleDensity(const float size) {
			_sample = size;
			CalculateSurface();
		}

		float VolumeRenderer::GetSampleDensity() const  { return _sample; }

		const Mesh& VolumeRenderer::GetMesh() const  { return _mesh; }

		void VolumeRenderer::CalculateSurface() {
			_mesh.clear();
			//_mesh.addTriangle(Point3(0,0,0), Point3(0,1,0), Point3(0,1,1), Vector3(1,0,0));

			int iX, iY, iZ;
			float fStepSize = 1.0/_sample;
			for(iX = 0; iX < _sample; iX++)
			for(iY = 0; iY < _sample; iY++)
			for(iZ = 0; iZ < _sample; iZ++)
			{
					MarchingCube(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
			}
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

		//MarchCube performs the Marching Cubes algorithm on a single cube
		void VolumeRenderer::MarchingCube(float fX, float fY, float fZ, float fScale)
		{
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
					afCubeValue[iVertex] = _voxel.getValue(fX + a2fVertexOffset[iVertex][0]*fScale,
														   fY + a2fVertexOffset[iVertex][1]*fScale,
														   fZ + a2fVertexOffset[iVertex][2]*fScale);
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

								asEdgeVertex[iEdge][0] = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
								asEdgeVertex[iEdge][1] = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
								asEdgeVertex[iEdge][2] = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

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
			_voxel.loadVolume(fileName);
			UpdateBoundingBox();
		}

		void VolumeRenderer::UpdateBoundingBox() {
			for(int i = 0; i < 3; i++) {
				minPts[i] = 0;
			}
			maxPts[0] = _voxel.data->getSizeX()-1;
			maxPts[1] = _voxel.data->getSizeY()-1;
			maxPts[2] = _voxel.data->getSizeZ()-1;
		}

		void VolumeRenderer::GetNormal(Vector3 &normal, float fX, float fY, float fZ) {
				normal[0] = _voxel.getValue(fX-0.01, fY, fZ) - _voxel.getValue(fX+0.01, fY, fZ);
				normal[1] = _voxel.getValue(fX, fY-0.01, fZ) - _voxel.getValue(fX, fY+0.01, fZ);
				normal[2] = _voxel.getValue(fX, fY, fZ-0.01) - _voxel.getValue(fX, fY, fZ+0.01);
				normal.normalize();
		}

		string VolumeRenderer::GetSupportedFileFormats() {
			return "Volumes (*.mrc)";
		}

		float VolumeRenderer::GetMinDensity() {
			return _voxel.data->getMin();
		}

		float VolumeRenderer::GetMaxDensity(){
			return _voxel.data->getMax();
		}
	}
}

#endif