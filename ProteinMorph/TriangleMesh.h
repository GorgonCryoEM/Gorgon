#ifndef PROTEINMORPH_TRIANGLE_MESH_H
#define PROTEINMORPH_TRIANGLE_MESH_H

#include <map>
#include <vector>
#include "TriangleMeshFace.h"
#include "TriangleMeshVertex.h"

using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		template <class TVertex, class TFace> class TriangleMesh{
		public:
			TriangleMesh();
			~TriangleMesh();

			void Clear();
			unsigned long long AddVertex(TriangleMeshVertex<TVertex> vertex);
			unsigned long long AddVertex(TriangleMeshVertex<TVertex> vertex, unsigned long long hashKey);
			unsigned long long AddFace(TriangleMeshFace<TFace> face);
			unsigned long long AddFace(unsigned long long vertexHash0, unsigned long long vertexHash1, unsigned long long vertexHash2);

			Vector3DFloat GetVertexNormal(unsigned long long vertexHash);
			Vector3DFloat GetFaceNormal(unsigned long long faceHash);
			void Draw(bool drawSurfaces, bool annotateSurfaces);


		private:
			map<unsigned long long, TriangleMeshVertex<TVertex> > vertices;
			vector<TriangleMeshFace<TFace> > faces;
		};

		template <class TVertex, class TFace> TriangleMesh<TVertex, TFace>::TriangleMesh() {
			Clear();
		}

		template <class TVertex, class TFace> TriangleMesh<TVertex, TFace>::~TriangleMesh() {
			Clear();
		}

		template <class TVertex, class TFace> void  TriangleMesh<TVertex, TFace>::Clear() {
			vertices.clear();
			faces.clear();
		}

		template <class TVertex, class TFace> unsigned long long  TriangleMesh<TVertex, TFace>::AddVertex(TriangleMeshVertex<TVertex> vertex) {
			unsigned long long key = vertices.count();
			vertices[key] = vertex;
			return key;
		}

		template <class TVertex, class TFace> unsigned long long  TriangleMesh<TVertex, TFace>::AddVertex(TriangleMeshVertex<TVertex> vertex, unsigned long long hashKey) {
			vertices[hashKey] = vertex;
			return hashKey;
		}

		template <class TVertex, class TFace> unsigned long long TriangleMesh<TVertex, TFace>::AddFace(TriangleMeshFace<TFace> face) {
			unsigned long long faceHash = faces.size();
			faces.push_back(face);
			vertices[face.vertexHashes[0]].faceHashes.push_back(faceHash);
			vertices[face.vertexHashes[1]].faceHashes.push_back(faceHash);
			vertices[face.vertexHashes[2]].faceHashes.push_back(faceHash);
			return faceHash;
		}

		template <class TVertex, class TFace> unsigned long long TriangleMesh<TVertex, TFace>::AddFace(unsigned long long vertexHash0, unsigned long long vertexHash1, unsigned long long vertexHash2) {
			TriangleMeshFace<TFace> face;
			face.vertexHashes[0] = vertexHash0;
			face.vertexHashes[1] = vertexHash1;
			face.vertexHashes[2] = vertexHash2;
			return AddFace(face);
		}
		template <class TVertex, class TFace> Vector3DFloat TriangleMesh<TVertex, TFace>::GetVertexNormal(unsigned long long vertexHash) {
			Vector3DFloat normal = Vector3DFloat(0,0,0);
			for(unsigned int i = 0; i < vertices[vertexHash].faceHashes.size(); i++) {
				normal += GetFaceNormal(vertices[vertexHash].faceHashes[i]);
			}
			normal.Normalize();
			return normal;
		}

		template <class TVertex, class TFace> Vector3DFloat TriangleMesh<TVertex, TFace>::GetFaceNormal(unsigned long long faceHash) {
			TriangleMeshFace<TFace> face = faces[faceHash];
			Vector3DFloat normal = (vertices[face.vertexHashes[1]].position - vertices[face.vertexHashes[0]].position) ^
								   (vertices[face.vertexHashes[2]].position - vertices[face.vertexHashes[0]].position);

			normal.Normalize();			
			return normal;
		}


		template <class TVertex, class TFace> void TriangleMesh<TVertex, TFace>::Draw(bool drawSurfaces, bool annotateSurfaces) {
			int k;
			if(drawSurfaces) {
				if(annotateSurfaces) {
					glPushName(0);
					glPushName(0);
				}
				for(int i = 0; i < faces.size(); i++) {					
					if(annotateSurfaces) {
						glLoadName(i);
					}
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(unsigned int j = 0; j < 3; j++) {
						k = faces[i].vertexHashes[j];
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
			glFlush();
		}
	}
}

#endif