#ifndef GORGON_MARCHING_CUBES_H
#define GORGON_MARCHING_CUBES_H

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <glut.h>
#include <glext.h>
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



using namespace std;

using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {
		const int VIEWING_TYPE_ISO_SURFACE = 0;
		const int VIEWING_TYPE_CROSS_SECTION = 1;
		const int VIEWING_TYPE_SOLID = 2;
		#ifndef _WIN32
			typedef int PFNGLTEXIMAGE3DPROC;
		#endif

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
			void SetDisplayRadius(const int radius);
			void SetViewingType(const int type);
			void SetSampleInterval(const int size);
			void SetSurfaceValue(const float value);
			bool SetCuttingPlane(float position, float vecX, float vecY, float vecZ);
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
			bool CalculateSurface();
			bool CalculateCuttingSurface();
			bool CalculateSolidRendering();
			bool CalculateDisplay();
			void Load3DTexture();
			void MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale);
			int Smallest2ndPower(int value);
		private:
			bool textureLoaded;
			int textureSizeX, textureSizeY, textureSizeZ;
			GLuint textureName;
			float surfaceValue;
			int sampleInterval;
			int displayRadius;
			int viewingType;			
			Volume * dataVolume;
			Volume * cuttingVolume;
			NonManifoldMesh_NoTags * surfaceMesh;
			NonManifoldMesh_NoTags * cuttingMesh;
			#ifdef _WIN32
				PFNGLTEXIMAGE3DPROC glTexImage3D;
			#endif
		};

		VolumeRenderer::VolumeRenderer() {
			textureLoaded = false;
			viewingType = VIEWING_TYPE_ISO_SURFACE;
			surfaceMesh = new NonManifoldMesh_NoTags();
			dataVolume = NULL;
			surfaceValue = 1.5;
			displayRadius = 1;
			sampleInterval = 1;
			cuttingVolume = new Volume(2, 2, 2);
			cuttingMesh = new NonManifoldMesh_NoTags();
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
			return "Volumes (*.mrc)";
		}

		string VolumeRenderer::GetSupportedSaveFileFormats() {
			return "Volumes (*.mrc);;Bitmap Image set (*.bmp)";
		}

		void VolumeRenderer::SetViewingType(const int type) {
			viewingType = type;
			if((viewingType == VIEWING_TYPE_SOLID) || (viewingType == VIEWING_TYPE_CROSS_SECTION)) {
				Load3DTexture();
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
		void VolumeRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if((viewingType == VIEWING_TYPE_ISO_SURFACE) && (surfaceMesh != NULL)) {
					surfaceMesh->Draw(true, false, false, selectEnabled, false, false);
				} else if((viewingType == VIEWING_TYPE_CROSS_SECTION) || (viewingType == VIEWING_TYPE_SOLID)) {
					glPushAttrib(GL_ENABLE_BIT);
					glDisable(GL_LIGHTING);
					glDisable(GL_CULL_FACE);
					Vector3DFloat vertex;
					// The outside box
					if((viewingType == VIEWING_TYPE_CROSS_SECTION)) {
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


					float material[4];
					float alphaValue = 0.9;
					if(viewingType == VIEWING_TYPE_SOLID) {
						glGetMaterialfv(GL_FRONT, GL_DIFFUSE, material);
						material[3] = alphaValue;
						glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
						glGetMaterialfv(GL_FRONT, GL_AMBIENT, material);
						material[3] = alphaValue;
						glMaterialfv(GL_FRONT, GL_AMBIENT, material);
						glGetMaterialfv(GL_FRONT, GL_SPECULAR, material);
						material[3] = alphaValue;
						glMaterialfv(GL_FRONT, GL_SPECULAR, material);

						glGetMaterialfv(GL_BACK, GL_DIFFUSE, material);
						material[3] = alphaValue;
						glMaterialfv(GL_BACK, GL_DIFFUSE, material);
						glGetMaterialfv(GL_BACK, GL_AMBIENT, material);
						material[3] = alphaValue;
						glMaterialfv(GL_BACK, GL_AMBIENT, material);
						glGetMaterialfv(GL_BACK, GL_SPECULAR, material);
						material[3] = alphaValue;
						glMaterialfv(GL_BACK, GL_SPECULAR, material);
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

		bool VolumeRenderer::CalculateSurface() {
			surfaceMesh->Clear();
			bool redraw = false;
			if(dataVolume != NULL) {
				redraw = true;
				int iX, iY, iZ;
				int maxX = dataVolume->getSizeX();
				int maxY = dataVolume->getSizeY();
				int maxZ = dataVolume->getSizeZ();
				for(iX = max(maxX/2 - displayRadius, 0); iX < min(maxX, maxX/2 + displayRadius); iX+=sampleInterval) {
					for(iY = max(maxY/2 - displayRadius, 0); iY < min(maxY, maxY/2 + displayRadius); iY+=sampleInterval) {
						for(iZ = max(maxZ/2 - displayRadius, 0); iZ < min(maxZ, maxZ/2 + displayRadius); iZ+=sampleInterval) {
							MarchingCube(dataVolume, surfaceMesh, surfaceValue, iX, iY, iZ, sampleInterval);
						}
					}
				}
			}
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
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			dataVolume = VolumeFormatConverter::LoadVolume(fileName);
			UpdateBoundingBox();

			#ifdef _WIN32
				glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
			#endif

		}

		void VolumeRenderer::Load3DTexture() {
			if(textureLoaded) {
				glDeleteTextures(1, &textureName);
				textureLoaded = false;
			}

			if(dataVolume != NULL) {
				textureSizeX = Smallest2ndPower(dataVolume->getSizeX());
				textureSizeY = Smallest2ndPower(dataVolume->getSizeY());
				textureSizeZ = Smallest2ndPower(dataVolume->getSizeZ());
				double maxVal = dataVolume->getMax();
				double minVal = surfaceValue;
				unsigned char val;

				unsigned char * texels = new unsigned char[textureSizeX * textureSizeY * textureSizeZ];
				unsigned int pos = 0;
				for(int z = 0; z < textureSizeZ; z++) {
					for(int y = 0; y < textureSizeY; y++) {
						for(int x = 0; x < textureSizeX; x++) {
							if((x < dataVolume->getSizeX()) && (y < dataVolume->getSizeY()) && (z < dataVolume->getSizeZ())) {
								val = (unsigned char)round((max(dataVolume->getDataAt(x, y, z), minVal) - minVal) * 255.0 / (maxVal - minVal));
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
					glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, textureSizeX, textureSizeY, textureSizeZ, 0, GL_RED, GL_UNSIGNED_BYTE, texels);
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
				} else if(strcmp(extension.c_str(), "BMP") == 0) {
					ImageReaderBMP::SaveVolumeAsImageSet(dataVolume, fileName);
				} else {
					printf("Input format %s not supported!\n", extension.c_str());
				}

				
			}
		}
		void VolumeRenderer::MarchingCube(Volume * vol, NonManifoldMesh_NoTags * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
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
			if(viewingType == VIEWING_TYPE_CROSS_SECTION)
			{
				printf("\n ");
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
					Load3DTexture();
					break;
				case VIEWING_TYPE_SOLID:
					Load3DTexture();
					break;
			}
		}


		void VolumeRenderer::SetDisplayRadius(const int radius) {
			displayRadius = radius;
			switch(viewingType) {
				case VIEWING_TYPE_ISO_SURFACE:
					CalculateSurface();
					break;
			}
		}
		void VolumeRenderer::Unload() {
			Renderer::Unload();
			if(dataVolume != NULL) {
				delete dataVolume;
			}
			dataVolume = NULL;
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
				Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(dataVolume, startDensity, dataVolume->getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
				delete skeletonizer;
				return outputVol;
			} else {
				return NULL;
			}
		}
		Volume * VolumeRenderer::GetVolume() {
			return dataVolume;
		}
	}
}

#endif
