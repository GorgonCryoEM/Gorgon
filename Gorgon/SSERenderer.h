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
			void LoadHelixFile(string fileName);
			void LoadSheetFile(string fileName);
			void Unload();
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
			vector<GeometricShape*> shapes;
			vector<GLUquadric *> shapeQuadrics;
			NonManifoldMesh_NoTags * sheetMesh;
		};


		SSERenderer::SSERenderer() {
			shapes.clear();
			shapeQuadrics.clear();	
			sheetMesh = NULL;
		}

		SSERenderer::~SSERenderer() {
			for(unsigned int i = 0; i < shapes.size(); i++) {
				delete shapes[i];
				gluDeleteQuadric(shapeQuadrics[i]);
			}
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
		}

		void SSERenderer::Draw() {
			Point3 pt;
			for(unsigned int i = 0; i < shapes.size(); i++) {
				if(shapes[i]->GetGeometricShapeType() == GRAPHEDGE_HELIX) {
					glPushMatrix();
					glMultMatrixd(shapes[i]->worldToObject.mat);
					glRotated(90, 1,0,0);
					glTranslated(0.0, 0.0, -0.5);
					gluCylinder(gluNewQuadric(), 0.5, 0.5, 1.0, 10, 10);
					glPopMatrix();
				} 
			}
			if(sheetMesh != NULL) {
				sheetMesh->Draw(true, false, false);
			}
		}

		void SSERenderer::LoadHelixFile(string fileName) {
			vector<GeometricShape *> helices;
			helices.clear();
			SkeletonReader::ReadHelixFile((char *)fileName.c_str(), NULL, helices);
			for(unsigned int i = 0; i < helices.size(); i++) {
				shapes.push_back(helices[i]);
				shapeQuadrics.push_back(gluNewQuadric());
			}	
			UpdateBoundingBox();			
		}

		void SSERenderer::LoadSheetFile(string fileName) {
			vector<GeometricShape *> sheets;
			sheets.clear();
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = new NonManifoldMesh_NoTags();
			SkeletonReader::ReadSheetFile((char *)fileName.c_str(), sheets);
			for(unsigned int i = 0; i < sheets.size(); i++) {
				shapes.push_back(sheets[i]);
				shapeQuadrics.push_back(gluNewQuadric());
				for(unsigned int j = 0; j < sheets[i]->polygonPoints.size(); j++) {
					sheetMesh->AddVertex(Vector3DFloat(sheets[i]->polygonPoints[j][0], sheets[i]->polygonPoints[j][1], sheets[i]->polygonPoints[j][2]));
				}

				for(unsigned int j = 0; j < sheets[i]->polygons.size(); j++) {								
					sheetMesh->AddTriangle(sheets[i]->polygons[j].pointIndex1, sheets[i]->polygons[j].pointIndex2, sheets[i]->polygons[j].pointIndex3);					
				}				
			}	
			UpdateBoundingBox();			
		}

		void SSERenderer::Unload() {
			for(unsigned int i = 0; i < shapes.size(); i++) {
				delete shapes[i];
				gluDeleteQuadric(shapeQuadrics[i]);
			}
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = NULL;
			UpdateBoundingBox();
		}

		void SSERenderer::UpdateBoundingBox() {
			for(int i = 0; i < 3; i++) {
				minPts[i] = -0.5;
				maxPts[i] = 0.5;
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