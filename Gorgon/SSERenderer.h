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
//   Revision 1.39  2010/01/14 23:34:25  ssa1
//   Allowing the deletion of SSEs from the SSEBuilder window
//
//   Revision 1.38  2009/12/28 17:42:27  ssa1
//   Fixing SSEBuilder bug when adding sheets
//
//   Revision 1.37  2009/12/24 21:53:49  ssa1
//   Giving back color control to the SSE Visualization options form when SSE Correspondence engine is not running (Bug ID: 58)
//
//   Revision 1.36  2009/12/22 01:02:24  schuhs
//   Adding support for beta sheet matching to the SSE correspondence search algorithm
//
//   Revision 1.35  2009/12/08 22:08:29  ssa1
//   Fixing SheetGenerator syntax issues (and instead of &&, etc..)
//
//   Revision 1.34  2009/12/08 20:06:06  abramsa
//   Some algorithms and implementation for better sheet generation, given an initial set of points.  For use in the SSEHunter.
//
//   Revision 1.33  2009/12/07 21:34:36  ssa1
//   Finding Rotation using SVD, and removing compiler warnings
//
//   Revision 1.32  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.31  2009/09/21 19:03:22  ssa1
//   Linear least squares fit implementation, and using it in helix positioning of SSE Builder
//
//   Revision 1.30  2009/09/17 20:00:24  ssa1
//   Steps towards exporting to Rosetta
//
//   Revision 1.29  2009/07/01 21:25:13  ssa1
//   Centering the volume cropped using a radius around the point selected by the atom selection tool.
//
//   Revision 1.28  2009/06/24 21:33:48  ssa1
//   SSE Builder Functionality: Sheet building and better camera functionality when loading new data.
//
//   Revision 1.27  2009/06/24 13:06:51  ssa1
//   Fixing compilation issues on MacOS
//
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
#include <ProteinMorph/SheetGenerator.h>
#include <GraphMatch/StandardGraph.h>
#include <GraphMatch/SkeletonReader.h>
#include <GraphMatch/GeometricShape.h>
#include <GraphMatch/VectorMath.h>
#include <MathTools/LinearSolver.h>
#include <ProteinMorph/SSEFlexibleFitter.h>
#include <vector>
#include <map>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::SkeletonMaker;
using namespace std;

namespace wustl_mm {
	namespace Visualization {	

		const float HELIX_LENGTH_TO_RESIDUE_RATIO = 1.54;

		class SSERenderer : public Renderer{
		public:
			SSERenderer();
			~SSERenderer();

			void AddHelix(Vector3DFloat p1, Vector3DFloat p2);
			void StartNewSSE();
			void AddSSEPoint(Vector3DFloat p);
			void FinalizeSheet();
			void FinalizeHelix();
			void Draw(int subSceneIndex, bool selectEnabled);
			void LoadHelixFile(string fileName);			
			void LoadSheetFile(string fileName);			
			void Unload();
			void LoadGraphSSE(int index, GeometricShape* sse, float offsetx, float offsety, float offsetz, float scalex, float scaley, float scalez);
			void UnloadGraphSSEs();
			void SetHelixColor(int index, float r, float g, float b, float a);
			void SetSheetColor(int index, float r, float g, float b, float a);
			void SetSSEColor(int index, float r, float g, float b, float a);
			bool SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle);
			int SelectionObjectCount();
			Vector3DFloat SelectionCenterOfMass();
			bool SelectionMove(Vector3DFloat moveDirection);
			bool SelectionClear();
			void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void SaveHelixFile(string fileName);
			void SaveSheetFile(string fileName);
			string GetSupportedHelixLoadFileFormats();
			string GetSupportedHelixSaveFileFormats();
			string GetSupportedSheetLoadFileFormats();
			string GetSupportedSheetSaveFileFormats();
			Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			void FitSelectedSSEs(Volume * vol);
			void SetSSESpecificColoring(bool isSSESpecific);
			void RemoveSelectedSSEs();
		private:
			void SheetListToMesh(vector<GeometricShape*> & sheets);
			void LoadHelixFileSSE(string fileName);
			void LoadHelixFileVRML(string fileName);
			void SaveHelixFileSSE(FILE* fout);
			void SaveHelixFileVRML(FILE* fout);
			void SaveSheetFileVRML(FILE* fout);
			void UpdateBoundingBox();
			vector<GeometricShape*> helices;
			vector<GeometricShape*> sheets;
			NonManifoldMesh_SheetIds * sheetMesh;
			NonManifoldMesh_SheetIds * graphSheetMesh;
			int sheetCount;
			int graphSheetCount;
			bool selectedSheets[256];
			bool selectedGraphSheets[256];
			vector<Vector3DFloat> tempSSEPoints;
			bool isSSESpecificColoring;
		};


		SSERenderer::SSERenderer() {
			helices.clear();
			sheetMesh = NULL;
			graphSheetMesh = NULL;
			sheetCount = 0;
			graphSheetCount = 0;

			isSSESpecificColoring = false;
		}

		SSERenderer::~SSERenderer() {
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];				
			}
			for(unsigned int i = 0; i < sheets.size(); i++) {
				delete sheets[i];				
			}
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
		}
		
		void SSERenderer::SetSSESpecificColoring(bool isSSESpecific) {
			isSSESpecificColoring = isSSESpecific;
		}

		void SSERenderer::AddHelix(Vector3DFloat p1, Vector3DFloat p2) {

			GeometricShape * newHelix = GeometricShape::CreateHelix(p1, p2, 2.5);

			helices.push_back(newHelix);
			UpdateBoundingBox();
		}

		void SSERenderer::FinalizeHelix() {
			Vector3DFloat p1, p2;
			LinearSolver::FindBestFitLine(p1, p2, tempSSEPoints);
			AddHelix(p1, p2);
		}
		
		void SSERenderer::StartNewSSE() {
			tempSSEPoints.clear();
		}		
		
		void SSERenderer::AddSSEPoint(Vector3DFloat p) {
			tempSSEPoints.push_back(p);
		}
		
		void SSERenderer::FinalizeSheet() {
			if(sheetMesh == NULL) {
				sheetMesh = new NonManifoldMesh_SheetIds();
				sheetCount = 0;
			}

			
			vector<int> vertexIxs;
			Vector3DFloat center = Vector3DFloat(0,0,0);
			
			for(unsigned int i = 0; i < tempSSEPoints.size(); i++) {
				vertexIxs.push_back(sheetMesh->AddVertex(tempSSEPoints[i], false));
				center+= tempSSEPoints[i];
			}
			if(tempSSEPoints.size() > 0) {
				center = center * (1.0/(double)tempSSEPoints.size());
			}
			
			GeometricShape * sheetShape = new GeometricShape();
			sheetShape->geometricShapeType = GRAPHEDGE_SHEET;
			sheetShape->SetCenter(center);

			sheets.push_back(sheetShape);

			sheetCount++;	
			
			SheetIdsAndSelect sheetTag;
			sheetTag.id = sheetCount;
			sheetTag.selected = false;
			

			vector<SheetGeneratorTriangle> newTriangles = SheetGenerator::sheetGenerator(tempSSEPoints, vertexIxs);
			for(unsigned int i = 0; i < newTriangles.size(); i++) {
				sheetMesh->AddTriangle(newTriangles[i].a, newTriangles[i].b, newTriangles[i].c, NULL, sheetTag);
			}
					
			tempSSEPoints.clear();
			UpdateBoundingBox();
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
					if(isSSESpecificColoring) {
						helices[i]->GetColor(colorR, colorG, colorB, colorA);	
						OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);
					}

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
				// for color code
				int prevSheet = -1;
				int thisSheet;
				float colorR, colorG, colorB, colorA;
				GLfloat diffuseMaterial[4];
				GLfloat ambientMaterial[4];
				GLfloat specularMaterial[4];
				// end color code
				for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {

					glPushAttrib(GL_LIGHTING_BIT);
					if(sheetMesh->faces[i].tag.selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}
					if(selectEnabled) {
						glLoadName(sheetMesh->faces[i].tag.id);
					}
					// color code
					if(sheetMesh->faces[i].tag.id != prevSheet) {
						thisSheet = (int) (sheetMesh->faces[i].tag.id);
						sheets[thisSheet-1]->GetColor(colorR, colorG, colorB, colorA);
						prevSheet = thisSheet;
						diffuseMaterial[0] = colorR;
						diffuseMaterial[1] = colorG;
						diffuseMaterial[2] = colorB;
						diffuseMaterial[3] = colorA;
						ambientMaterial[0] = colorR*0.2;
						ambientMaterial[1] = colorG*0.2;
						ambientMaterial[2] = colorB*0.2;
						ambientMaterial[3] = colorA;
						specularMaterial[0] = 1.0;
						specularMaterial[1] = 1.0; 
						specularMaterial[2] = 1.0;
						specularMaterial[3] = 1.0;
					}
					if(isSSESpecificColoring) {
						glColor4f(colorR, colorG, colorB, colorA);
					
						glMaterialfv(GL_BACK, GL_AMBIENT,   ambientMaterial);
						glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_BACK, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_BACK, GL_SHININESS, 0.1);
						glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) ;
						glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_FRONT, GL_SHININESS, 0.1);
					}

					glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					// end color code
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
					glPopAttrib(); // for color code
				}
				if(selectEnabled) {
					glPopName();
				}
			}
			else if((subSceneIndex == 2) && (graphSheetMesh != NULL)) {
				int k;
				if(selectEnabled) {
					glPushName(0);
				}
				// for color code
				int prevSheet = -1;
				int thisSheet;
				float colorR, colorG, colorB, colorA;
				GLfloat diffuseMaterial[4];
				GLfloat ambientMaterial[4];
				GLfloat specularMaterial[4];
				// end color code
				for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
					glPushAttrib(GL_LIGHTING_BIT);
					if(graphSheetMesh->faces[i].tag.selected) {
						glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
						glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
					}
					if(selectEnabled) {
						glLoadName(graphSheetMesh->faces[i].tag.id);
					}
					
					// color code
					if(graphSheetMesh->faces[i].tag.id != prevSheet) {
						//cout << "picking graph sheet color. i=" << i << ", id=" << (int) (graphSheetMesh->faces[i].tag.id) << endl;
						thisSheet = (int) (graphSheetMesh->faces[i].tag.id);
						sheets[thisSheet-1]->GetColor(colorR, colorG, colorB, colorA); // probably gets the wrong color.
						prevSheet = thisSheet;
						diffuseMaterial[0] = colorR;
						diffuseMaterial[1] = colorG;
						diffuseMaterial[2] = colorB;
						diffuseMaterial[3] = colorA;
						ambientMaterial[0] = colorR*0.2;
						ambientMaterial[1] = colorG*0.2;
						ambientMaterial[2] = colorB*0.2;
						ambientMaterial[3] = colorA;
						specularMaterial[0] = 1.0;
						specularMaterial[1] = 1.0; 
						specularMaterial[2] = 1.0;
						specularMaterial[3] = 1.0;
					}
					
					if(isSSESpecificColoring) {
						glColor4f(colorR, colorG, colorB, colorA);
					
						glMaterialfv(GL_BACK, GL_AMBIENT,   ambientMaterial);
						glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_BACK, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_BACK, GL_SHININESS, 0.1);
						glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) ;
						glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_FRONT, GL_SHININESS, 0.1);
					}

					glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					// end color code
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(unsigned int j = 0; j < graphSheetMesh->faces[i].vertexIds.size(); j++) {
						normal = graphSheetMesh->GetVertexNormal(graphSheetMesh->faces[i].vertexIds[j]);
						k = graphSheetMesh->GetVertexIndex(graphSheetMesh->faces[i].vertexIds[j]);
						glNormal3f(normal.X(), normal.Y(), normal.Z());
						glVertex3fv(graphSheetMesh->vertices[k].position.values);
					}
					glEnd();
					glPopAttrib();
					glPopAttrib(); // for color code
				}
				// end graph-type sheet rendering code

				if(selectEnabled) {
					glPopName();
				}
			}

			glPopName();
		}


		void SSERenderer::LoadHelixFileSSE(string fileName) {

			FILE* fin = fopen((char*)fileName.c_str(), "rt");
			
			char line[1000];
			string lineStr;
			float x1, x2, y1, y2, z1, z2;
			while(!feof(fin)) {
				fscanf(fin, "%s", line);
				lineStr = string(line);
				if(lineStr.compare("ALPHA") == 0) {
					fscanf(fin, "%s", line);
					fscanf(fin, "%s", line);
					fscanf(fin, "%s", line);
					fscanf(fin, "%s", line);
					fscanf(fin, "%f", &x1);
					fscanf(fin, "%f", &y1);
					fscanf(fin, "%f", &z1);
					fscanf(fin, "%f", &x2);
					fscanf(fin, "%f", &y2);
					fscanf(fin, "%f", &z2);
					AddHelix(Vector3DFloat(x1, y1, z2), Vector3DFloat(x2, y2, z2));
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
			//vector<GeometricShape *> sheets;
			sheets.clear();
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			SkeletonReader::ReadSheetFile((char *)fileName.c_str(), sheets);
			
			SheetListToMesh(sheets);

			UpdateBoundingBox();			
		}
		
		void SSERenderer::SheetListToMesh(vector<GeometricShape*> & sheets) {
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = new NonManifoldMesh_SheetIds();
			
			Point3 pt;
			vector<int> indices;
			SheetIdsAndSelect faceTag;
			faceTag.selected = false;

			sheetCount = sheets.size();
			for(int i = 0; i <= sheetCount; i++) {
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
			indices.clear();			
		}

		void SSERenderer::Unload() {
			Renderer::Unload();
			for(unsigned int i = 0; i < helices.size(); i++) {
				delete helices[i];
			}
			helices.clear();
			for(unsigned int i = 0; i < sheets.size(); i++) {
				delete sheets[i];
			}
			sheets.clear();
			if(sheetMesh != NULL) {
				delete sheetMesh;
			}
			sheetMesh = NULL;
			if(graphSheetMesh != NULL) {
				delete graphSheetMesh;
			}
			graphSheetMesh = NULL;
			UpdateBoundingBox();
		}

		void SSERenderer::LoadGraphSSE(int index, GeometricShape* sse, float offsetx, float offsety, float offsetz, float scalex, float scaley, float scalez) {
			// make a volume from the internal cells
			int xmin=MAXINT, xmax=-MAXINT, ymin=MAXINT, ymax=-MAXINT, zmin=MAXINT, zmax=-MAXINT;
			for (unsigned int i = 0; i < sse->internalCells.size(); i++) {
				if (sse->internalCells[i].x < xmin) {xmin = sse->internalCells[i].x;}
				if (sse->internalCells[i].x > xmax) {xmax = sse->internalCells[i].x;}
				if (sse->internalCells[i].y < ymin) {ymin = sse->internalCells[i].y;}
				if (sse->internalCells[i].y > ymax) {ymax = sse->internalCells[i].y;}
				if (sse->internalCells[i].z < zmin) {zmin = sse->internalCells[i].z;}
				if (sse->internalCells[i].z > zmax) {zmax = sse->internalCells[i].z;}
			}
			int dimx = xmax-xmin+1;
			int dimy = ymax-ymin+1;
			int dimz = zmax-zmin+1;
			Volume* vol = new Volume( dimx, dimy, dimz) ;
			vol->setOrigin(xmin, ymin, zmin);
			for (unsigned int i = 0; i < sse->internalCells.size(); i++) {
				vol->setDataAt( sse->internalCells[i].x-xmin, sse->internalCells[i].y-ymin, sse->internalCells[i].z-zmin, 1.0 ) ;
			}

			// make a mesh from the volume
			NonManifoldMesh_Annotated * thisSheetMesh = new NonManifoldMesh_Annotated(vol);
			delete vol;
			// add offset to all points in new mesh
			for (unsigned int i = 0; i < thisSheetMesh->vertices.size(); i++) {
				Vector3DFloat newpos = thisSheetMesh->vertices[i].position;
				newpos.values[0] *= scalex;
				newpos.values[1] *= scaley;
				newpos.values[2] *= scalez;
				thisSheetMesh->vertices[i].position = newpos + Vector3DFloat(xmin * scalex, ymin * scaley, zmin * scalez) + Vector3DFloat(offsetx, offsety, offsetz);
			}

			// merge this mesh with the mesh containing other sheets
			if(graphSheetMesh == NULL) {
				graphSheetMesh = new NonManifoldMesh_SheetIds();
				graphSheetCount = 0;
			}
			if (thisSheetMesh->faceCount > 0) {
				// mesh merging code. not using code NonManifoldMesh::mergeMesh because it doesn't allow setting of tags
				graphSheetCount++;
				SheetIdsAndSelect faceTag = SheetIdsAndSelect();
				faceTag.id = graphSheetCount;
				selectedGraphSheets[index - helices.size() + 1] = false;
				faceTag.selected = false;
				vector<int> indices;
				indices.clear();
				for(unsigned int i = 0; i < thisSheetMesh->vertices.size(); i++) {
					indices.push_back(graphSheetMesh->AddVertex(thisSheetMesh->vertices[i]));
				}
				for(unsigned int i = 0; i < thisSheetMesh->edges.size(); i++) {
					graphSheetMesh->AddEdge(indices[thisSheetMesh->edges[i].vertexIds[0]], indices[thisSheetMesh->edges[i].vertexIds[1]], thisSheetMesh->edges[i].tag);
				}
				for(unsigned int i = 0; i < thisSheetMesh->faces.size(); i++) {
					if(thisSheetMesh->faces[i].vertexIds.size() == 3) {
						graphSheetMesh->AddTriangle(indices[thisSheetMesh->faces[i].vertexIds[0]], indices[thisSheetMesh->faces[i].vertexIds[1]], indices[thisSheetMesh->faces[i].vertexIds[2]], NULL, faceTag);
					}
				}
				// end mesh merging
			}

			delete thisSheetMesh;
			return;
		}

		void SSERenderer::UnloadGraphSSEs() {
			graphSheetCount = 0;
			if(graphSheetMesh != NULL) {
				delete graphSheetMesh;
			}
			graphSheetMesh = NULL;

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

		void SSERenderer::SetSheetColor(int index, float r, float g, float b, float a) {
			sheets[index]->SetColor(r, g, b, a);
		}

		// set the color of an SSE. assumes that SSEs are indexed with helices first and sheets second.
		void SSERenderer::SetSSEColor(int index, float r, float g, float b, float a) {
			int numHelices = helices.size();
			if (index < numHelices) {
				helices[index]->SetColor(r, g, b, a);
			} else {
				sheets[index - numHelices]->SetColor(r, g, b, a);
			}
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
				for(int i = 0; i <= sheetCount; i++) {
					if(selectedSheets[i]) {
						count++;
					}
				}
			}

			if(graphSheetMesh != NULL) {
				for(int i = 0; i <= graphSheetCount; i++) {
					if(selectedGraphSheets[i]) {
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

				for(int j = 0; j <= this->sheetCount; j++) {
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
					for(int i = 0; i <= sheetCount; i++) {
						selectedSheets[i] = false;
					}
				}

				if(graphSheetMesh != NULL) {
					for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
						graphSheetMesh->faces[i].tag.selected = false;
					}
					for(int i = 0; i <= graphSheetCount; i++) {
						selectedGraphSheets[i] = false;
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
				sheets[ix0-1]->SetSelected(selectedSheets[ix0]);
			}
			if((subsceneIndex == 2)) {
				for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
					if(graphSheetMesh->faces[i].tag.id == ix0) {
						graphSheetMesh->faces[i].tag.selected = forceTrue || !graphSheetMesh->faces[i].tag.selected;
					}
				}

				selectedGraphSheets[ix0] = forceTrue || !selectedGraphSheets[ix0];
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
		
		void SSERenderer::SaveSheetFileVRML(FILE* fout) {
			
			map<int, int> vertexIxs;
			vector<Vector3DFloat> vertices;
			bool addVertex;
			
			NonManifoldMeshVertex<bool> tempVertex;
			NonManifoldMeshEdge<bool> tempEdge;
			NonManifoldMeshFace<SheetIdsAndSelect> tempFace;
			
			fprintf(fout, "#VRML V2.0 utf8\n");
			
			for(int i = 0; i < sheetCount; i++) {
				vertexIxs.clear();
				vertices.clear();
				
				for(unsigned int j = 0; j < sheetMesh->vertices.size(); j++) {
					tempVertex = sheetMesh->vertices[j];
					addVertex = false;
					
					for(unsigned int k = 0; k < tempVertex.edgeIds.size(); k++) {
						tempEdge = sheetMesh->edges[sheetMesh->GetEdgeIndex(tempVertex.edgeIds[k])];
						
						for(unsigned int l = 0; l < tempEdge.faceIds.size(); l++) {
							tempFace = sheetMesh->faces[sheetMesh->GetFaceIndex(tempEdge.faceIds[l])];
							if(tempFace.tag.id == i+1) {
								addVertex = true;
							}
						}
						
					}
					
					if(addVertex) {
						vertexIxs[j] = vertices.size();
						vertices.push_back(tempVertex.position);
					}					
				}
								
				fprintf(fout, "Shape {\n");
				fprintf(fout, "    appearance Appearance {\n");
				fprintf(fout, "        material Material {\n");
				fprintf(fout, "            emissiveColor 0.0 0.0 0.5\n");
				fprintf(fout, "        }\n");
				fprintf(fout, "    }\n");
				fprintf(fout, "    geometry IndexedFaceSet {\n");
				fprintf(fout, "       coord Coordinate {\n");
				fprintf(fout, "            point [\n");
				
				for(unsigned int j = 0; j < vertices.size(); j++) {
					fprintf(fout, "                 %f %f %f,\n", vertices[j].X(), vertices[j].Y(), vertices[j].Z());
				}
				
				fprintf(fout, "            ]\n");
				fprintf(fout, "        }\n");
				fprintf(fout, "        coordIndex [\n");
				
				int vertexIx, startVertexIx;;
				for(unsigned int j = 0; j < sheetMesh->faces.size(); j++) {
					tempFace = sheetMesh->faces[j];
					if(tempFace.tag.id == i+1) {
						fprintf(fout, "            ");
						
						for(unsigned int k = 0; k < tempFace.vertexIds.size(); k++) {
							vertexIx = sheetMesh->GetVertexIndex(tempFace.vertexIds[k]);
							if(k == 0) {
								startVertexIx = vertexIx;
							}
							fprintf(fout, "%d,", vertexIxs[vertexIx]);				
						}
						
						if(tempFace.vertexIds.size() > 0) {
							fprintf(fout, "%d\n", vertexIxs[startVertexIx]);
						} else {
							fprintf(fout, "\n");
						}				
					}
					
				}
				
				fprintf(fout, "        ]\n");
				fprintf(fout, "    }\n");
				fprintf(fout, "}\n");
			}
			
		}		
		
		void SSERenderer::SaveSheetFile(string fileName) {
			FILE* fout = fopen((char*)fileName.c_str(), "wt");
			int pos = fileName.rfind(".") + 1;
			string extension = fileName.substr(pos, fileName.length()-pos);
			
			extension = StringUtils::StringToUpper(extension);
			
			if(strcmp(extension.c_str(), "WRL") == 0) {
				SaveSheetFileVRML(fout);
			} else if(strcmp(extension.c_str(), "VRML") == 0) {
				SaveSheetFileVRML(fout);
			} 
			fclose(fout);
		}		

		string SSERenderer::GetSupportedHelixLoadFileFormats() {
			return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
		}

		string SSERenderer::GetSupportedSheetLoadFileFormats() {
			return "VRML models (*.vrml *.wrl)";
		}
		
		string SSERenderer::GetSupportedHelixSaveFileFormats() {
			return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
		}

		string SSERenderer::GetSupportedSheetSaveFileFormats() {
			return "All Supported Formats(*.vrml *.wrl);; VRML models (*.vrml *.wrl)";
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
		
		void SSERenderer::RemoveSelectedSSEs() {
			vector<GeometricShape*> newHelices;
			for(unsigned int i = 0; i < helices.size(); i++) {
				if(helices[i]->GetSelected()) {
					delete helices[i];
				} else {
					newHelices.push_back(helices[i]);
				}
			}
			helices = newHelices;
			
			
			vector<GeometricShape*> newSheets;
			for(int i = 0; i < (int)sheets.size(); i++) {
				if(sheets[i]->GetSelected()) {
					delete sheets[i];
				} else {
					newSheets.push_back(sheets[i]);
				}
			}
			sheets = newSheets;	
			sheetCount = newSheets.size();
			
			SheetListToMesh(sheets);
			UpdateBoundingBox();
		}
	}
}


#endif
