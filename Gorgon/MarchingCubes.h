#ifndef GRAYSKELETONCPP_MARCHING_CUBES_H
#define GRAYSKELETONCPP_MARCHING_CUBES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SkeletonMaker/volume.h>
#include <SkeletonMaker/reader.h>
#include <GL/glut.h>
#include <ProteinMorph/NonManifoldMesh.h>

using namespace wustl_mm::Protein_Morph;

namespace wustl_mm {
	namespace Visualization {	
		class MarchingCubes {
		public:
			MarchingCubes();
			~MarchingCubes();

			void drawMesh(bool dummy);
			void loadMRC(string fileName);
			void loadOFF(string fileName);
			void setSurfaceValue(const float value);
			float getSurfaceValue() const;
			void setSampleDensity(const int rate);
			int  getSampleDensity() const;
		private:
			Volume * vol;
			float surfaceValue;
			int sampleDensity;
			NonManifoldMesh * mesh;

		};


		MarchingCubes::MarchingCubes() {
			vol = NULL;
			mesh = NULL;
		}

		MarchingCubes::~MarchingCubes() {
			if(vol != NULL) {
				delete vol;
			}
			if(mesh != NULL) {
				delete mesh;
			}
		}

		void MarchingCubes::drawMesh(bool dummy) {
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

		void MarchingCubes::loadMRC(string fileName) {
			if(vol != NULL) {
				delete vol;
			}
			MRCReader * reader = (MRCReader*)MRCReaderPicker::pick((char *)fileName.c_str());
			vol = reader->getVolume();
			delete reader;
		}

		void MarchingCubes::loadOFF(string fileName) {
			if(mesh != NULL) {
				delete mesh;
			}
			mesh = NonManifoldMesh::LoadOffFile(fileName);
			
		}

		void MarchingCubes::setSurfaceValue(const float value) {
			surfaceValue = value;			
		}

		float MarchingCubes::getSurfaceValue() const {
			return surfaceValue;
		}

		void MarchingCubes::setSampleDensity(const int rate) {
			sampleDensity = rate;
		}

		int MarchingCubes::getSampleDensity() const {			
			return sampleDensity;
		}
	}
}

#endif
