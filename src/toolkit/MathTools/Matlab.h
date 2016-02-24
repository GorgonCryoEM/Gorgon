#ifndef TOOLKIT_MATHTOOLS_MATLAB_WRAPPER_H
#define TOOLKIT_MATHTOOLS_MATLAB_WRAPPER_H

//#include <cstdio>
//#include <cstdlib>
//#include <string>
#include "DataStructures.h"
//#include "Vector3.h"
//#include "ComplexNumber.h"
#include <schaefer_eigen/Eigen.h>
//#include <cmath>
#ifdef USE_MATLAB
#include "engine.h"
#endif

namespace MathTools {
    class Matlab {
    public:
        Matlab();
        ~Matlab();
        void EigenAnalysis(Eigen2D & in);
        void EigenAnalysis(Eigen3D & in);
        #ifdef USE_MATLAB
        void EigenAnalysisMatlab(Eigen3D & in);
        #endif
    private:
        #ifdef USE_MATLAB
        Engine * mathEngine;
        #endif
    };

    Matlab::Matlab() {
        #ifdef USE_MATLAB
        mathEngine = engOpen(NULL);
        engSetVisible(mathEngine, false);
        #endif
    }

    Matlab::~Matlab() {
        #ifdef USE_MATLAB
        engClose(mathEngine);
        delete mathEngine;
        #endif
    }

    void Matlab::EigenAnalysis(Eigen2D & in) {
        double a = in.tensor[0][0];
        double b = in.tensor[0][1];
        double c = in.tensor[1][0];
        double d = in.tensor[1][1];

        double insidesqrt = sqrt((a+d) * (a+d) - 4.0*(a*d - b*c));
        double x1 = (a+d + insidesqrt) / 2.0;
        double x2 = (a+d - insidesqrt) / 2.0;

        Vector3Double v1, v2, mv1, mv2;
        if((b == 0) && (d == 0)) {
            v1 = Vector3Double(1.0, 0.0, 0.0);
            v2 = Vector3Double(0.0, 1.0, 0.0);
        } else if (b == 0) {
            v1 = Vector3Double(0.0, 1.0, 0.0);
            v2 = Vector3Double(1.0, 0.0, 0.0);
        } else {
            v1 = Vector3Double(1.0, (x1-a)/b, 0.0);
            v2 = Vector3Double(1.0, (x2-a)/b, 0.0);
        }

        v1.normalize();
        v2.normalize();

        in.eigenVals[0] = (float)x1;
        in.eigenVals[1] = (float)x2;
        in.eigenVecs[0][0] = (float)v1[0];
        in.eigenVecs[0][1] = (float)v1[1];
        in.eigenVecs[1][0] = (float)v2[0];
        in.eigenVecs[1][1] = (float)v2[1];
    }

    void Matlab::EigenAnalysis(Eigen3D & in) {
        float st[3][3] = {{in.tensor[0][0], in.tensor[0][1], in.tensor[0][2]},
                          {in.tensor[1][0], in.tensor[1][1], in.tensor[1][2]},
                          {in.tensor[2][0], in.tensor[2][1], in.tensor[2][2]}
                         };

        float values[3];
        float vectors[3][3];
        jacobi(st, values, vectors);

        for(int i = 0; i < 3; i++) {
            in.eigenVals[i] = fabs(values[i]);
            for(int j = 0; j < 3; j++) {
                in.eigenVecs[i][j] = vectors[i][j];
            }
        }
    }

    #ifdef USE_MATLAB
    void Matlab::EigenAnalysisMatlab(Eigen3D & in) {
        mxArray * mxMathData = mxCreateDoubleMatrix(3, 3, mxREAL);

        memcpy(mxGetPr(mxMathData), in.tensor, 9*sizeof(double));

        engPutVariable(mathEngine, "X", mxMathData);
        engEvalString(mathEngine, "[V D] = eigs(X); D = abs(D); D1 = D(1,1); D2 = D(2,2); D3 = D(3,3)");
        mxArray * mxEigenValue1 = engGetVariable(mathEngine, "D1");
        mxArray * mxEigenValue2 = engGetVariable(mathEngine, "D2");
        mxArray * mxEigenValue3 = engGetVariable(mathEngine, "D3");
        mxArray * mxEigenVectors = engGetVariable(mathEngine, "V");
        engEvalString(mathEngine, " clear 'D';clear 'X'; clear 'D1'; clear 'D2'; clear 'D3'; clear 'V';");

        memcpy(in.eigenVecs, mxGetPr(mxEigenVectors), 9*sizeof(double));
        memcpy(&in.eigenVals[0], mxGetPr(mxEigenValue1), sizeof(double));
        memcpy(&in.eigenVals[1], mxGetPr(mxEigenValue2), sizeof(double));
        memcpy(&in.eigenVals[2], mxGetPr(mxEigenValue3), sizeof(double));

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


#endif
