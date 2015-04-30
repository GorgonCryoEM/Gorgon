// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for rasterizing vector representations


#ifndef FOUNDATION_RASTERIZER_H
#define FOUNDATION_RASTERIZER_H

#include <vector>
#include <MathTools/Vector3D.h> 

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Foundation {
		class Rasterizer {
		public:
			static vector<Vector3DInt> ScanConvertLineC8(int x1, int y1, int z1, int x2, int y2, int z2);
			static vector<Vector3DInt> ScanConvertLine(int x1, int y1, int z1, int x2, int y2, int z2);
			static vector<Vector3DInt> ScanConvertLine(Vector3DInt p1, Vector3DInt p2);
		};

		vector<Vector3DInt> Rasterizer::ScanConvertLineC8(int x1, int y1, int z1, int x2, int y2, int z2) {
			vector<Vector3DInt> points = ScanConvertLine(x1, y1, z1, x2, y2, z2);
			vector<Vector3DInt> newPoints;
			for(unsigned int i = 0; i < points.size()-1; i++) {
				newPoints.push_back(points[i]);
				
				if( (points[i].XInt() != points[i+1].XInt()) && 
					(points[i].YInt() != points[i+1].YInt()) &&
					(points[i].ZInt() != points[i+1].ZInt())) {
					newPoints.push_back(Vector3DInt(points[i+1].XInt(), points[i].YInt(), points[i].ZInt()));
					newPoints.push_back(Vector3DInt(points[i+1].XInt(), points[i+1].YInt(), points[i].ZInt()));
				} else if ( (points[i].XInt() != points[i+1].XInt()) && 
					(points[i].YInt() != points[i+1].YInt())) {
					newPoints.push_back(Vector3DInt(points[i+1].XInt(), points[i].YInt(), points[i].ZInt()));
				} else if ( (points[i].XInt() != points[i+1].XInt()) && 
					(points[i].ZInt() != points[i+1].ZInt())) {
					newPoints.push_back(Vector3DInt(points[i+1].XInt(), points[i].YInt(), points[i].ZInt()));
				} else if ( (points[i].YInt() != points[i+1].YInt()) && 
					(points[i].ZInt() != points[i+1].ZInt())) {
					newPoints.push_back(Vector3DInt(points[i].XInt(), points[i+1].YInt(), points[i].ZInt()));
				}
			}
			newPoints.push_back(points[points.size()-1]);
			return newPoints;
		}

		// Scan converts to 26 connectivity
		vector<Vector3DInt> Rasterizer::ScanConvertLine(int x1, int y1, int z1, int x2, int y2, int z2) {
			vector<Vector3DInt> pseudoVertices;

			int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
			int pixel[3];


			pixel[0] = x1;
			pixel[1] = y1;
			pixel[2] = z1;

			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			x_inc = (dx < 0) ? -1 : 1;
			l = abs(dx);
			y_inc = (dy < 0) ? -1 : 1;
			m = abs(dy);
			z_inc = (dz < 0) ? -1 : 1;
			n = abs(dz);
			
			dx2 = l << 1;
			dy2 = m << 1;
			dz2 = n << 1;

			if ((l >= m) && (l >= n)) {
				err_1 = dy2 - l;
				err_2 = dz2 - l;
				for (i = 0; i < l; i++) {
					pseudoVertices.push_back(Vector3DInt(pixel[0], pixel[1], pixel[2]));
					if (err_1 > 0) {
						pixel[1] += y_inc;
						err_1 -= dx2;
					}
					if (err_2 > 0) {
						pixel[2] += z_inc;
						err_2 -= dx2;
					}
					err_1 += dy2;
					err_2 += dz2;
					pixel[0] += x_inc;
				}
			} else if ((m >= l) && (m >= n)) {
				err_1 = dx2 - m;
				err_2 = dz2 - m;
				for (i = 0; i < m; i++) {
					pseudoVertices.push_back(Vector3DInt(pixel[0], pixel[1], pixel[2]));
					if (err_1 > 0) {
						pixel[0] += x_inc;
						err_1 -= dy2;
					}
					if (err_2 > 0) {
						pixel[2] += z_inc;
						err_2 -= dy2;
					}
					err_1 += dx2;
					err_2 += dz2;
					pixel[1] += y_inc;
				}
			} else {
				err_1 = dy2 - n;
				err_2 = dx2 - n;
				for (i = 0; i < n; i++) {
					pseudoVertices.push_back(Vector3DInt(pixel[0], pixel[1], pixel[2]));
					if (err_1 > 0) {
						pixel[1] += y_inc;
						err_1 -= dz2;
					}
					if (err_2 > 0) {
						pixel[0] += x_inc;
						err_2 -= dz2;
					}
					err_1 += dy2;
					err_2 += dx2;
					pixel[2] += z_inc;
				}
			}
			pseudoVertices.push_back(Vector3DInt(pixel[0], pixel[1], pixel[2]));
			return pseudoVertices;
		}

		vector<Vector3DInt> Rasterizer::ScanConvertLine(Vector3DInt p1, Vector3DInt p2) {
			return ScanConvertLine(p1.X(), p1.Y(), p1.Z(), p2.X(), p2.Y(), p2.Z());
		}
	}
}

#endif
