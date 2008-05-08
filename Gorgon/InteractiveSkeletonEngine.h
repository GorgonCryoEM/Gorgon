#ifndef GORGON_INTERACTIVE_SKELETON_ENGINE_H
#define GORGON_INTERACTIVE_SKELETON_ENGINE_H

#include "MeshRenderer.h"
#include <ProteinMorph/NonManifoldMesh.h>
#include <GraySkeletonCPP/InteractiveSkeletonizer.h>
#include <hash_map>
#include <MathTools/Vector3D.h>

using namespace wustl_mm::Protein_Morph;
using namespace wustl_mm::GraySkeletonCPP;
using namespace stdext;

namespace wustl_mm {
	namespace Visualization {	
		class InteractiveSkeletonEngine {
		public:
			InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float skeletonRatio, float stRatio, float minGray, int stepCount, int curveRadius, int minCurveSize);
			~InteractiveSkeletonEngine();
			void SelectEndSeed(int hit0, int hit1);
			void SelectStartSeed(int hit0, int hit1);
			void AnalyzePath(int hit0, int hit1);
			void FinalizeSkeleton();
			void Draw(int subscene);
		private:
			Volume * volume;
			NonManifoldMesh_Annotated * skeleton;
			InteractiveSkeletonizer * skeletonizer;
			hash_map<int, int> locationToVertexMap;
			hash_map<int, Vector3DInt> vertexToLocationMap;
			bool started;
			Vector3DInt startPos;
			Vector3DInt currentPos;
			GLUquadric * quadricSphere;			
		};

		InteractiveSkeletonEngine::InteractiveSkeletonEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float skeletonRatio, float stRatio, float minGray, int stepCount, int curveRadius, int minCurveSize) {
			this->volume = volume;
			this->skeleton = skeleton;
			float minVal = minGray;
			float maxVal = volume->getMax();
			float stepSize = (maxVal - minVal)/(float)stepCount;
			locationToVertexMap.clear();
			vertexToLocationMap.clear();


			skeletonizer = new InteractiveSkeletonizer(volume, minVal, maxVal, stepSize, curveRadius, minCurveSize, false);
			Volume * skelVol = skeletonizer->GetSkeletonCopy();
			int ix;
			for(int x = 0; x < skelVol->getSizeX(); x++) {
				for(int y = 0; y < skelVol->getSizeY(); y++) {
					for(int z = 0; z < skelVol->getSizeZ(); z++) {
						if(skelVol->getDataAt(x, y, z) > 0.0) {
							ix = skeleton->AddVertex(Vector3DFloat(x, y, z), false);
							locationToVertexMap[skelVol->getIndex(x, y, z)] = ix;
							vertexToLocationMap[ix] = Vector3DInt(x, y, z);
						}
					}
				}
			}

			delete skelVol;
			started = false;
			skeletonizer->SetGraphWeights(skeletonRatio, stRatio);
			quadricSphere = gluNewQuadric();			
		}

		InteractiveSkeletonEngine::~InteractiveSkeletonEngine() {
			delete skeletonizer;
			gluDeleteQuadric(quadricSphere);
		}

		void InteractiveSkeletonEngine::SelectEndSeed(int hit0, int hit1) {
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
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					skeleton->edges[i].tag = true;
				}
			}
		}

		void InteractiveSkeletonEngine::SelectStartSeed(int hit0, int hit1) {
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
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				startPos = vertexToLocationMap[hit1];
				skeletonizer->CalculateMinimalSpanningTree(startPos);
			}
		}

		void InteractiveSkeletonEngine::AnalyzePath(int hit0, int hit1) {
			if(started && (hit0 == 2) && (hit1 >= 0)) {
				for(unsigned int i = 0; i < skeleton->edges.size(); i++) {
					if(!skeleton->edges[i].tag) {
						skeleton->RemoveEdge(i);
					}
				}
				skeleton->RemoveNullEntries();
				int v1, v2;
				currentPos = vertexToLocationMap[hit1];
				vector<Vector3DInt> path = skeletonizer->GetPath(currentPos);
				for(unsigned int i = 1; i < path.size(); i++) {
					v1 = locationToVertexMap[volume->getIndex(path[i-1].X(),path[i-1].Y(), path[i-1].Z())];
					v2 = locationToVertexMap[volume->getIndex(path[i].X(),path[i].Y(), path[i].Z())];
					if(!skeleton->IsEdgePresent(v1, v2)) {
						skeleton->AddEdge(v1, v2, false);
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
			printf("aaa : %d\n" ,subscene);
			flushall();
			if(started) {
				switch (subscene) {
					case(0) :
						glPushMatrix();
						glTranslatef(currentPos.X(), currentPos.Y(), currentPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
						break;
					case(1) :
						glPushMatrix();
						glTranslatef(startPos.X(), startPos.Y(), startPos.Z());
						gluSphere(quadricSphere, 1.0, 10, 10);  
						glPopMatrix();
						break;
					case(2):
						break;
					case(3):
						//skeleton->Draw(false, false, true, false, false, true);
						break;
				}
			}
		}
	}
}


#endif