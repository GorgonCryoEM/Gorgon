// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Data storage and basic functions on primitive geometric shapes

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.15.2.3  2009/05/15 21:34:00  schuhs
//   Adding comments to method that determines if a point is inside a polygon
//
//   Revision 1.15.2.2  2009/05/15 15:42:13  schuhs
//   Adding a method to return the minimum distance between a polygon (set of triangles) and a point.
//
//   Revision 1.15.2.1  2009/05/13 20:26:47  schuhs
//   Adding comments to FindCornerCellsInHelix method
//
//   Revision 1.15  2009/03/16 16:17:34  ssa1
//   Fitting SSEs into the Density
//
//   Revision 1.14  2008/11/24 18:32:28  ssa1
//   Giving helix end points
//
//   Revision 1.13  2008/11/18 18:10:24  ssa1
//   Changing the scaling functions when doing graph matching to find correspondences
//
//   Revision 1.12  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//


#ifndef GEOMETRICSHAPE_H
#define GEOMETRICSHAPE_H

#include "VectorMath.h"
#include <vector>
#include <MathTools/Vector3D.h>

using namespace std;
using namespace wustl_mm::MathTools;

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
			bool GetSelected();
			bool IsHelix();
			bool IsSheet();
			bool IsInsideShape(Point3 p);
			double PolygonsDistanceToPoint(Point3 p);
			double GetHeight();
			double GetRadius();
			double GetCornerCellsMaxLength();
			int GetLocationInVector(vector<Point3Int> v, Point3Int point);
			int GetGeometricShapeType();
			Matrix4 GetRotationMatrix();
			Matrix4 GetWorldToObjectMatrix();
			Point3 GetCenter();
			void AddInternalCell(Point3Int point);
			void FindCornerCellsInHelix();
			void Rotate(Vector3 axis, double angle);
			void Translate(Vector3 translationVector);
			void SetColor(float r, float g, float b, float a);
			void SetCenter(Point3 center);
			void SetHeight(double height);
			void SetRadius(double radius);
			void GetColor(float & r, float & g, float & b, float & a);
			void SetSelected(bool selected);


			Point3 GetWorldCoordinates(Point3 point);
			Point3Int GetCornerCell(int node);
			Vector3DFloat GetCornerCell2(int node);
			Vector3DFloat GetCornerCell3(int node);
		private:
			bool IsInsideCylinder(Point3 point);
			bool IsInsidePolygon(Point3 point);
			void Scale(double x, double y, double z);
			void UpdateWorldToObjectMatrix();

		public:
			int geometricShapeType;
			float length;
			vector<Point3Int> internalCells;
			vector<Point3Int> cornerCells;
			vector<Point3> polygonPoints;
			vector<Polygon> polygons;
		private:
			Matrix4 worldToObject;
			Matrix4 objectToWorld;
			Point3	centerPoint;
			double  radius;
			double  height;
			Matrix4 rotationMatrix;
			Matrix4 inverseRotationMatrix;
			float colorR;
			float colorG;
			float colorB;
			float colorA;
			bool selected;
		};

		GeometricShape::GeometricShape() {
			worldToObject = Matrix4::identity();
			objectToWorld = Matrix4::identity();
			rotationMatrix = Matrix4::identity();
			inverseRotationMatrix = Matrix4::identity();
			internalCells.clear();
			colorR = 0.0f;
			colorG = 1.0f;
			colorB = 0.0f;
			colorA = 1.0f;
			selected = false;
			
		}

		GeometricShape::~GeometricShape(){
			internalCells.clear();
			cornerCells.clear();
			polygonPoints.clear();
			polygons.clear();
		}

		bool GeometricShape::GetSelected() {
			return selected;
		}

		bool GeometricShape::IsHelix() {
			return (geometricShapeType == GRAPHEDGE_HELIX);
		}

		bool GeometricShape::IsSheet() {
			return (geometricShapeType != GRAPHEDGE_HELIX);
		}

		bool GeometricShape::IsInsideShape(Point3 point) {
			Point3 newPoint = Point3(point[0], point[1], point[2]);
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
				// read triangle vertices
				a = (Point3)polygonPoints[poly.pointIndex1]; 
				b = (Point3)polygonPoints[poly.pointIndex2]; 
				c = (Point3)polygonPoints[poly.pointIndex3];
				// find surface normal
				n = ((b-a)^(c-a)) / ((b-a)^(c-a)).length();
				// measure distance from point p to triangle
				d = n * (p - a);
				// find projection of p onto triangle
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

		// returns the minimum distance between a point p and a triangle
		// uses algorithm from http://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
		// also read for reference http://geant4.cern.ch/support/source/geant4/source/geometry/solids/specific/src/G4TriangularFacet.cc
		double GeometricShape::PolygonsDistanceToPoint(Point3 P) {
			Polygon tri;
			Point3 A,B,C;
			Vector3 D, E0, E1;
			Vector3 n;
			double a, b, c, d, e, f, det, invDet, numer, denom, s, t, tmp0, tmp1;
			int region;
			double dmin = 9999; // initialize to be large number -- need to fix this
	
			// find min distance from point p to any triangle in the polygon
			for(unsigned int i = 0; i < polygons.size(); i++) {
				tri = (Polygon)polygons[i];
				A = (Point3)polygonPoints[tri.pointIndex1];
				B = (Point3)polygonPoints[tri.pointIndex2];
				C = (Point3)polygonPoints[tri.pointIndex3];
				D = B - P;
				E0 = C - B;
				E1 = A - B;
				a = E0 * E0;
				b = E0 * E1;
				c = E1 * E1;
				d = E0 * D;
				e = E1 * D;
				f = D * D;
				det = a * c - b * b;
				s = b * e - c * d; 
				t = b * d - a * e;
				if (s+t <= det){
					if ( s < 0 ) { if ( t < 0 ) { region=4; } else { region=3; } }
					else if ( t < 0 ) { region=5; }
					else { region=0; }
				} else {
					if ( s < 0 ) { region=2; }
					else if ( t < 0 ) { region=6; }
					else { region=1; }
				}
				switch (region) {
				case 0:
					invDet = 1.0/det;
					s *= invDet;
					t *= invDet;
					break;
				case 1:
					if ( numer <= 0.0 ) {
						s = 0.0;
					} else {
						denom = a-2.0*b+c; // positive quantity
						s = ( numer >= denom ? 1 : numer/denom );
					}
					t = 1.0-s;
					break;
				case 2:
					tmp0 = b+d;
					tmp1 = c+e;
					if ( tmp1 > tmp0 ) {// minimum on edge s+t=1
						numer = tmp1 - tmp0;
						denom = a-2.0*b+c;
						s = ( numer >= denom ? 1.0 : numer/denom );
						t = 1.0-s;
					} else { // minimum on edge s=0
						s = 0.0;
						t = ( tmp1 <= 0.0 ? 1.0 : ( e >= 0.0 ? 0.0 : -e/c ) );
					}
					break;
				case 3:
					s = 0.0;
					t = ( e >= 0.0 ? 0.0 : ( -e >= c ? 1.0 : -e/c ) );
					break;
				case 4:
					// fixed?
					if (d < 0.0) {
						  t = 0.0;
						  s = ( -d >= a ? 1.0 : -d/a );
					} else {
						  s = 0.0;
						  t = ( e >= 0.0 ? 0.0 : (-e >= c ? 1.0 : -e/c) );
					}
					break;
				case 5:
					s = ( d >= 0.0 ? 0.0 : ( -d >= a ? 1.0 : -d/a ) );
					t = 0.0;
					break;
				case 6:
					// fixed?
					tmp0 = b+e;
					tmp1 = a+d;
					if ( tmp1 > tmp0 ) {// minimum on edge s+t=1
						numer = tmp1 - tmp0;
						denom = a-2.0*b+c;
						t = ( numer >= denom ? 1.0 : numer/denom );
						s = 1.0-t;
					} else { // minimum on edge s=0
						t = 0.0;
						s = ( tmp1 <= 0.0 ? 1.0 : ( d >= 0.0 ? 0.0 : -d/a ) );
					}
					break;
				default:
					break;

				}
				Point3 closePt;
				closePt = B + E0*s + E1*t;
				//closePt += B;
				//closePt += Point3(E0*=s);
				//closePt += Point3(E1*=t);
				double dist = closePt.distanceTo(P);
				dmin = min(dist, dmin);
			}
			return dmin;
		}

		bool GeometricShape::IsInsideCylinder(Point3 point) {
			point = objectToWorld * point;
			return ((point[0]*point[0] + point[2]*point[2] <= 0.5) && (abs(point[1]) <= 0.5));
		}

		double GeometricShape::GetHeight() {
			return height;
		}

		double GeometricShape::GetRadius(){
			return radius;
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

		int GeometricShape::GetGeometricShapeType() {
			return geometricShapeType;
		}
		Matrix4 GeometricShape::GetRotationMatrix() {
			return rotationMatrix;
		}

		Matrix4 GeometricShape::GetWorldToObjectMatrix() {
			return worldToObject;
		}
		Point3 GeometricShape::GetCenter() {
			return centerPoint;
		}

		Point3 GeometricShape::GetWorldCoordinates(Point3 point) {
			return worldToObject * point;
		}

		Point3Int GeometricShape::GetCornerCell(int node) {
			for(unsigned int i = 0; i < cornerCells.size(); i++) {
				if(cornerCells[i].node == node) {
					return cornerCells[i];
				}
			}
			return Point3Int(0,0,0,0);
		}

		Vector3DFloat GeometricShape::GetCornerCell2(int node) {
			Point3Int cell = GetCornerCell(node);
			return Vector3DFloat((float)cell.x, (float)cell.y, (float)cell.z);
		}

		Vector3DFloat GeometricShape::GetCornerCell3(int node) {
			Point3 pt;
			if(node == 1) {
				pt = Point3(0, 0.5, 0);
			} else {
				pt = Point3(0, -0.5, 0);
			}
			pt = GetWorldCoordinates(pt);

			return Vector3DFloat((float)pt[0], (float)pt[1], (float)pt[2]);
		}


		void GeometricShape::AddInternalCell(Point3Int point) {
			internalCells.push_back(point);
		}
		
		// Search through all voxels inside a helix to find the two corners, which have only one neighbor in the helix.
		void GeometricShape::FindCornerCellsInHelix() {

			// array to help iterate over 6 neighbor voxels
			int d[6][3];
			d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
			d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
			d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
			d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
			d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
			d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;
			
			// counter of number of neighbor cells are inside the helix
			int insideCounter;

			// for each cell inside the helix
			for(unsigned int i = 0; i < internalCells.size(); i++) {
				insideCounter = 0;
				// count the number of neighbor cells inside the helix
				for(int j = 0; j < 6; j++) {
					if(GetLocationInVector(internalCells, Point3Int(internalCells[i].x + d[j][0], internalCells[i].y + d[j][1], internalCells[i].z + d[j][2], 0)) >= 0) {
						insideCounter++;
					}
				}
				// if only one neighbor inside the helix, this is a corner cell. add it to the list.
				if(insideCounter == 1) {
					cornerCells.push_back(internalCells[i]);
				}
			}

			// abort if more than two corner cells were found
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

		double GeometricShape::GetCornerCellsMaxLength() {
			double length = 0;
			for(int i = 0; i < (int)cornerCells.size() - 1; i++) {
				for(int j = i+1; j < (int)cornerCells.size(); j++) {
					length = max(length, Point3Int::EuclideanDistance(cornerCells[i], cornerCells[j]));
				}
			}
			return length;
		}
		
		void GeometricShape::Rotate(Vector3 axis, double angle){
			rotationMatrix = Matrix4::rotation(axis, angle) * rotationMatrix;
			inverseRotationMatrix = inverseRotationMatrix * Matrix4::rotation(axis, -angle);
			UpdateWorldToObjectMatrix();
		}

		void GeometricShape::Translate(Vector3 translationVector){
			
			centerPoint = centerPoint + translationVector;
			UpdateWorldToObjectMatrix();
		}

		void GeometricShape::SetColor(float r, float g, float b, float a) {
			colorR = r;
			colorG = g;
			colorB = b;
			colorA = a;
		}

		void GeometricShape::SetCenter(Point3 center) {
			this->centerPoint = center;
			UpdateWorldToObjectMatrix();
		}

		void GeometricShape::SetHeight(double height) {
			this->height = height;
			UpdateWorldToObjectMatrix();
		}

		void GeometricShape::SetRadius(double radius) {
			this->radius = radius;
			UpdateWorldToObjectMatrix();
		}

		void GeometricShape::UpdateWorldToObjectMatrix() {
			worldToObject = Matrix4::translation(centerPoint) * rotationMatrix * Matrix4::scaling(radius*2, height, radius*2);
			objectToWorld = Matrix4::scaling(1.0/(radius*2.0), 1.0/height, 1.0/(radius*2.0)) * inverseRotationMatrix * Matrix4::translation(Point3(-centerPoint[0], -centerPoint[1], -centerPoint[2]));
		}
		void GeometricShape::GetColor(float & r, float & g, float & b, float & a) {
			r = colorR;
			g = colorG;
			b = colorB;
			a = colorA;
		}
		void GeometricShape::SetSelected(bool selected) {
			this->selected = selected;
		}
	}
}
#endif