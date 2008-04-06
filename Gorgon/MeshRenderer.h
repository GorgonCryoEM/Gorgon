#ifndef GORGON_MESH_RENDERER_H
#define GORGON_MESH_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SkeletonMaker/volume.h>
#include <SkeletonMaker/reader.h>
#include <GL/glut.h>
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>
#include "Renderer.h"

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;

namespace wustl_mm {
	namespace Visualization {	
		class MeshRenderer : public Renderer{
		public:
			MeshRenderer();
			~MeshRenderer();

			void Draw();
			void LoadFile(string fileName);
			void Unload();
			void SmoothLaplacian(double convergenceRate, int iterations);
			string GetSupportedFileFormats();
		private:
			void UpdateBoundingBox();
			NonManifoldMesh * mesh;
		};


		MeshRenderer::MeshRenderer() {
			mesh = NULL;
			drawBoundingBox = true;
		}

		MeshRenderer::~MeshRenderer() {
			if(mesh != NULL) {
				delete mesh;
			}
		}

		void MeshRenderer::Draw() {
			if(mesh != NULL) {
				int vertices[40];
				int lastVertex;
				int k;

				for(unsigned int i = 0; i < mesh->faces.size(); i++) {
					lastVertex = -1;

					k = 0;
					for(int j = mesh->faces[i].edgeIds.size()-1; j >= 0; j--) {
						if((mesh->edges[mesh->faces[i].edgeIds[j]].vertexIds[0] == mesh->edges[mesh->faces[i].edgeIds[(j+1)%mesh->faces[i].edgeIds.size()]].vertexIds[0]) || 
							(mesh->edges[mesh->faces[i].edgeIds[j]].vertexIds[0] == mesh->edges[mesh->faces[i].edgeIds[(j+1)%mesh->faces[i].edgeIds.size()]].vertexIds[1])) {
							lastVertex = mesh->edges[mesh->faces[i].edgeIds[j]].vertexIds[1];						
						} else {
							lastVertex = mesh->edges[mesh->faces[i].edgeIds[j]].vertexIds[0];
						}
						vertices[k] = lastVertex;
						k++;						
					}

					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(int j = 0; j < k; j++) {
						normal = mesh->GetVertexNormal(vertices[j]);

						glNormal3f(normal.X(), normal.Y(), normal.Z());
						glVertex3f(mesh->vertices[vertices[j]].position.X(), mesh->vertices[vertices[j]].position.Y(), mesh->vertices[vertices[j]].position.Z());
					}
					glEnd();

					//glBegin(GL_POLYGON);
					//for(int j = k-1; j >=0; j--) {
					//	normal = -mesh->GetVertexNormal(vertices[j]);

					//	glNormal3f(normal.X(), normal.Y(), normal.Z());
					//	glVertex3f(mesh->vertices[vertices[j]].position.X(), mesh->vertices[vertices[j]].position.Y(), mesh->vertices[vertices[j]].position.Z());
					//}
					//glEnd();
				}



				glPushAttrib(GL_LIGHTING_BIT);
				glDisable(GL_LIGHTING);

				glBegin(GL_LINES);
				for(unsigned int i = 0; i < mesh->edges.size(); i++) {
					if(mesh->edges[i].faceIds.size() == 0) {
						glVertex3f(mesh->vertices[mesh->edges[i].vertexIds[0]].position.X(), mesh->vertices[mesh->edges[i].vertexIds[0]].position.Y(), mesh->vertices[mesh->edges[i].vertexIds[0]].position.Z());
						glVertex3f(mesh->vertices[mesh->edges[i].vertexIds[1]].position.X(), mesh->vertices[mesh->edges[i].vertexIds[1]].position.Y(), mesh->vertices[mesh->edges[i].vertexIds[1]].position.Z());
					}
				}
				glEnd();

				glBegin(GL_POINTS);
				for(unsigned int i = 0; i < mesh->vertices.size(); i++) {
					if(mesh->vertices[i].edgeIds.size() == 0) {
						glVertex3f(mesh->vertices[i].position.X(), mesh->vertices[i].position.Y(), mesh->vertices[i].position.Z());
					}
				}
				glEnd();
				glPopAttrib();

				glFlush();
			}
		}


		void MeshRenderer::LoadFile(string fileName) {
			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);
			//printf("<%s>\n", (char *)extension.c_str());

			if(mesh != NULL) {
				delete mesh;
			}

			if(stricmp(extension.c_str(), "OFF") == 0) {
				mesh = NonManifoldMesh::LoadOffFile(fileName);				
			} else if((stricmp(extension.c_str(), "MRC") == 0) || (stricmp(extension.c_str(), "ATOM") == 0)) {
				Volume * volume = VolumeFormatConverter::LoadVolume(fileName);
				mesh = new NonManifoldMesh(volume);
				delete volume;			
			} else {
				printf("Input format %s not supported!\n", extension);
			}

			UpdateBoundingBox();
			
		}

		void MeshRenderer::Unload() {
			if(mesh != NULL) {
				delete mesh;
				mesh = NULL;
			}
			UpdateBoundingBox();

		}
		void MeshRenderer::SmoothLaplacian(double convergenceRate, int iterations) {
			NonManifoldMesh * newMesh;
			for(int i = 0; i < iterations; i++) {
				newMesh = mesh->SmoothLaplacian(convergenceRate);
				delete mesh;
				mesh = newMesh;
			}

		}
		void MeshRenderer::UpdateBoundingBox() {
			if(mesh != NULL && mesh->vertices.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = mesh->vertices[0].position.values[i];
					maxPts[i] = mesh->vertices[0].position.values[i];
				}

				for(int i = 1; i < mesh->vertices.size(); i++) {
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
		string MeshRenderer::GetSupportedFileFormats() {
			return "Meshes (*.off);;Volumes (*.mrc *.atom);;All Files (*.off *.mrc *.atom)";
		}
	}
}


#endif