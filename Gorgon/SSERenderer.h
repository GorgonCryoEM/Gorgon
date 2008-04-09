#ifndef GORGON_SSE_RENDERER_H
#define GORGON_SSE_RENDERER_H

#include "Renderer.h"
#include "MeshRenderer.h"
#include <GraphMatch/SkeletonReader.h>
#include <GraphMatch/GeometricShape.h>
#include <GraphMatch/VectorMath.h>
#include <vector>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::GraphMatch;
using namespace std;

namespace wustl_mm {
	namespace Visualization {	
		class SSERenderer : public Renderer{
		public:
			SSERenderer();
			~SSERenderer();

			void Draw();
			void LoadFile(string fileName);
			void LoadHelixFile(string fileName, bool reset);
			void PerformAutomaticAnnotation(double maximumHelixAngle, double minHelixLength, double maximumSheetAngle, double minSheetArea);
			void SetSkeletonRenderer(MeshRenderer * skeletonRenderer);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
			MeshRenderer * skeletonRenderer;
			vector<GeometricShape*> helices;
		};


		SSERenderer::SSERenderer() {
			helices.clear();
		}

		SSERenderer::~SSERenderer() {
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];
			}
		}

		void SSERenderer::Draw() {
			NonManifoldMesh_Annotated * mesh = skeletonRenderer->GetMesh();
			if(mesh != NULL) {
				glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				glLineWidth(3.0);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				glBegin(GL_LINES);
				int k;

				for(unsigned int i = 0; i < mesh->edges.size(); i++) {
					if((mesh->edges[i].faceIds.size() == 0) && (mesh->edges[i].tag > 0)) {
						k = mesh->GetVertexIndex(mesh->edges[i].vertexIds[0]);
						glVertex3f(mesh->vertices[k].position.X(), mesh->vertices[k].position.Y(), mesh->vertices[k].position.Z());
						k = mesh->GetVertexIndex(mesh->edges[i].vertexIds[1]);
						glVertex3f(mesh->vertices[k].position.X(), mesh->vertices[k].position.Y(), mesh->vertices[k].position.Z());			
					}
				}
				glEnd();
				glPopAttrib();
			}
		}


		void SSERenderer::LoadFile(string fileName) {
			LoadHelixFile(fileName, true);
		}

		void SSERenderer::LoadHelixFile(string fileName, bool reset) {
			NonManifoldMesh_Annotated * mesh = skeletonRenderer->GetMesh();
			if(mesh != NULL) {
				if(reset) {
					for(unsigned int i = 0; i < helices.size(); i++) {
						delete helices[i];
					}
					helices.clear();
					for(unsigned int i = 0; i < mesh->edges.size(); i++) {
						mesh->edges[i].tag = 0;
					}
				}
				SkeletonReader::ReadHelixFile((char *)fileName.c_str(), NULL, helices);

				Vector3DFloat vertex0, vertex1;
				for(unsigned int j = 0; j < helices.size(); j++) {
					for(unsigned int i = 0; i < mesh->edges.size(); i++) {
						if(mesh->edges[i].tag == 0) {
							vertex0 = mesh->vertices[mesh->GetVertexIndex(mesh->edges[i].vertexIds[0])].position;
							vertex1 = mesh->vertices[mesh->GetVertexIndex(mesh->edges[i].vertexIds[1])].position;
							if(helices[j]->IsInsideShape(Point3(vertex0.X(), vertex0.Y(), vertex0.Z())) && helices[j]->IsInsideShape(Point3(vertex1.X(), vertex1.Y(), vertex1.Z()))) {
								mesh->edges[i].tag = j+1;
							}
						}
					}
				}
			}

			UpdateBoundingBox();			
		}

		void SSERenderer::PerformAutomaticAnnotation(double maximumHelixAngle, double minHelixLength, double maximumSheetAngle, double minSheetArea) {
			NonManifoldMesh_Annotated * mesh = skeletonRenderer->GetMesh();
			if(mesh != NULL) {
				bool * edgeVisited = new bool[mesh->edges.size()];
				for(unsigned int i = 0; i < mesh->edges.size(); i++) {
					edgeVisited[i] = false;
				}

				for(unsigned int i = 0; i < mesh->edges.size(); i++) {
					if(!edgeVisited[i]) {
						mesh->edges[i].tag = (int)((double)rand()*10.0/(double)RAND_MAX);
					}
					
				}

				delete [] edgeVisited;
			}

		}


		void SSERenderer::SetSkeletonRenderer(MeshRenderer * skeletonRenderer) {
			this->skeletonRenderer = skeletonRenderer;
		}

		void SSERenderer::Unload() {
			UpdateBoundingBox();
		}

		void SSERenderer::UpdateBoundingBox() {
			if(skeletonRenderer != NULL) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = skeletonRenderer->GetMin(i);
					maxPts[i] = skeletonRenderer->GetMax(i);
				}			
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = -0.5;
					maxPts[i] = 0.5;
				}
			}
		}

		string SSERenderer::GetSupportedLoadFileFormats() {
			return "VRML models (*.vrml *.wrl)";
		}
		string SSERenderer::GetSupportedSaveFileFormats() {
			return "VRML models (*.vrml, *.wrl)";
		}
	}
}


#endif