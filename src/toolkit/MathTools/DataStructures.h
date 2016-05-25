#ifndef TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H
#define TOOLKIT_MATHTOOLS_DATA_STRUCTURES_H

#include "Vector3.h"

using namespace GraphMatch;

namespace MathTools {
    const int MAX_PROBABILITY_DISTRIBUTION_RADIUS = 10;
    const int MAX_PROBABILITY_DISTRIBUTION_WIDTH = MAX_PROBABILITY_DISTRIBUTION_RADIUS*2 + 1;

    struct EigenVectorsAndValues2D{
        float eigenVectors[2][2];
        float eigenValues[2];
        float structureTensor[2][2];
    };

    struct EigenVectorsAndValues3D{
        float eigenVectors[3][3];
        float eigenValues[3];
        float structureTensor[3][3];
    };

    struct ProbDistr1D{
        int R;
        float vals[MAX_PROBABILITY_DISTRIBUTION_WIDTH];
    };

    struct ProbDistr2D{
        int R;
        float vals[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
    };

    struct ProbDistr3D{
        int R;
        float vals[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
    };

    struct EigenResults3D{
        float vals[3];
        Vector3Float vecs[3];
    };

}

#endif
