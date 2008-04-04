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

			void Center();
			void Draw();
			void Focus();
			void LoadFile(string fileName);
			string GetSupportedFileFormats();
		private:
			NonManifoldMesh * mesh;
		};


		MeshRenderer::MeshRenderer() {
			mesh = NULL;
		}

		MeshRenderer::~MeshRenderer() {
			if(mesh != NULL) {
				delete mesh;
			}
		}

		void MeshRenderer::Center() {
			//gluLookAt(
		}

		void MeshRenderer::Draw() {
			if(mesh != NULL) {
				//glClear(GL_COLOR_BUFFER_BIT);
				glColor3f(1.0, 1.0, 1.0);
				//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); 
				int vertices[4];

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

					for(int j = 0; j < k; j++) {
						glBegin(GL_POLYGON);
						glVertex3f(mesh->vertices[vertices[j]].position.X(), mesh->vertices[vertices[j]].position.Y(), mesh->vertices[vertices[j]].position.Z());
						glEnd();
					}

					for(int j = k-1; j >= 0; j--) {
						glBegin(GL_POLYGON);
						glVertex3f(mesh->vertices[vertices[j]].position.X(), mesh->vertices[vertices[j]].position.Y(), mesh->vertices[vertices[j]].position.Z());
						glEnd();
					}
				}

				glBegin(GL_LINES);
				for(unsigned int i = 0; i < mesh->edges.size(); i++) {
					glVertex3f(mesh->vertices[mesh->edges[i].vertexIds[0]].position.X(), mesh->vertices[mesh->edges[i].vertexIds[0]].position.Y(), mesh->vertices[mesh->edges[i].vertexIds[0]].position.Z());
					glVertex3f(mesh->vertices[mesh->edges[i].vertexIds[1]].position.X(), mesh->vertices[mesh->edges[i].vertexIds[1]].position.Y(), mesh->vertices[mesh->edges[i].vertexIds[1]].position.Z());
				}
				glEnd();

				glFlush();
			}
		}


		void MeshRenderer::Focus() {
		}
		void MeshRenderer::LoadFile(string fileName) {
			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);
			printf("<%s>\n", (char *)extension.c_str());

			if(mesh != NULL) {
				delete mesh;
			}

			if(stricmp(extension.c_str(), "OFF") == 0) {
				mesh = NonManifoldMesh::LoadOffFile(fileName);				
			} else if((stricmp(extension.c_str(), "MRC") == 0) || (stricmp(extension.c_str(), "ATOM") == 0)) {
				Volume * volume = VolumeFormatConverter::LoadVolume(fileName, extension);
				mesh = new NonManifoldMesh(volume);
				delete volume;			
			} else {
				printf("Input format %s not supported!\n", extension);
			}						
			
		}
		string MeshRenderer::GetSupportedFileFormats() {
			return "Meshes (*.off);;Volumes (*.mrc *.atom);;All Files (*.off *.mrc *.atom)";
		}
	}
}


#endif