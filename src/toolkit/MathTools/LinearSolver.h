/*
 * LinearSolver.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_
#define SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_

#include "Vector3.h"

using namespace GraphMatch;

namespace MathTools {

    namespace LinearSolver {
            void FindBestFitLine(Vec3F & pt1, Vec3F & pt2, vector<Vec3F> pts);
            // Find the orthogonal distances from points to a line given by pt1 & pt2. Return the sum of the squares of those distance.
            double SumDistSqrd(Vec3F pt1, Vec3F pt2, vector<Vec3F> pts);
            MatrixFloat FindRotationTranslation(vector<Vec3F> l1, vector<Vec3F> l2);
    };

} /* namespace MathTools */

#endif /* SRC_TOOLKIT_MATHTOOLS_LINEARSOLVER_H_ */
