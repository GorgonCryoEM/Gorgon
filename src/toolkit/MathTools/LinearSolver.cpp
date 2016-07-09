/*
 * LinearSolver.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "LinearSolver.h"
#include "Matlab.h"
#include "Core/Output.h"

#include <numeric>

using namespace Core;

namespace MathTools {

    void LinearSolver::FindBestFitLine(Vec3F & pt1, Vec3F & pt2, vector<Vec3F> pts) {
        
        cout<<"   LinearSolver::FindBestFitLine"<<endl;
        
        Vec3F avg = accumulate(pts.begin(), pts.end(), Vec3F(0,0,0));
        
        if(pts.size() > 0)
            avg = avg * (1.0f/(float)pts.size());

        float sX2 = 0, sY2 = 0, sZ2 = 0, sXY = 0, sXZ = 0, sYZ = 0;
        Vec3F dp;
        for(unsigned int i = 0; i < pts.size(); i++) {
            dp = pts[i] - avg;
            sX2 += (dp.X() * dp.X());
            sY2 += (dp.Y() * dp.Y());
            sZ2 += (dp.Z() * dp.Z());
            sXY += (dp.X() * dp.Y());
            sXZ += (dp.X() * dp.Z());
            sYZ += (dp.Y() * dp.Z());
        }

        Eigen3D eigenInfo;
        eigenInfo.tensor[0][0] = sX2;
        eigenInfo.tensor[0][1] = sXY;
        eigenInfo.tensor[0][2] = sXZ;
        eigenInfo.tensor[1][0] = sXY;
        eigenInfo.tensor[1][1] = sY2;
        eigenInfo.tensor[1][2] = sYZ;
        eigenInfo.tensor[2][0] = sXZ;
        eigenInfo.tensor[2][1] = sYZ;
        eigenInfo.tensor[2][2] = sZ2;
        Matlab wrapper;
        wrapper.EigenAnalysis(eigenInfo);

        Vec3F n = Vec3F(eigenInfo.vecs[0][0], eigenInfo.vecs[0][1], eigenInfo.vecs[0][2]);

        /*
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
        */

        pt1 = avg + n * ((pts[0] - avg) * n);
        pt2 = avg + n * ((pts[pts.size()-1] - avg) * n);
        
        cout<<"    ...pts:\n"<<pts<<endl;
        cout<<"    ...pt1: "<<pt1<<endl;
        cout<<"    ...pt2: "<<pt2<<endl;

    }

    double LinearSolver::SumDistSqrd(Vec3F pt1, Vec3F pt2, vector<Vec3F> pts) {
        Vec3F lineDirection = pt2 - pt1;
        lineDirection = lineDirection * (1/lineDirection.length());
        Vec3F vect;
        double totalDistSqrd = 0;
        for (unsigned int i=0; i < pts.size(); i++) {
            vect = pts[i] - pt1;
            vect = lineDirection^vect; //cross product ==> orthogonal distance vector
            totalDistSqrd += vect*vect; //dot product
        }
        return totalDistSqrd;
    }


    MatrixFloat LinearSolver::FindRotationTranslation(vector<Vec3F> l1, vector<Vec3F> l2) {
        if(l1.size() != l2.size()) {
            printf("Error! Cannot find rotation and translation for point lists with different sizes\n");
            exit(0);
        }

        Vec3F c1(0,0,0);
        Vec3F c2(0,0,0);

        unsigned int n = l1.size();

        for(unsigned int i = 0; i < n; i++) {
            c1 += l1[i];
            c2 += l2[i];
        }
        c1 = c1 * (1.0f / (float)n);
        c2 = c2 * (1.0f / (float)n);

        MatrixFloat m1 = MatrixFloat(n, 3);
        MatrixFloat m2 = MatrixFloat(n, 3);

        Vec3F v1, v2;
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
        MatrixFloat u(3,3);
        MatrixFloat v(3,3);
        MatrixFloat w(3,3);

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
            t1.SetValue(-c1[i], i, 3);
            t2.SetValue( c1[i], i, 3);
            t3.SetValue( c2[i] - c1[i], i, 3);
        }

        MatrixFloat transform = t3 * t2 * rot4 * t1;

        return transform;
    }

} /* namespace MathTools */
