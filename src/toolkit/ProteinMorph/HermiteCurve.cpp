/*
 * HermiteCurve.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "HermiteCurve.h"

namespace Visualization {

    void HermiteCurve::setCurve(Vec3F pstart, Vec3F pend, Vec3F tstart, Vec3F tend){
        p0 = pstart;
        p1 = pend;
        m0 = tstart;
        m1 = tend;
    }

    Vec3F HermiteCurve::getPos(double t){
        double tsquared = t*t;
        double tcubed = tsquared * t;

        double cp0 = 2*tcubed - 3*tsquared + 1;
        double cm0 = tcubed - 2*tsquared + t;
        double cp1 = (cp0 - 1)*(-1);
        double cm1 = tcubed - tsquared;

        double xt = cp0*p0.X() + cm0*m0.X() + cp1*p1.X() + cm1*m1.X();
        double yt = cp0*p0.Y() + cm0*m0.Y() + cp1*p1.Y() + cm1*m1.Y();
        double zt = cp0*p0.Z() + cm0*m0.Z() + cp1*p1.Z() + cm1*m1.Z();

        return Vec3F(xt, yt, zt);
    }

    // I don't know how this method works, but it is a part of the entirely functional
    // molscript code - BC
    Vec3F HermiteCurve::getTangent(double t){
        double t2 = t * t;
        double cp0 = 6.0 * (t2 - t);
        double cp1 = 6.0 * (-t2 + t);
        double cm0 = 3.0 * t2 - 4.0 * t + 1.0;
        double cm1 = 3.0 * t2 - 2.0 * t;
        double vxt = p0.X()*cp0 + p1.X() * cp1 + m0.X() * cm0 + m1.X() * cm1;
        double vyt = p0.Y()*cp0 + p1.Y() * cp1 + m0.Y() * cm0 + m1.Y() * cm1;
        double vzt = p0.Z()*cp0 + p1.Z() * cp1 + m0.Z() * cm0 + m1.Z() * cm1;

        return Vec3F(vxt, vyt, vzt);
    }

} /* namespace Visualization */
