// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for a non-manifold mesh.



#ifndef PROTEINMORPH_NON_MANIFOLD_MESH2_H
#define PROTEINMORPH_NON_MANIFOLD_MESH2_H

#include <map>

using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		template <class TEdge> class NonManifoldMeshEdge2 {
		public:
			unsigned long long vertexHashes[2];
			vector<unsigned long long> faceHashes;
			TEdge tag;
			unsigned long long GetHash() {
				return GetHash(vertexHashes[0], vertexHashes[1]);
			}

			static unsigned long long GetHash(unsigned long long vertexHash0, unsigned long long vertexHash1) {
				if(vertexHash0 > vertexHash1) {
					return vertexHash0 * 4294967296 + vertexHash1;
				} else {
					return vertexHash1 * 4294967296 + vertexHash0;
				} 

			}
		};


		template <class TFace> class NonManifoldMeshFace2 {
		public:
			unsigned long long vertexHashes[3];
			TFace tag;

			unsigned long long GetHash() {
				return GetHash(vertexHashes[0], vertexHashes[1], vertexHashes[2]);
			}

			static unsigned long long GetHash(unsigned long long vertexHash0, unsigned long long vertexHash1, unsigned long long vertexHash2) {
				unsigned long long hashes[3] = {vertexHash0, vertexHash1, vertexHash2};
				unsigned long long temp;
				for(int i = 0; i < 2; i++) {
					for(int j = i+1; j < 3; j++) {
						if(hashes[i] < hashes[j]) {
							temp = hashes[i];
							hashes[i] = hashes[j];
							hashes[j] = temp;
						}
					}
				}
				return hashes[0] * 4398046511104 + hashes[1] * 2097152 + hashes[2];
			}
		};

		template <class TVertex> class NonManifoldMeshVertex2 {
		public:
			Vector3DFloat position;
			vector<unsigned long long> edgeHashes;
			vector<unsigned long long> faceHashes;
			TVertex tag;

			NonManifoldMeshVertex2() {
				position = Vector3DFloat(0, 0, 0);
			}

			NonManifoldMeshVertex2(Vector3DFloat position) {
				this->position = position;
			}
		};		

		template <class TVertex, class TEdge, class TFace> class NonManifoldMesh2{
		public:
			NonManifoldMesh2();
			~NonManifoldMesh2();

			void Clear();
			unsigned long long AddVertex(NonManifoldMeshVertex2<TVertex> vertex);
			unsigned long long AddVertex(NonManifoldMeshVertex2<TVertex> vertex, unsigned long long hashKey);
			unsigned long long AddEdge(NonManifoldMeshEdge2<TEdge> edge);
			unsigned long long AddEdge(unsigned long long vertexHash0, unsigned long long vertexHash1);
			unsigned long long AddFace(NonManifoldMeshFace2<TFace> face);
			unsigned long long AddFace(unsigned long long vertexHash0, unsigned long long vertexHash1, unsigned long long vertexHash2);

			Vector3DFloat GetVertexNormal(unsigned long long vertexHash);
			Vector3DFloat GetFaceNormal(unsigned long long faceHash);
			void Draw(bool drawSurfaces, bool drawLines, bool drawPoints, bool annotateSurfaces, bool annotateLines, bool annotatePoints, bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting, int lineThickness);


		private:
			map<unsigned long long, NonManifoldMeshVertex2<TVertex> > vertices;
			map<unsigned long long, NonManifoldMeshEdge2<TEdge> > edges;
			map<unsigned long long, NonManifoldMeshFace2<TFace> > faces;
		};

		template <class TVertex, class TEdge, class TFace> NonManifoldMesh2<TVertex, TEdge, TFace>::NonManifoldMesh2() {
			vertices.clear();
			edges.clear();
			faces.clear();
		}

		template <class TVertex, class TEdge, class TFace> NonManifoldMesh2<TVertex, TEdge, TFace>::~NonManifoldMesh2() {
			Clear();
		}

		template <class TVertex, class TEdge, class TFace> void  NonManifoldMesh2<TVertex, TEdge, TFace>::Clear() {
			vertices.clear();
			edges.clear();
			faces.clear();
		}

		template <class TVertex, class TEdge, class TFace> unsigned long long  NonManifoldMesh2<TVertex, TEdge, TFace>::AddVertex(NonManifoldMeshVertex2<TVertex> vertex) {
			unsigned long long key = vertices.count();
			vertices[key] = vertex;
			return key;
		}

		template <class TVertex, class TEdge, class TFace> unsigned long long  NonManifoldMesh2<TVertex, TEdge, TFace>::AddVertex(NonManifoldMeshVertex2<TVertex> vertex, unsigned long long hashKey) {
			vertices[hashKey] = vertex;
			return hashKey;
		}

		template <class TVertex, class TEdge, class TFace> unsigned long long NonManifoldMesh2<TVertex, TEdge, TFace>::AddEdge(NonManifoldMeshEdge2<TEdge> edge) {
			unsigned long long edgeHash = edge.GetHash();
			edges[edgeHash] = edge;
			vertices[edge.vertexHashes[0]].edgeHashes.push_back(edgeHash);
			vertices[edge.vertexHashes[1]].edgeHashes.push_back(edgeHash);
			return edgeHash;
		}

		template <class TVertex, class TEdge, class TFace> unsigned long long NonManifoldMesh2<TVertex, TEdge, TFace>::AddEdge(unsigned long long vertexHash0, unsigned long long vertexHash1) {
			NonManifoldMeshEdge2<TEdge> edge;
			edge.vertexHashes[0] = vertexHash0;
			edge.vertexHashes[1] = vertexHash1;
			return AddEdge(edge);
		}

		template <class TVertex, class TEdge, class TFace> unsigned long long NonManifoldMesh2<TVertex, TEdge, TFace>::AddFace(NonManifoldMeshFace2<TFace> face) {
			unsigned long long faceHash = face.GetHash();
			faces[faceHash] = face;
			vertices[face.vertexHashes[0]].faceHashes.push_back(faceHash);
			vertices[face.vertexHashes[1]].faceHashes.push_back(faceHash);
			vertices[face.vertexHashes[2]].faceHashes.push_back(faceHash);

			for(int i = 0; i < 3; i++) {	
				unsigned long long edgeHash = NonManifoldMeshEdge2<TEdge>::GetHash(face.vertexHashes[i], face.vertexHashes[(i+1) % 3]);
				if(edges.find(edgeHash) == edges.end()) {
					edgeHash = AddEdge(face.vertexHashes[i], face.vertexHashes[(i+1) % 3]);					
				}
				edges[edgeHash].faceHashes.push_back(faceHash);				
			}			

			return faceHash;
		}
		

		template <class TVertex, class TEdge, class TFace> unsigned long long NonManifoldMesh2<TVertex, TEdge, TFace>::AddFace(unsigned long long vertexHash0, unsigned long long vertexHash1, unsigned long long vertexHash2) {
			NonManifoldMeshFace2<TFace> face;
			face.vertexHashes[0] = vertexHash0;
			face.vertexHashes[1] = vertexHash1;
			face.vertexHashes[2] = vertexHash2;
			return AddFace(face);
		}
		
		template <class TVertex, class TEdge, class TFace> Vector3DFloat NonManifoldMesh2<TVertex, TEdge, TFace>::GetVertexNormal(unsigned long long vertexHash) {
			unsigned long long edgeHash;
			Vector3DFloat normal = Vector3DFloat(0,0,0);
			for(unsigned int i = 0; i < vertices[vertexHash].faceHashes.size(); i++) {
				normal += GetFaceNormal(vertices[vertexHash].faceHashes[i]);
			}
			normal.Normalize();
			return normal;
		}

		template <class TVertex, class TEdge, class TFace> Vector3DFloat NonManifoldMesh2<TVertex, TEdge, TFace>::GetFaceNormal(unsigned long long faceHash) {
			Vector3DFloat normal = Vector3DFloat(1,0,0);
			NonManifoldMeshFace2<TFace> face = faces[faceHash];
			normal = (vertices[face.vertexHashes[1]].position - vertices[face.vertexHashes[0]].position) ^
							(vertices[face.vertexHashes[2]].position - vertices[face.vertexHashes[0]].position);

			normal.Normalize();			
			return normal;
		}


		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh2<TVertex, TEdge, TFace>::Draw(bool drawSurfaces, bool drawLines, bool drawPoints, bool annotateSurfaces, bool annotateLines, bool annotatePoints, bool disableSurfaceLighting, bool disableCurveLighting, bool disablePointLighting, int lineThickness) {
			int k;
			//printf("%d vertices, %d edges, %d faces\n", vertices.size(), edges.size(), faces.size()); flushall();
			glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT);			

			if(drawSurfaces) {
				if(disableSurfaceLighting) {
					glDisable(GL_LIGHTING);
				}
				if(annotateSurfaces) {
					glPushName(0);
					glPushName(0);
				}
				map<unsigned long long, NonManifoldMeshFace2<TFace>>::iterator i;
				for(i = faces.begin(); i != faces.end(); i++) {					
					if(annotateSurfaces) {
						glLoadName(i->first);
					}
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(unsigned int j = 0; j < 3; j++) {
						k = i->second.vertexHashes[j];
						normal = GetVertexNormal(k);						
						glNormal3f(normal.X(), normal.Y(), normal.Z());
						glVertex3fv(vertices[k].position.values);
					}
					glEnd();
				}
				if(annotateSurfaces) {
					glPopName();
					glPopName();
				}
			}			
			
			/*if(drawLines) {		
				if(disableCurveLighting) {
					glDisable(GL_LIGHTING);
				}
				if(annotateLines) {
					glPushName(1);
					glPushName(0);
				}
				glLineWidth(lineThickness);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				
				for(unsigned int i = 0; i < edges.size(); i++) {					
					if(edges[i].faceIds.size() == 0) {
						if(annotateLines) {
							glLoadName(i);
						}
						glBegin(GL_LINES);
						k = GetVertexIndex(edges[i].vertexIds[0]);
						glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());
						k = GetVertexIndex(edges[i].vertexIds[1]);
						glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());			
						glEnd();
					}
				}	
				if(annotateLines) {
					glPopName();
					glPopName();
				}
			}			
			
			if(drawPoints) {
				if(disablePointLighting) {
					glDisable(GL_LIGHTING);
				}


				if(annotatePoints) {
					glPushName(2);
					glPushName(0);
				}
				glPointSize(2);
				glEnable(GL_POINT_SMOOTH);
				glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);								
				for(unsigned int i = 0; i < vertices.size(); i++) {										
					if(vertices[i].edgeIds.size() == 0) {
						if(annotatePoints) {
							glLoadName(i);
						}
						glBegin(GL_POINTS);
						glVertex3f(vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
						glEnd();
					}
				}		
				if(annotatePoints) {
					glPopName();
					glPopName();
				}
			}	*/		
			glPopAttrib();

			glFlush();
		}





	}
}

#endif
