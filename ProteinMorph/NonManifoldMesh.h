#ifndef PROTEINMORPH_NON_MANIFOLD_MESH_H
#define PROTEINMORPH_NON_MANIFOLD_MESH_H

#include <vector>
#include <MathTools/Vector3D.h>
#include <SkeletonMaker/volume.h>
#include <string>

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Protein_Morph {
		const int NOT_A_HELIX_VERTEX = -1;
		struct NonManifoldMeshEdge {
			unsigned int id;
			unsigned int vertexIds[2];
			vector<unsigned int> faceIds;
			bool isHelix;
			bool valid;
		};

		struct NonManifoldMeshFace {
			unsigned int id;
			vector<unsigned int> edgeIds;
			bool isSheet;
			bool valid;
		};

		struct NonManifoldMeshVertex {
			unsigned int id;
			Vector3DFloat position;
			vector<unsigned int> edgeIds;
			bool valid;
			bool stationary;
			bool articulationPoint;
			int helixId;
		};

		class NonManifoldMesh{
		public:
			NonManifoldMesh();
			NonManifoldMesh(NonManifoldMesh * srcMesh);
			NonManifoldMesh(Volume * sourceVol, Volume * helixVol = NULL, Volume * sheetVol = NULL);
			bool IsEdgePresent(int vertexId1, int vertexId2);
			int AddVertex(NonManifoldMeshVertex vertex);
			int AddVertex(Vector3DFloat location, bool stationary, bool articulationPoint, int helixId);
			int AddEdge(NonManifoldMeshEdge edge);
			int AddFace(NonManifoldMeshFace face);
			int GetVertexIndex(int vertexId);
			int GetFaceIndex(int faceId);
			int GetEdgeIndex(int edgeId);
			void AddEdge(int vertexId1, int vertexId2, bool isHelix);
			void AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, bool isSheet);
			void AddTriangle(int vertexId1, int vertexId2, int vertexId3, bool isSheet);
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
			static NonManifoldMesh * LoadOffFile(string fileName);

		public:
			vector<NonManifoldMeshVertex> vertices;
			vector<NonManifoldMeshEdge> edges;
			vector<NonManifoldMeshFace> faces;
			int edgeCount;
			int vertexCount;
			int faceCount;
		};

		NonManifoldMesh::NonManifoldMesh() {
			vertices.clear();
			edges.clear();
			faces.clear();
			vertexCount = 0;
			edgeCount = 0;
			faceCount = 0;
			
		}

		NonManifoldMesh::NonManifoldMesh(NonManifoldMesh * srcMesh) {
			vertices.clear();
			edges.clear();
			faces.clear();
			vertexCount = 0;
			edgeCount = 0;
			faceCount = 0;
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

		NonManifoldMesh::NonManifoldMesh(Volume * sourceVol, Volume * helixVol, Volume * sheetVol) {
			vertices.clear();
			edges.clear();
			faces.clear();
			vertexCount = 0;
			edgeCount = 0;
			faceCount = 0;

			int x, y, z, i, j, index, index2;
			int * vertexLocations = new int[sourceVol->getSizeX() * sourceVol->getSizeY() * sourceVol->getSizeZ()];
			int value;

			// Adding vertices
			NonManifoldMeshVertex tempVertex;
			tempVertex.edgeIds.clear();
			for(x = 0; x < sourceVol->getSizeX(); x++) {
				for(y = 0; y < sourceVol->getSizeY(); y++) {
					for(z = 0; z < sourceVol->getSizeZ(); z++) {
						index = sourceVol->getIndex(x, y, z);
						vertexLocations[index] = -1;
						value = (int)round(sourceVol->getDataAt(index));
						if(value > 0) {							
							tempVertex.position = Vector3DFloat(x,y,z);	
							tempVertex.articulationPoint = (value == 2);
							tempVertex.stationary = (value >= 2);
							if(value >= 3) {
								tempVertex.helixId = value-3;
							} else {
								tempVertex.helixId = -1;
							}
							vertexLocations[index] = AddVertex(tempVertex);
						}
					}
				}
			}

			//Adding edges
			int edgeNeighbors[3][3] = {{1,0,0}, {0,1,0}, {0,0,1}};
			bool isHelix;
			for(x = 0; x < sourceVol->getSizeX()-1; x++) {
				for(y = 0; y < sourceVol->getSizeY()-1; y++) {
					for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
						index = sourceVol->getIndex(x, y, z);
						for(i = 0; i < 3; i++) {
							index2 = sourceVol->getIndex(x+edgeNeighbors[i][0], y+edgeNeighbors[i][1], z+edgeNeighbors[i][2]);		
							if((vertexLocations[index] >= 0) && (vertexLocations[index2] >= 0)) {
								isHelix = (helixVol != NULL) && (helixVol->getDataAt(index) > 0) && (helixVol->getDataAt(index2) > 0);
								AddEdge(vertexLocations[index], vertexLocations[index2], isHelix);								
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
			bool faceFound, isSheet;
			for(x = 0; x < sourceVol->getSizeX()-1; x++) {
				for(y = 0; y < sourceVol->getSizeY()-1; y++) {
					for(z = 0; z < sourceVol->getSizeZ()-1; z++) {
						index = sourceVol->getIndex(x, y, z);
						if(vertexLocations[index] >= 0) {
							for(i = 0; i < 3; i++) {
								faceFound = true;
								isSheet = (sheetVol != NULL) && (sheetVol->getDataAt(index) > 0);
								indices[0] = vertexLocations[index];
								for(j = 0; j < 3; j++) {
									index2 = sourceVol->getIndex(x+faceNeighbors[i][j][0], y+faceNeighbors[i][j][1], z+faceNeighbors[i][j][2]);
									indices[j+1] = vertexLocations[index2];
									isSheet = isSheet && (sheetVol->getDataAt(index2) > 0);
									faceFound = faceFound && vertexLocations[index2] >= 0;
								}
								if(faceFound) {
									AddQuad(indices[0], indices[1], indices[2], indices[3], isSheet);
								}
							}
						}
					}
				}
			}
			delete [] vertexLocations;
			MarkFixedVertices();
		}

		bool NonManifoldMesh::IsEdgePresent(int vertexId1, int vertexId2) {
			bool isPresent = false;
			int v1Index = GetVertexIndex(vertexId1);
			int v2Index = GetVertexIndex(vertexId2);
			for(unsigned int i = 0; (i < vertices[v1Index].edgeIds.size()) && !isPresent; i++) {
				isPresent = (edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[0] == v2Index) || (edges[GetEdgeIndex(vertices[v1Index].edgeIds[i])].vertexIds[1] == v2Index);
			}
			return isPresent;

		}
		int NonManifoldMesh::AddVertex(NonManifoldMeshVertex vertex) {			
			vertex.id = vertices.size();
			vertex.valid = true;
			vertices.push_back(vertex);
			vertexCount++;
			return vertex.id;
			
		}
		
		int NonManifoldMesh::AddVertex(Vector3DFloat location, bool stationary, bool articulationPoint, int helixId) {
			NonManifoldMeshVertex v;
			v.position = location;
			v.stationary = stationary;
			v.articulationPoint = articulationPoint;
			v.helixId = helixId;
			return AddVertex(v);

		}

		int NonManifoldMesh::AddEdge(NonManifoldMeshEdge edge) {
			edge.id = edges.size();
			edge.valid = true;
			edges.push_back(edge);
			edgeCount++;
			return edge.id;
		}

		int NonManifoldMesh::AddFace(NonManifoldMeshFace face) {
			face.id = faces.size();
			face.valid = true;
			faces.push_back(face);
			faceCount++;
			return face.id;
		}

		int NonManifoldMesh::GetVertexIndex(int vertexId) {
			return vertexId;
		}

		int NonManifoldMesh::GetFaceIndex(int faceId) {
			return faceId;
		}

		int NonManifoldMesh::GetEdgeIndex(int edgeId) {
			return edgeId;
		}

		void NonManifoldMesh::AddEdge(int vertexId1, int vertexId2, bool isHelix){
			NonManifoldMeshEdge edge;
			edge.isHelix = isHelix;
			edge.faceIds.clear();
			edge.vertexIds[0] = vertexId1;
			edge.vertexIds[1] = vertexId2;
			int edgeId = AddEdge(edge);			
			vertices[GetVertexIndex(vertexId1)].edgeIds.push_back(edgeId);
			vertices[GetVertexIndex(vertexId2)].edgeIds.push_back(edgeId);
		}

		void NonManifoldMesh::AddQuad(int vertexId1, int vertexId2, int vertexId3, int vertexId4, bool isSheet) {
			if(!IsEdgePresent(vertexId1, vertexId3)) {
					AddEdge(vertexId1, vertexId3, false);
			}

			AddTriangle(vertexId1, vertexId2, vertexId3, isSheet);
			AddTriangle(vertexId1, vertexId3, vertexId4, isSheet);
		}

		void NonManifoldMesh::AddTriangle(int vertexId1, int vertexId2, int vertexId3, bool isSheet) {
			NonManifoldMeshFace face;
			face.isSheet = isSheet;
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

		void NonManifoldMesh::MarkFixedVertices() {
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

					vertices[i].stationary = vertices[i].stationary || (vertices[i].edgeIds.size() <= 1) || (sheetFound && edgeFound);
				}
			}
		}

		void NonManifoldMesh::RemoveFace(int faceId) {
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

		void NonManifoldMesh::RemoveEdge(int edgeId) {
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

		void NonManifoldMesh::RemoveVertex(int vertexId) {
			int vertexIndex = GetVertexIndex(vertexId);
			vertices[vertexIndex].valid = false;
			vertexCount--;

			if(vertices[vertexIndex].edgeIds.size() > 0) {
				printf("Cannot remove vertexId %i as it has edge associations\n", vertexId);
				exit(0);
			}
		}

		void NonManifoldMesh::RemoveNullEntries() {
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


		void NonManifoldMesh::ToOffCells(string fileName) {
			RemoveNullEntries();
			FILE * outFile = fopen(fileName.c_str(), "wt");
			fprintf(outFile, "OFF\n");
			fprintf(outFile, "%li %li %li\n", vertices.size(), faces.size() + edges.size(), 0);
			int i,j;
			for(i = 0; i < (int)vertices.size(); i++) {
				fprintf(outFile, "%lf %lf %lf\n", vertices[i].position.X(), vertices[i].position.Y(), vertices[i].position.Z());
			}
			int lastVertex;
			double r,g,b;
			for(i = 0; i < (int)faces.size(); i++) {
				fprintf(outFile, "%li ", faces[i].edgeIds.size());
				lastVertex = -1;

				for(j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
					if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) || 
						(edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
						lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];						
					} else {
						lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
					}

					//printf("%li %li %li \n", lastVertex, edges[faces[i].edgeIds[j]].vertexIds[0], edges[faces[i].edgeIds[j]].vertexIds[1]);
					fprintf(outFile, "%li ", lastVertex);
				}
				if(faces[i].isSheet) {
					r = 0.0;
					g = 0.0;
					b = 1.0;
				} else {
					r = 0.5;
					g = 0.5;
					b = 0.0;
				}
				fprintf(outFile, "%lf %lf %lf\n", r, g, b);
			}

			for(i = 0; i < (int)edges.size(); i++) {
				if(edges[i].isHelix) {
					r = 0.0;
					g = 1.0;
					b = 0;0;
				} else {
					r = 0.5;
					g = 0.5;
					b = 0.0;
				}
				fprintf(outFile, "4 %li %li %li %li %lf %lf %lf\n", edges[i].vertexIds[0], edges[i].vertexIds[0], edges[i].vertexIds[1], edges[i].vertexIds[1], r, g, b);
			}
			fclose(outFile);
		}


		void NonManifoldMesh::ToMathematicaFile(string fileName) {
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
			fprintf(outF, "},\n");

			// Articulation Points
			fprintf(outF, "{");
			bool first = true;
			for(unsigned int i = 0; i < vertices.size(); i++) {
				if(vertices[i].articulationPoint) {
					if(!first) {
						fprintf(outF, ", ");
					} 
					fprintf(outF, "%li", i+1);
					first = false;
				}
			}
			fprintf(outF, "},\n");

			// Fixed Points
			fprintf(outF, "{");
			first = true;
			for(unsigned int i = 0; i < vertices.size(); i++) {
				if(vertices[i].stationary) {
					if(!first) {
						fprintf(outF, ", ");
					} 
					fprintf(outF, "%li", i+1);
					first = false;
				}
			}
			fprintf(outF, "},\n");

			// Helix Ends
			fprintf(outF, "{");
			first = true;
			for(unsigned int i = 0; i < vertices.size(); i++) {
				if(vertices[i].helixId >= 0) {
					if(!first) {
						fprintf(outF, ", ");
					} 
					fprintf(outF, "{%li, %li}", i+1, vertices[i].helixId+1);
					first = false;
				}
			}
			fprintf(outF, "}\n");

			fprintf(outF, "}");

			fclose(outF);
		}
		Vector3DFloat NonManifoldMesh::GetVertexNormal(int vertexId) {
			int index = GetVertexIndex(vertexId);
			int edgeIndex;
			Vector3DFloat normal = Vector3DFloat(0,0,0);
			for(int i = 0; i < vertices[index].edgeIds.size(); i++) {
				edgeIndex = GetEdgeIndex(vertices[index].edgeIds[i]);
				for(int j = 0; j < edges[edgeIndex].faceIds.size(); j++) {
					normal += GetFaceNormal(edges[edgeIndex].faceIds[j]);
				}
			}
			normal.Normalize();
			return normal;
		}

		Vector3DFloat NonManifoldMesh::GetFaceNormal(int faceId) {

			int lastVertex = -1;
			int i = GetFaceIndex(faceId);
			int vertexIds[40];
			vertexCount = 0;

			for(int j = (int)faces[i].edgeIds.size()-1; j >= 0; j--) {
				if((edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[0]) || 
					(edges[faces[i].edgeIds[j]].vertexIds[0] == edges[faces[i].edgeIds[(j+1)%faces[i].edgeIds.size()]].vertexIds[1])) {
					lastVertex = edges[faces[i].edgeIds[j]].vertexIds[1];						
				} else {
					lastVertex = edges[faces[i].edgeIds[j]].vertexIds[0];
				}
				vertexIds[vertexCount] = lastVertex;
				vertexCount++;
			}

			Vector3DFloat normal = Vector3DFloat(1,0,0);
			if(vertexCount >= 3) {
				normal = -(vertices[GetVertexIndex(vertexIds[1])].position - vertices[GetVertexIndex(vertexIds[0])].position) ^
								(vertices[GetVertexIndex(vertexIds[2])].position - vertices[GetVertexIndex(vertexIds[0])].position);
				normal.Normalize();
			} 
			return normal;
		}
		NonManifoldMesh * NonManifoldMesh::SmoothLaplacian(double converganceRate) {
			NonManifoldMesh * smoothedMesh = new NonManifoldMesh(this);
			int i, j, vertexIndex;
			Vector3DFloat newPosition;
			NonManifoldMeshVertex vertex;
			for(i = 0; i < (int)vertices.size(); i++) {
				vertex = vertices[i];
				if(vertex.valid && !vertex.stationary) {
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


		NonManifoldMesh * NonManifoldMesh::LoadOffFile(string fileName) {
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
				mesh->AddVertex(Vector3DFloat(xPos, yPos, zPos), false, false, NOT_A_HELIX_VERTEX);
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
						for(int i = 0; i < nFaceNodes; i++) {
							if(!mesh->IsEdgePresent(faceNodes[i], faceNodes[(i+1)%nFaceNodes])) {
								mesh->AddEdge(faceNodes[i], faceNodes[(i+1)%nFaceNodes], false);
							}
						}

						if(!mesh->IsEdgePresent(faceNodes[0], faceNodes[2])) {
								mesh->AddEdge(faceNodes[0], faceNodes[2], false);
						}

						if((faceNodes[0] != faceNodes[1]) && (faceNodes[0] != faceNodes[2]) && (faceNodes[0] != faceNodes[3])
							&& (faceNodes[1] != faceNodes[2]) && (faceNodes[1] != faceNodes[3]) && (faceNodes[2] != faceNodes[3])) {
							mesh->AddQuad(faceNodes[0], faceNodes[1], faceNodes[2], faceNodes[3], false);
						}
						break;
					default :
						lFaces++;
						for(int i = 0; i < nFaceNodes; i++) {
							fscanf(inFile, "");
							fscanf(inFile, "%d", &faceNodes[i]);
						}
						for(int i = 0; i < nFaceNodes; i++) {
							if(!mesh->IsEdgePresent(faceNodes[i], faceNodes[(i+1)%nFaceNodes])) {
								mesh->AddEdge(faceNodes[i], faceNodes[(i+1)%nFaceNodes], false);
							}
						}

						for(int i = 2; i < nFaceNodes; i++) {
							mesh->AddTriangle(faceNodes[0], faceNodes[i-1], faceNodes[i], false);
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