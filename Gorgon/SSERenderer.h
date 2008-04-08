#ifndef GORGON_SSE_RENDERER_H
#define GORGON_SSE_RENDERER_H

#include "Renderer.h"
#include "MeshRenderer.h"

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;

namespace wustl_mm {
	namespace Visualization {	
		class SSERenderer : public Renderer{
		public:
			SSERenderer();
			~SSERenderer();

			void Draw();
			void LoadFile(string fileName);
			void PerformAutomaticAnnotation(double maximumHelixAngle, double minHelixLength, double maximumSheetAngle, double minSheetArea);
			void SetSkeletonRenderer(MeshRenderer * skeletonRenderer);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			float GetMin(int dimension);
			float GetMax(int dimension);
		private:
			MeshRenderer * skeletonRenderer;
		};


		SSERenderer::SSERenderer() {
		}

		SSERenderer::~SSERenderer() {
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

		float SSERenderer::GetMin(int dimension) {
			return skeletonRenderer->GetMin(dimension);
		}

		float SSERenderer::GetMax(int dimension) {
			return skeletonRenderer->GetMax(dimension);
		}
		string SSERenderer::GetSupportedLoadFileFormats() {
			return "VRML models (*.vrml, *.wrl)";
		}
		string SSERenderer::GetSupportedSaveFileFormats() {
			return "VRML models (*.vrml, *.wrl)";
		}
	}
}


#endif