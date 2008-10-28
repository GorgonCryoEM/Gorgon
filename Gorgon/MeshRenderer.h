// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Renderer for rendering mesh objects

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.23  2008/10/28 18:46:52  ssa1
//   Fixing octree neighbor search, and changing the structure tensor cost function
//
//   Revision 1.22  2008/09/29 20:36:35  ssa1
//   Drawing skeletal curves as cylinders and spheres
//
//   Revision 1.21  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//


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
		private:
			void UpdateBoundingBox();
			NonManifoldMesh_Annotated * mesh;
			vector<Vector3DFloat> intersectionPoints;
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

		Vector3DFloat MeshRenderer::GetIntersectionPoint(int ix) {
			return intersectionPoints[ix];
		}

		void MeshRenderer::Draw(int subSceneIndex, bool selectEnabled) {
			if(subSceneIndex == 0) {
				if(mesh != NULL) {
					mesh->Draw(true, true, false, selectEnabled, selectEnabled, false, true, true, true);

					/*

					if(selectEnabled) {
						glPushName(1);
						glPushName(0);
					}
					
					for(unsigned int i = 0; i < mesh->vertices.size(); i++) {
						mesh->vertices[i].tag = false;
					}

					for(unsigned int i = 0; i <  mesh->edges.size(); i++) {					
						if(mesh->edges[i].faceIds.size() == 0) {
							if(selectEnabled) {
								glLoadName(i);
							}
							NonManifoldMeshVertex<bool> v0 = mesh->vertices[mesh->GetVertexIndex(mesh->edges[i].vertexIds[0])];
							NonManifoldMeshVertex<bool> v1 = mesh->vertices[mesh->GetVertexIndex(mesh->edges[i].vertexIds[1])];
							if(!v0.tag) {
								DrawSphere(v0.position, 0.25);
								v0.tag = true;
							}
							if(!v1.tag) {
								DrawSphere(v1.position, 0.25);
								v1.tag = true;
							}

							DrawCylinder( v0.position, v1.position, 0.25);
						}
					}	
					if(selectEnabled) {
						glPopName();
						glPopName();
					} */

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