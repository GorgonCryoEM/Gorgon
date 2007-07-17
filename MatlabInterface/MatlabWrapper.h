#ifndef MATLAB_WRAPPER_H
#define MATLAB_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include <string.h>

namespace wustl_mm {
	namespace MatlabInterface {
		const int MAX_GAUSSIAN_FILTER_WIDTH = 10;

		struct EigenVectorsAndValues2D{
			double eigenVectors[2][2];
			double eigenValues[2];
			double structureTensor[2][2];
		};

		struct GaussianFilter2D{
			int width;
			double values[MAX_GAUSSIAN_FILTER_WIDTH][MAX_GAUSSIAN_FILTER_WIDTH];
		};

		struct GaussianFilter3D{
			int width;
			double values[MAX_GAUSSIAN_FILTER_WIDTH][MAX_GAUSSIAN_FILTER_WIDTH][MAX_GAUSSIAN_FILTER_WIDTH];
		};

		class MatlabWrapper {
		public:
			MatlabWrapper();
			~MatlabWrapper();
			void EigenAnalysis(EigenVectorsAndValues2D & eigenInformation);
			void GetGaussianFilter(GaussianFilter2D & filterInformation);
			void GetGaussianFilter(GaussianFilter3D & filterInformation);
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
			mxArray * mxMathData = mxCreateDoubleMatrix(2, 2, mxComplexity::mxREAL);

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

		void MatlabWrapper::GetGaussianFilter(GaussianFilter2D & filterInformation) {
			mxArray * mxWidth = mxCreateDoubleScalar(filterInformation.width);
			engPutVariable(mathEngine, "width", mxWidth);
			engEvalString(mathEngine, "G = fspecial('gaussian',[width,width]);");
			mxArray * mxFilter = engGetVariable(mathEngine, "G");
			engEvalString(mathEngine, "clear 'width'; clear 'G'");

			double * filter = new double[MAX_GAUSSIAN_FILTER_WIDTH*MAX_GAUSSIAN_FILTER_WIDTH];
			memcpy(filter, mxGetPr(mxFilter), filterInformation.width*filterInformation.width*sizeof(double));
			
			for(int x = 0; x < filterInformation.width; x++) {
				for(int y = 0; y < filterInformation.width; y++) {
					filterInformation.values[x][y] = filter[x + y*filterInformation.width];
				}
			}			

			mxDestroyArray(mxWidth);
			mxDestroyArray(mxFilter);
			delete mxWidth;
			delete mxFilter;
			delete [] filter;

		}
		void MatlabWrapper::GetGaussianFilter(GaussianFilter3D & filterInformation) {
			mxArray * mxWidth = mxCreateDoubleScalar(filterInformation.width);
			engPutVariable(mathEngine, "width", mxWidth);
			engEvalString(mathEngine, "G = convn(reshape(fspecial('gaussian', [width width]), [width width 1]), reshape(fspecial('gaussian', [width 1]), 1,1, width));");
			mxArray * mxFilter = engGetVariable(mathEngine, "G");
			engEvalString(mathEngine, "clear 'width'; clear 'G'");

			double * filter = new double[MAX_GAUSSIAN_FILTER_WIDTH * MAX_GAUSSIAN_FILTER_WIDTH * MAX_GAUSSIAN_FILTER_WIDTH];
			memcpy(filter, mxGetPr(mxFilter), filterInformation.width * filterInformation.width * filterInformation.width * sizeof(double));
			
			for(int x = 0; x < filterInformation.width; x++) {
				for(int y = 0; y < filterInformation.width; y++) {
					for(int z = 0; z < filterInformation.width; z++) {
						filterInformation.values[x][y][z] = filter[x * filterInformation.width * filterInformation.width + y * filterInformation.width + z];
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
