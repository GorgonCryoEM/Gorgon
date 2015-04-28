// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Renderer for rendering mesh objects



#ifndef GORGON_MESH_RENDERER_H
#define GORGON_MESH_RENDERER_H

#include <cstdlib>
#include <cstdio>
#include <glut.h>
#include <string>
#include <SkeletonMaker/volume.h>
#include <SkeletonMaker/reader.h>
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>
#include "Renderer.h"
#include <Foundation/StringUtils.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::Foundation;
using namespace wustl_mm::SkeletonMaker;

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
			int IntersectMeshAndSphere(Vector3DFloat center, float radius);
			Vector3DFloat GetIntersectionPoint(int ix);
			void SetLineThickness(int thickness);
			void SetSpacing(float spX, float spY, float spZ);
			float GetSpacingX();
			float GetSpacingY();
			float GetSpacingZ();
			void SetOrigin(float orgX, float orgY, float orgZ);
			float GetOriginX();
			float GetOriginY();
			float GetOriginZ();
			void UpdateBoundingBox();
		private:
			NonManifoldMesh_Annotated * mesh;
			vector<Vector3DFloat> intersectionPoints;
			int lineThickness;
		};


		MeshRenderer::MeshRenderer() {
			mesh = NULL;
		}

		MeshRenderer::~MeshRenderer() {
			if(mesh != NULL) {
				delete mesh;
			}
		}

		void MeshRenderer::SetLineThickness(int thickness) {
			lineThickness = thickness;
		}

		NonManifoldMesh_Annotated * MeshRenderer::GetMesh() {
			if(mesh == NULL) {
				mesh = new NonManifoldMesh_Annotated();
			}
			return mesh;
		}

		Vector3DFloat MeshRenderer::GetIntersectionPoint(int ix) {
			return intersectionPoints[ix];
		}

		void MeshRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			switch(subSceneIndex) {
				case 0:
					if(mesh != NULL) {
						mesh->Draw(false, false, true, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
					}
					break;
				case 1:
					if(mesh != NULL) {
						mesh->Draw(false, true, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
					}
					break;
				case 2:
					if(mesh != NULL) {
						mesh->Draw(true, false, false, false, selectEnabled, selectEnabled, false, false, true, true, lineThickness, false);
					}
					break;
			}
			if(subSceneIndex == 0) {
			}
		}


		void MeshRenderer::LoadFile(string fileName) {
			Renderer::LoadFile(fileName);
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
				printf("Input format %s not supported!\n", extension.c_str());
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
					printf("Input format %s not supported!\n", extension.c_str());
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

		int MeshRenderer::IntersectMeshAndSphere(Vector3DFloat center, float radius) {
			float x1, y1, z1, x2, y2, z2, x3, y3, z3, r, a, b, c, d, u;
			Vector3DFloat p1, p2;
			x3 = center.X();
			y3 = center.Y();
			z3 = center.Z();
			r = radius;
			intersectionPoints.clear();

			for(unsigned int i = 0; i < mesh->edges.size(); i++) {
				p1 = mesh->vertices[mesh->edges[i].vertexIds[0]].position;
				p2 = mesh->vertices[mesh->edges[i].vertexIds[1]].position;
				x1 = p1.X();
				y1 = p1.Y();
				z1 = p1.Z();
				x2 = p2.X();
				y2 = p2.Y();
				z2 = p2.Z();
				a = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
				b = 2*((x2-x1)*(x1-x3) + (y2-y1)*(y1-y3) + (z2-z1)*(z1-z3));
				c = x3*x3 + y3*y3 + z3*z3 + x1*x1 + y1*y1 + z1*z1 - 2*(x3*x1 + y3*y1 + z3*z1) - r*r;
				d = b*b - 4*a*c;

				if(a != 0) {
					if(d >= 0) {
						u = (-b + sqrt(d))/2*a;
						if((u >=0) && (u <= 1)) {
							intersectionPoints.push_back(p1 + (p2 - p1)*u);
						}
					} 
					if (d > 0) {
						u = (-b - sqrt(d))/2*a;
						if((u >=0) && (u <= 1)) {
							intersectionPoints.push_back(p1 + (p2 - p1)*u);
						}
					}			
				}
			}
			return intersectionPoints.size();
		}

		string MeshRenderer::GetSupportedLoadFileFormats() {
			return "All Files (*.off *.mrc *.atom);;Volumes (*.mrc *.atom);;Meshes (*.off)";
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

		void MeshRenderer::SetSpacing(float spX, float spY, float spZ) {
			if(mesh != NULL) {
				mesh->SetScale(spX, spY, spZ);
			} else {
				Renderer::SetSpacing(spX, spY, spZ);
			}
		}

		float MeshRenderer::GetSpacingX() {
			if(mesh != NULL) {
				return mesh->scale[0];
			}
			return Renderer::GetSpacingX();
		}

		float MeshRenderer::GetSpacingY() {
			if(mesh != NULL) {
				return mesh->scale[1];
			}
			return Renderer::GetSpacingY();
		}

		float MeshRenderer::GetSpacingZ() {
			if(mesh != NULL) {
				return mesh->scale[2];
			}
			return Renderer::GetSpacingZ();
		}

		void MeshRenderer::SetOrigin(float orgX, float orgY, float orgZ) {
			if(mesh != NULL) {
				mesh->SetOrigin(orgX, orgY, orgZ);
			} else {
				Renderer::SetOrigin(orgX, orgY, orgZ);
			}
		}

		float MeshRenderer::GetOriginX() {
			if(mesh != NULL) {
				return mesh->origin[0];
			}
			return Renderer::GetOriginX();
		}

		float MeshRenderer::GetOriginY() {
			if(mesh != NULL) {
				return mesh->origin[1];
			}
			return Renderer::GetOriginY();
		}

		float MeshRenderer::GetOriginZ() {
			if(mesh != NULL) {
				return mesh->origin[2];
			}
			return Renderer::GetOriginZ();
		}
	}
}


#endif
