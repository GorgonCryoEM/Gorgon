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
				correspondence.push_back(SSECorrespondenceResult(nodes, cost));
			}

			fclose(fin);

			return correspondenceCount;
		}

		SSECorrespondenceResult SSECorrespondenceEngine::GetResult(int rank) {
			if(rank <= (int)correspondence.size() && (rank >= 1)) {
				return correspondence[rank-1];
			} else {
				return NULL;
			}
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


				for(int i = 2; i < result.GetNodeCount(); i += 2) {
					n1 = result.GetSkeletonNode(i-1);
					n2 = result.GetSkeletonNode(i);
					if((n1 >= 0)  && (n2 >= 0)) {
						path = skeleton->paths[n1][n2];
						if(path.size() == 0) {
							path = skeleton->paths[n2][n1];
						}
						glBegin(GL_LINE_STRIP);
						for(unsigned int j = 0; j < path.size(); j++) {
							glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
						}
						glEnd();
					}
				}
				glPopAttrib();
			}
		}	
		
		// Draw all possible paths through the skeleton
		void SSECorrespondenceEngine::DrawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors) {
			//std::cout << "SSECorrespondenceEngine::DrawAllPaths called" << std::endl;
			int n1, n2;
			vector<Vector3DInt> path;
			if (showPaths) {
				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				//glDisable(GL_LIGHTING);
				glLineWidth(5);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	

				
				for(int i = 0; i < skeleton->GetNodeCount(); i++) {
					for(int j = i; j < skeleton->GetNodeCount(); j++) {
						n1 = i;
						n2 = j;
						if((n1 >= 0)  && (n2 >= 0)) {
							path = skeleton->paths[n1][n2];
							if(path.size() == 0) {
								path = skeleton->paths[n2][n1];
							}
							glBegin(GL_LINE_STRIP);
							for(unsigned int j = 0; j < path.size(); j++) {
								glVertex3d(path[j].X(), path[j].Y(), path[j].Z());
							}
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
				for (int i = 1; i < (int)skeleton->skeletonSheets.size(); i++) {
					int correspondingSheet = skeleton->skeletonSheetCorrespondence[i];
					if (correspondingSheet != -1) {
						sheetMeshRenderer->LoadVolume(skeleton->skeletonSheets[i]);
						float colorR, colorG, colorB, colorA;
						skeleton->skeletonHelixes[correspondingSheet]->GetColor(colorR, colorG, colorB, colorA);	
						//cout << "this sheet matches SSE result " << correspondingSheet << ", which has colors " << colorR << "," << colorG << "," << colorB << "," << colorA << endl;
						/*
						float color = 0.3 * (float)i;
						colorR = color;
						colorG = color;
						colorB = 1.0;
						colorA = 1.0;
						cout << "chosen colors for this sheet are " << colorR << "," << colorG << "," << colorB << "," << colorA << endl;
						*/
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
		}
	}
}

#endif