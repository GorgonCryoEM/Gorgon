/*
 * Matlab.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Matlab.h"
using namespace GraphMatch;
namespace MathTools {

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

        Vec3D v1, v2, mv1, mv2;
        if((b == 0) && (d == 0)) {
            v1 = Vec3D(1.0, 0.0, 0.0);
            v2 = Vec3D(0.0, 1.0, 0.0);
        } else if (b == 0) {
            v1 = Vec3D(0.0, 1.0, 0.0);
            v2 = Vec3D(1.0, 0.0, 0.0);
        } else {
            v1 = Vec3D(1.0, (x1-a)/b, 0.0);
            v2 = Vec3D(1.0, (x2-a)/b, 0.0);
        }

        v1.normalize();
        v2.normalize();

        in.vals[0] = (float)x1;
        in.vals[1] = (float)x2;
        in.vecs[0][0] = (float)v1[0];
        in.vecs[0][1] = (float)v1[1];
        in.vecs[1][0] = (float)v2[0];
        in.vecs[1][1] = (float)v2[1];
    }

    #ifndef USE_MATLAB
    void Matlab::EigenAnalysis(Eigen3D & in) {
        float st[3][3] = {{in.tensor[0][0], in.tensor[0][1], in.tensor[0][2]},
                          {in.tensor[1][0], in.tensor[1][1], in.tensor[1][2]},
                          {in.tensor[2][0], in.tensor[2][1], in.tensor[2][2]}
                         };

        float vals[3];
        float vecs[3][3];
        jacobi(st, vals, vecs);

        for(int i = 0; i < 3; i++) {
            in.vals[i] = fabs(vals[i]);
            for(int j = 0; j < 3; j++) {
                in.vecs[i][j] = vecs[i][j];
            }
        }
    }
    #else
    void Matlab::EigenAnalysis(Eigen3D & in) {
        mxArray * mxMathData = mxCreateDoubleMatrix(3, 3, mxREAL);

        memcpy(mxGetPr(mxMathData), in.tensor, 9*sizeof(double));

        engPutVariable(mathEngine, "X", mxMathData);
        engEvalString(mathEngine, "[V D] = eigs(X); D = abs(D); D1 = D(1,1); D2 = D(2,2); D3 = D(3,3)");
        mxArray * mxEigenValue1 = engGetVariable(mathEngine, "D1");
        mxArray * mxEigenValue2 = engGetVariable(mathEngine, "D2");
        mxArray * mxEigenValue3 = engGetVariable(mathEngine, "D3");
        mxArray * mxEigenVectors = engGetVariable(mathEngine, "V");
        engEvalString(mathEngine, " clear 'D';clear 'X'; clear 'D1'; clear 'D2'; clear 'D3'; clear 'V';");

        memcpy(in.vecs, mxGetPr(mxEigenVectors), 9*sizeof(double));
        memcpy(&in.vals[0], mxGetPr(mxEigenValue1), sizeof(double));
        memcpy(&in.vals[1], mxGetPr(mxEigenValue2), sizeof(double));
        memcpy(&in.vals[2], mxGetPr(mxEigenValue3), sizeof(double));

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

} /* namespace MathTools */
