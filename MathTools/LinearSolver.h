// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Performs Linear solving

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$

#ifndef MATH_TOOLS_LINEAR_SOLVER_H
#define MATH_TOOLS_LINEAR_SOLVER_H

#include "Vector3D.h"
#include <vector>
#include "MatlabWrapper.h"

using namespace std;

namespace wustl_mm {
	namespace MathTools {

		class LinearSolver{
		public:
			static void FindBestFitLine(Vector3DFloat & pt1, Vector3DFloat & pt2, vector<Vector3DFloat> pts);	
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
			for(unsigned int i = 0; i < pts.size(); i++) {
				t = (pts[i] - avg) * n;
				minT = min(minT, t);
				maxT = max(maxT, t);
			}

			pt1 = avg + n * minT;
			pt2 = avg + n * maxT;

		}
	}
}

#endif