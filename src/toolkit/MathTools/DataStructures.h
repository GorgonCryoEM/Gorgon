#ifndef TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H
#define TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H

#include "Vector3.h"

using namespace GraphMatch;

namespace MathTools {
    const int MAX_PROBAB_DISTR_R = 10;
    const int MAX_PROBAB_DISTR_W = MAX_PROBAB_DISTR_R*2 + 1;

    struct Eigen2D{
        float vecs[2][2];
        float vals[2];
        float tensor[2][2];
    };

    struct EigenResults3D{
        float vals[3];
        Vector3Float vecs[3];
    };

    struct Eigen3D : public EigenResults3D {
        float tensor[3][3];
    };

    struct ProbDistr1D{
        int R;
        float vals[MAX_PROBAB_DISTR_W];
    };

    struct ProbDistr2D{
        int R;
        float vals[MAX_PROBAB_DISTR_W][MAX_PROBAB_DISTR_W];
    };

    struct ProbDistr3D{
        int R;
        float vals[MAX_PROBAB_DISTR_W][MAX_PROBAB_DISTR_W][MAX_PROBAB_DISTR_W];
    };
}

#endif
