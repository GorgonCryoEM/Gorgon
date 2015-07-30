#ifndef MATLAB_WRAPPER_H
#define MATLAB_WRAPPER_H

#include <cstdio>
#include <cstdlib>
#include "engine.h"
#include <string>
#include "MatlabWrapper.h"

namespace wustl_mm {
	namespace MatlabInterface {

		/*MatlabWrapper::MatlabWrapper() {
			mathEngine = engOpen(NULL);
			engSetVisible(mathEngine, false);
		}*/

	/*	MatlabWrapper::~MatlabWrapper() {
			engClose(mathEngine);
			delete mathEngine;
		}*/

		void MatlabWrapper::Misc() {
			double xValues[3][4] = {{1,1,1,3}, {2,2,2,4}, {3,3,3,5}};

			mxArray * X = NULL;	
			
			X = mxCreateDoubleMatrix(3, 4, mxComplexity::mxREAL);
			mxSetData(X, xValues);
			
			
			engPutVariable(mathEngine, "X", X);
			engEvalString(mathEngine, "[U, S, V] = svd(X);");
			mxArray * U = engGetVariable(mathEngine, "U");
			mxArray * S = engGetVariable(mathEngine, "S");
			mxArray * V = engGetVariable(mathEngine, "V");

			double * myArray;
			myArray = (double*)mxGetData(U);

			printf("%i %i %f, %f \n", mxGetM(V), mxGetN(V), myArray[2,3], myArray[1,2] );	
		}


	}
}


#endif MATLAB_WRAPPER_H
