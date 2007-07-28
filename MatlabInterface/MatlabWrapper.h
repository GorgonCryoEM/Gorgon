#ifndef MATLAB_WRAPPER_H
#define MATLAB_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include <string.h>
#include "DataStructures.h"

namespace wustl_mm {
	namespace MatlabInterface {
		class MatlabWrapper {
		public:
			MatlabWrapper();
			~MatlabWrapper();
			void EigenAnalysis(EigenVectorsAndValues2D & eigenInformation);
			void EigenAnalysis(EigenVectorsAndValues3D & eigenInformation);
			void GetGaussianFilter(ProbabilityDistribution2D & filterInformation);
			void GetGaussianFilter(ProbabilityDistribution3D & filterInformation);
		private:
			Engine * mathEngine;
		};

		MatlabWrapper::MatlabWrapper() {
			mathEngine = engOpen(NULL);
			engSetVisible(mathEngine, false);
		}

		MatlabWrapper::~MatlabWrapper() {
			engClose(mathEngine);
			delete mathEngine;
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues2D & eigenInformation) {
			mxArray * mxMathData = mxCreateDoubleMatrix(2, 2, mxREAL);

			memcpy(mxGetPr(mxMathData), eigenInformation.structureTensor, 4*sizeof(double));
		
			engPutVariable(mathEngine, "X", mxMathData);
			engEvalString(mathEngine, "[V D] = eigs(X); D = abs(D); D1 = D(1,1); D2 = D(2,2);");
			mxArray * mxEigenValue1 = engGetVariable(mathEngine, "D1");
			mxArray * mxEigenValue2 = engGetVariable(mathEngine, "D2");
			mxArray * mxEigenVectors = engGetVariable(mathEngine, "V");
			engEvalString(mathEngine, " clear 'D';clear 'X'; clear 'D1'; clear 'D2'; clear 'V';");

			memcpy(eigenInformation.eigenVectors, mxGetPr(mxEigenVectors), 4*sizeof(double));
			memcpy(&eigenInformation.eigenValues[0], mxGetPr(mxEigenValue1), sizeof(double));
			memcpy(&eigenInformation.eigenValues[1], mxGetPr(mxEigenValue2), sizeof(double));
			
			mxDestroyArray(mxMathData);
			mxDestroyArray(mxEigenValue1);
			mxDestroyArray(mxEigenValue2);
			mxDestroyArray(mxEigenVectors);			
			delete mxMathData;
			delete mxEigenValue1;
			delete mxEigenValue2;
			delete mxEigenVectors;
		}

		void MatlabWrapper::EigenAnalysis(EigenVectorsAndValues3D & eigenInformation) {
			mxArray * mxMathData = mxCreateDoubleMatrix(3, 3, mxREAL);

			memcpy(mxGetPr(mxMathData), eigenInformation.structureTensor, 9*sizeof(double));
		
			engPutVariable(mathEngine, "X", mxMathData);
			engEvalString(mathEngine, "[V D] = eigs(X); D = abs(D); D1 = D(1,1); D2 = D(2,2); D3 = D(3,3)");
			mxArray * mxEigenValue1 = engGetVariable(mathEngine, "D1");
			mxArray * mxEigenValue2 = engGetVariable(mathEngine, "D2");
			mxArray * mxEigenValue3 = engGetVariable(mathEngine, "D3");
			mxArray * mxEigenVectors = engGetVariable(mathEngine, "V");
			engEvalString(mathEngine, " clear 'D';clear 'X'; clear 'D1'; clear 'D2'; clear 'D3'; clear 'V';");

			memcpy(eigenInformation.eigenVectors, mxGetPr(mxEigenVectors), 4*sizeof(double));
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


		void MatlabWrapper::GetGaussianFilter(ProbabilityDistribution2D & filterInformation) {
			int width = filterInformation.radius * 2 + 1;
			mxArray * mxWidth = mxCreateDoubleScalar(width);
			engPutVariable(mathEngine, "width", mxWidth);
			engEvalString(mathEngine, "G = fspecial('gaussian',[width,width]);");
			mxArray * mxFilter = engGetVariable(mathEngine, "G");
			engEvalString(mathEngine, "clear 'width'; clear 'G'");


			double * filter = new double[MAX_PROBABILITY_DISTRIBUTION_WIDTH * MAX_PROBABILITY_DISTRIBUTION_WIDTH];
			memcpy(filter, mxGetPr(mxFilter), width * width * sizeof(double));
			
			for(int x = 0; x < width; x++) {
				for(int y = 0; y < width; y++) {
					filterInformation.values[x][y] = filter[x + y * width];
				}
			}			

			mxDestroyArray(mxWidth);
			mxDestroyArray(mxFilter);
			delete mxWidth;
			delete mxFilter;
			delete [] filter;

		}
		void MatlabWrapper::GetGaussianFilter(ProbabilityDistribution3D & filterInformation) {
			int width = filterInformation.radius * 2 + 1;
			mxArray * mxWidth = mxCreateDoubleScalar(width);
			engPutVariable(mathEngine, "width", mxWidth);
			engEvalString(mathEngine, "G = convn(reshape(fspecial('gaussian', [width width]), [width width 1]), reshape(fspecial('gaussian', [width 1]), 1,1, width));");
			mxArray * mxFilter = engGetVariable(mathEngine, "G");
			engEvalString(mathEngine, "clear 'width'; clear 'G'");

			double * filter = new double[MAX_PROBABILITY_DISTRIBUTION_WIDTH * MAX_PROBABILITY_DISTRIBUTION_WIDTH * MAX_PROBABILITY_DISTRIBUTION_WIDTH];
			memcpy(filter, mxGetPr(mxFilter), width * width * width * sizeof(double));
			
			for(int x = 0; x < width; x++) {
				for(int y = 0; y < width; y++) {
					for(int z = 0; z < width; z++) {
						filterInformation.values[x][y][z] = filter[x * width * width + y * width + z];
						printf("%f\t", filterInformation.values[x][y][z]);
					}
					printf("\n");
				}
				printf("\n");
			}			

			mxDestroyArray(mxWidth);
			mxDestroyArray(mxFilter);
			delete mxWidth;
			delete mxFilter;
			delete [] filter;

		}

	}
}


#endif MATLAB_WRAPPER_H
