// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for a triangle mesh



#ifndef PROTEINMORPH_TRIANGLE_MESH_H
#define PROTEINMORPH_TRIANGLE_MESH_H

#include <map>
#include <vector>
#include <string>
#include <Foundation/OpenGLUtils.h>
#include "TriangleMeshFace.h"
#include "TriangleMeshVertex.h"


using namespace std;
using namespace wustl_mm::Foundation;

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
			void Draw(bool drawSurfaces, bool annotateSurfaces, bool fadeExtreme, int radius, Vector3DFloat center);
			void SaveFile(string fileName);


		private:
			typedef map< unsigned long long, TriangleMeshVertex< TVertex > > TriangleMeshVertexType;
			
			TriangleMeshVertexType vertices;
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


		template <class TVertex, class TFace> void TriangleMesh<TVertex, TFace>::Draw(bool drawSurfaces, bool annotateSurfaces, bool fadeExtreme, int radius, Vector3DFloat center) {
			int k;
			if(drawSurfaces) {
				if(annotateSurfaces) {
					glPushName(0);
					glPushName(0);
				}
				float r,g,b,a;
				OpenGLUtils::GetColor(r,g,b,a);
				for(unsigned int i = 0; i < faces.size(); i++) {					
					if(annotateSurfaces) {
						glLoadName(i);
					}
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					bool drawTriangle = true;
					if(fadeExtreme) {
						for(unsigned int j = 0; j < 3; j++) {
							k = faces[i].vertexHashes[j];
							drawTriangle = drawTriangle && ((vertices[k].position - center).Length() <= radius);
						}
					}
					if(drawTriangle) {
						for(unsigned int j = 0; j < 3; j++) {
							k = faces[i].vertexHashes[j];
							normal = GetVertexNormal(k);			
							glNormal3f(normal.X(), normal.Y(), normal.Z());
							glVertex3fv(vertices[k].position.values);
						}
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

		template <class TVertex, class TFace> void TriangleMesh<TVertex, TFace>::SaveFile(string fileName) {
			FILE * outFile = fopen(fileName.c_str(), "wt");
			fprintf(outFile, "OFF\n");
			fprintf(outFile, "%d %d %d\n", (int)vertices.size(), (int)faces.size(), 0);

			map<unsigned long long, int> indexedVertices;
			vector<Vector3DFloat> vertexList;

			int index = 0;
			for(typename TriangleMeshVertexType::iterator i = vertices.begin(); i != vertices.end(); i++) {
				vertexList.push_back(i->second.position);
				indexedVertices[i->first] = index;
				index++;
			}

			for(int i = 0; i < index; i++) {
				fprintf(outFile, "%f %f %f \n", vertexList[i].X(), vertexList[i].Y(), vertexList[i].Z());
			}

			for(unsigned int i = 0; i < faces.size(); i++) {					
				fprintf(outFile, "3 %d %d %d\n", indexedVertices[faces[i].vertexHashes[2]], indexedVertices[faces[i].vertexHashes[1]], indexedVertices[faces[i].vertexHashes[0]]); 
			}
			fclose(outFile);
		}
	}
}

#endif
