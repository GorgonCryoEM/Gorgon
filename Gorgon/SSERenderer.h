// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Renderer for rendering secondary structure elements.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.26  2009/06/23 16:50:34  ssa1
//   Adding in SSEBuilder Functionality: Saving helix as WRL and SSE files
//
//   Revision 1.25  2009/06/22 20:17:27  ssa1
//   Adding in SSEBuilder Functionality: Selection to Helix functionality
//
//   Revision 1.24  2009/03/17 20:00:17  ssa1
//   Removing Sheets from fiting process
//
//   Revision 1.23  2009/03/16 17:08:46  ssa1
//   Fixing bug when densities have negative values
//
//   Revision 1.22  2009/03/16 16:17:34  ssa1
//   Fitting SSEs into the Density
//
//   Revision 1.21  2008/12/03 21:58:25  ssa1
//   Selection rotations for atoms and helices.
//
//   Revision 1.20  2008/11/13 20:54:40  ssa1
//   Using the correct scale when loading volumes
//
//   Revision 1.19  2008/09/29 16:09:44  ssa1
//   Removing GLVisualizer.h
//
//   Revision 1.18  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//

#ifndef GORGON_SSE_RENDERER_H
#define GORGON_SSE_RENDERER_H

#include "Renderer.h"
#include "MeshRenderer.h"
#include <GraphMatch/StandardGraph.h>
#include <GraphMatch/SkeletonReader.h>
#include <GraphMatch/GeometricShape.h>
#include <GraphMatch/VectorMath.h>
#include <ProteinMorph/SSEFlexibleFitter.h>
#include <vector>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::GraphMatch;
using namespace std;

namespace wustl_mm {
	namespace Visualization {	

		const float HELIX_LENGTH_TO_RESIDUE_RATIO = 1.54;

		class SSERenderer : public Renderer{
		public:
			SSERenderer();
			~SSERenderer();

			void AddHelix(Vector3DFloat p1, Vector3DFloat p2);
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadHelixFile(string fileName);			
			void LoadSheetFile(string fileName);			
			void Unload();
			void SetHelixColor(int index, float r, float g, float b, float a);
			bool SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle);
			int SelectionObjectCount();
			Vector3DFloat SelectionCenterOfMass();
			bool SelectionMove(Vector3DFloat moveDirection);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void SaveHelixFile(string fileName);
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void FitSelectedSSEs(Volume * vol);
		private:
			void LoadHelixFileSSE(string fileName);
			void LoadHelixFileVRML(string fileName);
			void SaveHelixFileSSE(FILE* fout);
			void SaveHelixFileVRML(FILE* fout);
			void UpdateBoundingBox();
			vector<GeometricShape*> helices;
			NonManifoldMesh_SheetIds * sheetMesh;
			int sheetCount;
			bool selectedSheets[256];
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

		void SSERenderer::AddHelix(Vector3DFloat p1, Vector3DFloat p2) {
			GeometricShape * newHelix = new GeometricShape();
			newHelix->geometricShapeType = GRAPHEDGE_HELIX;
			Vector3DFloat center = (p1+p2) * 0.5;
			Vector3DFloat dir = p1-p2;
			Vector3DFloat yaxis = Vector3DFloat(0, 1, 0);

			newHelix->SetCenter(Point3(center.X(), center.Y(), center.Z()));
			newHelix->SetRadius(2.5);
			newHelix->SetHeight(dir.Length());
			Vector3DFloat axis = dir^yaxis;

			dir.Normalize();
			double angle = acos(dir * yaxis);
			newHelix->Rotate(Vector3(axis.X(), axis.Y(), axis.Z()), -angle);

			helices.push_back(newHelix);
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


		void SSERenderer::LoadHelixFileSSE(string fileName) {

			// NEED to FIX THIS FUNCTION.. DOES NOT WORK PROPER.
			FILE* fin = fopen((char*)fileName.c_str(), "rt");

			char line[1000];
			char t1[1000], t2[1000], t3[1000];
			string lineStr;
			string token;
			int length;
			float x1, y1, z1, x2, y2, z2;
			while(!feof(fin))
			{		
				fscanf(fin, "%s", line);
				lineStr = line;

				if(lineStr.compare("ALPHA") == 0) {
					fscanf(fin, " %s %s %s %d %f %f %f %f %f %f", t1, t2, t3, &length, &x1, &y1, &z1, &x2, &y2, &z2);
					AddHelix(Vector3DFloat(x1, y1, z1), Vector3DFloat(x2, y2, z2));
				} else {
					fscanf(fin, "%s\n", line);					
				}
			}

			fclose(fin);
		}

		void SSERenderer::LoadHelixFileVRML(string fileName) {
			SkeletonReader::ReadHelixFile((char *)fileName.c_str(), NULL, helices);
		}

		void SSERenderer::LoadHelixFile(string fileName) {

			if(sheetMesh == NULL) {
				Renderer::LoadFile(fileName);
			}
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];
			}
			helices.clear();

			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);			
			extension = StringUtils::StringToUpper(extension);			
			if(strcmp(extension.c_str(), "WRL") == 0) {
				LoadHelixFileVRML(fileName);
			} else if(strcmp(extension.c_str(), "VRML") == 0) {
				LoadHelixFileVRML(fileName);
			} else if(strcmp(extension.c_str(), "SSE") == 0) {
				LoadHelixFileSSE(fileName);
			} 
			
			UpdateBoundingBox();			
		}

		void SSERenderer::LoadSheetFile(string fileName) {
			if(helices.size() == 0) {
				Renderer::LoadFile(fileName);
			}
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

			sheetCount = sheets.size();
			for(unsigned int i = 0; i <= sheetCount; i++) {
				selectedSheets[i] = false;
			}

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

		bool SSERenderer::SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle) {
			bool rotated = false;
			Point3 centerOfMassP3 = Point3(centerOfMass.X(), centerOfMass.Y(), centerOfMass.Z());
			Vector3 rotationV3 = Vector3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z());
			
			for(unsigned int i = 0; i < helices.size(); i++) {					
				if(helices[i]->GetSelected()) {
					rotated = true;
					Vector3 move = centerOfMassP3 - helices[i]->GetCenter();										
					Matrix4 rotMatrix = Matrix4::rotation(rotationV3, angle);
					Vector3 newMove = rotMatrix * move; 
					helices[i]->SetCenter(centerOfMassP3 - newMove);		

					helices[i]->Rotate(Vector3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z()), angle);
				}
			}

			// I think the sheet center of mass is wrong!  Need to fix this
			/*for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
				sheetMesh->vertices[i].tag = false;
			}

			unsigned int vertexIx;
			for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
				if(sheetMesh->faces[i].tag.selected) {
					rotated = true;
					for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
						vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
						NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
						if(!v->tag) {							
							v->tag = true;
							v->position = v->position - centerOfMass;
							v->position = v->position.Rotate(rotationAxis, angle);
							v->position = v->position + centerOfMass;
						}
					}
				}
			}		*/

			return rotated;
		}

		int SSERenderer::SelectionObjectCount() {
			int count = 0;
			for(unsigned int i = 0; i < helices.size(); i++) {					
				if(helices[i]->GetSelected()) {
					count++;
				}
			}

			if(sheetMesh != NULL) {
				for(unsigned int i = 0; i <= sheetCount; i++) {
					if(selectedSheets[i]) {
						count++;
					}
				}
			}

			return count;
		}

		Vector3DFloat SSERenderer::SelectionCenterOfMass(){ 
			Vector3DFloat helixCenterOfMass = Vector3DFloat(0,0,0);
			Point3 helixCenter;
			int helixCount = 0;

			for(unsigned int i = 0; i < helices.size(); i++) {					
				if(helices[i]->GetSelected()) {
					helixCount++;
					helixCenter = helices[i]->GetCenter();
					helixCenterOfMass = helixCenterOfMass + Vector3DFloat(helixCenter[0], helixCenter[1], helixCenter[2]);
				}
			}

			int totalCount = SelectionObjectCount();
			int sheetCount = totalCount - helixCount;

			Vector3DFloat sheetsCenterOfMass = Vector3DFloat(0,0,0);
			Vector3DFloat currentFaceCenterOfMass;
			Vector3DFloat currentSheetCenterOfMass;

			if((sheetCount > 0) && (sheetMesh != NULL)) {


				int currentSheetFaceCount;

				for(unsigned int j = 0; j <= this->sheetCount; j++) {
					if(selectedSheets[j]) {
						currentSheetCenterOfMass = Vector3DFloat(0,0,0);
						currentSheetFaceCount = 0;
						for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
							if(sheetMesh->faces[i].tag.id == j) {
								currentSheetFaceCount++;
								currentFaceCenterOfMass = Vector3DFloat(0,0,0);
								for(unsigned int k = 0; k < sheetMesh->faces[i].vertexIds.size(); k++) {
									currentFaceCenterOfMass = currentFaceCenterOfMass + sheetMesh->vertices[sheetMesh->faces[i].vertexIds[k]].position;
								}
								currentFaceCenterOfMass = currentFaceCenterOfMass * (1.0f / (float)sheetMesh->faces[i].vertexIds.size());
								currentSheetCenterOfMass = currentSheetCenterOfMass + currentFaceCenterOfMass;
							}
						}
						
						currentSheetCenterOfMass = currentSheetCenterOfMass * (1.0f / (float)currentSheetFaceCount);
					}
					sheetsCenterOfMass = sheetsCenterOfMass + currentSheetCenterOfMass;
				}
			}
			
			Vector3DFloat centerOfMass;
			if(totalCount == 0) {
				centerOfMass = Renderer::SelectionCenterOfMass();
			} else if ((helixCount > 0) && (sheetCount > 0)) {
				centerOfMass = (helixCenterOfMass + sheetsCenterOfMass) * (1.0f/(float)totalCount);
			} else if (helixCount > 0) {
				centerOfMass = helixCenterOfMass * (1.0f/(float)helixCount);
			} else {
				centerOfMass = sheetsCenterOfMass * (1.0f/(float)sheetCount);
			}
			return centerOfMass;
		}

		bool SSERenderer::SelectionMove(Vector3DFloat moveDirection) {
			bool moved = false;
			for(unsigned int i = 0; i < helices.size(); i++) {					
				if(helices[i]->GetSelected()) {
					helices[i]->SetCenter(helices[i]->GetCenter() + Vector3(moveDirection.X(), moveDirection.Y(), moveDirection.Z()));
					moved = true;
				}
			}
			if(sheetMesh != NULL) {
				for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
					sheetMesh->vertices[i].tag = false;
				}


				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
					if(sheetMesh->faces[i].tag.selected) {
						for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
							NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[sheetMesh->faces[i].vertexIds[j]]);
							if(!v->tag) {
								v->position = v->position + moveDirection;
								moved = true;
								v->tag = true;
							}
						}
					}
				}			
			}
			UpdateBoundingBox();
			return moved;
		}

		bool SSERenderer::SelectionClear() {
			if(Renderer::SelectionClear()) {
				for(unsigned int i = 0; i < helices.size(); i++) {					
					helices[i]->SetSelected(false);
				}

				if(sheetMesh != NULL) {
					for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
						sheetMesh->faces[i].tag.selected = false;
					}
					for(unsigned int i = 0; i <= sheetCount; i++) {
						selectedSheets[i] = false;
					}
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

				selectedSheets[ix0] = forceTrue || !selectedSheets[ix0];
			}
		}

		void SSERenderer::SaveHelixFileVRML(FILE* fout) {
			Point3 center;
			Vector3DFloat start, end, axis;
			double angle;
			float helixLength;
			fprintf(fout, "#VRML V2.0 utf8\n");

			for(unsigned int i = 0; i < helices.size(); i++) {
				center = helices[i]->GetCenter();
				start = helices[i]->GetCornerCell3(1);
				end = helices[i]->GetCornerCell3(2);
				helixLength = (start-end).Length();
				helices[i]->GetRotationAxisAndAngle(axis, angle);

				fprintf(fout, "Group {\n children [\n Transform {\n  translation %f %f %f\n", center[0], center[1], center[2]);
				fprintf(fout, "  rotation %f %f %f %f\n", axis.X(), axis.Y(), axis.Z(), angle);
				fprintf(fout, "  children [\n   Shape {\n    appearance \n     Appearance {\n      material Material {\n       emissiveColor 0 0.5 0\n       }\n     }\n");
				fprintf(fout, "    geometry\n     Cylinder {\n      height %f \n      radius 2.5 \n     }\n   }\n  ]\n }\n ]\n}", helixLength);
			}
		}

		void SSERenderer::SaveHelixFileSSE(FILE* fout) {
			Vector3DFloat start, end;
			float helixLength;
			int intLength;

			for(unsigned int i = 0; i < helices.size(); i++) {
				end = helices[i]->GetCornerCell3(1);
				start = helices[i]->GetCornerCell3(2);				
				helixLength = (start-end).Length();
				intLength = (int)ceil(helixLength / HELIX_LENGTH_TO_RESIDUE_RATIO);
								
				fprintf(fout, "ALPHA 'A%d' '%d' '%d' %d %f %f %f %f %f %f\n", i, i*100,i*100+(intLength-1), intLength, start.X(), start.Y(), start.Z(), end.X(), end.Y(), end.Z());
			}
		}

		void SSERenderer::SaveHelixFile(string fileName) {
			FILE* fout = fopen((char*)fileName.c_str(), "wt");
			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);
			
			extension = StringUtils::StringToUpper(extension);
			
			if(strcmp(extension.c_str(), "WRL") == 0) {
				SaveHelixFileVRML(fout);
			} else if(strcmp(extension.c_str(), "VRML") == 0) {
				SaveHelixFileVRML(fout);
			} else if(strcmp(extension.c_str(), "SSE") == 0) {
				SaveHelixFileSSE(fout);
			} 
			fclose(fout);
		}

		string SSERenderer::GetSupportedLoadFileFormats() {
			return "VRML models (*.vrml *.wrl)";
		}

		string SSERenderer::GetSupportedSaveFileFormats() {
			return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
		}

		Vector3DFloat SSERenderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			Vector3DFloat position;
			switch(subsceneIndex) {
				case(0):
					if((ix0 >= 0) && (ix0 <= (int)helices.size())) {
						Point3 pt = helices[ix0]->GetCenter();
						position = Vector3DFloat(pt[0], pt[1], pt[2]);
					}
					break;
				case(1):
					position = Vector3DFloat(0,0,0);
					for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
						if(sheetMesh->faces[i].tag.id == ix0) {
							for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
								position += sheetMesh->vertices[sheetMesh->faces[i].vertexIds[j]].position;
							}						
						}
					}
					position = position * (1.0/(sheetMesh->faces.size() * 3.0));
					break;
				default:
					position = Vector3DFloat(0,0,0);
					break;
			}
			return position;
		}

		void SSERenderer::FitSelectedSSEs(Volume * vol) {
			SSEFlexibleFitter * fitter = new SSEFlexibleFitter(vol);

			const double discretizationStep = 0.01;


			for(unsigned int i = 0; i < helices.size(); i++) {					
				if(helices[i]->GetSelected()) {
					fitter->FitHelix(helices[i], 0.005, 1.0/360.0, discretizationStep, 200);
				}
			}

			/*if(sheetMesh != NULL) {
				for(unsigned int i = 0; i <= sheetCount; i++) {
					if(selectedSheets[i]) {
						fitter->FitSheet(i, sheetMesh, 0.005, 1.0/360.0, 0.1, 200);
					}
				}
			} */

			delete fitter;
			UpdateBoundingBox();
		}
	}
}


#endif