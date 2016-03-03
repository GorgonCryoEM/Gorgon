/*
 * HermiteCurve.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_HERMITECURVE_H_
#define SRC_TOOLKIT_VISUALIZATION_HERMITECURVE_H_

#include "MathTools/Vector3.h"

using namespace GraphMatch;

namespace Visualization {

    /**
    This code based on molscript's hermite_curve.c file, and produced with the help
    of wikipedia's article on the cubic hermite spline
    */
    class HermiteCurve{
    public:
        Vec3F p0, p1, m0, m1;

        void setCurve(Vec3F pstart, Vec3F pend, Vec3F tstart, Vec3F tend);
        Vec3F getPos(double t);
        Vec3F getTangent(double t);
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_HERMITECURVE_H_ */
