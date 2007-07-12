/*
Has basic functions on primitive geometric shapes

Author: Sasakthi S. Abeysinghe
Date  : 01/28/2006
*/

#ifndef GEOMETRICSHAPE_H
#define GEOMETRICSHAPE_H

#include "VectorMath.h"
#include <vector>

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		struct Polygon  {
			int pointIndex1;
			int pointIndex2;
			int pointIndex3;
			int pointIndex4;
		};

		class GeometricShape {
		public:
			GeometricShape();
			~GeometricShape();
			void Translate(Vector3 translationVector);
			void Rotate(Vector3 axis, double angle);
			void Scale(double x, double y, double z);
			bool IsInsideShape(Point3 p);
			Point3 GetWorldCoordinates(Point3 point);
			void AddInternalCell(Point3Int point);
			int GetLocationInVector(vector<Point3Int> v, Point3Int point);
			void FindCornerCellsInHelix();
			Point3Int GetCornerCell(int node);
		public:
			int geometricShapeType;
			int length;
			vector<Point3Int> internalCells;
			vector<Point3Int> cornerCells;
			vector<Point3> polygonPoints;
			vector<Polygon> polygons;
			Matrix4 worldToObject;
			Matrix4 objectToWorld;
		private:
			bool IsInsideCylinder(Point3 point);
			bool IsInsidePolygon(Point3 point);
		};

		GeometricShape::GeometricShape() {
			worldToObject = Matrix4::identity();
			objectToWorld = Matrix4::identity();
			internalCells.clear();
		}

		GeometricShape::~GeometricShape(){
			internalCells.clear();
			cornerCells.clear();
			polygonPoints.clear();
			polygons.clear();
		}

		void GeometricShape::Translate(Vector3 translationVector) {
			worldToObject = worldToObject * Matrix4::translation(translationVector);
			objectToWorld = Matrix4::translation(-translationVector) * objectToWorld;
		}

		void GeometricShape::Rotate(Vector3 axis, double angle){
			worldToObject = worldToObject * Matrix4::rotation(axis, angle);
			objectToWorld = Matrix4::rotation(axis, -angle) * objectToWorld;
		}
		void GeometricShape::Scale(double x, double y, double z){
			worldToObject = worldToObject * Matrix4::scaling(x, y, z);
			objectToWorld = Matrix4::scaling(1.0/x, 1.0/y, 1.0/z) * objectToWorld;
		}

		bool GeometricShape::IsInsideShape(Point3 point) {
			Point3 newPoint = Point3(point[0] * VOXEL_SIZE, point[1] * VOXEL_SIZE, point[2] * VOXEL_SIZE);
			if(geometricShapeType == GRAPHEDGE_HELIX) {
				return IsInsideCylinder(newPoint);
			} else {
				return IsInsidePolygon(newPoint);
			}
		}

		bool GeometricShape::IsInsidePolygon(Point3 p) {
			Polygon poly;
			Point3 a,b,c,q;
			double l1, l2;
			Vector3 n;
			double d;
			bool isInside = false;
			double A,B,C,D,E,F,G,H,I;
			for(unsigned int i = 0; i < polygons.size(); i++) {
				poly = (Polygon)polygons[i];
				a = (Point3)polygonPoints[poly.pointIndex1];
				b = (Point3)polygonPoints[poly.pointIndex2];
				c = (Point3)polygonPoints[poly.pointIndex3];
				n = ((b-a)^(c-a)) / ((b-a)^(c-a)).length();
				d = n * (p - a);
				q = p + n*d;

				A = a[0]-c[0];
				B = b[0]-c[0];
				C = c[0]-q[0];
				D = a[1]-c[1];
				E = b[1]-c[1];
				F = c[1]-q[1];
				G = a[2]-c[2];
				H = b[2]-c[2];
				I = c[2]-q[2];

				l1 = (B*(F + I) - C*(E + H)) / (A*(E + H) - B*(D + G));
				l2 = (A*(F + I) - C*(D + G)) / (B*(D + G) - A*(E + H));
				
				isInside = isInside || ((abs(d) < 1) && (l1 >= 0) && (l1 <= 1) && (l2 >= 0) && (l2 <= 1));
			}
			return isInside;
		}

		bool GeometricShape::IsInsideCylinder(Point3 point) {
			point = objectToWorld * point;
			return ((point[0]*point[0] + point[2]*point[2] <= 1) && (abs(point[1]) <= 0.5));
		}

		Point3 GeometricShape::GetWorldCoordinates(Point3 point) {
			return objectToWorld * point;
		}

		void GeometricShape::AddInternalCell(Point3Int point) {
			internalCells.push_back(point);
		}

		void GeometricShape::FindCornerCellsInHelix() {
			int d[6][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
			d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
			d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;
			int insideCounter;
			for(unsigned int i = 0; i < internalCells.size(); i++) {
				insideCounter = 0;
				for(int j = 0; j < 6; j++) {
					if(GetLocationInVector(internalCells, Point3Int(internalCells[i].x + d[j][0], internalCells[i].y + d[j][1], internalCells[i].z + d[j][2], 0)) >= 0) {
						insideCounter++;
					}
				}
				if(insideCounter == 1) {
					cornerCells.push_back(internalCells[i]);
				}
			}
			assert(cornerCells.size() >= 2);

			double maxDistance = -1;
			double dist1, dist2;
			int corner1 = 0;
			int corner2 = cornerCells.size() - 1;

			for(int i = 0; i < (int)cornerCells.size() - 1; i++){
				for(unsigned int j = i+1; j < cornerCells.size(); j++) {
					dist1 = Point3Int::EuclideanDistance(cornerCells[i], cornerCells[j]);
					if(maxDistance < dist1) {
						corner1 = i;
						corner2 = j;
						maxDistance = dist1;
					}
				}
			}

			cornerCells[corner1].node = 1;
			cornerCells[corner2].node = 2;

			for(unsigned int i = 0; i < cornerCells.size(); i++) {
				dist1 = Point3Int::EuclideanDistance(cornerCells[corner1], cornerCells[i]);
				dist2 = Point3Int::EuclideanDistance(cornerCells[corner2], cornerCells[i]);
				if((dist1 > BORDER_MARGIN_THRESHOLD) && (dist2 > BORDER_MARGIN_THRESHOLD)) {
					cornerCells[i].node = 0;
				} else if(dist1 < dist2) {
					cornerCells[i].node = 1;
				} else {
					cornerCells[i].node = 2;
				}
			}

			for(int i = (int)cornerCells.size() - 1; i >= 0; i--) {
				if(cornerCells[i].node == 0) {
					cornerCells.erase(cornerCells.begin() + i);
				}
			}
			assert(cornerCells.size() >= 2);
		}

		int GeometricShape::GetLocationInVector(vector<Point3Int> v, Point3Int point) {
			int loc = -1;
			for(unsigned int i = 0; (i < v.size() && loc < 0); i++) {
				if(v[i] == point) {
					loc = i;
				}
			}
			return loc;
		}

		Point3Int GeometricShape::GetCornerCell(int node) {
			for(unsigned int i = 0; i < cornerCells.size(); i++) {
				if(cornerCells[i].node == node) {
					return cornerCells[i];
				}
			}
			return Point3Int(0,0,0,0);
		}
	}
}
#endif