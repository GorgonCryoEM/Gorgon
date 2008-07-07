#ifndef GORGON_INTERACTIVE_SKELETON_ENGINE_H
#define GORGON_INTERACTIVE_SKELETON_ENGINE_H

#include "MeshRenderer.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraySkeletonCPP/InteractiveSkeletonizer.h>
#include <MathTools/Vector3D.h>
#include <Foundation/Octree.h>
#include <vector>
#ifdef _WIN32
	#include <hash_map>
	using namespace stdext;
#else
	#include <map>
#endif

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace std;


namespace wustl_mm {
	namespace Visualization {	
		class InteractiveSkeletonEngine {
		public:
			InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float skeletonRatio, float stRatio, float minGray, int stepCount, int curveRadius, int minCurveSize);
			~InteractiveSkeletonEngine();			
			void AnalyzePathSingle(int hit0, int hit1);
			void AnalyzePathMultiple(int hit0, int hit1, bool reset, bool final);
			void AnalyzePathRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void Draw(int subscene);
			void FinalizeSkeleton();
			void SelectEndSeed(int hit0, int hit1);
			void SelectStartSeedSingle(int hit0, int hit1);
			void SelectStartSeedMultiple(int hit0, int hit1, bool reset, bool final);
			void SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth);
			void SetIsoValue(float isoValue);
		private:
			Volume * volume;
			NonManifoldMesh_Annotated * skeleton;
			InteractiveSkeletonizer * skeletonizer;
			#ifdef _WIN32
				hash_map<int, int> locationToVertexMap;
				hash_map<int, Vector3DInt> vertexToLocationMap;
			#else
				map<int, int> locationToVertexMap;
				map<int, Vector3DInt> vertexToLocationMap;
			#endif 
			bool started;
			bool analyzed;
			Vector3DInt startPos;
			vector<Vector3DInt> startPositions;
			Vector3DInt currentPos;
			vector<Vector3DInt> currentPositions;
			GLUquadric * quadricSphere;			
			float isoValue;
		};

		InteractiveSkeletonEngine::InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float skeletonRatio, float stRatio, float minGray, int stepCount, int curveRadius, int minCurveSize) {
			this->volume = volume;
			this->skeleton = skeleton;
			float minVal = minGray;
			float maxVal = volume->getMax();
			float stepSize = (maxVal - minVal)/(float)stepCount;
			this->isoValue = 0.0f;
			locationToVertexMap.clear();
			vertexToLocationMap.clear();


			skeletonizer = new InteractiveSkeletonizer(volume, minVal, maxVal, stepSize, curveRadius, minCurveSize, false);
			wustl_mm::GraySkeletonCPP::GraphType * graph = skeletonizer->GetGraph();
			OctreeNode<unsigned int> * node;
			unsigned int ix;
			for(unsigned int i = 0; i < graph->vertices.size(); i++) {
				ix = skeleton->AddVertex(graph->vertices[i].position, false);
				node = graph->vertices[i].tag.octreeNode;
				locationToVertexMap[volume->getIndex(node->pos[0], node->pos[1], node->pos[2])] = ix;
				vertexToLocationMap[ix] = Vector3DInt(node->pos[0], node->pos[1], node->pos[2]);
			}

			started = false;
			analyzed = false;
			skeletonizer->SetGraphWeights(skeletonRatio, stRatio);
			quadricSphere = gluNewQuadric();			
		}

		InteractiveSkeletonEngine::~InteractiveSkeletonEngine() {
			delete skeletonizer;
			gluDeleteQuadric(quadricSphere);			
		}

		void InteractiveSkeletonEngine::SelectEndSeed(int hit0, int hit1) {
			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				skeleton->edges[i].tag = true;
			}
		}

		void InteractiveSkeletonEngine::SelectStartSeedSingle(int hit0, int hit1) {
			if((hit0 == 1) && (hit1 >= 0)) {
				hit0 = 2;
				if(skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[hit1].vertexIds[0])].edgeIds.size() == 1) {
					hit1 = skeleton->edges[hit1].vertexIds[0];
				} else {
					hit1 = skeleton->edges[hit1].vertexIds[1];
				}
			}
			if((hit0 == 2) && (hit1 >= 0)) { // Clicked on a vertex point
				started = true;
				analyzed = false;
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				startPos = vertexToLocationMap[hit1];
				skeletonizer->CalculateMinimalSpanningTree(startPos);
			} else if((hit0 == -1) && (hit1 == -1)) { // Clicked on the current Point
				started = true;
				analyzed = false;
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				startPos = currentPos;
				skeletonizer->CalculateMinimalSpanningTree(startPos);
			}
		}

		void InteractiveSkeletonEngine::SelectStartSeedMultiple(int hit0, int hit1, bool reset, bool final) {
			if(reset) {
				started = false;
				analyzed = false;
				startPositions.clear();				
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
			}

			if((hit0 == 1) && (hit1 >= 0)) { // Clicked on an edge
				hit0 = 2;
				if(skeleton->vertices[skeleton->GetVertexIndex(skeleton->edges[hit1].vertexIds[0])].edgeIds.size() == 1) {
					hit1 = skeleton->edges[hit1].vertexIds[0];
				} else {
					hit1 = skeleton->edges[hit1].vertexIds[1];
				}
			}
			if((hit0 == 2) && (hit1 >= 0)) { // Clicked on a vertex point
				startPositions.push_back(vertexToLocationMap[hit1]);
			} else if((hit0 == -1) && (hit1 == -1)) { // Clicked on the current Point
				startPositions.push_back(currentPos);
			}

			if(final) {
				started = true;
				skeletonizer->CalculateMinimalSpanningTree(startPositions);
			}
		}

		void InteractiveSkeletonEngine::SelectStartSeedRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			analyzed = false;
			started = false;
			startPositions.clear();				
			for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
				if(!skeleton->edges[i].tag) {
					skeleton->RemoveEdge(i);
				}
			}
			skeleton->RemoveNullEntries();

			vector<OctreeNode<unsigned int> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);

			for(unsigned int i = 0; i < intersectingCells.size(); i++) {
				if(intersectingCells[i]->cellSize == 1) {
					startPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
				}
			}			
			if(startPositions.size() > 0) {
				skeletonizer->CalculateMinimalSpanningTree(startPositions);
				startPos = startPositions[0];
				started = true;
			}					
		}

		void InteractiveSkeletonEngine::SetIsoValue(float isoValue) {
			this->isoValue = isoValue;
		}
		void InteractiveSkeletonEngine::AnalyzePathSingle(int hit0, int hit1) {
			analyzed = true;
			if(started && (hit0 == 2) && (hit1 >= 0)) {
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				int v1, v2;
				currentPos = vertexToLocationMap[hit1];
				//printf("Getting Path {%ld %ld %ld}\n", currentPos.X(), currentPos.Y(), currentPos.Z()); flushall();
				vector<Vector3DInt> path = skeletonizer->GetPath(currentPos);
				//printf("Done\n"); flushall();
				//for(unsigned int i = 0; i < path.size(); i++) {
					//printf("{%ld %ld %ld} ", path[i].X(), path[i].Y(), path[i].Z()); flushall();
				//}
				//printf("\nGot Path\n"); flushall();
				for(unsigned int i = 1; i < path.size(); i++) {
					v1 = locationToVertexMap[volume->getIndex(path[i-1].X(),path[i-1].Y(), path[i-1].Z())];
					v2 = locationToVertexMap[volume->getIndex(path[i].X(),path[i].Y(), path[i].Z())];
					if(!skeleton->IsEdgePresent(v1, v2)) {
						skeleton->AddEdge(v1, v2, false);
					}

				}
			}
		}

		void InteractiveSkeletonEngine::AnalyzePathMultiple(int hit0, int hit1, bool reset, bool final) {
			if(reset) {		
				appTimeManager.PushCurrentTime();
				analyzed = false;	
				currentPositions.clear();
				if(started) {
					for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
						if(!skeleton->edges[i].tag) {
							skeleton->RemoveEdge(i);
						}
					}
					skeleton->RemoveNullEntries();
				}
			}
			if(started && (hit0 == 2) && (hit1 >= 0)) {
				currentPos = vertexToLocationMap[hit1];
				currentPositions.push_back(currentPos);
			}

			if(final) {
				int v1, v2;
				analyzed = true;
				vector<Vector3DInt> path = skeletonizer->GetPath(currentPositions);
				currentPos = path[0];
				startPos = path[path.size()-1];
				for(unsigned int i = 1; i < path.size(); i++) {
					v1 = locationToVertexMap[volume->getIndex(path[i-1].X(),path[i-1].Y(), path[i-1].Z())];
					v2 = locationToVertexMap[volume->getIndex(path[i].X(),path[i].Y(), path[i].Z())];
					if(!skeleton->IsEdgePresent(v1, v2)) {
						skeleton->AddEdge(v1, v2, false);
					}
				}
				appTimeManager.PopAndDisplayTime("Analyzed path: %f seconds! \n");
			}
		}

		void InteractiveSkeletonEngine::AnalyzePathRay(float rayX, float rayY, float rayZ, float eyeX, float eyeY, float eyeZ, float rayWidth) {
			if(started) {
				currentPositions.clear();
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();

				vector<OctreeNode<unsigned int> *> intersectingCells = skeletonizer->GetOctree()->IntersectRay(Vector3DFloat(rayX, rayY, rayZ), Vector3DFloat(eyeX, eyeY, eyeZ), rayWidth);

				for(unsigned int i = 0; i < intersectingCells.size(); i++) {
					//if(intersectingCells[i]->cellSize == 1) {
						currentPositions.push_back(Vector3DInt(intersectingCells[i]->pos[0], intersectingCells[i]->pos[1], intersectingCells[i]->pos[2]));
					//}
				}

				if(currentPositions.size() > 0) {

					int v1, v2;
					analyzed = true;
					vector<Vector3DInt> path = skeletonizer->GetPath(currentPositions);
					currentPos = path[0];
					startPos = path[path.size()-1];
					for(unsigned int i = 1; i < path.size(); i++) {
						v1 = locationToVertexMap[volume->getIndex(path[i-1].X(),path[i-1].Y(), path[i-1].Z())];
						v2 = locationToVertexMap[volume->getIndex(path[i].X(),path[i].Y(), path[i].Z())];
						if(!skeleton->IsEdgePresent(v1, v2)) {
							skeleton->AddEdge(v1, v2, false);
						}
					}
				}
			}
		}

		void InteractiveSkeletonEngine::FinalizeSkeleton() {
			for(unsigned int i = 0; i < skeleton->vertices.size(); i++) {
				if(skeleton->vertices[i].edgeIds.size() == 0) {
					skeleton->RemoveVertex(i);
				}
			}
			skeleton->RemoveNullEntries();
		}
		void InteractiveSkeletonEngine::Draw(int subscene) {
			switch (subscene) {
				case(0) :
					if(started) {
						glPushMatrix();
						glTranslatef(startPos.X(), startPos.Y(), startPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();

						/*for(unsigned int i = 0; i < startPositions.size(); i++) {
							glPushMatrix();
							glTranslatef(startPositions[i].X(), startPositions[i].Y(), startPositions[i].Z());
							gluSphere(quadricSphere, 1.0, 10, 10);  
							glPopMatrix();
						}*/
					}
					break;
				case(1) :
					if(analyzed && started) {
						glPushMatrix();
						glTranslatef(currentPos.X(), currentPos.Y(), currentPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();

						/*for(unsigned int i = 0; i < currentPositions.size(); i++) {
							glPushMatrix();
							glTranslatef(currentPositions[i].X(), currentPositions[i].Y(), currentPositions[i].Z());
							gluSphere(quadricSphere, 1.0, 10, 10);  
							glPopMatrix();
						}*/

					}
					break;
				case(2):
					break;
				case(3):
					/*glPushAttrib(GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT);
										
					glPushName(2);
					glPushName(0);

					glPointSize(2);
					glEnable(GL_POINT_SMOOTH);
					glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);								
					for(unsigned int i = 0; i < skeleton->vertices.size(); i++) {										
						if((skeleton->vertices[i].edgeIds.size() == 0) && 
								(volume->getDataAt(skeleton->vertices[i].position.XInt(), skeleton->vertices[i].position.YInt(), skeleton->vertices[i].position.ZInt()) > isoValue)) {
						//if((skeleton->vertices[i].edgeIds.size() == 0)) {
							glLoadName(i);
							glBegin(GL_POINTS);
							glVertex3f(skeleton->vertices[i].position.X(), skeleton->vertices[i].position.Y(), skeleton->vertices[i].position.Z());
							glEnd();
						}
					}		
					glPopName();
					glPopName();
					glPopAttrib();

					glFlush();*/
					break;
			}
		}
	}
}


#endif