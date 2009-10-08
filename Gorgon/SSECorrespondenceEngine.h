// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for finding the correspondance between secondary structure elements and the amino acid sequence.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.7.2.9  2009/08/17 21:48:38  schuhs
//   Fixing a bug that caused a crash when drawing a correspondence that ends with skip edges.
//
//   Revision 1.7.2.8  2009/08/11 20:50:21  schuhs
//   Paint helix corner 0 white and corner 1 gray.
//
//   Revision 1.7.2.7  2009/07/21 14:57:12  schuhs
//   Coloring paths between helices and sheets with color gradient to demonstrate the connectivity more clearly
//
//   Revision 1.7.2.6  2009/07/16 21:04:57  schuhs
//   Fixing indexing bug so that all paths are drawn with DrawAllPaths method
//
//   Revision 1.7.2.5  2009/07/14 19:58:29  schuhs
//   Adding support for correspondences that contain sheet-strand matches
//
//   Revision 1.7.2.4  2009/06/19 17:42:56  schuhs
//   Adding console messages to help debug memory error
//
//   Revision 1.7.2.3  2009/05/28 16:54:48  schuhs
//   Adding integer casts to loop variables eliminate compiler warnings
//
//   Revision 1.7.2.2  2009/05/22 19:21:18  schuhs
//   Adding parameters to DrawAllPaths method to enable or disable rendering of corners, paths, and colored sheets.
//
//   Revision 1.7.2.1  2009/05/13 20:52:52  schuhs
//   Adding method to draw all skeleton paths used by correspondence algorithm
//
//   Revision 1.7  2008/12/01 23:42:55  ssa1
//   Setting theming support for backbone trace
//
//   Revision 1.6  2008/11/07 21:22:25  ssa1
//   Fixing memory corruption errors when python garbage collects c++ objects
//
//   Revision 1.5  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//

#ifndef GORGON_SSE_CORRESPONDENCE_ENGINE_H
#define GORGON_SSE_CORRESPONDENCE_ENGINE_H

#include <GraphMatch/SSECorrespondenceResult.h>
#include <GraphMatch/BackEndInterface.h>
#include <vector>
#include <glut.h>
#include "Renderer.h" // to include DrawSphere function
#include "MeshRenderer.h" // to include Draw function

using namespace wustl_mm::GraphMatch;
using namespace std;

namespace wustl_mm {
	namespace Visualization {	
		class SSECorrespondenceEngine : public BackEndInterface {
		public:
			SSECorrespondenceEngine();
			~SSECorrespondenceEngine();

			int ExecuteQuery();			
			int GetResultCount();
			int LoadCorrespondenceFromFile(string fileName);
			SSECorrespondenceResult GetResult(int rank);
			string GetSupportedLoadFileFormats();
			string GetSupportedSaveFileFormats();
			void SaveCorrespondenceToFile(string fileName);
			GeometricShape * GetSkeletonSSE(int sseId);
			SecondaryStructure * GetSequenceSSE(int sseId);
			int GetSkeletonSSECount();
			int GetSequenceSSECount();
			void SetSSEColor(int index, float r, float g, float b, float a);
			void SetVisibleCorrespondence(int correspondenceIndex);
			void Draw(int sceneIndex);
			//void DrawAllPaths(int sceneIndex);
			void DrawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors);

			
		private:
			vector<SSECorrespondenceResult> correspondence;		
			int correspondenceIndex;
		};
		

		SSECorrespondenceEngine::SSECorrespondenceEngine() {
			correspondence.clear();
			correspondenceIndex = -1;
		}

		SSECorrespondenceEngine::~SSECorrespondenceEngine() {
			correspondence.clear();			
		}

		int SSECorrespondenceEngine::ExecuteQuery() {
			cout << "Begin SSECorrespondenceEngine::ExecuteQuery" << endl;
			if(skeleton != NULL && sequence != NULL) {
				cout << "Starting matching" << endl;
				int resultCount = queryEngine->DoGraphMatching(sequence, skeleton);
				cout << "Matching complete. Found " << resultCount << " results" << endl;
				correspondence.clear();
				for(int i = 1; i <= resultCount; i++) {
					correspondence.push_back(queryEngine->GetSolution(i));
				}
				cout << correspondence.size() << " results stored in correspondence" << endl;
				return resultCount;
			} else {
				cout << "skeleton or sequence is not defined." << endl;
				return 0;
			}
		}

		int SSECorrespondenceEngine::GetResultCount() {
			return correspondence.size();
		}

		int SSECorrespondenceEngine::LoadCorrespondenceFromFile(string fileName) {

			FILE* fin = fopen((char*)fileName.c_str(), "rt");
			if (fin == NULL)
			{
				printf("Error opening input file %s.\n", fileName.c_str()) ;
				exit(0) ;
			}

			correspondence.clear();

			int correspondenceCount = 0, nodeCount, skeletonNode;
			vector<int> nodes;
			double cost;
			fscanf(fin, "%d\n", &correspondenceCount);

			for(int i = 0; i < correspondenceCount; i++) {
				nodes.clear();
				fscanf(fin, "%d ", &nodeCount);				
				for(int j = 0; j < nodeCount; j++) {
					fscanf(fin, "%d ", &skeletonNode);
					nodes.push_back(skeletonNode);
				}
				fscanf(fin, "%lf\n", &cost);
				// TODO: Fix! 0 not acceptable!
				correspondence.push_back(SSECorrespondenceResult(nodes, cost, 0));
			}

			fclose(fin);

			return correspondenceCount;
		}

		SSECorrespondenceResult SSECorrespondenceEngine::GetResult(int rank) {
			// TODO: Fix!
			//if(rank <= (int)correspondence.size() && (rank >= 1)) {
				return correspondence[rank-1];
			//} else {
			//	return NULL;
			//}
		}

		string SSECorrespondenceEngine::GetSupportedLoadFileFormats() {
			return "Correspondence Results (*.corr)";
		}

		string SSECorrespondenceEngine::GetSupportedSaveFileFormats() {
			return "Correspondence Results (*.corr)";
		}
		void SSECorrespondenceEngine::SaveCorrespondenceToFile(string fileName) {
			FILE* fout = fopen((char*)fileName.c_str(), "wt");
			if (fout == NULL)
			{
				printf("Error opening output file %s.\n", fileName.c_str()) ;
				exit(0) ;
			}

			fprintf(fout, "%ld\n", correspondence.size());
			for(unsigned int i = 0; i < correspondence.size(); i++) {
				fprintf(fout, "%d ", correspondence[i].GetNodeCount());
				for(int j = 0; j < correspondence[i].GetNodeCount(); j++) {
					fprintf(fout, "%d ", correspondence[i].GetSkeletonNode(j));
				}
				fprintf(fout, "%lf\n", correspondence[i].GetCost());
			}

			fclose(fout);
		}

		GeometricShape * SSECorrespondenceEngine::GetSkeletonSSE(int sseId) {
			if((skeleton != NULL) && (sseId < (int)skeleton->skeletonHelixes.size())) {
				return skeleton->skeletonHelixes[sseId];				
			} else {
				return NULL;
			}			
		}

		SecondaryStructure * SSECorrespondenceEngine::GetSequenceSSE(int sseId) {
			if((sequence != NULL) && (sseId < (int)sequence->pdbStructures.size())) {
				return sequence->pdbStructures[sseId];				
			} else {
				return NULL;
			}			
		}


		int SSECorrespondenceEngine::GetSkeletonSSECount() {
			return skeleton->skeletonHelixes.size();
		}

		int SSECorrespondenceEngine::GetSequenceSSECount() {
			return sequence->pdbStructures.size();
		}

		// set the color of an SSE.
		void SSECorrespondenceEngine::SetSSEColor(int index, float r, float g, float b, float a) {
			skeleton->skeletonHelixes[index]->SetColor(r, g, b, a);
		}

		void SSECorrespondenceEngine::SetVisibleCorrespondence(int correspondenceIndex) {
			this->correspondenceIndex = correspondenceIndex;
		}

		void SSECorrespondenceEngine::Draw(int sceneIndex) {
			//std::cout << "SSECorrespondenceEngine::Draw called" << std::endl;
			int n1, n2;
			vector<Vector3DInt> path;
			if(correspondenceIndex >= 0) {
				SSECorrespondenceResult result = GetResult(correspondenceIndex + 1);

				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				glDisable(GL_LIGHTING);
				glLineWidth(5);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	
				glBegin(GL_LINE_STRIP); // test!!! 

				n1 = -1;
				n2 = -1;
				// the following code iterates over the correspondence, finding a valid edge at each iteration.
				// start at node 0 of this result, continue until i is at last node
				for(int i = 0; i < result.GetNodeCount()-1; ) {
					// set n1 to be the ith result. if the result is -1, increment i and repeat.
					for(n1 = -1; n1 < 0; ) {
						n1 = result.GetSkeletonNode(i);
						i++;
					}
					// set n2 to be the ith result. if the result is -1, increment i and repeat
					//for(n2 = -1; n2 < 0l; ) {
					for(n2 = -1; n2 < 0; ) {
						n2 = result.GetSkeletonNode(i);
						i++;
						if (i >= result.GetNodeCount()) {
							//cout << "found skip edge at end of correspondence. breaking out of loop." << endl;
							break;
						}
					}
					i--;
					path = skeleton->paths[n1][n2];
					//cout << "path sizes. fwd:" << skeleton->paths[n1][n2].size() << ", rev:" << skeleton->paths[n2][n1].size() << endl;
					if(path.size() == 0) {
						path = skeleton->paths[n2][n1];
						if(path.size() != 0) {
							cout << "CODE SHOULD NEVER GET HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
						}
						int n1old = n1;
						n1 = n2;
						n2 = n1old;
					}

					// color code

					// get colors of beginning and ending SSEs
					int numHelices = skeleton->GetHelixCount();

					// start SSE color
					int startSSENumber;
					float startColorR, startColorG, startColorB, startColorA;
					if(skeleton->adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
						startSSENumber = n1 - numHelices;
					} else {
						startSSENumber = n1/2;
					}
					skeleton->skeletonHelixes[startSSENumber]->GetColor(startColorR, startColorG, startColorB, startColorA);

					// end SSE color
					int endSSENumber;
					float endColorR, endColorG, endColorB, endColorA;
					if(skeleton->adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
						endSSENumber = n2 - numHelices;
					} else {
						endSSENumber = n2/2;
					}
					skeleton->skeletonHelixes[endSSENumber]->GetColor(endColorR, endColorG, endColorB, endColorA);

					// test!!! glBegin(GL_LINE_STRIP);
					int pathSize = path.size(); // for color
					float stepColorR = (endColorR - startColorR) / (pathSize-1);
					float stepColorG = (endColorG - startColorG) / (pathSize-1);
					float stepColorB = (endColorB - startColorB) / (pathSize-1);
					for(int j = 0; j < pathSize; j++) {
						//cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
						glColor3f(startColorR + stepColorR * j, startColorG + stepColorG * j, startColorB + stepColorB * j);
						glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
					}
					// test!!! glEnd();


					// end color code

					/* works fine, before coloring code added
					glBegin(GL_LINE_STRIP);
					//cout << "draw " << i << " to " << i+1 << " returns nodes " << n1+1 << " and " << n2+1 << ", which has length " << skeleton->paths[n1+1][n2+1].size() << endl;
					for(unsigned int j = 0; j < path.size(); j++) {
						glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
					}
					glEnd();
					*/

					//n1 = -1;
					//n2 = -1;
				}
				/* old code, worked
				for(int i = 0; i < result.GetNodeCount()-1; i++) {
					n1 = result.GetSkeletonNode(i);
					n2 = result.GetSkeletonNode(i+1);
					cout << "i=" << i << ", n1 = " << n1 << ", n2 = " << n2 << endl;
					//if((n1 >= 0)  && (n2 >= 0)) {
					cout << "draw " << i << " to " << i+1 << " returns nodes " << n1 << " and " << n2 << ", which has length " << skeleton->paths[n1][n2].size() << endl;
					//if((n1+1 >= 0)  && (n2+1 >= 0) && (skeleton->adjacencyMatrix[n1+1][n2+1][0] != GRAPHEDGE_HELIX)) {
					if((n1 >= 0)  && (n2 >= 0)){ // && (skeleton->adjacencyMatrix[n1+1][n2+1][0] != GRAPHEDGE_HELIX)) {
						path = skeleton->paths[n1][n2];
						if(path.size() == 0) {
							path = skeleton->paths[n2][n1];
						}
						glBegin(GL_LINE_STRIP);
						cout << "draw " << i << " to " << i+1 << " returns nodes " << n1+1 << " and " << n2+1 << ", which has length " << skeleton->paths[n1+1][n2+1].size() << endl;
						for(unsigned int j = 0; j < path.size(); j++) {
							glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
						}
						glEnd();
					}
				}*/
				glEnd(); // test!!! 
				glPopAttrib();
			}
		}	
		
		// Draw all possible paths through the skeleton
		void SSECorrespondenceEngine::DrawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors) {
			std::cout << "SSECorrespondenceEngine::DrawAllPaths called" << std::endl;
			int n1, n2;
			vector<Vector3DInt> path;
			if (showPaths) {
				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				//glDisable(GL_LIGHTING);
				glLineWidth(5);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	

				int nodeCount = skeleton->GetNodeCount();
				for(int i = 0; i < nodeCount; i++) {
					for(int j = i+1; j < nodeCount; j++) {
						//cout << "adding path from " << i << " to " << j << endl;
						n1 = i;
						n2 = j;
						if((n1 >= 0)  && (n2 >= 0)) {
							path = skeleton->paths[n1][n2];
							if(path.size() == 0) {
								path = skeleton->paths[n2][n1];
								int n1old = n1;
								n1 = n2;
								n2 = n1old;
							}
							//cout << "adding path from " << n1 << " to " << n2 << ", which has length " << path.size() << endl;

							// get colors of beginning and ending SSEs
							int numHelices = skeleton->GetHelixCount();

							// start SSE color
							int startSSENumber;
							float startColorR, startColorG, startColorB, startColorA;
							if(skeleton->adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
								startSSENumber = n2 - numHelices;
							} else {
								startSSENumber = n2/2;
							}
							skeleton->skeletonHelixes[startSSENumber]->GetColor(startColorR, startColorG, startColorB, startColorA);

							// end SSE color
							int endSSENumber;
							float endColorR, endColorG, endColorB, endColorA;
							if(skeleton->adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
								endSSENumber = n1 - numHelices;
							} else {
								endSSENumber = n1/2;
							}
							skeleton->skeletonHelixes[endSSENumber]->GetColor(endColorR, endColorG, endColorB, endColorA);

							/*
							if(skeleton->adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
								int correspondingSheet = skeleton->skeletonSheetCorrespondence[n2];
								skeleton->skeletonHelixes[correspondingSheet]->GetColor(endColorR, endColorG, endColorB, endColorA);	
							} else {
								skeleton->skeletonHelixes[n2/2]->GetColor(endColorR, endColorG, endColorB, endColorA);
							}*/

							glBegin(GL_LINE_STRIP);
							int pathSize = path.size(); // for color
							float stepColorR = (endColorR - startColorR) / (pathSize-1);
							float stepColorG = (endColorG - startColorG) / (pathSize-1);
							float stepColorB = (endColorB - startColorB) / (pathSize-1);
							for(int k = 0; k < pathSize; k++) {
								//cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
								glColor3f(startColorR + stepColorR * k, startColorG + stepColorG * k, startColorB + stepColorB * k);
								glVertex3d(path[k].X(), path[k].Y(), path[k].Z());
							}
							/* before colors
							for(unsigned int j = 0; j < path.size(); j++) {
								//cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
								glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
							} */
							glEnd();
							// draw start and end of paths (corner nodes)
							//Renderer::DrawSphere(Vector3DFloat(path[0].X(), path[0].Y(), path[0].Z()), 1.0);
						}
					}
				} 
				glPopAttrib();
			}

			// draw start and end of paths (subset of corner nodes)
			/*
			for(int i = 0; i < skeleton->GetNodeCount(); i++) {
				for(int j = i; j < skeleton->GetNodeCount(); j++) {
					n1 = i;
					n2 = j;
					if((n1 >= 0)  && (n2 >= 0)) {
						path = skeleton->paths[n1][n2];
						if(path.size() > 0) {
							Renderer::DrawSphere(Vector3DFloat(path[0].X(), path[0].Y(), path[0].Z()), 1.0);
							Renderer::DrawSphere(Vector3DFloat(path[path.size()-1].X(), path[path.size()-1].Y(), path[path.size()-1].Z()), 1.0);
						}
					}
				}
			}
			*/

			// draw corner nodes (helices)
			if (showHelixCorners) {
				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				for(int i = 0; i < (int)skeleton->skeletonHelixes.size(); i++) {
					if (skeleton->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
						for(int j = 0; j < (int)skeleton->skeletonHelixes[i]->cornerCells.size(); j++) {
							// Color first helix corner white, second corner gray
							GLfloat col = 1.0 - 0.6 * (skeleton->skeletonHelixes[i]->cornerCells[j].node - 1);
							glColor3f(col, col, col);
							Renderer::DrawSphere(Vector3DFloat(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z), 0.25);
						}
					}
				}
				glPopAttrib();
			}			

			// draw corner nodes (sheets)
			if (showSheetCorners) {
				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				for(int i = 0; i < (int)skeleton->skeletonHelixes.size(); i++) {
					if (skeleton->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
						for(int j = 0; j < (int)skeleton->skeletonHelixes[i]->cornerCells.size(); j++) {
							Renderer::DrawSphere(Vector3DFloat(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z), 0.25);
						}
					}
				}
				glPopAttrib();
			}			


			MeshRenderer * sheetMeshRenderer = new MeshRenderer();

			// render each skeleton sheet
			if (showSheetColors) {

				
				cout << "debug 1" << endl;
				// draw internal nodes of sheets
				cout << "debug 2" << endl;
				float colorR, colorG, colorB, colorA;
				cout << "debug 3" << endl;
				for(int i = 0; i < (int)skeleton->skeletonHelixes.size(); i++) {
				cout << "debug 4" << endl;
					if (skeleton->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
						cout << "debug 5" << endl;
						skeleton->skeletonHelixes[i]->GetColor(colorR, colorG, colorB, colorA);	
						glColor4f(colorR, colorG, colorB, colorA);
						GLfloat diffuseMaterial[4] = {colorR, colorG, colorB, colorA};
						GLfloat ambientMaterial[4] = {colorR*0.2, colorG*0.2, colorB*0.2, colorA};
						GLfloat specularMaterial[4] = {1.0, 1.0, 1.0, 1.0};

						glMaterialfv(GL_BACK, GL_AMBIENT,   ambientMaterial);
						glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_BACK, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_BACK, GL_SHININESS, 0.1);
						glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) ;
						glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_FRONT, GL_SHININESS, 0.1);

						cout << "debug 6" << endl;
						glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
						cout << "debug 7" << endl;

						for(int j = 0; j < (int)skeleton->skeletonHelixes[i]->internalCells.size(); j++) {
							Renderer::DrawSphere(Vector3DFloat(skeleton->skeletonHelixes[i]->internalCells[j].x, skeleton->skeletonHelixes[i]->internalCells[j].y, skeleton->skeletonHelixes[i]->internalCells[j].z), 0.25);
						}
						cout << "debug 8" << endl;
						glPopAttrib();
						cout << "debug 9" << endl;
					}
				}

			}

			/* old method for rendering sheets: render each from stored volumes 

			MeshRenderer * sheetMeshRenderer = new MeshRenderer();
			// render each skeleton sheet
			if (showSheetColors) {

				for (int i = 1; i < (int)skeleton->skeletonSheets.size(); i++) {
					int correspondingSheet = skeleton->skeletonSheetCorrespondence[i];
					if (correspondingSheet != -1) {
						// when sheets stored as volumes:
						//sheetMeshRenderer->LoadVolume(skeleton->skeletonSheets[i]);
						// when sheets stored as meshes:
						sheetMeshRenderer->LoadVolume(skeleton->skeletonSheets[i]);
						float colorR, colorG, colorB, colorA;
						skeleton->skeletonHelixes[correspondingSheet]->GetColor(colorR, colorG, colorB, colorA);	
						//cout << "this sheet matches SSE result " << correspondingSheet << ", which has colors " << colorR << "," << colorG << "," << colorB << "," << colorA << endl;
						glColor4f(colorR, colorG, colorB, colorA);
						GLfloat diffuseMaterial[4] = {colorR, colorG, colorB, colorA};
						GLfloat ambientMaterial[4] = {colorR*0.2, colorG*0.2, colorB*0.2, colorA};
						GLfloat specularMaterial[4] = {1.0, 1.0, 1.0, 1.0};

						glMaterialfv(GL_BACK, GL_AMBIENT,   ambientMaterial);
						glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_BACK, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_BACK, GL_SHININESS, 0.1);
						glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) ;
						glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) ;
						glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) ;
						glMaterialf(GL_FRONT, GL_SHININESS, 0.1);

						glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

						sheetMeshRenderer->Draw(0, false);

						glPopAttrib();
					}
				}
			}
			delete sheetMeshRenderer;
			*/
		}
	}
}

#endif