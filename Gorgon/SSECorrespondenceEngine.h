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
//   Revision 1.7.2.15  2009/12/20 20:25:53  schuhs
//   Preparing to merge with trunk: copied in changes from trunk
//
//   Revision 1.7.2.14  2009/12/20 20:17:36  schuhs
//   Removing old, unused sheet rendering code
//
//   Revision 1.7.2.13  2009/12/15 22:54:34  schuhs
//   Labeling path with node numbers at entry points of sheets and helices.
//
//   Revision 1.7.2.12  2009/11/05 17:25:30  schuhs
//   Comment out console messages used for debugging
//
//   Revision 1.7.2.11  2009/10/29 16:32:26  schuhs
//   Add text labels of node numbers to sheet and helix corners
//
//   Revision 1.7.2.10  2009/10/08 21:52:21  schuhs
//   Rendering sheets by drawing colored spheres rather than a colored volume. Eliminates the need for multiple stored volumes.
//
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
#include <GraphMatch/GeometricShape.h>
#include <vector>
#include <map>
#include <set>
#include "MeshRenderer.h" // to include Draw function
#include "Renderer.h" // to include DrawSphere function
#include <glut.h>
#include <Foundation/StringUtils.h>

using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::Foundation;
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
			void DrawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors);

			void InitializePathFinder(NonManifoldMesh_Annotated * mesh);
			void InitializePathHelix(int helixIndex, Vector3DFloat p1, Vector3DFloat p2, float radius);
			void PrunePathMesh(NonManifoldMesh_Annotated * mesh, vector<unsigned int> pathVertices, set<unsigned int> preserve);
			void GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start);
			int GetPathVertexCount();
			Vector3DFloat GetPathVertex(int index);
			int GetPathEdgeCount();
			int GetEdgeVertexIndex(int index, int side);
			void ClearPathSpace();
			void ClearPathFinder();
			
		private:
			vector<SSECorrespondenceResult> correspondence;		
			int correspondenceIndex;
			
			// Attributes for path calculation
			NonManifoldMesh_Annotated * pathMesh;
			NonManifoldMesh_Annotated * singlePathMesh;
			map<unsigned int, vector<unsigned int> > helixStartPoints;
			map<unsigned int, vector<unsigned int> > helixEndPoints;
			int pathCount;

		};
		

		SSECorrespondenceEngine::SSECorrespondenceEngine() {
			correspondence.clear();
			correspondenceIndex = -1;
			pathCount = 0;
		}

		SSECorrespondenceEngine::~SSECorrespondenceEngine() {
			correspondence.clear();			
		}

		int SSECorrespondenceEngine::ExecuteQuery() {
			if(skeleton != NULL && sequence != NULL) {
				int resultCount = queryEngine->DoGraphMatching(sequence, skeleton);
				correspondence.clear();
				for(int i = 1; i <= resultCount; i++) {
					correspondence.push_back(queryEngine->GetSolution(i));
				}
				return resultCount;
			} else {
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
			int n1, n2, sse1, sse2;
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
				sse1 = -1;
				sse2 = -1;

				int seqNumber = 0;
				int seqIndex = 0;
				int strandsPassed = 0;

				// the following code iterates over the correspondence, finding a valid edge at each iteration.
				// start at node 0 of this result, continue until i is at last node
				int numNodes = result.GetNodeCount();
				for(int i = 0; i < result.GetNodeCount()-1; ) {

					// set n1 to be the ith result. if the result is -1, increment i and repeat.
					for(n1 = -1; n1 < 0; ) {
						n1 = result.GetSkeletonNode(i);
						sse1 = result.NodeToHelix(n1);
						i++;
						// update the seqIndex 
						if (sequence->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
							strandsPassed ++;
						}
					}
					// set n2 to be the ith result. if the result is -1, increment i and repeat
					//for(n2 = -1; n2 < 0l; ) {
					for(n2 = -1; n2 < 0; ) {
						n2 = result.GetSkeletonNode(i);
						sse2 = result.NodeToHelix(n2);
						i++;
						// update the seqIndex 
						if (sequence->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
							strandsPassed ++;
						}
						if (i >= result.GetNodeCount()) {
							//cout << "found skip edge at end of correspondence. breaking out of loop." << endl;
							break;
						}
					}
					if (sequence->adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
						strandsPassed --;
					}
					i--;
					seqIndex = (i + strandsPassed + 1)/2 + 1;

					path = skeleton->paths[n1][n2];
					//cout << "path sizes. fwd:" << skeleton->paths[n1][n2].size() << ", rev:" << skeleton->paths[n2][n1].size() << endl;
					if(path.size() == 0) {
						path = skeleton->paths[n2][n1];
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

					if(startSSENumber == endSSENumber && startSSENumber < numHelices){
						seqNumber += 0; // internal helix loop
					} else {
						seqNumber += 1;
					}


					if (path.size() != 0) {
						glEnd(); // test!!! 
						// draw labeled sphere at beginning of path
						//GLfloat col = 1.0;
						//glColor3f(col, col, col);
						double sphereRadius = 0.5;
						Renderer::DrawSphere(Vector3DFloat(path[0].X(), path[0].Y(), path[0].Z()), sphereRadius);
						/*
						// Label the points with their graph node numbers
						glColor3f(1.0, 1.0, 1.0);

						glRasterPos3d(path[0].X(), path[0].Y(), path[0].Z());
						//int labelInt = seqNumber;
						int labelInt = seqIndex;
						//int labelInt = sse1;
						std::ostringstream tmpStream;
						tmpStream << labelInt;
						string labelStr = tmpStream.str();
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
						for (int i = 0; i < labelStr.length(); i++) {
							//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
							glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
						}
						*/
						// labeled sphere code
						glBegin(GL_LINE_STRIP);
					}

					int pathSize = path.size(); // for color
					float stepColorR = (endColorR - startColorR) / (pathSize-1);
					float stepColorG = (endColorG - startColorG) / (pathSize-1);
					float stepColorB = (endColorB - startColorB) / (pathSize-1);
					for(int j = 0; j < pathSize; j++) {
						//cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
						glColor3f(startColorR + stepColorR * j, startColorG + stepColorG * j, startColorB + stepColorB * j);
						double offset = 0.8*(-0.5 + (double)i / (double)numNodes );
						glVertex3d(path[j].X()+offset, path[j].Y()+offset, path[j].Z()+offset);
					}

					if (path.size() != 0) {
						glEnd(); 
						// draw labeled sphere at end of path
						//GLfloat col = 1.0;
						//glColor3f(col, col, col);
						double sphereRadius = 0.5;
						Renderer::DrawSphere(Vector3DFloat(path[pathSize-1].X(), path[pathSize-1].Y(), path[pathSize-1].Z()), sphereRadius);

						// Label the points with their graph node numbers
						glColor3f(1.0, 1.0, 1.0);

						glRasterPos3d(path[pathSize-1].X(), path[pathSize-1].Y(), path[pathSize-1].Z());
						//int labelInt = seqNumber + 1;
						int labelInt = seqIndex;
						//int labelInt = sse2;
						std::ostringstream tmpStream;
						tmpStream << labelInt;
						string labelStr = tmpStream.str();
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
						for (unsigned int i = 0; i < labelStr.length(); i++) {
							//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
							glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
						}
						// labeled sphere code
						glBegin(GL_LINE_STRIP);
					}





				}
				glEnd();
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
							glEnd();
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
							double sphereRadius = 0.25;
							Renderer::DrawSphere(Vector3DFloat(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z), sphereRadius);


							// Label the points with their graph node numbers
							glColor3f(1.0, 1.0, 1.0);

							glRasterPos3d(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z);
							int cornerNum = skeleton->skeletonHelixes[i]->cornerCells[j].node; // 0 or 1
							int labelInt = 2 * i + cornerNum;
							std::ostringstream tmpStream;
							tmpStream << labelInt;
							string labelStr = tmpStream.str();
							glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
							glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
							for (unsigned int i = 0; i < labelStr.length(); i++) {
								//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
								glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
							}
						}
					}
				}
				glPopAttrib();
			}			

			// draw corner nodes (sheets)
			if (showSheetCorners) {
				glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
				int lastHelix = 0;
				for(int i = 0; i < (int)skeleton->skeletonHelixes.size(); i++) {
					if (skeleton->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_HELIX) {
						lastHelix=i;
					}
					if (skeleton->skeletonHelixes[i]->geometricShapeType == GRAPHEDGE_SHEET) {
						for(int j = 0; j < (int)skeleton->skeletonHelixes[i]->cornerCells.size(); j++) {
							glColor3f(1.0, 1.0, 1.0);
							Renderer::DrawSphere(Vector3DFloat(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z), 0.25);

							// Label the points with their graph node numbers
							glRasterPos3d(skeleton->skeletonHelixes[i]->cornerCells[j].x, skeleton->skeletonHelixes[i]->cornerCells[j].y, skeleton->skeletonHelixes[i]->cornerCells[j].z);
							int labelInt = i + lastHelix + 2;
							std::ostringstream tmpStream;
							tmpStream << labelInt;
							string labelStr = tmpStream.str();
							glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
							glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
							for (unsigned int i = 0; i < labelStr.length(); i++) {
								glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
								//glutBitmapCharacter(GLUT_BITMAP_9_BY_15, labelStr[i]);
								//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
							}

						}
					}
				}
				glPopAttrib();
			}			
		}

		void SSECorrespondenceEngine::InitializePathFinder(NonManifoldMesh_Annotated * mesh) {
			pathMesh = new NonManifoldMesh_Annotated(mesh);
			for(unsigned int i = 0; i < pathMesh->vertices.size(); i++) {
				pathMesh->vertices[i].tag = true;
			}
			helixEndPoints.clear();
			pathCount++;
		}

		void SSECorrespondenceEngine::InitializePathHelix(int helixIndex, Vector3DFloat p1, Vector3DFloat p2, float radius) {
			GeometricShape * helix = GeometricShape::CreateHelix(p1, p2, radius);
			set<unsigned int> internalVertices;
			internalVertices.clear();
			vector<unsigned int> startPoints;
			vector<unsigned int> endPoints;
			helixStartPoints[helixIndex] = startPoints;
			helixEndPoints[helixIndex] = endPoints;

			for(unsigned int i = 0; i < pathMesh->vertices.size(); i++) {
				if(helix->IsInsideShape(pathMesh->vertices[i].position)) {
					internalVertices.insert(i);
					pathMesh->vertices[i].tag = false;					
				}
			}
			vector<unsigned int> neighbors;
			bool isEnd = false, isStart;
			float dist1, dist2;
			for(set<unsigned int>::iterator i = internalVertices.begin(); i != internalVertices.end(); i++) {				
				neighbors = pathMesh->GetNeighboringVertexIndices(*i);
				for(unsigned int j = 0; j < neighbors.size(); j++) {
					isEnd = isEnd || (internalVertices.find(neighbors[j]) == internalVertices.end());
				}
				if(isEnd) {
					dist1 = (p1 - pathMesh->vertices[*i].position).Length();
					dist2 = (p2 - pathMesh->vertices[*i].position).Length();
					isStart = (dist1 <= dist2);
					if(isStart && (dist1 <= radius)) {
						helixStartPoints[helixIndex].push_back(*i);
						pathMesh->vertices[*i].tag = true;
					} else if (!isStart && (dist2 <= radius)) {
						helixEndPoints[helixIndex].push_back(*i);
						pathMesh->vertices[*i].tag = true;
					}
				}
			}
			if(helixStartPoints[helixIndex].size() == 0) {
				printf("Error <SSECorrespondenceEngine, InitializePathHelix>: No helix start points found for helix %d\n", helixIndex);
			}
			if(helixEndPoints[helixIndex].size() == 0) {
				printf("Error <SSECorrespondenceEngine, InitializePathHelix>: No helix end points found for helix %d\n", helixIndex);
			}

			delete helix;

		}

		void SSECorrespondenceEngine::PrunePathMesh(NonManifoldMesh_Annotated * mesh, vector<unsigned int> pathVertices, set<unsigned int> preserve) {
			for(unsigned int i = 0; i < mesh->vertices.size(); i++) {
				mesh->vertices[i].tag = true;
			}
			for(unsigned int i = 0; i < pathVertices.size(); i++) {
				mesh->vertices[pathVertices[i]].tag = false;
			}

			//vector<unsigned int> endPoints;

			//vector<unsigned int> neighbors, n2;
			//for(unsigned int i=0; i < pathVertices.size(); i++) {
			//	neighbors = mesh->GetNeighboringVertexIndices(pathVertices[i]);
			//	int neighborCount = 0;
			//	for(unsigned int j = 0; j < neighbors.size(); j++) {
			//		if(!mesh->vertices[neighbors[j]].tag) {
			//			neighborCount++;
			//		}
			//	}
			//	if(neighborCount < 2) {
			//		endPoints.push_back(pathVertices[i]);
			//	}
			//}

			//int currIx;
			//while(endPoints.size() > 0) {
			//	currIx = endPoints[0];
			//	endPoints.erase(endPoints.begin());
			//	printf("Can I prune: %d", currIx);
			//	if(!mesh->vertices[currIx].tag && (preserve.find(currIx) == preserve.end())) {
			//		printf(" Yes!");
			//		mesh->vertices[currIx].tag = true;
			//		neighbors = mesh->GetNeighboringVertexIndices(currIx);

			//		for(unsigned int j = 0; j < neighbors.size(); j++) {
			//			if((!mesh->vertices[neighbors[j]].tag) && (preserve.find(neighbors[j]) == preserve.end())) {
			//				int neighborCount = 0;
			//				n2 = mesh->GetNeighboringVertexIndices(neighbors[j]);
			//				for(unsigned int k = 0; k < n2.size(); k++) {
			//					if(!mesh->vertices[n2[k]].tag) {
			//						neighborCount++;
			//					}
			//				}

			//				if(neighborCount < 2) {
			//					endPoints.push_back(neighbors[j]);
			//				}
			//			}
			//		}
			//	}
			//	printf("\n");
			//}	
		}

		void SSECorrespondenceEngine::GetPathSpace(int helix1Ix, bool helix1Start, int helix2Ix, bool helix2Start) {			
			NonManifoldMesh_Annotated * mesh = new NonManifoldMesh_Annotated(pathMesh);

			vector<unsigned int> queue;
			for(unsigned int i = 0; i < helixStartPoints[helix1Ix].size(); i++) {
				queue.push_back(helixStartPoints[helix1Ix][i]);
			}

			unsigned int currIx;
			vector<unsigned int> neighbors;
			vector<unsigned int> pathVertices;
			pathVertices.clear();
			while(queue.size() > 0){ 
				currIx = queue[0];
				queue.erase(queue.begin());
				if(mesh->vertices[currIx].tag) {
					mesh->vertices[currIx].tag = false;
					pathVertices.push_back(currIx);
					neighbors = mesh->GetNeighboringVertexIndices(currIx);
					for(unsigned int i = 0; i < neighbors.size(); i++) {
						if(mesh->vertices[neighbors[i]].tag) {
							queue.push_back(neighbors[i]);
						}
					}
				}
			}

			// Preserving start and end terminus, while pruning away the single directional branches.
			set<unsigned int> preserve;
			if(helix1Start) {
				for(unsigned int i = 0; i < helixStartPoints[helix1Ix].size(); i++) {
					preserve.insert(helixStartPoints[helix1Ix][i]);
				}
			} else {
				for(unsigned int i = 0; i < helixEndPoints[helix1Ix].size(); i++) {
					preserve.insert(helixEndPoints[helix1Ix][i]);
				}
			}

			if(helix2Start) {
				for(unsigned int i = 0; i < helixStartPoints[helix2Ix].size(); i++) {
					preserve.insert(helixStartPoints[helix2Ix][i]);
				}
			} else {
				for(unsigned int i = 0; i < helixEndPoints[helix2Ix].size(); i++) {
					preserve.insert(helixEndPoints[helix2Ix][i]);
				}
			}

			//printf("Preserving:");
			//for(set<unsigned int>::iterator i = preserve.begin(); i != preserve.end(); i++) {
			//	printf("%d ", *i);
			//}
			//printf("\n");

			PrunePathMesh(mesh, pathVertices, preserve);		

			singlePathMesh = new NonManifoldMesh_Annotated();
			map<unsigned int, unsigned int> vertexMap;
			for(unsigned int i=0; i < mesh->vertices.size(); i++) {
				if(!mesh->vertices[i].tag) {
					vertexMap[i] = singlePathMesh->AddVertex(mesh->vertices[i]);
				}
			}

			for(unsigned int i=0; i < mesh->edges.size(); i++) {
				if(!mesh->vertices[mesh->edges[i].vertexIds[0]].tag && !mesh->vertices[mesh->edges[i].vertexIds[1]].tag) {
					singlePathMesh->AddEdge(vertexMap[mesh->edges[i].vertexIds[0]], vertexMap[mesh->edges[i].vertexIds[1]], mesh->edges[i].tag);
				}
			}


			//char filename[100];
			//sprintf(filename, "C:\\path_%d_%d.off", pathCount, helix1Ix);
			//printf("vertex count: %d, edgeCount: %d, faceCount: %d\n", singlePathMesh->vertices.size(), singlePathMesh->edges.size(), singlePathMesh->faces.size()); flushall();
			//singlePathMesh->ToOffCells(filename);

			vertexMap.clear();
			pathVertices.clear();
			delete mesh;
		}

		void SSECorrespondenceEngine::ClearPathSpace() {
			delete singlePathMesh;
			singlePathMesh = NULL;
		}

		void SSECorrespondenceEngine::ClearPathFinder() {
			helixStartPoints.clear();
			helixEndPoints.clear();
			delete pathMesh;
		}

		int SSECorrespondenceEngine::GetPathVertexCount() {
			return singlePathMesh->vertices.size();
		}

		Vector3DFloat SSECorrespondenceEngine::GetPathVertex(int index) {
			return singlePathMesh->vertices[index].position;
		}

		int SSECorrespondenceEngine::GetPathEdgeCount() {
			return singlePathMesh->edges.size();
		}

		int SSECorrespondenceEngine::GetEdgeVertexIndex(int index, int side) {
			return singlePathMesh->edges[index].vertexIds[side];
		}
	}
}

#endif