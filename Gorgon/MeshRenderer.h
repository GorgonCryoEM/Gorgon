#ifndef GORGON_MESH_RENDERER_H
#define GORGON_MESH_RENDERER_H

#include <cstdlib>
#include <cstdio>
#include <glut.h>
#include <string>
#include <SkeletonMaker/volume.h>
#include <SkeletonMaker/reader.h>
#include "ProteinMorph/NonManifoldMesh.h"
#include "GraySkeletonCPP/VolumeFormatConverter.h"
#include "Renderer.h"
#include "Foundation/StringUtils.h"

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace Visualization {	
		typedef NonManifoldMesh<bool, unsigned char, unsigned char> NonManifoldMesh_Annotated;
		class MeshRenderer : public Renderer{
		public:
			MeshRenderer();
			~MeshRenderer();

			NonManifoldMesh_Annotated * GetMesh();
			Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadFile(string fileName);
			void LoadVolume(Volume * sourceVolume);
			void SaveFile(string fileName);
			void Unload();
			void PerformSmoothLaplacian(double convergenceRate, int iterations);
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
			NonManifoldMesh_Annotated * mesh;
		};


		MeshRenderer::MeshRenderer() {
			mesh = NULL;
		}

		MeshRenderer::~MeshRenderer() {
			if(mesh != NULL) {
				delete mesh;
			}
		}

		NonManifoldMesh_Annotated * MeshRenderer::GetMesh() {
			if(mesh == NULL) {
				mesh = new NonManifoldMesh_Annotated();
			}
			return mesh;
		}

		void MeshRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if(mesh != NULL) {
					mesh->Draw(true, true, false, selectEnabled, selectEnabled, false);
				}
			}
		}


		void MeshRenderer::LoadFile(string fileName) {
			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);
			extension = StringUtils::StringToUpper(extension);
			
			if(mesh != NULL) {
				delete mesh;
			}

			if(strcmp(extension.c_str(), "OFF") == 0) {
				mesh = NonManifoldMesh_Annotated::LoadOffFile(fileName);				
			} else if((strcmp(extension.c_str(), "MRC") == 0) || (strcmp(extension.c_str(), "ATOM") == 0)) {
				Volume * volume = VolumeFormatConverter::LoadVolume(fileName);
				mesh = new NonManifoldMesh_Annotated(volume);
				delete volume;			
			} else {
				printf("Input format %s not supported!\n", extension);
			}

			UpdateBoundingBox();
			
		}

		void MeshRenderer::SaveFile(string fileName) {
			if(mesh != NULL) {
				int pos = fileName.rfind(".") + 1;
				string extension = fileName.substr(pos, fileName.length()-pos);
				
				extension = StringUtils::StringToUpper(extension);
				
				if(strcmp(extension.c_str(), "OFF") == 0) {
					mesh->ToOffCells(fileName);
				} else if(strcmp(extension.c_str(), "MRC") == 0) {
					Volume * volume = mesh->ToVolume();
					volume->toMRCFile((char *)fileName.c_str());
					delete volume;			
				} else {
					printf("Input format %s not supported!\n", extension);
				}
			}


			if(mesh != NULL) {
				
			}
		}

		void MeshRenderer::LoadVolume(Volume * sourceVolume) {
			if(mesh != NULL) {
				delete mesh;
			}
			mesh = new NonManifoldMesh_Annotated(sourceVolume);
			UpdateBoundingBox();
		}

		void MeshRenderer::Unload() {
			Renderer::Unload();
			if(mesh != NULL) {
				delete mesh;
				mesh = NULL;
			}
			UpdateBoundingBox();

		}
		void MeshRenderer::PerformSmoothLaplacian(double convergenceRate, int iterations) {
			NonManifoldMesh_Annotated * newMesh = mesh->SmoothLaplacian(convergenceRate, iterations);
			delete mesh;
			mesh = newMesh;
		}
		void MeshRenderer::UpdateBoundingBox() {
			if(mesh != NULL && mesh->vertices.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = mesh->vertices[0].position.values[i];
					maxPts[i] = mesh->vertices[0].position.values[i];
				}

				for(unsigned int i = 1; i < mesh->vertices.size(); i++) {
					for(int j = 0; j < 3; j++) {
						minPts[j] = min(minPts[j], mesh->vertices[i].position.values[j]);
						maxPts[j] = max(maxPts[j], mesh->vertices[i].position.values[j]);
					}
				}
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = -0.5;
					maxPts[i] = 0.5;
				}
			}
		}
		string MeshRenderer::GetSupportedLoadFileFormats() {
			return "Meshes (*.off);;Volumes (*.mrc *.atom);;All Files (*.off *.mrc *.atom)";
		}
		string MeshRenderer::GetSupportedSaveFileFormats() {
			return "Meshes (*.off);;Volumes (*.mrc)";
		}

		Vector3DFloat MeshRenderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Vector3DFloat position = Vector3DFloat(0, 0, 0);
			if((subsceneIndex >= 0) && (ix0 >= 0)) {
				switch(subsceneIndex){
					case 0:
						for(unsigned int i = 0; i < mesh->faces[ix1].vertexIds.size(); i++) {
							position += mesh->vertices[mesh->GetVertexIndex(mesh->faces[ix0].vertexIds[i])].position;
						}
						position = position * (1.0 / mesh->faces[ix1].vertexIds.size());
						break;
					case 1:
						for(unsigned int i = 0; i < 2; i++) {
							position += mesh->vertices[mesh->GetVertexIndex(mesh->edges[ix0].vertexIds[i])].position;
						}
						position = position * 0.5;
						break;
					case 2:
						position = mesh->vertices[ix0].position;
						break;
				}
			}
			return position;
		}
	}
}


#endif