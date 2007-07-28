#ifndef DISCRETE_MESH_H
#define DISCRETE_MESH_H

#include "../SkeletonMaker/volume.h"
#include "../MatlabInterface/DataStructures.h"
#include "../MatlabInterface/VectorLib.h"
#include <string.h>

using namespace std;
using namespace wustl_mm::MatlabInterface;

namespace wustl_mm {
	namespace GraySkeletonCPP {		
		const unsigned char CURVE_TYPE_XPOS = 1;
		const unsigned char CURVE_TYPE_YPOS = 2;
		const unsigned char CURVE_TYPE_ZPOS = 4;
		const unsigned char CURVE_TYPES[2][2][2] = {{{0,CURVE_TYPE_ZPOS},{CURVE_TYPE_YPOS,0}}, {{CURVE_TYPE_XPOS,0}, {0,0}}};
		const unsigned char CURVE_TYPES_3[3] = {CURVE_TYPE_XPOS, CURVE_TYPE_YPOS, CURVE_TYPE_ZPOS};
		const int VOLUME_NEIGHBOR_CURVES_6[6][7] = {{0,0,0,CURVE_TYPE_XPOS,1,0,0}, {-1,0,0,CURVE_TYPE_XPOS,-1,0,0}, 
													{0,0,0,CURVE_TYPE_YPOS,0,1,0}, {0,-1,0,CURVE_TYPE_YPOS,0,-1,0},
													{0,0,0,CURVE_TYPE_ZPOS,0,0,1}, {0,0,-1,CURVE_TYPE_ZPOS,0,0,-1}};

		const unsigned char SURFACE_TYPE_XY = 1;
		const unsigned char SURFACE_TYPE_XZ = 2;
		const unsigned char SURFACE_TYPE_YZ = 4;
		const unsigned char SURFACE_TYPES[2][2][2] = {{{0,0},{0,SURFACE_TYPE_YZ}},{{0,SURFACE_TYPE_XZ},{SURFACE_TYPE_XY,0}}};

		class DiscreteMesh {
		public:
			DiscreteMesh(int sizeX, int sizeY, int sizeZ);
			DiscreteMesh(Volume * volume);
			DiscreteMesh(DiscreteMesh * mesh);
			~DiscreteMesh();

			void AddVoxel(int x, int y, int z);
			int GetIndex(int x, int y, int z);
			bool IsPointPresent(int x, int y, int z);
			bool IsCurvePresent(int x, int y, int z, unsigned char direction);
			bool IsCurvePresent(Vector3DInt point1, Vector3DInt point2);
			bool FollowCurve(int & x, int & y, int & z);
			int GetCurveNeighbors(int x, int y, int z, Vector3DInt * & neighbors);
			int GetCurveNeighborsCount(int x, int y, int z);

			static int GetC6(Vector3DInt * neighbors, int neighborCount, Vector3DInt currPoint); 
			static int GetN6(Vector3DInt * & n6, Volume * sourceVolume, int x, int y, int z);
			static int GetN6_2(Vector3DInt * & n6_2, Volume * sourceVolume, int x, int y, int z);
			static int GetN18(Vector3DInt * & n18, Volume * sourceVolume, int x, int y, int z);
			static int GetN6Count(Volume * sourceVolume, int x, int y, int z);
			static int GetN6_2Count(Volume * sourceVolume, int x, int y, int z);
			static int GetN18Count(Volume * sourceVolume, int x, int y, int z);
			static int GetN26Count(Volume * sourceVolume, int x, int y, int z);
			static bool IsPoint(Volume * sourceVolume, int x, int y, int z);
			static bool IsCurveEnd(Volume * sourceVolume, int x, int y, int z);
			static bool IsCurveBody(Volume * sourceVolume, int x, int y, int z);
			static bool IsSurfaceBorder(Volume * sourceVolume, int x, int y, int z);
			static bool IsSurfaceBody(Volume * sourceVolume, int x, int y, int z, bool doDependantChecks);
			static bool IsVolumeBorder(Volume * sourceVolume, int x, int y, int z, bool doDependantChecks);
			static bool IsVolumeBody(Volume * sourceVolume, int x, int y, int z);
			static void FindCurveBase(Vector3D &p1, Vector3D &p2);
			static void FindCurveBase(Vector3DInt &p1, Vector3DInt &p2);
			static void FindSurfaceBase(Vector3D &p1, Vector3D &p2, Vector3D &p3, Vector3D &p4);
			static void FindSurfaceBase(Vector3DInt &p1, Vector3DInt &p2, Vector3DInt &p3, Vector3DInt &p4);


			void AddPoint(Vector3DInt point);
			void AddCurve(Vector3DInt p1, Vector3DInt p2); 
			void AddSurface(Vector3DInt p1, Vector3DInt p2, Vector3DInt p3, Vector3DInt p4);
			void RemovePoint(Vector3DInt point);
			void RemoveCurve(Vector3DInt p1, Vector3DInt p2); 

		private:
			Volume * volume;
			bool * points;
			unsigned char * curves;
			unsigned char * surfaces;
			int sizeX, sizeY, sizeZ;
		};

		DiscreteMesh::DiscreteMesh(int sizeX, int sizeY, int sizeZ) {
			volume = new Volume(sizeX, sizeY, sizeZ);
			points = new bool[sizeX * sizeY * sizeZ];
			curves = new unsigned char[sizeX * sizeY * sizeZ];
			surfaces = new unsigned char[sizeX * sizeY * sizeZ];
			for(int i = 0; i < sizeX*sizeY*sizeZ; i++) {
				points[i] = false;
				curves[i] = 0;
				surfaces[i] = 0;
			}
			this->sizeX = sizeX;
			this->sizeY = sizeY;
			this->sizeZ = sizeZ;
		}

		DiscreteMesh::DiscreteMesh(Volume * volume) {
			sizeX = volume->getSizeX();
			sizeY = volume->getSizeY();
			sizeZ = volume->getSizeZ();
			this->volume = new Volume(sizeX, sizeY, sizeZ);
			points = new bool[sizeX * sizeY * sizeZ];
			curves = new unsigned char[sizeX * sizeY * sizeZ];
			surfaces = new unsigned char[sizeX * sizeY * sizeZ];
			for(int i = 0; i < sizeX*sizeY*sizeZ; i++) {
				points[i] = false;
				curves[i] = 0;
				surfaces[i] = 0;
			}

			for(int x = 0; x < sizeX; x++) {
				for(int y = 0; y < sizeY; y++) {
					for(int z = 0; z < sizeZ; z++) {
						if(volume->getDataAt(x, y, z) > 0) {
							AddVoxel(x, y, z);
						}
					}
				}
			}
		}

		DiscreteMesh::DiscreteMesh(DiscreteMesh * mesh) {
			this->sizeX = mesh->sizeX;
			this->sizeY = mesh->sizeY;
			this->sizeZ = mesh->sizeZ;
			this->volume = new Volume(sizeX, sizeY, sizeZ, 0, 0, 0, mesh->volume);
			points = new bool[sizeX * sizeY * sizeZ];
			curves = new unsigned char[sizeX * sizeY * sizeZ];
			surfaces = new unsigned char[sizeX * sizeY * sizeZ];
			for(int i = 0; i < sizeX*sizeY*sizeZ; i++) {
				this->points[i] = mesh->points[i];
				this->curves[i] = mesh->curves[i];
				this->surfaces[i] = mesh->surfaces[i];
			}
		}

		DiscreteMesh::~DiscreteMesh() {
			delete [] points;
			delete [] curves;
			delete [] surfaces;
		}

		void DiscreteMesh::AddVoxel(int x, int y, int z) {
			volume->setDataAt(x, y, z, 1);
			Vector3DInt p;			
			int neighborCount = 0;

			p.values[0] = x;
			p.values[1] = y;
			p.values[2] = z;

			if(IsPoint(volume, x, y, z)) {
				AddPoint(p);			
			} else if (IsCurveEnd(volume, x, y, z) || IsCurveBody(volume, x, y, z)) {
				Vector3DInt neighbors[6];
				for(int n = 0; n < 6; n++) {
					if(volume->getDataAt(x + VOLUME_NEIGHBORS_6[n][0], y + VOLUME_NEIGHBORS_6[n][1], z + VOLUME_NEIGHBORS_6[n][2]) > 0) {
						neighbors[neighborCount].values[0] = x + VOLUME_NEIGHBORS_6[n][0];
						neighbors[neighborCount].values[1] = y + VOLUME_NEIGHBORS_6[n][1];
						neighbors[neighborCount].values[2] = z + VOLUME_NEIGHBORS_6[n][2];
						neighborCount++;
					}
				}
				for(int n = 0; n < neighborCount; n++) {
					AddCurve(p, neighbors[n]);
				}
			} else if (IsSurfaceBorder(volume, x, y, z) || IsSurfaceBody(volume, x, y, z, true)) {
				Vector3DInt faces[12][3];
				int faceCount = 0;
				bool allPoints;
				for(int n = 0; n < 12; n++) {
					allPoints = true;
					for(int p = 0; p < 3; p++) {
						faces[faceCount][p].values[0] = x + VOLUME_NEIGHBOR_FACES[n][p][0];
						faces[faceCount][p].values[1] = y + VOLUME_NEIGHBOR_FACES[n][p][1];
						faces[faceCount][p].values[2] = z + VOLUME_NEIGHBOR_FACES[n][p][2];
						allPoints = allPoints && (volume->getDataAt(faces[faceCount][p].values[0], faces[faceCount][p].values[1], faces[faceCount][p].values[2]) > 0);
					}
					if(allPoints) {
						faceCount++;
					}
				}

				for(int n = 0; n < faceCount; n++) {
					AddSurface(p, faces[n][0], faces[n][1], faces[n][2]);
				}


			}
		}

		int DiscreteMesh::GetIndex(int x, int y, int z) {
			return volume->getIndex(x, y, z);
		}

		bool DiscreteMesh::IsPointPresent(int x, int y, int z) {
			return points[GetIndex(x, y, z)];
		}

		bool DiscreteMesh::IsCurvePresent(int x, int y, int z, unsigned char direction) {
			return ((curves[GetIndex(x, y, z)] & direction) == direction);
		}

		bool DiscreteMesh::IsCurvePresent(Vector3DInt point1, Vector3DInt point2) {
			Vector3DInt p1, p2;
			p1 = point1;
			p2 = point2;
			FindCurveBase(p1, p2);

			unsigned char curveType = CURVE_TYPES[p2.values[0] - p1.values[0]][p2.values[1] - p1.values[1]][p2.values[2] - p1.values[2]];
			return IsCurvePresent(p1.values[0], p1.values[1], p1.values[2], curveType);
		}


		bool DiscreteMesh::FollowCurve(int & x, int & y, int & z) {
			Vector3DInt * neighbors;
			int count = GetCurveNeighbors(x, y, z, neighbors);
			if(count==1) {
				x = neighbors[0].values[0];
				y = neighbors[0].values[1];
				z = neighbors[0].values[2];
			}
			delete [] neighbors;
			return (count==1);
		}

		int DiscreteMesh::GetCurveNeighbors(int x, int y, int z, Vector3DInt * & neighbors) {
			int count = 0;
			neighbors = new Vector3DInt[6];

			for(int i = 0; i < 6; i++) {
				if(IsCurvePresent(x+VOLUME_NEIGHBOR_CURVES_6[i][0], y+VOLUME_NEIGHBOR_CURVES_6[i][1], z+VOLUME_NEIGHBOR_CURVES_6[i][2], VOLUME_NEIGHBOR_CURVES_6[i][3])) {
					neighbors[count].values[0] = x + VOLUME_NEIGHBOR_CURVES_6[i][4];	
					neighbors[count].values[1] = y + VOLUME_NEIGHBOR_CURVES_6[i][5];	
					neighbors[count].values[2] = z + VOLUME_NEIGHBOR_CURVES_6[i][6];
					count++;
				}
			}
			return count;
		}

		int DiscreteMesh::GetCurveNeighborsCount(int x, int y, int z) {
			int count = 0;
			for(int i = 0; i < 6; i++) {
				if(IsCurvePresent(x+VOLUME_NEIGHBOR_CURVES_6[i][0], y+VOLUME_NEIGHBOR_CURVES_6[i][1], z+VOLUME_NEIGHBOR_CURVES_6[i][2], VOLUME_NEIGHBOR_CURVES_6[i][3])) {
					count++;
				}
			}
			return count;
		}

		void DiscreteMesh::AddPoint(Vector3DInt point){
			//printf("Adding Point: {%i %i %i}\n", point.values[0], point.values[1], point.values[2]);
			points[volume->getIndex(point.values[0], point.values[1], point.values[2])] = true;
		}

		void DiscreteMesh::AddCurve(Vector3DInt p1, Vector3DInt p2){
			RemovePoint(p1);
			RemovePoint(p2);
			Vector3DInt p11 = p1, p22 = p2;
			FindCurveBase(p11, p22);

			unsigned char curveType = CURVE_TYPES[p22.values[0] - p11.values[0]][p22.values[1] - p11.values[1]][p22.values[2] - p11.values[2]];
			int index = volume->getIndex(p11.values[0], p11.values[1], p11.values[2]);
			
			//printf("Adding Curve: {%i %i %i} - {%i %i %i} %i\n", 
			//	p11.values[0], p11.values[1], p11.values[2], 
			//	p22.values[0], p22.values[1], p22.values[2], curveType);

			curves[index] = curves[index] | curveType;
		}

		void DiscreteMesh::AddSurface(Vector3DInt p1, Vector3DInt p2, Vector3DInt p3, Vector3DInt p4){
			RemovePoint(p1);
			RemovePoint(p2);
			RemovePoint(p3);
			RemovePoint(p4);

			Vector3DInt p11 = p1, p22 = p2, p33 = p3, p44 = p4;
			FindSurfaceBase(p11, p22, p33, p44);
			RemoveCurve(p11, p22);
			RemoveCurve(p22, p33);
			RemoveCurve(p44, p33);
			RemoveCurve(p11, p44);
			int surfaceType = SURFACE_TYPES[p33.values[0] - p11.values[0]][p33.values[1] - p11.values[1]][p33.values[2] - p11.values[2]];
			//printf("Adding Surface: {%i %i %i} - {%i %i %i} - {%i %i %i} - {%i %i %i} - %i\n", 
			//	p11.values[0], p11.values[1], p11.values[2], 
			//	p22.values[0], p22.values[1], p22.values[2],
			//	p33.values[0], p33.values[1], p33.values[2], 
			//	p44.values[0], p44.values[1], p44.values[2],
			//	surfaceType);
		}

		void DiscreteMesh::RemovePoint(Vector3DInt point){
			//printf("Removing Point: {%i %i %i}\n", point.values[0], point.values[1], point.values[2]);
			points[volume->getIndex(point.values[0], point.values[1], point.values[2])] = false;
		}

		void DiscreteMesh::RemoveCurve(Vector3DInt p1, Vector3DInt p2) {
			Vector3DInt p11 = p1, p22 = p2;
			FindCurveBase(p11, p22);

			unsigned char curveType = CURVE_TYPES[p22.values[0] - p11.values[0]][p22.values[1] - p11.values[1]][p22.values[2] - p11.values[2]];
			int index = volume->getIndex(p11.values[0], p11.values[1], p11.values[2]);
			
			//printf("Removing Curve: {%i %i %i} - {%i %i %i} %i\n", 
			//	p11.values[0], p11.values[1], p11.values[2], 
			//	p22.values[0], p22.values[1], p22.values[2], curveType);

			curves[index] = curves[index] & ~curveType;
		}

		void DiscreteMesh::FindCurveBase(Vector3D &p1, Vector3D &p2) {
			Vector3D temp;
			if ((p1.values[0] > p2.values[0]) || 
				(p1.values[1] > p2.values[1]) || 
				(p1.values[2] > p2.values[2])) {
				temp = p1;
				p1 = p2;
				p2 = temp;
			}
		}

		void DiscreteMesh::FindCurveBase(Vector3DInt &p1, Vector3DInt &p2) {
			Vector3DInt temp;
			if ((p1.values[0] > p2.values[0]) || 
				(p1.values[1] > p2.values[1]) || 
				(p1.values[2] > p2.values[2])) {
				temp = p1;
				p1 = p2;
				p2 = temp;
			}
		}

		void DiscreteMesh::FindSurfaceBase(Vector3D &p1, Vector3D &p2, Vector3D &p3, Vector3D &p4) {

			Vector3D points[4] = {p1, p2, p3, p4};
			Vector3D temp;
			int jVal, minVal, minIndex;

			for(int i = 0; i < 3; i++) {
				minVal = points[i].values[0] + points[i].values[1] + points[i].values[2]; 
				minIndex = i;
				for(int j = i+1; j < 4; j++) {
					jVal = points[j].values[0] + points[j].values[1] + points[j].values[2];
					if(jVal < minVal) {
						minVal = jVal;
						minIndex = j;
					}
				}
				temp = points[i];
				points[i] = points[minIndex];
				points[minIndex] = temp;
			}


			p1 = points[0];
			p2 = points[1];
			p3 = points[3];
			p4 = points[2];
			delete [] points;
		}

		void DiscreteMesh::FindSurfaceBase(Vector3DInt &p1, Vector3DInt &p2, Vector3DInt &p3, Vector3DInt &p4) {

			Vector3DInt points[4] = {p1, p2, p3, p4};
			Vector3DInt temp;
			int jVal, minVal, minIndex;

			for(int i = 0; i < 3; i++) {
				minVal = points[i].values[0] + points[i].values[1] + points[i].values[2]; 
				minIndex = i;
				for(int j = i+1; j < 4; j++) {
					jVal = points[j].values[0] + points[j].values[1] + points[j].values[2];
					if(jVal < minVal) {
						minVal = jVal;
						minIndex = j;
					}
				}
				temp = points[i];
				points[i] = points[minIndex];
				points[minIndex] = temp;
			}


			p1 = points[0];
			p2 = points[1];
			p3 = points[3];
			p4 = points[2];
			delete [] points;
		}

		int DiscreteMesh::GetC6(Vector3DInt * neighbors, int neighborCount, Vector3DInt currPoint) {
			Volume * vol = new Volume(5, 5, 5);
			for(int i = 0; i < neighborCount; i++) {
				vol->setDataAt(neighbors[i].values[0] - currPoint.values[0], neighbors[i].values[1] - currPoint.values[1], neighbors[i].values[2] - currPoint.values[2], 1);
			}

			int c6Count = 0;
			Vector3DInt * n6;
			int n6Count;
			Vector3DInt queue[26];
			Vector3DInt temp;
			int queueSize = 0;

			for(int x = 2; x < 4; x++) {
				for(int y = 2; y < 4; y++) {
					for(int z = 2; z < 4; z++) {
						if(vol->getDataAt(x, y, z) > 0) {
							c6Count++;
							VectorLib::Initialize(queue[0], x, y, z);
							queueSize = 1; 
							while (queueSize > 0) {
								temp = queue[queueSize-1];
								queueSize--;
								vol->setDataAt(temp.values[0], temp.values[1], temp.values[2], 0);
								n6Count = GetN6(n6, vol, temp.values[0], temp.values[1], temp.values[2]);
								for(int i = 0; i < n6Count; i++) {
									VectorLib::Initialize(queue[queueSize], n6[i].values[0], n6[i].values[1], n6[i].values[2]);
									queueSize++;
								}
								delete [] n6;
							}
						}
					}
				}
			}


			delete vol;
			return c6Count;
		}

		int DiscreteMesh::GetN6(Vector3DInt * & n6, Volume * sourceVolume, int x, int y, int z){
			int n6Count = 0;
			n6 = new Vector3DInt[6];
			for(int i = 0; i < 6; i++) {	
				n6[n6Count].values[0] = x + VOLUME_NEIGHBORS_6[i][0];
				n6[n6Count].values[1] = y + VOLUME_NEIGHBORS_6[i][1];
				n6[n6Count].values[2] = z + VOLUME_NEIGHBORS_6[i][2];
				if(sourceVolume->getDataAt(n6[n6Count].values[0], n6[n6Count].values[1], n6[n6Count].values[2]) > 0) {
					n6Count++;
				}
			}
			return n6Count;
		}

		int DiscreteMesh::GetN6_2(Vector3DInt * & n6_2, Volume * sourceVolume, int x, int y, int z) {
			n6_2 = new Vector3DInt[18];
			Vector3DInt * n18;
			Vector3DInt * n6X, * n6Y;
			Vector3DInt n62List[36];
			int n62Count = 0;

			int n18Count = GetN18(n18, sourceVolume, x, y, z);
			int n6CountX = GetN6(n6X, sourceVolume, x, y, z);
			int n6CountY;

			for(int i = 0; i < n6CountX; i++) {
				n6CountY = GetN6(n6Y, sourceVolume, n6X[i].values[0], n6X[i].values[1], n6X[i].values[2]);
				for(int j = 0; j < n6CountY; j++) {
					VectorLib::Initialize(n62List[n62Count], n6Y[j].values[0], n6Y[j].values[1], n6Y[j].values[2]);
					n62Count++;
				}
				delete [] n6Y;
			}

			int retVal = 0;
			bool found;
			for(int i = 0; i < n18Count; i++) {
				found = false;
				for(int j = 0; j < n62Count; j++) {
					found = found || ((n18[i].values[0] == n62List[j].values[0]) && (n18[i].values[1] == n62List[j].values[1]) && (n18[i].values[2] == n62List[j].values[2]));
				}				
				if(found) {
					VectorLib::Initialize(n6_2[retVal], n18[i].values[0], n18[i].values[1], n18[i].values[2]);
					retVal++;
				}
			}

			delete [] n18;
			delete [] n6X;
			delete [] n62List;
			return retVal;
		}

		int DiscreteMesh::GetN18(Vector3DInt * & n18, Volume * sourceVolume, int x, int y, int z) {
			n18 = new Vector3DInt[18];
			int n18Count = 0;
			for(int i = 0; i < 18; i++) {	
				n18[n18Count].values[0] = x + VOLUME_NEIGHBORS_18[i][0];
				n18[n18Count].values[1] = y + VOLUME_NEIGHBORS_18[i][1];
				n18[n18Count].values[2] = z + VOLUME_NEIGHBORS_18[i][2];
				if(sourceVolume->getDataAt(n18[n18Count].values[0], n18[n18Count].values[1], n18[n18Count].values[2]) > 0) {
					n18Count++;
				}
			}
			return n18Count;
		}

		int DiscreteMesh::GetN6Count(Volume * sourceVolume, int x, int y, int z) {
			int n6Count = 0;
			for(int i = 0; i < 6; i++) {
				if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0)  {
					n6Count++;
				}
			}
			return n6Count;
		}

		int DiscreteMesh::GetN6_2Count(Volume * sourceVolume, int x, int y, int z) {
			Vector3DInt * n6_2;
			int count = GetN6_2(n6_2, sourceVolume, x, y, z);
			delete [] n6_2;
			return count;			
		}

		int DiscreteMesh::GetN18Count(Volume * sourceVolume, int x, int y, int z) {
			int n18Count = 0;
			for(int i = 0; i < 18; i++) {
				if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_18[i][0], y + VOLUME_NEIGHBORS_18[i][1], z + VOLUME_NEIGHBORS_18[i][2]) > 0)  {
					n18Count++;
				}
			}
			return n18Count;
		}

		int DiscreteMesh::GetN26Count(Volume * sourceVolume, int x, int y, int z) {
			int n26Count = 0;
			for(int i = 0; i < 26; i++) {
				if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_26[i][0], y + VOLUME_NEIGHBORS_26[i][1], z + VOLUME_NEIGHBORS_26[i][2]) > 0)  {
					n26Count++;
				}
			}
			return n26Count;
		}

		bool DiscreteMesh::IsPoint(Volume * sourceVolume, int x, int y, int z) {
			return (GetN6Count(sourceVolume, x, y, z) == 0);
		}

		bool DiscreteMesh::IsCurveEnd(Volume * sourceVolume, int x, int y, int z) {
			return (GetN6Count(sourceVolume, x, y, z) == 1);
		}

		bool DiscreteMesh::IsCurveBody(Volume * sourceVolume, int x, int y, int z) {
			bool foundFace = false;
			bool allPoints;
			for(int n = 0; n < 12; n++) {
				allPoints = true;
				for(int p = 0; p < 3; p++) {
					allPoints = allPoints && (sourceVolume->getDataAt(x + VOLUME_NEIGHBOR_FACES[n][p][0], y + VOLUME_NEIGHBOR_FACES[n][p][1], z + VOLUME_NEIGHBOR_FACES[n][p][2]) > 0);
				}
				foundFace = foundFace || allPoints;
			}

			return (!foundFace && GetN6Count(sourceVolume, x, y, z) >= 2);

		}

		bool DiscreteMesh::IsSurfaceBorder(Volume * sourceVolume, int x, int y, int z) {
			return sourceVolume->isSheetEnd(x, y, z);

			/*if(doDependantChecks && 
				(IsVolumeBody(sourceVolume, x, y, z) || IsVolumeBorder(sourceVolume, x, y, z, false) || IsSurfaceBody(sourceVolume, x, y, z, false) )) {
				return false;
			}

			bool visited[3][3][3];
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					for(int k = 0; k < 3; k++) {
						visited[i][j][k] = false;
					}
				}
			}


			bool isSurface = false;
			bool allInFace;
			for(int face = 0; face < 12; face++) {
				allInFace = true;
				for(int p = 0; p < 3; p++) {
					allInFace = allInFace && (sourceVolume->getDataAt(x + VOLUME_NEIGHBOR_FACES[face][p][0], y + VOLUME_NEIGHBOR_FACES[face][p][1], z + VOLUME_NEIGHBOR_FACES[face][p][2]) > 0);
				}
				if(allInFace) {
					for(int p = 0; p < 3; p++) {
						visited[VOLUME_NEIGHBOR_FACES[face][p][0]+1][VOLUME_NEIGHBOR_FACES[face][p][1] +1][VOLUME_NEIGHBOR_FACES[face][p][2]+1] = true;
					}
				}
				isSurface = isSurface || allInFace;
			}

			int visitCount = 0;
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					for(int k = 0; k < 3; k++) {
						if(visited[i][j][k]) {
							visitCount++;
						}
					}
				}
			}
			return isSurface && (visitCount < 8);*/
		}

		bool DiscreteMesh::IsSurfaceBody(Volume * sourceVolume, int x, int y, int z, bool doDependantChecks) {
			if(doDependantChecks && 
				(IsPoint(sourceVolume, x, y, z) || IsCurveEnd(sourceVolume, x, y, z) ||
				IsCurveBody(sourceVolume, x, y, z) || IsSurfaceBorder(sourceVolume, x, y, z) ||				
				IsVolumeBody(sourceVolume, x, y, z) || IsVolumeBorder(sourceVolume, x, y, z, false))) {
				return false;
			}
			return true;

			//bool visited[3][3][3];
			//for(int i = 0; i < 3; i++) {
			//	for(int j = 0; j < 3; j++) {
			//		for(int k = 0; k < 3; k++) {
			//			visited[i][j][k] = false;
			//		}
			//	}
			//}


			//bool isSurfaceBody = false;
			//bool allInFace;
			//for(int face = 0; face < 12; face++) {
			//	allInFace = true;
			//	for(int p = 0; p < 3; p++) {
			//		allInFace = allInFace && (sourceVolume->getDataAt(x + VOLUME_NEIGHBOR_FACES[face][p][0], y + VOLUME_NEIGHBOR_FACES[face][p][1], z + VOLUME_NEIGHBOR_FACES[face][p][2]) > 0);
			//	}
			//	if(allInFace) {
			//		for(int p = 0; p < 3; p++) {
			//			visited[VOLUME_NEIGHBOR_FACES[face][p][0]+1][VOLUME_NEIGHBOR_FACES[face][p][1] +1][VOLUME_NEIGHBOR_FACES[face][p][2]+1] = true;
			//		}
			//	}
			//	isSurfaceBody = isSurfaceBody || allInFace;
			//}

			//int visitCount = 0;
			//for(int i = 0; i < 3; i++) {
			//	for(int j = 0; j < 3; j++) {
			//		for(int k = 0; k < 3; k++) {
			//			if(visited[i][j][k]) {
			//				visitCount++;
			//			}
			//		}
			//	}
			//}
			//return isSurfaceBody && (visitCount >= 8);
		}


		bool DiscreteMesh::IsVolumeBorder(Volume * sourceVolume, int x, int y, int z, bool doDependantChecks) {
			if(doDependantChecks && IsVolumeBody(sourceVolume, x, y, z)) {
				return false;
			}

			bool isBorder = false;
			bool allInCube;
			for(int cube = 0; cube < 8; cube++) {
				allInCube = true;
				for(int p = 0; p < 7; p++) {
					allInCube = allInCube && (sourceVolume->getDataAt(x + VOLUME_NEIGHBOR_CUBES[cube][p][0], y + VOLUME_NEIGHBOR_CUBES[cube][p][1], z + VOLUME_NEIGHBOR_CUBES[cube][p][2]) > 0);
				}
				isBorder = isBorder || allInCube;
			}
			return isBorder;
		}
		bool DiscreteMesh::IsVolumeBody(Volume * sourceVolume, int x, int y, int z) {
			return (GetN26Count(sourceVolume, x, y, z) == 26);
		}		



	}
}

#endif
