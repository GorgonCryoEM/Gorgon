#ifndef PROTEINMORPH_NON_MANIFOLD_MESH_H
#define PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/Vector3D.h>
#include <SkeletonMaker/volume.h>
#include <string>
#include <hash_map>
#include <GL/glut.h>

using namespace std;
using namespace stdext;

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Protein_Morph {
		template <class TEdge> struct NonManifoldMeshEdge {
			unsigned int id;
			unsigned int vertexIds[2];
			vector<unsigned int> faceIds;
			TEdge tag;
			bool valid;
		};

		template <class TFace> struct NonManifoldMeshFace {
			unsigned int id;
			vector<unsigned int> edgeIds;
			vector<unsigned int> vertexIds;
			TFace tag;
			bool valid;
		};

		template <class TVertex> struct NonManifoldMeshVertex {
			unsigned int id;
			Vector3DFloat position;
			vector<unsigned int> edgeIds;
			bool valid;
			TVertex tag;
		};


		typedef hash_map<int, int> HashMapType;
		template <class TVertex, class TEdge, class TFace> class NonManifoldMesh{
		public:
			NonManifoldMesh();
			NonManifoldMesh(NonManifoldMesh<TVertex, TEdge, TFace> * srcMesh);
			NonManifoldMesh(Volume * sourceVol);
			bool IsEdgePresent(int vertexId1, int vertexId2);
			int AddVertex(NonManifoldMeshVertex<TVertex> vertex);
			int AddVertex(Vector3DFloat location, TVertex tag = NULL);
			int AddHashedVertex(Vector3DFloat location, int hashKey, TVertex tag = NULL);
			int AddEdge(NonManifoldMeshEdge<TEdge> edge);
			int AddFace(NonManifoldMeshFace<TFace> face);
			int GetVertexIndex(int vertexId);
			int GetFaceIndex(int faceId);
			int GetEdgeIndex(int edgeId);
			void AddEdge(int vertexId1, int vertexId2, TEdge tag = NULL);
			void AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, TEdge newEdgeTag = NULL, TFace faceTag = NULL);
			void AddTriangle(int vertexId1, int vertexId2, int vertexId3, TEdge newEdgeTag = NULL, TFace faceTag = NULL);
			void Clear();
			void Draw(bool drawSurfaces, bool drawLines, bool drawPoints);
			void MarkFixedVertices();
			void RemoveFace(int faceId);
			void RemoveEdge(int edgeId);
			void RemoveVertex(int vertexId);
			void RemoveNullEntries();
			void ToOffCells(string fileName);
			void ToMathematicaFile(string fileName);
			Vector3DFloat GetVertexNormal(int vertexId);
			Vector3DFloat GetFaceNormal(int faceId);
			NonManifoldMesh * SmoothLaplacian(double converganceRate);	
			NonManifoldMesh * SmoothLaplacian(double converganceRate, int iterations);	
			static NonManifoldMesh * LoadOffFile(string fileName);

		public:
			vector<NonManifoldMeshVertex<TVertex>> vertices;
			vector<NonManifoldMeshEdge<TEdge>> edges;
			vector<NonManifoldMeshFace<TFace>> faces;
			int edgeCount;
			int vertexCount;
			int faceCount;
			HashMapType vertexHashMap;
		};


		typedef NonManifoldMesh<bool, bool, bool> NonManifoldMesh_NoTags;

		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace>::NonManifoldMesh() {
			Clear();			
		}

		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace>::NonManifoldMesh(NonManifoldMesh<TVertex, TEdge, TFace> * srcMesh) {
			Clear();
			for(unsigned int i = 0; i < srcMesh->vertices.size(); i++) {
				vertices.push_back(srcMesh->vertices[i]);
			}
			for(unsigned int i = 0; i < srcMesh->edges.size(); i++) {
				edges.push_back(srcMesh->edges[i]);
			}
			for(unsigned int i = 0; i < srcMesh->faces.size(); i++) {
				faces.push_back(srcMesh->faces[i]);
			}
		}

		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace>::NonManifoldMesh(Volume * sourceVol) {
			Clear();

			int x, y, z, i, j, index, index2;
			int * vertexLocations = new int[sourceVol->getSizeX() * sourceVol->getSizeY() * sourceVol->getSizeZ()];
			int value;

			// Adding vertices
			NonManifoldMeshVertex<TVertex> tempVertex;
			tempVertex.edgeIds.clear();
			for(x = 0; x < sourceVol->getSizeX(); x++) {
				for(y = 0; y < sourceVol->getSizeY(); y++) {
					for(z = 0; z < sourceVol->getSizeZ(); z++) {
						index = sourceVol->getIndex(x, y, z);
						vertexLocations[index] = -1;
						value = (int)round(sourceVol->getDataAt(index));
						if(value > 0) {							
							tempVertex.position = Vector3DFloat(x,y,z);								
							vertexLocations[index] = AddVertex(tempVertex);
						}
					}
				}
			}

			//Adding edges
			int edgeNeighbors[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
			for(x = 0; x < sourceVol->getSizeX()-1; x++) {
				for(y = 0; y < sourceVol->getSizeY()-1; y++) {
					for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
						index = sourceVol->getIndex(x, y, z);
						for(i = 0; i < 3; i++) {
							index2 = sourceVol->getIndex(x+edgeNeighbors[i][0], y+edgeNeighbors[i][1], z+edgeNeighbors[i][2]);		
							if((vertexLocations[index] >= 0) && (vertexLocations[index2] >= 0)) {								
								AddEdge(vertexLocations[index], vertexLocations[index2]);								
							}
						}
					}
				}
			}

			//Adding Faces
			int faceNeighbors[3][3][3] = {  {{1,0,0}, {1,0,1}, {0,0,1}}, 
											{{1,0,0}, {1,1,0}, {0,1,0}}, 
											{{0,1,0}, {0,1,1}, {0,0,1}}};
			int indices[4];
			bool faceFound;
			for(x = 0; x < sourceVol->getSizeX()-1; x++) {
				for(y = 0; y < sourceVol->getSizeY()-1; y++) {
					for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
						index = sourceVol->getIndex(x, y, z);
						if(vertexLocations[index] >= 0) {
							for(i = 0; i < 3; i++) {
								faceFound = true;
								indices[0] = vertexLocations[index];
								for(j = 0; j < 3; j++) {
									index2 = sourceVol->getIndex(x+faceNeighbors[i][j][0], y+faceNeighbors[i][j][1], z+faceNeighbors[i][j][2]);
									indices[j+1] = vertexLocations[index2];
									faceFound = faceFound && vertexLocations[index2] >= 0;
								}
								if(faceFound) {
									AddQuad(indices[0], indices[1], indices[2], indices[3]);
								}
							}
						}
					}
				}
			}
			delete [] vertexLocations;
			MarkFixedVertices();
		}

		template <class TVertex, class TEdge, class TFace> bool NonManifoldMesh<TVertex, TEdge, TFace>::IsEdgePresent(int vertexId1, int vertexId2) {
			bool isPresent = false;
			int v1Index = GetVertexIndex(vertexId1);
			int v2Index = GetVertexIndex(vertexId2);
			for(unsigned int i = 0; (i < vertices[v1Index].edgeIds.size()) && !isPresent; i++) {
				isPresent = (edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[0] == v2Index) || (edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[1] == v2Index);
			}
			return isPresent;

		}
		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::AddVertex(NonManifoldMeshVertex<TVertex> vertex) {			
			vertex.id = vertices.size();
			vertex.valid = true;
			vertices.push_back(vertex);
			vertexCount++;
			return vertex.id;
		}	

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::AddVertex(Vector3DFloat location, TVertex tag) {
			NonManifoldMeshVertex<TVertex> v;
			v.position = location;
			v.tag = tag;
			return AddVertex(v);
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::AddHashedVertex(Vector3DFloat location, int hashKey, TVertex tag) {
			HashMapType::const_iterator pos = vertexHashMap.find(hashKey);
			int vertexId;
			if(pos == vertexHashMap.end()) {
				vertexId = AddVertex(location, tag);
				vertexHashMap[hashKey] = vertexId;
			} else {
				vertexId = pos->second;
			}
			return vertexId;
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::AddEdge(NonManifoldMeshEdge<TEdge> edge) {
			edge.id = edges.size();
			edge.valid = true;
			edges.push_back(edge);
			edgeCount++;
			return edge.id;
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::AddFace(NonManifoldMeshFace<TFace> face) {
			face.id = faces.size();
			face.valid = true;
			faces.push_back(face);
			faceCount++;
			return face.id;
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::GetVertexIndex(int vertexId) {
			return vertexId;
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::GetFaceIndex(int faceId) {
			return faceId;
		}

		template <class TVertex, class TEdge, class TFace> int NonManifoldMesh<TVertex, TEdge, TFace>::GetEdgeIndex(int edgeId) {
			return edgeId;
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::AddEdge(int vertexId1, int vertexId2, TEdge tag){
			NonManifoldMeshEdge<TEdge> edge;
			edge.tag = tag;
			edge.faceIds.clear();
			edge.vertexIds[0] = vertexId1;
			edge.vertexIds[1] = vertexId2;
			int edgeId = AddEdge(edge);			
			vertices[GetVertexIndex(vertexId1)].edgeIds.push_back(edgeId);
			vertices[GetVertexIndex(vertexId2)].edgeIds.push_back(edgeId);
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, TEdge newEdgeTag, TFace faceTag) {
			AddTriangle(vertexId1, vertexId2, vertexId3, newEdgeTag, faceTag);
			AddTriangle(vertexId1, vertexId3, vertexId4, newEdgeTag, faceTag);
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::AddTriangle(int vertexId1, int vertexId2, int vertexId3, TEdge newEdgeTag, TFace faceTag) {
			if(!IsEdgePresent(vertexId1, vertexId2)) {
					AddEdge(vertexId1, vertexId2, newEdgeTag);
			}
			if(!IsEdgePresent(vertexId2, vertexId3)) {
					AddEdge(vertexId2, vertexId3, newEdgeTag);
			}
			if(!IsEdgePresent(vertexId3, vertexId1)) {
					AddEdge(vertexId3, vertexId1, newEdgeTag);
			}

			NonManifoldMeshFace<TFace> face;
			face.tag = faceTag;
			face.vertexIds.clear();
			face.vertexIds.push_back(vertexId1);
			face.vertexIds.push_back(vertexId2);
			face.vertexIds.push_back(vertexId3);
			face.edgeIds.clear();

			int vertexIds[4] = {vertexId1, vertexId2, vertexId3, vertexId1};
			int i,j, edgeIndex, vertexIndex;

			for(i = 0; i < 3; i++) {
				vertexIndex = GetVertexIndex(vertexIds[i]);
				for (j = 0; j < (int)vertices[vertexIndex].edgeIds.size(); j++) {
					edgeIndex = GetEdgeIndex(vertices[vertexIndex].edgeIds[j]);

					if( ((edges[edgeIndex].vertexIds[0] == vertexIds[i])   && (edges[edgeIndex].vertexIds[1] == vertexIds[i+1])) ||
						((edges[edgeIndex].vertexIds[0] == vertexIds[i+1]) && (edges[edgeIndex].vertexIds[1] == vertexIds[i]))) {
						face.edgeIds.push_back(vertices[vertexIndex].edgeIds[j]);					 
					}
				}				
			}
			int faceId = AddFace(face);
			for(i = 0; i < (int)face.edgeIds.size(); i++) {		
				edges[GetEdgeIndex(face.edgeIds[i])].faceIds.push_back(faceId);
			}
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::Clear() {
			vertices.clear();
			edges.clear();
			faces.clear();
			vertexCount = 0;
			edgeCount = 0;
			faceCount = 0;
			vertexHashMap.clear();
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::Draw(bool drawSurfaces, bool drawLines, bool drawPoints) {
			int k;
			glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);

			if(drawSurfaces) {
				for(unsigned int i = 0; i < faces.size(); i++) {
					glBegin(GL_POLYGON);
					Vector3DFloat normal;
					for(unsigned int j = 0; j < faces[i].vertexIds.size(); j++) {
						normal = GetVertexNormal(faces[i].vertexIds[j]);
						k = GetVertexIndex(faces[i].vertexIds[j]);
						glNormal3f(normal.X(), normal.Y(), normal.Z());
						glVertex3fv(vertices[k].position.values);
					}
					glEnd();
				}
			}

			if(drawLines) {
				glLineWidth(1.5);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				glBegin(GL_LINES);
				for(unsigned int i = 0; i < edges.size(); i++) {
					if(edges[i].faceIds.size() == 0) {
						k = GetVertexIndex(edges[i].vertexIds[0]);
						glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());
						k = GetVertexIndex(edges[i].vertexIds[1]);
						glVertex3f(vertices[k].position.X(), vertices[k].position.Y(), vertices[k].position.Z());			
					}
				}
				glEnd();
			}

			if(drawPoints) {
				glEnable(GL_POINT_SMOOTH);
				glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
				glBegin(GL_POINTS);
				for(unsigned int i = 0; i < vertices.size(); i++) {
					if(vertices[i].edgeIds.size() == 0) {
						glVertex3f(vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
					}
				}
				glEnd();
			}
			glPopAttrib();

			glFlush();
		}
		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::MarkFixedVertices() {
			bool sheetFound;
			bool edgeFound;

			for(unsigned int i = 0; i < vertices.size(); i++) {
				if(vertices[i].valid) {
					sheetFound = false;
					edgeFound = false;
					for(unsigned int j = 0; j < vertices[i].edgeIds.size(); j++) {
						sheetFound = sheetFound || (edges[GetEdgeIndex(vertices[i].edgeIds[j])].faceIds.size() > 0);
						edgeFound = edgeFound || (edges[GetEdgeIndex(vertices[i].edgeIds[j])].faceIds.size() == 0);
					}
				}
			}
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::RemoveFace(int faceId) {
			int faceIndex = GetFaceIndex(faceId); 
			int edgeIndex;
			faces[faceIndex].valid = false;
			faceCount--;
			for(unsigned int i = 0; i < faces[faceIndex].edgeIds.size(); i++) {
				edgeIndex = GetEdgeIndex(faces[faceIndex].edgeIds[i]);								
				for(int j = (int)edges[edgeIndex].faceIds.size()-1; j >= 0; j--) {
					if(edges[edgeIndex].faceIds[j] == faceId) {
						edges[edgeIndex].faceIds.erase(edges[edgeIndex].faceIds.begin() + j);
					}
				}
			}
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::RemoveEdge(int edgeId) {
			int edgeIndex = GetEdgeIndex(edgeId);
			int vertexIndex;
			if(edges[edgeIndex].faceIds.size() > 0) {
				printf("Cannot remove edgeId %i as it has face associations\n", edgeId);
				exit(0);
			}
			edges[edgeIndex].valid = false;			
			edgeCount--;

			for(int i = 0; i < 2; i++) {				
				vertexIndex = GetVertexIndex(edges[edgeIndex].vertexIds[i]);								
				for(int j = (int)vertices[vertexIndex].edgeIds.size()-1; j >= 0; j--) {
					if(vertices[vertexIndex].edgeIds[j] == edgeId) {
						vertices[vertexIndex].edgeIds.erase(vertices[vertexIndex].edgeIds.begin() + j);
					}
				}
			}
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::RemoveVertex(int vertexId) {
			int vertexIndex = GetVertexIndex(vertexId);
			vertices[vertexIndex].valid = false;
			vertexCount--;

			if(vertices[vertexIndex].edgeIds.size() > 0) {
				printf("Cannot remove vertexId %i as it has edge associations\n", vertexId);
				exit(0);
			}
		}

		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::RemoveNullEntries() {
			for(int i = (int)vertices.size()-1; i >= 0; i--) {
				if(!vertices[i].valid) {
					vertices.erase(vertices.begin() + i);
				}				
			}

			int oldId, newId;
			for(unsigned int i = 0; i < vertices.size(); i++) {								
				if(vertices[i].id != i) {
					oldId = vertices[i].id;
					newId = i;
					vertices[i].id = newId;
					for(unsigned int j = 0; j < vertices[i].edgeIds.size(); j++) {
						for(int k = 0; k < 2; k++) {
							if(edges[GetEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] == oldId) {
								edges[GetEdgeIndex(vertices[i].edgeIds[j])].vertexIds[k] = newId;
							}
						}
					}
				}
			}

			for(int i = (int)edges.size()-1; i >= 0; i--) {
				if(!edges[i].valid) {
					edges.erase(edges.begin() + i);
				}				
			}

			for(unsigned int i = 0; i < edges.size(); i++) {			
				if(edges[i].id != i) {
					oldId = edges[i].id;
					newId = i;
					edges[i].id = newId;

					for(int j = 0; j < 2; j++)  {
						for(unsigned int k = 0; k < vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds.size(); k++) {
							if(vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds[k] == oldId) {
								vertices[GetVertexIndex(edges[i].vertexIds[j])].edgeIds[k] = newId;
							}
						}
					}

					for(unsigned int j = 0; j < edges[i].faceIds.size(); j++)  {
						for(unsigned int k = 0; k < faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds.size(); k++) {
							if(faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds[k] == oldId) {
								faces[GetFaceIndex(edges[i].faceIds[j])].edgeIds[k] = newId;
							}
						}
					}
				}
			}

			for(int i = (int)faces.size()-1; i >= 0; i--) {
				if(!faces[i].valid) {
					faces.erase(faces.begin() + i);
				}				
			}

			for(unsigned int i = 0; i < faces.size(); i++) {			
				if(faces[i].id != i) {
					oldId = faces[i].id;
					newId = i;
					faces[i].id = newId;
					for(unsigned int j = 0; j < faces[i].edgeIds.size(); j++) {
						for(unsigned int k = 0; k < edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds.size(); k++) {
							if(edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds[k] == oldId) {
								edges[GetEdgeIndex(faces[i].edgeIds[j])].faceIds[k] = newId;
							}
						}
					}
				}
			}
		}	


		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::ToOffCells(string fileName) {
			RemoveNullEntries();
			FILE * outFile = fopen(fileName.c_str(), "wt");
			fprintf(outFile, "OFF\n");
			fprintf(outFile, "%li %li %li\n", vertices.size(), faces.size() + edges.size(), 0);
			int i,j;
			for(i = 0; i < (int)vertices.size(); i++) {
				fprintf(outFile, "%lf %lf %lf\n", vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
			}
			int lastVertex;
			for(i = 0; i < (int)faces.size(); i++) {
				fprintf(outFile, "%li ", faces[i].edgeIds.size());
				lastVertex = -1;

				for(j =0; j < (int)faces[i].vertexIds.size(); j++) {
					fprintf(outFile, "%li ", GetVertexIndex(faces[i].vertexIds[j]));
				}

				fprintf(outFile, "\n");
			}

			for(i = 0; i < (int)edges.size(); i++) {		
				fprintf(outFile, "4 %li %li %li %li \n", edges[i].vertexIds[0], edges[i].vertexIds[0], edges[i].vertexIds[1], edges[i].vertexIds[1]);
			}
			fclose(outFile);
		}


		template <class TVertex, class TEdge, class TFace> void NonManifoldMesh<TVertex, TEdge, TFace>::ToMathematicaFile(string fileName) {
			RemoveNullEntries();
			FILE * outF = fopen(fileName.c_str(), "wt");
			// Vertices
			fprintf(outF, "{\n");
			fprintf(outF, "{");
			for(unsigned int i = 0; i < vertices.size(); i++) {
				fprintf(outF, "{%lf, %lf, %lf}", vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
				if(i != (int)vertices.size()-1) {
					fprintf(outF, ", ");
				}
			}
			fprintf(outF, "},\n");

			// Edges
			fprintf(outF, "{");
			for(unsigned int i = 0; i < edges.size(); i++) {
				fprintf(outF, "{%li, %li}", edges[i].vertexIds[0]+1, edges[i].vertexIds[1]+1);
				if(i != (int)edges.size()-1) {
					fprintf(outF, ", ");
				}
			}
			fprintf(outF, "},\n");

			// Faces
			fprintf(outF, "{");
			int lastVertex;
			for(unsigned int i = 0; i < faces.size(); i++) {
				lastVertex = -1;
				fprintf(outF, "{");
				for(int j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
					if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) || 
						(edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
						lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];						
					} else {
						lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
					}
					fprintf(outF, "%li", lastVertex+1);
					if(j != 0) {
						fprintf(outF, ", ");
					}
				}
				fprintf(outF, "}");
				
				if(i != (int)faces.size()-1) {
					fprintf(outF, ", ");
				}
			}
			fprintf(outF, "}\n");

			fprintf(outF, "}");

			fclose(outF);
		}
		template <class TVertex, class TEdge, class TFace> Vector3DFloat NonManifoldMesh<TVertex, TEdge, TFace>::GetVertexNormal(int vertexId) {
			int index = GetVertexIndex(vertexId);
			int edgeIndex;
			Vector3DFloat normal = Vector3DFloat(0,0,0);
			for(unsigned int i = 0; i < vertices[index].edgeIds.size(); i++) {
				edgeIndex = GetEdgeIndex(vertices[index].edgeIds[i]);
				for(unsigned int j = 0; j < edges[edgeIndex].faceIds.size(); j++) {
					normal += GetFaceNormal(edges[edgeIndex].faceIds[j]);
				}
			}
			normal.Normalize();
			return normal;
		}

		template <class TVertex, class TEdge, class TFace> Vector3DFloat NonManifoldMesh<TVertex, TEdge, TFace>::GetFaceNormal(int faceId) {

			Vector3DFloat normal = Vector3DFloat(1,0,0);

			NonManifoldMeshFace<TFace> face = faces[GetFaceIndex(faceId)];

			if(face.vertexIds.size() >= 3) {
				normal = (vertices[GetVertexIndex(face.vertexIds[1])].position - vertices[GetVertexIndex(face.vertexIds[0])].position) ^
								(vertices[GetVertexIndex(face.vertexIds[2])].position - vertices[GetVertexIndex(face.vertexIds[0])].position);
				normal.Normalize();
			} 
			return normal;
		}
		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace> * NonManifoldMesh<TVertex, TEdge, TFace>::SmoothLaplacian(double converganceRate) {
			NonManifoldMesh * smoothedMesh = new NonManifoldMesh(this);
			int i, j, vertexIndex;
			Vector3DFloat newPosition;
			NonManifoldMeshVertex<TVertex> vertex;
			for(i = 0; i < (int)vertices.size(); i++) {
				vertex = vertices[i];
				if(vertex.valid) {
					if(vertex.edgeIds.size() > 0) {
						newPosition = Vector3DFloat(0,0,0);
						for(j = 0; j < (int)vertex.edgeIds.size(); j++) {
							if(edges[GetEdgeIndex(vertex.edgeIds[j])].vertexIds[0] == i) {
								vertexIndex = 1;
							} else {
								vertexIndex = 0;
							}

							newPosition = newPosition + vertices[GetVertexIndex(edges[GetEdgeIndex(vertex.edgeIds[j])].vertexIds[vertexIndex])].position;
						}
						newPosition = newPosition * (1.0/vertex.edgeIds.size());
					} else {
						newPosition = vertex.position;
					}
					smoothedMesh->vertices[i].position = smoothedMesh->vertices[i].position * (1.0 - converganceRate)+ newPosition * converganceRate;
				}
			}
			return smoothedMesh;
		}


		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace> * NonManifoldMesh<TVertex, TEdge, TFace>::SmoothLaplacian(double converganceRate, int iterations) {
			NonManifoldMesh * newMesh;
			NonManifoldMesh * oldMesh = new NonManifoldMesh(this);

			for(int i = 0; i < iterations; i++) {
				newMesh = oldMesh->SmoothLaplacian(converganceRate);
				delete oldMesh;
				oldMesh = newMesh;
			}

			return oldMesh;
		}
		template <class TVertex, class TEdge, class TFace> NonManifoldMesh<TVertex, TEdge, TFace> * NonManifoldMesh<TVertex, TEdge, TFace>::LoadOffFile(string fileName) {
			NonManifoldMesh * mesh = new NonManifoldMesh();
			FILE * inFile = fopen(fileName.c_str(), "rt");
			char strTemp[255];
			int nVertices, nEdges, nFaces;
			int lVertices, lFaces;
			lVertices = 0;
			lFaces = 0;

			fscanf(inFile, "%s\n", strTemp);
			//printf("[%s]\n", strTemp);
			fscanf(inFile, "%d %d %d\n", &nVertices, &nFaces, &nEdges);
			//printf("[%d] [%d] [%d]\n", nVertices, nFaces, nEdges);

			float xPos, yPos, zPos;
			lVertices = 0;
			for(int i=0; i < nVertices; i++) {
				lVertices++;
				fscanf(inFile, "%f %f %f", &xPos, &yPos, &zPos);
				//printf("[%f] [%f] [%f]\n", xPos, yPos, zPos);
				mesh->AddVertex(Vector3DFloat(xPos, yPos, zPos));
				fgets(strTemp, 255, inFile);
			}


			int faceNodes[100], nFaceNodes;
			lFaces = 0;
			for(int i=0; i < nFaces; i++) {
				fscanf(inFile, "%d", &nFaceNodes);
				//printf("[%d]\n", nFaceNodes);
				switch(nFaceNodes) {
					case 1:
					case 2:
						printf("Cannot load polygon... unsupported polygon size: %d\n", nFaceNodes);
						break;
					case 4:
						lFaces++;
						for(int i = 0; i < nFaceNodes; i++) {
							fscanf(inFile, "");
							fscanf(inFile, "%d", &faceNodes[i]);
						}

						if((faceNodes[0] != faceNodes[1]) && (faceNodes[0] != faceNodes[2]) && (faceNodes[0] != faceNodes[3])
							&& (faceNodes[1] != faceNodes[2]) && (faceNodes[1] != faceNodes[3]) && (faceNodes[2] != faceNodes[3])) {
							mesh->AddQuad(faceNodes[0], faceNodes[1], faceNodes[2], faceNodes[3]);
						} else {
							mesh->AddEdge(faceNodes[0], faceNodes[2]);
						}
						break;
					default :
						lFaces++;
						for(int i = 0; i < nFaceNodes; i++) {
							fscanf(inFile, "");
							fscanf(inFile, "%d", &faceNodes[i]);
						}
						for(int i = 2; i < nFaceNodes; i++) {
							mesh->AddTriangle(faceNodes[0], faceNodes[i-1], faceNodes[i]);
						}
						break;

				}
				fgets(strTemp, 255, inFile);
			}

			//printf(" Vertices %d of %d loaded.  Faces %d of %d loaded", lVertices, nVertices, lFaces, nFaces);

			//int v1, v2;
			//for(int i=0; i < nEdges; i++) {
			//	fscanf(inFile, "%d %d\n", &v1, &v2);
			//	if(!mesh->IsEdgePresent(v1, v2)) {
			//		mesh->AddEdge(v1, v2, false);
			//	}
			//}

			fclose(inFile);
			return mesh;
		}
	}
}

#endif