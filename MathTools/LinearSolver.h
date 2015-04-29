// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs Linear solving


#ifndef MATH_TOOLS_LINEAR_SOLVER_H
#define MATH_TOOLS_LINEAR_SOLVER_H

#include "Vector3D.h"
#include <vector>
#include "MatlabWrapper.h"
#include "Matrix.h"

using namespace std;

namespace wustl_mm {
	namespace MathTools {

		class LinearSolver{
		public:
			static void FindBestFitLine(Vector3DFloat & pt1, Vector3DFloat & pt2, vector<Vector3DFloat> pts);
			// Find the orthogonal distances from points to a line given by pt1 & pt2. Return the sum of the squares of those distance.
			static double SumDistSqrd(Vector3DFloat pt1, Vector3DFloat pt2, vector<Vector3DFloat> pts);
			static MatrixFloat FindRotationTranslation(vector<Vector3DFloat> l1, vector<Vector3DFloat> l2);
		};

		void LinearSolver::FindBestFitLine(Vector3DFloat & pt1, Vector3DFloat & pt2, vector<Vector3DFloat> pts) {
			Vector3DFloat avg = Vector3DFloat(0,0,0);
			for(unsigned int i = 0; i < pts.size(); i++) {
				avg += pts[i];
			}
			if(pts.size() > 0) {
				avg = avg * (1.0f/(float)pts.size());
			}

			float sX2 = 0, sY2 = 0, sZ2 = 0, sXY = 0, sXZ = 0, sYZ = 0;
			Vector3DFloat dp;
			for(unsigned int i = 0; i < pts.size(); i++) {
				dp = pts[i] - avg;
				sX2 += (dp.X() * dp.X());
				sY2 += (dp.Y() * dp.Y());
				sZ2 += (dp.Z() * dp.Z());				
				sXY += (dp.X() * dp.Y());
				sXZ += (dp.X() * dp.Z());
				sYZ += (dp.Y() * dp.Z());
			}

			EigenVectorsAndValues3D eigenInfo;
			eigenInfo.structureTensor[0][0] = sX2;
			eigenInfo.structureTensor[0][1] = sXY;
			eigenInfo.structureTensor[0][2] = sXZ;
			eigenInfo.structureTensor[1][0] = sXY;
			eigenInfo.structureTensor[1][1] = sY2;
			eigenInfo.structureTensor[1][2] = sYZ;
			eigenInfo.structureTensor[2][0] = sXZ;
			eigenInfo.structureTensor[2][1] = sYZ;
			eigenInfo.structureTensor[2][2] = sZ2;
			MatlabWrapper wrapper;
			wrapper.EigenAnalysis(eigenInfo);
			
			Vector3DFloat n = Vector3DFloat(eigenInfo.eigenVectors[0][0], eigenInfo.eigenVectors[0][1], eigenInfo.eigenVectors[0][2]);

			float maxT = MIN_FLOAT;
			float minT = MAX_FLOAT;
			float t;
			/*
			for(unsigned int i = 0; i < pts.size(); i++) {
				t = (pts[i] - avg) * n;
				minT = min(minT, t);
				maxT = max(maxT, t);
			}
			pt1 = avg + n * minT;
			pt2 = avg + n * maxT;
			*/

			pt1 = avg + n * ((pts[0] - avg) * n);
			pt2 = avg + n * ((pts[pts.size()-1] - avg) * n);

		}

		double LinearSolver::SumDistSqrd(Vector3DFloat pt1, Vector3DFloat pt2, vector<Vector3DFloat> pts) {
			Vector3DFloat lineDirection = pt2 - pt1;
			lineDirection = lineDirection * (1/lineDirection.Length());
			Vector3DFloat vect;
			double totalDistSqrd = 0;
			for (unsigned int i=0; i < pts.size(); i++) {
				vect = pts[i] - pt1;
				vect = lineDirection^vect; //cross product ==> orthogonal distance vector
				totalDistSqrd += vect*vect; //dot product
			}
			return totalDistSqrd;
		}


		MatrixFloat LinearSolver::FindRotationTranslation(vector<Vector3DFloat> l1, vector<Vector3DFloat> l2) {
			if(l1.size() != l2.size()) {
				printf("Error! Cannot find rotation and translation for point lists with different sizes\n");
				exit(0);
			}

			Vector3DFloat c1 = Vector3DFloat(0,0,0);
			Vector3DFloat c2 = Vector3DFloat(0,0,0);

			unsigned int n = l1.size();

			for(unsigned int i = 0; i < n; i++) {
				c1 += l1[i];
				c2 += l2[i];
			}
			c1 = c1 * (1.0f / (float)n);
			c2 = c2 * (1.0f / (float)n);

			MatrixFloat m1 = MatrixFloat(n, 3);
			MatrixFloat m2 = MatrixFloat(n, 3);

			Vector3DFloat v1, v2;
			for(unsigned int i = 0; i < n; i++) {
				v1 = l1[i] - c1;
				m1.SetValue(v1.X(), i, 0);
				m1.SetValue(v1.Y(), i, 1);
				m1.SetValue(v1.Z(), i, 2);

				v2 = l2[i] - c2;
				m2.SetValue(v2.X(), i, 0);
				m2.SetValue(v2.Y(), i, 1);
				m2.SetValue(v2.Z(), i, 2);		
			}

			MatrixFloat covar = MatrixFloat::Covariance(m1, m2);
			MatrixFloat u = MatrixFloat(3,3);
			MatrixFloat v = MatrixFloat(3,3);
			MatrixFloat w = MatrixFloat(3,3);

			covar.SingularValueDecomposition(u, w, v);

			MatrixFloat rot = u * v.Transpose();

			MatrixFloat rot4 = MatrixFloat(4,4);
			for(unsigned int i = 0; i < 3; i++) {
				for(unsigned int j = 0; j < 3; j++) {
					rot4.SetValue(rot.GetValue(i, j), i, j);
				}
			}
			rot4.SetValue(1.0f, 3,3);

			MatrixFloat t1 = MatrixFloat::Identity(4);
			MatrixFloat t2 = MatrixFloat::Identity(4);
			MatrixFloat t3 = MatrixFloat::Identity(4);
			for(unsigned int i = 0; i < 3; i++) {
				t1.SetValue(-c1.values[i], i, 3);
				t2.SetValue(c1.values[i], i, 3);
				t3.SetValue(c2.values[i] - c1.values[i], i, 3);
			}

			MatrixFloat transform = t3 * t2 * rot4 * t1;

			return transform;					
		}
	}
}

#endif
