// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class that acts as a wrapper to the Matlab libraries


#ifndef MATLAB_WRAPPER_H
#define MATLAB_WRAPPER_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include "DataStructures.h"
#include "Vector3D.h"
#include "ComplexNumber.h"
#include <schaefer_eigen/Eigen.h>
#include <cmath>
#ifdef USE_MATLAB
#include "engine.h"
#endif

namespace wustl_mm {
	namespace MathTools {
		class MatlabWrapper {
		public:
			MatlabWrapper();
			~MatlabWrapper();
			void EigenAnalysis(EigenVectorsAndValues2D & eigenInformation);
			void EigenAnalysis(EigenVectorsAndValues3D & eigenInformation);
			#ifdef USE_MATLAB
			void EigenAnalysisMatlab(EigenVectorsAndValues3D & eigenInformation);
			#endif
		private:
			#ifdef USE_MATLAB
			Engine * mathEngine;
			#endif
		};

		MatlabWrapper::MatlabWrapper() {
			#ifdef USE_MATLAB
			mathEngine = engOpen(NULL);
			engSetVisible(mathEngine, false);
			#endif
		}

		MatlabWrapper::~MatlabWrapper() {
			#ifdef USE_MATLAB
			engClose(mathEngine);
			delete mathEngine;
			#endif
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues2D & eigenInformation) {
			double a = eigenInformation.structureTensor[0][0];
			double b = eigenInformation.structureTensor[0][1];
			double c = eigenInformation.structureTensor[1][0];
			double d = eigenInformation.structureTensor[1][1];

			double insidesqrt = sqrt((a+d) * (a+d) - 4.0*(a*d - b*c));
			double x1 = (a+d + insidesqrt) / 2.0;
			double x2 = (a+d - insidesqrt) / 2.0;

			Vector3DDouble v1, v2, mv1, mv2;
			if((b == 0) && (d == 0)) {
				v1 = Vector3DDouble(1.0, 0.0, 0.0);
				v2 = Vector3DDouble(0.0, 1.0, 0.0);
			} else if (b == 0) {
				v1 = Vector3DDouble(0.0, 1.0, 0.0);
				v2 = Vector3DDouble(1.0, 0.0, 0.0);
			} else {
				v1 = Vector3DDouble(1.0, (x1-a)/b, 0.0);
				v2 = Vector3DDouble(1.0, (x2-a)/b, 0.0);
			}

			v1.Normalize();
			v2.Normalize();

			eigenInformation.eigenValues[0] = (float)x1;
			eigenInformation.eigenValues[1] = (float)x2;
			eigenInformation.eigenVectors[0][0] = (float)v1.values[0];
			eigenInformation.eigenVectors[0][1] = (float)v1.values[1];
			eigenInformation.eigenVectors[1][0] = (float)v2.values[0];
			eigenInformation.eigenVectors[1][1] = (float)v2.values[1];
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues3D & eigenInformation) {
			float st[3][3] = {{eigenInformation.structureTensor[0][0], eigenInformation.structureTensor[0][1], eigenInformation.structureTensor[0][2]},
							  {eigenInformation.structureTensor[1][0], eigenInformation.structureTensor[1][1], eigenInformation.structureTensor[1][2]},
							  {eigenInformation.structureTensor[2][0], eigenInformation.structureTensor[2][1], eigenInformation.structureTensor[2][2]}};

			float values[3];
			float vectors[3][3];
			jacobi(st, values, vectors);
			
			for(int i = 0; i < 3; i++) {
				eigenInformation.eigenValues[i] = fabs(values[i]);
				for(int j = 0; j < 3; j++) {
					eigenInformation.eigenVectors[i][j] = vectors[i][j];
				}
			}	
		}

		#ifdef USE_MATLAB
		void MatlabWrapper::EigenAnalysisMatlab(EigenVectorsAndValues3D & eigenInformation) {
			mxArray * mxMathData = mxCreateDoubleMatrix(3, 3, mxREAL);

			memcpy(mxGetPr(mxMathData), eigenInformation.structureTensor, 9*sizeof(double));
		
			engPutVariable(mathEngine, "X", mxMathData);
			engEvalString(mathEngine, "[V D] = eigs(X); D = abs(D); D1 = D(1,1); D2 = D(2,2); D3 = D(3,3)");
			mxArray * mxEigenValue1 = engGetVariable(mathEngine, "D1");
			mxArray * mxEigenValue2 = engGetVariable(mathEngine, "D2");
			mxArray * mxEigenValue3 = engGetVariable(mathEngine, "D3");
			mxArray * mxEigenVectors = engGetVariable(mathEngine, "V");
			engEvalString(mathEngine, " clear 'D';clear 'X'; clear 'D1'; clear 'D2'; clear 'D3'; clear 'V';");

			memcpy(eigenInformation.eigenVectors, mxGetPr(mxEigenVectors), 9*sizeof(double));
			memcpy(&eigenInformation.eigenValues[0], mxGetPr(mxEigenValue1), sizeof(double));
			memcpy(&eigenInformation.eigenValues[1], mxGetPr(mxEigenValue2), sizeof(double));
			memcpy(&eigenInformation.eigenValues[2], mxGetPr(mxEigenValue3), sizeof(double));
			
			mxDestroyArray(mxMathData);
			mxDestroyArray(mxEigenValue1);
			mxDestroyArray(mxEigenValue2);
			mxDestroyArray(mxEigenValue3);
			mxDestroyArray(mxEigenVectors);			
			delete mxMathData;
			delete mxEigenValue1;
			delete mxEigenValue2;
			delete mxEigenValue3;
			delete mxEigenVectors;
		}
		#endif
	}
}


#endif //MATLAB_WRAPPER_H
