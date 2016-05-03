/*
 * LinearSolver.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_
#define SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_

#include "Vector3D.h"
#include <vector>
#include "MatlabWrapper.h"
#include "Matrix.h"

namespace MathTools {

    class LinearSolver {
        public:
        public:
            static void FindBestFitLine(Vector3DFloat & pt1, Vector3DFloat & pt2, vector<Vector3DFloat> pts);
            // Find the orthogonal distances from points to a line given by pt1 & pt2. Return the sum of the squares of those distance.
            static double SumDistSqrd(Vector3DFloat pt1, Vector3DFloat pt2, vector<Vector3DFloat> pts);
            static MatrixFloat FindRotationTranslation(vector<Vector3DFloat> l1, vector<Vector3DFloat> l2);
    };

} /* namespace MathTools */

#endif /* SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_ */
