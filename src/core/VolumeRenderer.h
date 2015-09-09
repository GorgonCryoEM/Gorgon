// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Renderer for rendering volumetric data sets


#ifndef GORGON_MARCHING_CUBES_H
#define GORGON_MARCHING_CUBES_H

#define GL_GLEXT_PROTOTYPES
//#define USE_OCTREE_OPTIMIZATION


#include <iostream>
#include <glut.h>
#ifndef __APPLE__
	#include <glExt.h>
#endif
#include <string>
#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
#include "GlobalConstants.h"
#include "MeshRenderer.h"
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/GlobalDefinitions.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>
#include <ProteinMorph/NonManifoldMesh.h>
#include <ProteinMorph/TriangleMesh.h>
#include <MathTools/Vector3D.h>
#include <Foundation/Octree.h>
#include <queue>

using namespace std;

using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::SkeletonMaker;

namespace wustl_mm {
	namespace Visualization {
		const int VIEWING_TYPE_ISO_SURFACE = 0;
		const int VIEWING_TYPE_CROSS_SECTION = 1;
		const int VIEWING_TYPE_SOLID = 2;
		#ifndef _WIN32
			typedef int PFNGLTEXIMAGE3DPROC;
		#endif

		typedef Octree<OctreeProjectionTestMinMaxStruct> VolumeRendererOctreeType;
		typedef OctreeNode<OctreeProjectionTestMinMaxStruct> VolumeRendererOctreeNodeType;
		typedef TriangleMesh<bool, bool> VolumeSurfaceMeshType;

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
			void EnableDraw(bool enable);
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void LoadFileRAW(string fileName, int bitsPerCell, int sizeX, int sizeY, int sizeZ);
			void SaveFile(string fileName);
			void SetDisplayRadius(const int radius);
			void SetDisplayRadiusOrigin(float radiusOriginX, float radiusOriginY, float radiusOriginZ);
			void UseDisplayRadius(bool useRadius);
			void SetViewingType(const int type);
			void SetSampleInterval(const int size);
			void SetSurfaceValue(const float value);
			void SetMaxSurfaceValue(const float value);
			bool SetCuttingPlane(float position, float vecX, float vecY, float vecZ);
			void UpdateBoundingBox() ;
			void Unload();
			void NormalizeVolume();
			void DownsampleVolume();
			void CropVolume(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
			void PerformSmoothLaplacian(double convergenceRate, int iterations);
			Volume * GetVolume();
			Volume * PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize);
			Volume * PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonSmoothenRadius);
			Volume * PerformPreservingGrayscaleSkeletonizationAbeysinghe2008(NonManifoldMesh_Annotated * preserveMesh, double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonSmoothenRadius);
			void SetSpacing(float spX, float spY, float spZ);
			float GetSpacingX();
			float GetSpacingY();
			float GetSpacingZ();
			void SetOrigin(float orgX, float orgY, float orgZ);			
			float GetOriginX();
			float GetOriginY();
			float GetOriginZ();
		
		private:
			int GetHashKey(int x, int y, int z, int edge, int iScale);
			float GetVoxelData(Volume * vol, int x, int y, int z);
			float GetVoxelData(Volume * vol, float x, float y, float z);
			float GetOffset(float fValue1, float fValue2, float fValueDesired);
			bool CalculateSurface();
			bool CalculateCuttingSurface();
			bool CalculateSolidRendering();
			bool CalculateDisplay();
			void Load3DTextureSolidRendering();
			void Load3DTextureCrossSection();
			void InitializeOctree();
			void InitializeOctreeTag(VolumeRendererOctreeNodeType * node);
			void CalculateOctreeNode(VolumeRendererOctreeNodeType * node);
			void MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale);
			void MarchingCube(Volume * vol, VolumeSurfaceMeshType * mesh, const float iso_level, int iX, int iY, int iZ, int iScale);
			int Smallest2ndPower(int value);
		private:
			int marchingCubeCallCount;
			bool drawEnabled;
			bool textureLoaded;
			int textureSizeX, textureSizeY, textureSizeZ;
			GLuint textureName;
			float surfaceValue;
			float maxSurfaceValue;
			int sampleInterval;
			int displayRadius;
			int viewingType;			
			Volume * dataVolume;
			Volume * cuttingVolume;
			Vector3DFloat radiusOrigin;
			bool useDisplayRadius;
			
			VolumeSurfaceMeshType * surfaceMesh;
			NonManifoldMesh_NoTags * cuttingMesh;
			VolumeRendererOctreeType * octree;
			#ifdef _WIN32
				PFNGLTEXIMAGE3DPROC glTexImage3D;
			#endif
		};

		VolumeRenderer::VolumeRenderer() {
			textureLoaded = false;
			useDisplayRadius = false;
			viewingType = VIEWING_TYPE_ISO_SURFACE;
			surfaceMesh = new VolumeSurfaceMeshType();
			dataVolume = NULL;
			octree = NULL;
			surfaceValue = 1.5;
			displayRadius = 1;
			sampleInterval = 1;
			cuttingVolume = new Volume(2, 2, 2);
			cuttingMesh = new NonManifoldMesh_NoTags();
			drawEnabled = false;
		}

		VolumeRenderer::~VolumeRenderer() {
			if(textureLoaded) {
				glDeleteTextures(1, &textureName);
				textureLoaded = false;
			}
			delete surfaceMesh;
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			if(octree != NULL) {
				delete octree;
			}
			delete cuttingMesh;
			delete cuttingVolume;
		}

		float VolumeRenderer::GetMaxDensity(){
			return dataVolume->getMax();
		}

		float VolumeRenderer::GetMinDensity() {
			return dataVolume->getMin();
		}

		float VolumeRenderer::GetOffset(float fValue1, float fValue2, float fValueDesired) {
			double fDelta = fValue2 - fValue1;
			if(fDelta == 0.0) {
					return 0.5;
			}
			return (fValueDesired - fValue1)/fDelta;
		}



		float VolumeRenderer::GetSurfaceValue() const { 
			return surfaceValue; 
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
			return x * dataVolume->getSizeY() * dataVolume->getSizeZ() * 3 + y * dataVolume->getSizeZ() * 3 + z * 3 + edge;
		}

		int VolumeRenderer::GetSampleInterval() const  { 
			return sampleInterval; 
		}

		int VolumeRenderer::Smallest2ndPower(int value) {
			int power = 1;
			while (power < value) {
				power = power * 2;
			}
			return power;
		}
		string VolumeRenderer::GetSupportedLoadFileFormats() {
			return "All Files (*.mrc *.ccp4 *.map *.raw *.pts);; Volumes (*.mrc *.ccp4 *.map *.raw);;Point Cloud (*.pts)";
		}

		string VolumeRenderer::GetSupportedSaveFileFormats() {
			return "Volumes (*.mrc *.ccp4 *.map *.raw);;Mathematica List (*.nb);;Bitmap Image set (*.bmp);;Structure Tensor Field (*.tns);;Surface Mesh(*.off)";
		}

		void VolumeRenderer::EnableDraw(bool enable) {			
			if(drawEnabled != enable) {
				drawEnabled = enable;
				if(drawEnabled) {
					CalculateDisplay();
				}
			}
		}
		void VolumeRenderer::SetViewingType(const int type) {
			viewingType = type;
			if(viewingType == VIEWING_TYPE_SOLID) {
				Load3DTextureSolidRendering();
			} else if  (viewingType == VIEWING_TYPE_CROSS_SECTION) {
				Load3DTextureCrossSection();
			}
			CalculateDisplay();
		}

		bool VolumeRenderer::SetCuttingPlane(float position, float vecX, float vecY, float vecZ) {
			Renderer::SetCuttingPlane(position, vecX, vecY, vecZ);
			bool redraw = false;
			if((viewingType == VIEWING_TYPE_CROSS_SECTION) || (viewingType == VIEWING_TYPE_SOLID)) {
				redraw = CalculateDisplay();
			}
			return redraw;
		}
		void VolumeRenderer::DownsampleVolume() {
			Volume * sourceVol = dataVolume;
			Volume * destVol = new Volume(sourceVol->getSizeX()/2, sourceVol->getSizeY()/2, sourceVol->getSizeZ()/2);
			double val;

			int radius = 1;
			MathLib * math = new MathLib();

			ProbabilityDistribution3D gaussianFilter;
			gaussianFilter.radius = radius;
			math->GetBinomialDistribution(gaussianFilter);

			for(int x = radius; x < destVol->getSizeX()-radius; x++) {
				for(int y = radius; y < destVol->getSizeY()-radius; y++) {
					for(int z = radius; z < destVol->getSizeZ()-radius; z++) {
						val = 0;
						for(int xx = -radius; xx <= radius; xx++) {
							for(int yy = -radius; yy <= radius; yy++) {
								for(int zz = -radius; zz <= radius; zz++) {
									val += sourceVol->getDataAt(2*x+xx, 2*y+yy, 2*z+zz) * gaussianFilter.values[xx+radius][yy+radius][zz+radius] ;
								}
							}
						}
						destVol->setDataAt(x, y, z, val);					
					}
				}
			}

			delete math;
			delete sourceVol;
			dataVolume = destVol;
			InitializeOctree();
			UpdateBoundingBox();
		}


		void VolumeRenderer::CropVolume(int startX, int startY, int startZ, int endX, int endY, int endZ) {
			Volume * destVol = new Volume(endX-startX+1, endY-startY+1, endZ-startZ+1, startX, startY, startZ, dataVolume);
			delete dataVolume;
			dataVolume = destVol;
			InitializeOctree();
			UpdateBoundingBox();
		}

		void VolumeRenderer::InitializeOctree() {
			#ifdef USE_OCTREE_OPTIMIZATION
				if(octree != NULL) {
					delete octree;
				}
				unsigned int sizeX = dataVolume->getSizeX();
				unsigned int sizeY = dataVolume->getSizeY();
				unsigned int sizeZ = dataVolume->getSizeZ();
				octree  = new VolumeRendererOctreeType(sizeX, sizeY, sizeZ);
				for(unsigned int x = 0; x < sizeX-1; x++) {
					for(unsigned int y = 0; y < sizeY-1; y++) {
						for(unsigned int z = 0; z < sizeZ-1; z++) {
							octree->AddNewLeaf(x, y, z, 1);
						}
					}
				}
				InitializeOctreeTag(octree->GetRoot());
				float minVal, maxVal, val;
				VolumeRendererOctreeNodeType * node;

				for(unsigned int x = 0; x < sizeX-1; x++) {
					for(unsigned int y = 0; y < sizeY-1; y++) {
						for(unsigned int z = 0; z < sizeZ-1; z++) {
							node = octree->GetLeaf(x, y, z);
							minVal = MAX_FLOAT;
							maxVal = MIN_FLOAT;
							for(unsigned int xx = 0; xx < 2; xx++) {
								for(unsigned int yy = 0; yy < 2; yy++) {
									for(unsigned int zz = 0; zz < 2; zz++) {
										val = dataVolume->getDataAt(x+xx, y+yy, z+zz);
										minVal = min(minVal, val);
										maxVal = max(maxVal, val);
									}
								}
							}

							while(node != NULL) {
								node->tag.maxVal = max(node->tag.maxVal, maxVal);
								node->tag.minVal = min(node->tag.minVal, minVal);
								node = node->parent;
							}
						}
					}
				}
			#endif
		}

		void VolumeRenderer::InitializeOctreeTag(VolumeRendererOctreeNodeType * node) {
			if(node != NULL) {
				OctreeProjectionTestMinMaxStruct tag;
				tag.maxVal = MIN_FLOAT;
				tag.minVal = MAX_FLOAT;
				node->tag = tag;
				if (!node->isLeaf) {
					for(int i = 0; i < 8; i++) {
						InitializeOctreeTag(node->children[i]);
					}
				}
			}
		}

		void VolumeRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if((viewingType == VIEWING_TYPE_ISO_SURFACE) && (surfaceMesh != NULL)) {
					surfaceMesh->Draw(true, selectEnabled, useDisplayRadius, displayRadius, radiusOrigin);
				} else if((viewingType == VIEWING_TYPE_CROSS_SECTION) || (viewingType == VIEWING_TYPE_SOLID)) {
					glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
					glDisable(GL_LIGHTING);
					glDisable(GL_CULL_FACE);
					Vector3DFloat vertex;
					// The outside box

					if(viewingType == VIEWING_TYPE_CROSS_SECTION) {
						glBegin(GL_LINES);
						for(unsigned int i = 0; i < cuttingMesh->edges.size(); i++) {
							if(cuttingMesh->edges[i].faceIds.size() == 1) {
								
								for(unsigned int j = 0; j < 2; j++) {
									vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->edges[i].vertexIds[j])].position;
									glVertex3f(vertex.X() * (float)dataVolume->getSizeX(), vertex.Y() * (float)dataVolume->getSizeY(), vertex.Z() * (float)dataVolume->getSizeZ());
								}
							}
						}
						glEnd();
					}


					// The cutting surface
					glEnable(GL_TEXTURE_3D);
					//GLboolean resident;
					//glAreTexturesResident(1, &textureName, &resident);
					//if(resident) {
						glBindTexture(GL_TEXTURE_3D, textureName);

						double xRatio = (double)dataVolume->getSizeX() / (double)textureSizeX;
						double yRatio = (double)dataVolume->getSizeY() / (double)textureSizeY;
						double zRatio = (double)dataVolume->getSizeZ() / (double)textureSizeZ;

						for(unsigned int i = 0; i < cuttingMesh->faces.size(); i++) {
							glBegin(GL_POLYGON);
							for(unsigned int j = 0; j < cuttingMesh->faces[i].vertexIds.size(); j++) {
								vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->faces[i].vertexIds[j])].position;
								glTexCoord3d(vertex.X() * xRatio, vertex.Y()* yRatio, vertex.Z() * zRatio);
								glVertex3f(vertex.X() * (float)dataVolume->getSizeX(), vertex.Y() * (float)dataVolume->getSizeY(), vertex.Z() * (float)dataVolume->getSizeZ());
							}
							glEnd();
						}
					//}
					
					glPopAttrib();
				}
			}
		}


		void VolumeRenderer::CalculateOctreeNode(VolumeRendererOctreeNodeType * node) {
			queue<VolumeRendererOctreeNodeType *> q;
			q.push(node);

			while(!q.empty()) {
				node = q.front();
				q.pop();
				if((node->tag.minVal <= surfaceValue) && (node->tag.maxVal >= surfaceValue)) {
					if((int)node->cellSize <= sampleInterval + sampleInterval) {
						for(int i = 0; i < 8; i++) {
							if(node->children[i] != NULL) {
								MarchingCube(dataVolume, surfaceMesh, surfaceValue, node->children[i]->pos[0], node->children[i]->pos[1], node->children[i]->pos[2], sampleInterval);
							}
						}						
					} else {
						for(int i = 0; i < 8; i++) {
							if(node->children[i] != NULL) {
								q.push(node->children[i]);
							}
						}
					}
				}
			}
		}

		bool VolumeRenderer::CalculateSurface() {
			bool redraw = false;
			#ifndef USE_OCTREE_OPTIMIZATION
				//appTimeManager.PushCurrentTime();
				//appTimeManager.PushCurrentTime();
				surfaceMesh->Clear();
				//appTimeManager.PopAndDisplayTime("Marching Cubes)  Clearing : %f seconds |");
				redraw = false;
				marchingCubeCallCount = 0;
				if(drawEnabled && dataVolume != NULL) {
					redraw = true;
					int iX, iY, iZ;
					int maxX = dataVolume->getSizeX();
					int maxY = dataVolume->getSizeY();
					int maxZ = dataVolume->getSizeZ();
					for(iX = 0; iX < maxX; iX+=sampleInterval) {
						for(iY = 0; iY < maxY; iY+=sampleInterval) {
							for(iZ = 0; iZ < maxZ; iZ+=sampleInterval) {
								MarchingCube(dataVolume, surfaceMesh, surfaceValue, iX, iY, iZ, sampleInterval);
							}
						}
					}
					/*for(iX = max(radiusOrigin.X() - displayRadius, 0); iX < min(maxX, radiusOrigin.X() + displayRadius); iX+=sampleInterval) {
						for(iY = max(radiusOrigin.Y() - displayRadius, 0); iY < min(maxY, radiusOrigin.Y() + displayRadius); iY+=sampleInterval) {
							for(iZ = max(radiusOrigin.Z() - displayRadius, 0); iZ < min(maxZ, radiusOrigin.Z() + displayRadius); iZ+=sampleInterval) {
								MarchingCube(dataVolume, surfaceMesh, surfaceValue, iX, iY, iZ, sampleInterval);
							}
						}
					}*/
				}
				//appTimeManager.PopAndDisplayTime("Meshing: %f seconds |");
			#else 
				appTimeManager.PushCurrentTime();
				appTimeManager.PushCurrentTime();
				surfaceMesh->Clear();
				appTimeManager.PopAndDisplayTime("Octree)          Clearing : %f seconds |");
				marchingCubeCallCount = 0;
				redraw = false;
				if(drawEnabled && dataVolume != NULL && octree != NULL) {
					redraw = true;
					CalculateOctreeNode(octree->GetRoot());
				}

				appTimeManager.PopAndDisplayTime("Meshing: %f seconds |");
			#endif
			//printf("Marching Cubes called %d times\n", marchingCubeCallCount); 
			return redraw;
			
		}

		bool VolumeRenderer::CalculateCuttingSurface() {
			cuttingMesh->Clear();

			bool redraw = false;

			if(dataVolume != NULL) {
				redraw = true;

				if((cuttingPlaneCenter.X() >= minPts[0]) && (cuttingPlaneCenter.X() <= maxPts[0]) &&
					(cuttingPlaneCenter.Y() >= minPts[1]) && (cuttingPlaneCenter.Y() <= maxPts[1]) &&
					(cuttingPlaneCenter.Z() >= minPts[2]) && (cuttingPlaneCenter.Z() <= maxPts[2])) {


					int iX, iY, iZ;

					for(iX = 0; iX < 2; iX++) {
						for(iY = 0; iY < 2; iY++) {
							for(iZ = 0; iZ < 2; iZ++) {
								cuttingVolume->setDataAt(iX, iY, iZ, (cuttingPlaneCenter - Vector3DFloat(iX * dataVolume->getSizeX(), iY * dataVolume->getSizeY(), iZ * dataVolume->getSizeZ()))* cuttingPlaneDirection);
							}
						}
					}
					MarchingCube(cuttingVolume, cuttingMesh, 0.0f, 0, 0, 0, 1);	
				}
			}
			return redraw;
		}


		bool VolumeRenderer::CalculateSolidRendering() {
			cuttingMesh->Clear();
			bool redraw = false;
			if(dataVolume != NULL) {
				redraw = true;
				NonManifoldMesh_NoTags tempMesh;

				
				Vector3DFloat modelCenter = Vector3DFloat( (minPts[0] + maxPts[0])/2.0, (minPts[1] + maxPts[1])/2.0, (minPts[2] + maxPts[2])/2.0);
				Vector3DFloat center;
				float distance = (Vector3DFloat(minPts[0], minPts[1], minPts[2]) - modelCenter).Length();
				int iX, iY, iZ;

				for(float position = 1.0; position >= -1.0; position -= 0.01) {
					center = modelCenter + cuttingPlaneDirection * position * distance;

					if((center.X() >= minPts[0]) && (center.X() <= maxPts[0]) &&
						(center.Y() >= minPts[1]) && (center.Y() <= maxPts[1]) &&
						(center.Z() >= minPts[2]) && (center.Z() <= maxPts[2])) {


						for(iX = 0; iX < 2; iX++) {
							for(iY = 0; iY < 2; iY++) {
								for(iZ = 0; iZ < 2; iZ++) {
									cuttingVolume->setDataAt(iX, iY, iZ, (center - Vector3DFloat(iX * dataVolume->getSizeX(), iY * dataVolume->getSizeY(), iZ * dataVolume->getSizeZ()))* cuttingPlaneDirection);
								}
							}
						}
						tempMesh.Clear();
						MarchingCube(cuttingVolume, &tempMesh, 0.0f, 0, 0, 0, 1);	
						cuttingMesh->MergeMesh(&tempMesh);
					}
				}
			}
			return redraw;
		}
		bool VolumeRenderer::CalculateDisplay() {
			bool redraw = false;
			switch (viewingType) {
				case VIEWING_TYPE_ISO_SURFACE:
					redraw = CalculateSurface();
					break;
				case VIEWING_TYPE_CROSS_SECTION:
					redraw = CalculateCuttingSurface();
					break;
				case VIEWING_TYPE_SOLID:
					redraw = CalculateSolidRendering();
					break;
			}
			return redraw;
		}

		void VolumeRenderer::LoadFile(string fileName) {
			Renderer::LoadFile(fileName);
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			dataVolume = VolumeFormatConverter::LoadVolume(fileName);
			InitializeOctree();
			UpdateBoundingBox();

			#ifdef _WIN32
				glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
			#endif

			SetDisplayRadiusOrigin(dataVolume->getSizeX()/2, dataVolume->getSizeY()/2, dataVolume->getSizeZ()/2);
		}

		void VolumeRenderer::LoadFileRAW(string fileName, int bitsPerCell, int sizeX, int sizeY, int sizeZ) {
			Renderer::LoadFile(fileName);
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			dataVolume = VolumeFormatConverter::LoadVolume(fileName, bitsPerCell, sizeX, sizeY, sizeZ);
			InitializeOctree();
			UpdateBoundingBox();

			#ifdef _WIN32
				glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
			#endif

			SetDisplayRadiusOrigin(dataVolume->getSizeX()/2, dataVolume->getSizeY()/2, dataVolume->getSizeZ()/2);
		}

		void VolumeRenderer::Load3DTextureSolidRendering() {
			if(textureLoaded) {
				glDeleteTextures(1, &textureName);
				textureLoaded = false;
			}

			if(dataVolume != NULL) {
				textureSizeX = Smallest2ndPower(dataVolume->getSizeX());
				textureSizeY = Smallest2ndPower(dataVolume->getSizeY());
				textureSizeZ = Smallest2ndPower(dataVolume->getSizeZ());
				double maxVal = maxSurfaceValue;
				double minVal = surfaceValue;
				unsigned char val;

				unsigned char * texels = new unsigned char[textureSizeX * textureSizeY * textureSizeZ];
				unsigned int pos = 0;
				for(int z = 0; z < textureSizeZ; z++) {
					for(int y = 0; y < textureSizeY; y++) {
						for(int x = 0; x < textureSizeX; x++) {
							if((x < dataVolume->getSizeX()) && (y < dataVolume->getSizeY()) && (z < dataVolume->getSizeZ())) {
								val = (unsigned char)round((min(max(dataVolume->getDataAt(x, y, z), minVal), maxVal) - minVal) * 255.0 / (maxVal - minVal));
							} else {
								val = 0;
							}
							texels[pos] = val;
							pos++;
						}
					}
				}
				glGenTextures(1, &textureName);
				glBindTexture(GL_TEXTURE_3D, textureName);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				try {
					glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA, textureSizeX, textureSizeY, textureSizeZ, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texels);
					textureLoaded = true;
				}   catch (int) {
					textureLoaded = false;
				}
				delete [] texels;

			}

		}

		void VolumeRenderer::Load3DTextureCrossSection() {
			if(textureLoaded) {
				glDeleteTextures(1, &textureName);
				textureLoaded = false;
			}

			if(dataVolume != NULL) {
				textureSizeX = Smallest2ndPower(dataVolume->getSizeX());
				textureSizeY = Smallest2ndPower(dataVolume->getSizeY());
				textureSizeZ = Smallest2ndPower(dataVolume->getSizeZ());
				double maxVal = maxSurfaceValue;
				double minVal = surfaceValue;
				unsigned char val;

				// Approximations to avoid division by zero
				if(isZero(minVal - maxVal, 0.000000000001)) {
					maxVal = minVal + (dataVolume->getMax() - dataVolume->getMin()) / 1000.0;
				}
				if(isZero(minVal - maxVal, 0.000000000001)) {
					maxVal = minVal + 0.0001;
				}

				unsigned char * texels = new unsigned char[textureSizeX * textureSizeY * textureSizeZ];
				unsigned int pos = 0;
				for(int z = 0; z < textureSizeZ; z++) {
					for(int y = 0; y < textureSizeY; y++) {
						for(int x = 0; x < textureSizeX; x++) {
							if((x < dataVolume->getSizeX()) && (y < dataVolume->getSizeY()) && (z < dataVolume->getSizeZ())) {
								val = (unsigned char)round((min(max(dataVolume->getDataAt(x, y, z), minVal), maxVal) - minVal) * 255.0 / (maxVal - minVal));
							} else {
								val = 0;
							}
							texels[pos] = val;
							pos++;
						}
					}
				}
				glGenTextures(1, &textureName);
				glBindTexture(GL_TEXTURE_3D, textureName);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				try {
					glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, textureSizeX, textureSizeY, textureSizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texels);
					textureLoaded = true;
				}   catch (int) {
					textureLoaded = false;
				}
				delete [] texels;

			}

		}

		void VolumeRenderer::SaveFile(string fileName) {
			if(dataVolume != NULL) {
				int pos = fileName.rfind(".") + 1;
				string extension = fileName.substr(pos, fileName.length()-pos);
				
				extension = StringUtils::StringToUpper(extension);
				
				if(strcmp(extension.c_str(), "MRC") == 0) {
					dataVolume->toMRCFile((char *)fileName.c_str());
				} else if(strcmp(extension.c_str(), "CCP4") == 0) {
					dataVolume->toMRCFile((char *)fileName.c_str());
				} else if(strcmp(extension.c_str(), "RAW") == 0) {
					VolumeReaderRAW::SaveVolume16bit(dataVolume, fileName);
				} else if(strcmp(extension.c_str(), "NB") == 0) {
					dataVolume->toMathematicaFile((char *)fileName.c_str());
				} else if(strcmp(extension.c_str(), "TNS") == 0) {
					VolumeReaderTNS::SaveVolume(dataVolume, fileName);
				} else if(strcmp(extension.c_str(), "BMP") == 0) {
					ImageReaderBMP::SaveVolumeAsImageSet(dataVolume, fileName);
				} else if(strcmp(extension.c_str(), "OFF") == 0) {
					surfaceMesh->SaveFile(fileName);
				} else {
					printf("Input format %s not supported!\n", extension.c_str());
				}

				
			}
		}
		void VolumeRenderer::MarchingCube(Volume * vol, VolumeSurfaceMeshType * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
			marchingCubeCallCount++;
			extern int aiCubeEdgeFlags[256];
			extern int a2iTriangleConnectionTable[256][16];

			int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
			float fOffset;
			float afCubeValue[8];
			Vector3 asEdgeVertex[12];
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
							fOffset = GetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], iso_level);

							asEdgeVertex[iEdge][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0] +  fOffset * (float)a2iEdgeDirection[iEdge][0]) * (float)iScale;
							asEdgeVertex[iEdge][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1] +  fOffset * (float)a2iEdgeDirection[iEdge][1]) * (float)iScale;
							asEdgeVertex[iEdge][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] +  fOffset * (float)a2iEdgeDirection[iEdge][2]) * (float)iScale;
				
							vertexIds[iEdge] = mesh->AddVertex(TriangleMeshVertex<bool>(Vector3DFloat(asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2])), GetHashKey(iX, iY, iZ, iEdge, iScale));
					}
			}


			//Draw the triangles that were found.  There can be up to five per cube
			for(iTriangle = 0; iTriangle < 5; iTriangle++)
			{
					if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
							break;
					int triangleVertices[3];
					for(iCorner = 0; iCorner < 3; iCorner++)
					{
						iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
						triangleVertices[iCorner] = vertexIds[iVertex];
					}

					mesh->AddFace(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
			}
		}

		void VolumeRenderer::MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
			marchingCubeCallCount++;
			extern int aiCubeEdgeFlags[256];
			extern int a2iTriangleConnectionTable[256][16];

			int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
			float fOffset;
			float afCubeValue[8];
			Vector3 asEdgeVertex[12];
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
							fOffset = GetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], iso_level);

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
					int triangleVertices[3];
					for(iCorner = 0; iCorner < 3; iCorner++)
					{
						iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
						triangleVertices[iCorner] = vertexIds[iVertex];
					}

					mesh->AddTriangle(triangleVertices[0], triangleVertices[1], triangleVertices[2], false, false);
			}
		}


		void VolumeRenderer::NormalizeVolume(){
			dataVolume->normalize(0, 1);
		}
		void VolumeRenderer::SetSampleInterval(const int size) {
			sampleInterval = size;
			if(viewingType == VIEWING_TYPE_ISO_SURFACE) {
				CalculateSurface();
			} else if (viewingType == VIEWING_TYPE_CROSS_SECTION) {
				CalculateCuttingSurface();
			}
		}

		void VolumeRenderer::SetSurfaceValue(const float value) {
			surfaceValue = value;
			switch(viewingType) {
				case VIEWING_TYPE_ISO_SURFACE:
					CalculateSurface();
					break;
				case VIEWING_TYPE_CROSS_SECTION:
					Load3DTextureCrossSection();
					break;
				case VIEWING_TYPE_SOLID:
					Load3DTextureSolidRendering();
					break;
			}
		}

		void VolumeRenderer::SetMaxSurfaceValue(const float value) {
			maxSurfaceValue = value;
			switch(viewingType) {
				case VIEWING_TYPE_ISO_SURFACE:
					break;
				case VIEWING_TYPE_CROSS_SECTION:
					Load3DTextureCrossSection();
					break;
				case VIEWING_TYPE_SOLID:
					Load3DTextureSolidRendering();
					break;
			}
		}





		void VolumeRenderer::SetDisplayRadius(const int radius) {
			displayRadius = radius;
		}
		void VolumeRenderer::SetDisplayRadiusOrigin(float radiusOriginX, float radiusOriginY, float radiusOriginZ) {
			radiusOrigin = Vector3DFloat(radiusOriginX, radiusOriginY, radiusOriginZ);
		}

		void VolumeRenderer::UseDisplayRadius(bool useRadius) {
			useDisplayRadius = useRadius;
		}

		void VolumeRenderer::PerformSmoothLaplacian(double convergenceRate, int iterations) {
			if(dataVolume != NULL) {
				for(unsigned int i = 0; i < iterations; i++) {
					this->dataVolume->smooth(convergenceRate);
				}

				SetSurfaceValue(surfaceValue);
			}
		}


		void VolumeRenderer::Unload() {
			Renderer::Unload();
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			dataVolume = NULL;
			if(octree != NULL) {
				delete octree;
			}
			octree = NULL;
			if(textureLoaded) {
				glDeleteTextures(1, &textureName);
				textureLoaded = false;
			}
			CalculateSurface();
			UpdateBoundingBox();
		}

		void VolumeRenderer::UpdateBoundingBox() {
			if(dataVolume == NULL) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = 0;
					maxPts[i] = 1;
				}
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = 0;
				}
				maxPts[0] = dataVolume->getSizeX()-1;
				maxPts[1] = dataVolume->getSizeY()-1;
				maxPts[2] = dataVolume->getSizeZ()-1;
			}
		}

		Volume * VolumeRenderer::PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize) {
			VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
			Volume * outputVol = skeletonizer->PerformPureJuSkeletonization(dataVolume, "", threshold, minCurveSize, minSurfaceSize);
			delete skeletonizer;
			return outputVol;
		}

		Volume * VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonRadius) {
			double stepSize = (dataVolume->getMax() - startDensity) / stepCount;
			if(!isZero(stepSize)) {
				VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, curveRadius, surfaceRadius, skeletonRadius);
				Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(dataVolume, NULL, startDensity, dataVolume->getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
				delete skeletonizer;
				return outputVol;
			} else {
				return NULL;
			}
		}

		Volume * VolumeRenderer::PerformPreservingGrayscaleSkeletonizationAbeysinghe2008(NonManifoldMesh_Annotated * preserveMesh, double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonRadius) {
			double stepSize = (dataVolume->getMax() - startDensity) / stepCount;
			if(!isZero(stepSize)) {
				VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, curveRadius, surfaceRadius, skeletonRadius);
				Volume * preserveVol = preserveMesh->ToVolume();
				Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(dataVolume, preserveVol, startDensity, dataVolume->getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);				
				delete skeletonizer;
				delete preserveVol;
				return outputVol;
			} else {
				return NULL;
			}
		}

		Volume * VolumeRenderer::GetVolume() {
			return dataVolume;
		}

		void VolumeRenderer::SetSpacing(float spX, float spY, float spZ) {
			if(dataVolume != NULL) {
				dataVolume->setSpacing(spX, spY, spZ);
			} else {
				Renderer::SetSpacing(spX, spY, spZ);
			}
		}

		float VolumeRenderer::GetSpacingX() {
			if(dataVolume != NULL) {
				return dataVolume->getSpacingX();
			}
			return Renderer::GetSpacingX();
		}

		float VolumeRenderer::GetSpacingY() {
			if(dataVolume != NULL) {
				return dataVolume->getSpacingY();
			}
			return Renderer::GetSpacingY();
		}

		float VolumeRenderer::GetSpacingZ() {
			if(dataVolume != NULL) {
				return dataVolume->getSpacingZ();
			}
			return Renderer::GetSpacingZ();
		}

		void VolumeRenderer::SetOrigin(float orgX, float orgY, float orgZ) {
			if(dataVolume != NULL) {
				dataVolume->setOrigin(orgX, orgY, orgZ);
			} else {
				Renderer::SetOrigin(orgX, orgY, orgZ);
			}
		}

		float VolumeRenderer::GetOriginX() {
			if(dataVolume != NULL) {
				return dataVolume->getOriginX();
			}
			return Renderer::GetOriginX();
		}

		float VolumeRenderer::GetOriginY() {
			if(dataVolume != NULL) {
				return dataVolume->getOriginY();
			}
			return Renderer::GetOriginY();
		}

		float VolumeRenderer::GetOriginZ() {
			if(dataVolume != NULL) {
				return dataVolume->getOriginZ();
			}
			return Renderer::GetOriginZ();
		}
	}
}

#endif
