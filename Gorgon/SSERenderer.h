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
		struct SheetIdsAndSelect {
			unsigned char id;
			bool selected;
		};

		typedef NonManifoldMesh<bool, bool, SheetIdsAndSelect> NonManifoldMesh_SheetIds;
		class SSERenderer : public Renderer{
		public:
			SSERenderer();
			~SSERenderer();

			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadHelixFile(string fileName);			
			void LoadSheetFile(string fileName);			
			void Unload();
			void SetHelixColor(int index, float r, float g, float b, float a);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
		private:
			void UpdateBoundingBox();
			vector<GeometricShape*> helices;
			NonManifoldMesh_SheetIds * sheetMesh;
		};


		SSERenderer::SSERenderer() {
			helices.clear();
			sheetMesh = NULL;
		}

		SSERenderer::~SSERenderer() {
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];				
			}
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
		}

		void SSERenderer::Draw(int subSceneIndex, bool selectEnabled) {
			GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};

			glPushName(subSceneIndex);
			float colorR, colorG, colorB, colorA;
			if(subSceneIndex == 0) {
				if(selectEnabled) {
					glPushName(0);
				}
				
				Point3 pt;
				for(int i = 0; i < (int)helices.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					helices[i]->GetColor(colorR, colorG, colorB, colorA);	
					SetColor(colorR, colorG, colorB, colorA);

					if(helices[i]->GetSelected()) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}
					glPushMatrix();
					glMultMatrixd(helices[i]->GetWorldToObjectMatrix().mat);
					glRotated(90, 1, 0, 0);
					glTranslated(0.0, 0.0, -0.5);
					if(selectEnabled) {
						glLoadName(i);
					}

					GLUquadric * quadricCylinder = gluNewQuadric();
					gluCylinder(quadricCylinder, 0.5, 0.5, 1.0, 10, 10);
					gluDeleteQuadric(quadricCylinder);
					glPopMatrix();
					glPopAttrib();
				}
				if(selectEnabled) {
					glPopName();
				}

			}
			else if((subSceneIndex == 1) && (sheetMesh != NULL)) {
				int k;
				if(selectEnabled) {
					glPushName(0);
				}
				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(sheetMesh->faces[i].tag.selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}
					if(selectEnabled) {
						glLoadName(sheetMesh->faces[i].tag.id);
					}
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
						normal = sheetMesh->GetVertexNormal(sheetMesh->faces[i].vertexIds[j]);
						k = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
						glNormal3f(normal.X(), normal.Y(), normal.Z());
						glVertex3fv(sheetMesh->vertices[k].position.values);
					}
					glEnd();
					glPopAttrib();
					
				}
				if(selectEnabled) {
					glPopName();
				}
			}
			glPopName();
		}

		void SSERenderer::LoadHelixFile(string fileName) {
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];
			}
			helices.clear();
			SkeletonReader::ReadHelixFile((char *)fileName.c_str(), NULL, helices);
			UpdateBoundingBox();			
		}

		void SSERenderer::LoadSheetFile(string fileName) {
			vector<GeometricShape *> sheets;
			sheets.clear();
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = new NonManifoldMesh_SheetIds();
			SkeletonReader::ReadSheetFile((char *)fileName.c_str(), sheets);
			Point3 pt;
			vector<int> indices;
			SheetIdsAndSelect faceTag;
			faceTag.selected = false;

			for(unsigned int i = 0; i < sheets.size(); i++) {
				indices.clear();
				for(unsigned int j = 0; j < sheets[i]->polygonPoints.size(); j++) {
					pt = sheets[i]->polygonPoints[j];
					indices.push_back(sheetMesh->AddVertex(Vector3DFloat((float)pt[0], (float)pt[1], (float)pt[2])));
				}

				for(unsigned int j = 0; j < sheets[i]->polygons.size(); j++) {								
					faceTag.id = i+1;
					sheetMesh->AddTriangle(indices[sheets[i]->polygons[j].pointIndex1], indices[sheets[i]->polygons[j].pointIndex2], indices[sheets[i]->polygons[j].pointIndex3], NULL, faceTag);					
				}				
			}
			for(unsigned int i = 0; i < sheets.size(); i++) { 
				delete sheets[i];
			}
			indices.clear();
			UpdateBoundingBox();			
		}

		void SSERenderer::Unload() {
			Renderer::Unload();
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];
			}
			helices.clear();
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = NULL;
			UpdateBoundingBox();
		}

		void SSERenderer::UpdateBoundingBox() {
			Point3 pt;
			if(sheetMesh != NULL && sheetMesh->vertices.size() > 0) {
				for(int i = 0; i < 3; i++) {
					minPts[i] = sheetMesh->vertices[0].position.values[i];
					maxPts[i] = sheetMesh->vertices[0].position.values[i];
				}
			} else if (helices.size() > 0) {
				pt = helices[0]->GetWorldCoordinates(Point3(0,0.5,0));
				for(int j = 0; j < 3; j++) {
					minPts[j] = pt[j];
					maxPts[j] = pt[j];
				}					
			}

			if(helices.size() > 0 || sheetMesh != NULL) {
				for(unsigned int i = 0; i < helices.size(); i++) {
					pt = helices[i]->GetWorldCoordinates(Point3(0,0.5,0));
					for(int j = 0; j < 3; j++) {
						minPts[j] = min(minPts[j], (float)pt[j]);
						maxPts[j] = max(maxPts[j], (float)pt[j]);
					}
					pt = helices[i]->GetWorldCoordinates(Point3(0,-0.5,0));
					for(int j = 0; j < 3; j++) {
						minPts[j] = min(minPts[j], (float)pt[j]);
						maxPts[j] = max(maxPts[j], (float)pt[j]);
					}
				}
				if(sheetMesh != NULL) {
					for(unsigned int i = 0; i < sheetMesh->vertices.size(); i++) {
						for(int j = 0; j < 3; j++) {
							minPts[j] = min(minPts[j], sheetMesh->vertices[i].position.values[j]);
							maxPts[j] = max(maxPts[j], sheetMesh->vertices[i].position.values[j]);
						}	
					}
				}
			} else {
				for(int i = 0; i < 3; i++) {
					minPts[i] = -0.5;
					maxPts[i] = 0.5;
				}
			}
		}

		void SSERenderer::SetHelixColor(int index, float r, float g, float b, float a) {
			helices[index]->SetColor(r, g, b, a);

		}

		bool SSERenderer::SelectionClear() {
			if(Renderer::SelectionClear()) {
				for(unsigned int i = 0; i < helices.size(); i++) {					
					helices[i]->SetSelected(false);
				}

				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					sheetMesh->faces[i].tag.selected = false;
				}

				return true;
			}
			return false;
		}

		void SSERenderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Renderer::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
			if((subsceneIndex == 0) && (ix0 >= 0) && (ix0 <= (int)helices.size())) {
				helices[ix0]->SetSelected(forceTrue || !helices[ix0]->GetSelected());			
			}

			if((subsceneIndex == 1)) {
				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					if(sheetMesh->faces[i].tag.id == ix0) {
						sheetMesh->faces[i].tag.selected = forceTrue || !sheetMesh->faces[i].tag.selected;
					}
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