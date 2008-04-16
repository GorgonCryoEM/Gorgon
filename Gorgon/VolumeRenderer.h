#ifndef GORGON_MARCHING_CUBES_H
#define GORGON_MARCHING_CUBES_H

#include <iostream>
#include <tchar.h>
#include <GL/glut.h>
#include <string>
#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
#include "GlobalConstants.h"
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/GlobalDefinitions.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>
#include <ProteinMorph/NonManifoldMesh.h>
#include <MathTools/Vector3D.h>
#include <hash_map>

using namespace std;
using namespace stdext;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {
		const int VIEWING_TYPE_ISO_SURFACE = 0;
		const int VIEWING_TYPE_CROSS_SECTION = 1;

		class VolumeRenderer : public Renderer {
		public:
			VolumeRenderer();
			~VolumeRenderer();

			float GetMaxDensity();
			float GetMinDensity();
			float GetSurfaceValue() const ;
			int GetSampleInterval() const ;
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void SaveFile(string fileName);
			void SetViewingType(const int type);
			void SetSampleInterval(const int size);
			void SetSurfaceValue(const float value);
			bool SetCuttingPlane(float ptX, float ptY, float ptZ, float vecX, float vecY, float vecZ);
			void UpdateBoundingBox() ;
			void Unload();
			Volume * GetVolume();
			Volume * PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize);
			Volume * PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonSmoothenRadius);
		
		private:
			int GetHashKey(int x, int y, int z, int edge, int iScale);
			float GetVoxelData(Volume * vol, int x, int y, int z);
			float GetVoxelData(Volume * vol, float x, float y, float z);
			float GetOffset(float fValue1, float fValue2, float fValueDesired);
			void CalculateSurface();
			bool CalculateCuttingSurface();
			void MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale);

		private:
			float _surf_value;
			int _sample;
			int viewingType;
			Volume * _voxel;
			NonManifoldMesh_NoTags * _mesh;
			Volume * cuttingVol;
			NonManifoldMesh_NoTags * cuttingMesh;
		};

		VolumeRenderer::VolumeRenderer() {
			viewingType = VIEWING_TYPE_ISO_SURFACE;
			_mesh = new NonManifoldMesh_NoTags();
			cuttingMesh = new NonManifoldMesh_NoTags();
			_voxel = NULL;
			cuttingVol = NULL;
			_surf_value = 1.5;
			_sample = 1;
		}
		VolumeRenderer::~VolumeRenderer() {
			delete _mesh;
			if(_voxel != NULL) {
				delete _voxel;
			}
			delete cuttingMesh;
			if(cuttingVol != NULL) {
				delete cuttingVol;
			}
		}

		float VolumeRenderer::GetMaxDensity(){
			return _voxel->getMax();
		}

		float VolumeRenderer::GetMinDensity() {
			return _voxel->getMin();
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



		float VolumeRenderer::GetSurfaceValue() const { 
			return _surf_value; 
		}

		float VolumeRenderer::GetVoxelData(Volume * vol, int x, int y, int z) {
			if((x < 0) || (x > vol->getSizeX()-1) || (y < 0) || (y > vol->getSizeY()-1) || (z < 0) || (z > vol->getSizeZ()-1)) {
				return 0.0f;
			} else {
				return vol->getDataAt(x, y, z);
			}
		}

		float VolumeRenderer::GetVoxelData(Volume * vol, float x, float y, float z) {
			int f[3] = {(int)x, (int)y, (int)z};
			int c[3] = {f[0]+1, f[1]+1, f[2]+1};
			float d[3] = {x - f[0], y - f[1], z - f[2]};

			float i1 = GetVoxelData(vol, f[0], f[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, f[0], f[1], c[2]) * d[2];
			float i2 = GetVoxelData(vol, f[0], c[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, f[0], c[1], c[2]) * d[2];
			float j1 = GetVoxelData(vol, c[0], f[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, c[0], f[1], c[2]) * d[2];
			float j2 = GetVoxelData(vol, c[0], c[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, c[0], c[1], c[2]) * d[2];

			float w1 = i1 * (1.0 - d[1]) + i2 * d[1];
			float w2 = j1 * (1.0 - d[1]) + j2 * d[1];

			return w1 * (1.0 - d[0]) + w2 * d[0];
		}

		int VolumeRenderer::GetHashKey(int x, int y, int z, int edge, int iScale) {

			x += a2iEdgeHash[edge][1]*iScale;
			y += a2iEdgeHash[edge][2]*iScale;
			z += a2iEdgeHash[edge][3]*iScale;

			edge = a2iEdgeHash[edge][0];
			return x * _voxel->getSizeY() * _voxel->getSizeZ() * 3 + y * _voxel->getSizeZ() * 3 + z * 3 + edge;
		}

		int VolumeRenderer::GetSampleInterval() const  { 
			return _sample; 
		}

		string VolumeRenderer::GetSupportedLoadFileFormats() {
			return "Volumes (*.mrc)";
		}

		string VolumeRenderer::GetSupportedSaveFileFormats() {
			return "Volumes (*.mrc)";
		}

		void VolumeRenderer::SetViewingType(const int type) {
			viewingType = type;
			if(viewingType == VIEWING_TYPE_ISO_SURFACE) {
				CalculateSurface();
			} else if(viewingType == VIEWING_TYPE_CROSS_SECTION) {
				CalculateCuttingSurface();
			} 
		}
		bool VolumeRenderer::SetCuttingPlane(float ptX, float ptY, float ptZ, float vecX, float vecY, float vecZ) {
			cuttingPlaneCenter = Vector3DFloat(ptX, ptY, ptZ);
			cuttingPlaneDirection = Vector3DFloat(vecX, vecY, vecZ);	
			cuttingPlaneDirection.Normalize();
			cuttingPlaneCenter = cuttingPlaneCenter + cuttingPlaneDirection * 1;
			bool redraw = false;
			if(viewingType == VIEWING_TYPE_CROSS_SECTION) {
				redraw = CalculateCuttingSurface();
			}
			return redraw;
		}
		void VolumeRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if((viewingType == VIEWING_TYPE_ISO_SURFACE) && (_mesh != NULL)) {
					_mesh->Draw(true, false, false, selectEnabled, false, false);
				} else if((viewingType == VIEWING_TYPE_CROSS_SECTION) && (cuttingMesh != NULL)) {
					float material[4];
					float material2[4];					
					glGetMaterialfv(GL_FRONT, GL_DIFFUSE, material);
					material2[3] = material[3];
					float maxVal = _voxel->getMax();
					float minVal = _voxel->getMin();
					float val;
					int k;
					if(selectEnabled) {
						glPushName(0);
						glPushName(0);
					}
					for(unsigned int i = 0; i < cuttingMesh->faces.size(); i++) {
						if(selectEnabled) {
							glLoadName(i);
						}
						glBegin(GL_POLYGON);
						Vector3DFloat normal;
						for(unsigned int j = 0; j < cuttingMesh->faces[i].vertexIds.size(); j++) {
							normal = cuttingMesh->GetVertexNormal(cuttingMesh->faces[i].vertexIds[j]);
							k = cuttingMesh->GetVertexIndex(cuttingMesh->faces[i].vertexIds[j]);
							val = this->GetVoxelData(_voxel, cuttingMesh->vertices[k].position.values[0], cuttingMesh->vertices[k].position.values[1], cuttingMesh->vertices[k].position.values[2]);
							material2[0] = (val - minVal)* material[0] / (maxVal - minVal);
							material2[1] = (val - minVal)* material[1] / (maxVal - minVal);
							material2[2] = (val - minVal)* material[2] / (maxVal - minVal);							

							glMaterialfv(GL_FRONT, GL_DIFFUSE, material2);
							glMaterialfv(GL_BACK, GL_DIFFUSE, material2);
							glNormal3f(normal.X(), normal.Y(), normal.Z());
							glVertex3fv(cuttingMesh->vertices[k].position.values);
						}
						glEnd();
					}
					if(selectEnabled) {
						glPopName();
						glPopName();
					}

				}
			}
		}

		void VolumeRenderer::CalculateSurface() {
			_mesh->Clear();
			if(_voxel != NULL) {
				int iX, iY, iZ;
				for(iX = 0; iX < _voxel->getSizeX(); iX+=_sample) {
					for(iY = 0; iY < _voxel->getSizeY(); iY+=_sample) {
						for(iZ = 0; iZ < _voxel->getSizeZ(); iZ+=_sample) {
							MarchingCube(_voxel, _mesh, _surf_value, iX, iY, iZ, _sample);
						}
					}
				}
			}
		}

		bool VolumeRenderer::CalculateCuttingSurface() {
			bool redraw = false;
			if(_voxel != NULL) {
				cuttingMesh->Clear();
				if((cuttingPlaneCenter.X() >= minPts[0]) && (cuttingPlaneCenter.X() <= maxPts[0]) &&
					(cuttingPlaneCenter.Y() >= minPts[1]) && (cuttingPlaneCenter.Y() <= maxPts[1]) &&
					(cuttingPlaneCenter.Z() >= minPts[2]) && (cuttingPlaneCenter.Z() <= maxPts[2])) {

					redraw = true;

					int iX, iY, iZ;

					for(iX = 0; iX < _voxel->getSizeX(); iX+=_sample) {
						for(iY = 0; iY < _voxel->getSizeY(); iY+=_sample) {
							for(iZ = 0; iZ < _voxel->getSizeZ(); iZ+=_sample) {
								cuttingVol->setDataAt(iX, iY, iZ, (cuttingPlaneCenter - Vector3DFloat(iX, iY, iZ))* cuttingPlaneDirection);
							}
						}
					}				

					float data;
					int iV;
					bool posFound = false, negFound = false;

					for(iX = 0; iX < _voxel->getSizeX() - _sample; iX+=_sample) {
						for(iY = 0; iY < _voxel->getSizeY() - _sample ; iY+=_sample) {
							for(iZ = 0; iZ < _voxel->getSizeZ() - _sample; iZ+=_sample) {
								posFound = false;
								negFound = false;
								for(iV = 0; iV < 8; iV++) {
									data = GetVoxelData(cuttingVol, iX + a2iVertexOffset[iV][0] * _sample,  iY + a2iVertexOffset[iV][1] * _sample, iZ + a2iVertexOffset[iV][2] * _sample);
									posFound = posFound || (data >= 0);
									negFound = negFound || (data < 0);																		
								}
								if(posFound && negFound) {
									MarchingCube(cuttingVol, cuttingMesh, 0.0f, iX, iY, iZ, _sample);							
								}
							}
						}
					}
				}
			}
			return redraw;
		}



		void VolumeRenderer::LoadFile(string fileName) {
			if(_voxel != NULL) {
				delete _voxel;
			}
			if(cuttingVol != NULL) {
				delete cuttingVol;
			}
			_voxel = VolumeFormatConverter::LoadVolume(fileName);
			cuttingVol = new Volume(_voxel->getSizeX(), _voxel->getSizeY(), _voxel->getSizeZ());
			UpdateBoundingBox();
		}

		void VolumeRenderer::SaveFile(string fileName) {
			if(_voxel != NULL) {
				_voxel->toMRCFile((char *)fileName.c_str());
			}
		}
		void VolumeRenderer::MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
			extern int aiCubeEdgeFlags[256];
			extern int a2iTriangleConnectionTable[256][16];

			int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
			float fOffset;
			float afCubeValue[8];
			Vector3 asEdgeVertex[12];
			Vector3 asEdgeNorm[12];
			int vertexIds[12];

			//Make a local copy of the values at the cube's corners
			for(iVertex = 0; iVertex < 8; iVertex++) {
				afCubeValue[iVertex] = GetVoxelData(vol, 
													iX + a2iVertexOffset[iVertex][0]*iScale,
													iY + a2iVertexOffset[iVertex][1]*iScale,
													iZ + a2iVertexOffset[iVertex][2]*iScale);
			}

			//Find which vertices are inside of the surface and which are outside
			iFlagIndex = 0;
			for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
			{
					if(afCubeValue[iVertexTest] <= iso_level) 
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
							fOffset = GetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], _surf_value);

							asEdgeVertex[iEdge][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0] +  fOffset * (float)a2iEdgeDirection[iEdge][0]) * (float)iScale;
							asEdgeVertex[iEdge][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1] +  fOffset * (float)a2iEdgeDirection[iEdge][1]) * (float)iScale;
							asEdgeVertex[iEdge][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] +  fOffset * (float)a2iEdgeDirection[iEdge][2]) * (float)iScale;
				
							vertexIds[iEdge] = mesh->AddHashedVertex(Vector3DFloat(asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2]), GetHashKey(iX, iY, iZ, iEdge, iScale)); 
					}
			}


			//Draw the triangles that were found.  There can be up to five per cube
			for(iTriangle = 0; iTriangle < 5; iTriangle++)
			{
					if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
							break;
					Point3 pts[3];
					int triangleVertices[3];
					for(iCorner = 0; iCorner < 3; iCorner++)
					{
						iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
						triangleVertices[iCorner] = vertexIds[iVertex];
					}

					mesh->AddTriangle(triangleVertices[0], triangleVertices[1], triangleVertices[2], false, false);
			}
		}

		void VolumeRenderer::SetSampleInterval(const int size) {
			_sample = size;
			if(viewingType == VIEWING_TYPE_ISO_SURFACE) {
				CalculateSurface();
			} else if (viewingType == VIEWING_TYPE_CROSS_SECTION) {
				CalculateCuttingSurface();
			}
		}

		void VolumeRenderer::SetSurfaceValue(const float value) {
			_surf_value = value;
			CalculateSurface();
		}


		void VolumeRenderer::Unload() {
			Renderer::Unload();
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

		Volume * VolumeRenderer::PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize) {
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
			Volume * outputVol = skeletonizer->PerformPureJuSkeletonization(_voxel, "", threshold, minCurveSize, minSurfaceSize);
			delete skeletonizer;
			return outputVol;
		}

		Volume * VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonRadius) {
			double stepSize = (_voxel->getMax() - startDensity) / stepCount;
			if(!isZero(stepSize)) {
				VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, curveRadius, surfaceRadius, skeletonRadius);
				Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(_voxel, startDensity, _voxel->getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
				delete skeletonizer;
				return outputVol;
			} else {
				return NULL;
			}
		}
		Volume * VolumeRenderer::GetVolume() {
			return _voxel;
		}
	}
}

#endif