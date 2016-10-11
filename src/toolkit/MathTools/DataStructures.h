#ifndef TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H
#define TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H

#include "Vector3.h"

using namespace GraphMatch;

namespace MathTools {
    const int maxR = 10;
    const int maxWidth = maxR*2 + 1;

    struct Eigen2D{
        float vecs[2][2];
        float vals[2];
        float tensor[2][2];
    };

    struct EigenResults3D{
        float vals[3];
        Vec3F vecs[3];
    };

    struct Eigen3D : public EigenResults3D {
        float tensor[3][3];
    };

    struct ProbDistr1D{
        int R;
        float vals[maxWidth];
    };

    struct ProbDistr2D{
        int R;
        float vals[maxWidth][maxWidth];
    };

    struct ProbDistr3D{
        int R;
        float vals[maxWidth][maxWidth][maxWidth];
    };
}

#endif
