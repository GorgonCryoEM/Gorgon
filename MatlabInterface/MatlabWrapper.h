#ifndef MATLAB_WRAPPER_H
#define MATLAB_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataStructures.h"
#include <math.h>
#include "VectorLib.h"
#include "ComplexNumber.h"
#include "eigen.h"

namespace wustl_mm {
	namespace MatlabInterface {
		class MatlabWrapper {
		public:
			MatlabWrapper();
			~MatlabWrapper();
			void EigenAnalysis(EigenVectorsAndValues2D & eigenInformation);
			void EigenAnalysis(EigenVectorsAndValues3D & eigenInformation);
		private:
		};

		MatlabWrapper::MatlabWrapper() {
		}

		MatlabWrapper::~MatlabWrapper() {
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues2D & eigenInformation) {
			double a = eigenInformation.structureTensor[0][0];
			double b = eigenInformation.structureTensor[0][1];
			double c = eigenInformation.structureTensor[1][0];
			double d = eigenInformation.structureTensor[1][1];

			double insidesqrt = sqrt((a+d) * (a+d) - 4.0*(a*d - b*c));
			double x1 = (a+d + insidesqrt) / 2.0;
			double x2 = (a+d - insidesqrt) / 2.0;

			Vector3D v1, v2, mv1, mv2;
			if(b == 0) {
				VectorLib::Initialize(v1, 1.0, 0.0, 0.0);
				VectorLib::Initialize(v2, 0.0, 1.0, 0.0);
			} else {
				VectorLib::Initialize(v1, 1.0, (x1-a)/b, 0.0);
				VectorLib::Initialize(v2, 1.0, (x2-a)/b, 0.0);
			}

			VectorLib::Normalize(v1);
			VectorLib::Normalize(v2);

			eigenInformation.eigenValues[0] = x1;
			eigenInformation.eigenValues[1] = x2;
			eigenInformation.eigenVectors[0][0] = v1.values[0];
			eigenInformation.eigenVectors[0][1] = v1.values[1];
			eigenInformation.eigenVectors[1][0] = v2.values[0];
			eigenInformation.eigenVectors[1][1] = v2.values[1];
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues3D & eigenInformation) {
			float st[3][3] = {{eigenInformation.structureTensor[0][0], eigenInformation.structureTensor[0][1], eigenInformation.structureTensor[0][2]},
							  {eigenInformation.structureTensor[1][0], eigenInformation.structureTensor[1][1], eigenInformation.structureTensor[1][2]},
							  {eigenInformation.structureTensor[2][0], eigenInformation.structureTensor[2][1], eigenInformation.structureTensor[2][2]}};
			float values[3];
			float vectors[3][3];
			jacobi(st, values, vectors);
			
			for(int i = 0; i < 3; i++) {
				eigenInformation.eigenValues[i] = abs(values[i]);
				for(int j = 0; j < 3; j++) {
					eigenInformation.eigenVectors[i][j] = vectors[i][j];
				}
			}	
		}
	}
}


#endif MATLAB_WRAPPER_H
